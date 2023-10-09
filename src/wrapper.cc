#include <napi.h>
#include <stdio.h>
#include <string.h>
#include "wrapper.h"
#include "core.h"
#include "encode.h"

std::function<void(void*, char*)> FreeCallback = [](void*, char* data) {
  free(data);
};

JxlToJpegAsyncWorker::JxlToJpegAsyncWorker(Napi::Buffer<uint8_t> &input, size_t input_size, uint32_t quality, Napi::Function &callback) 
                                          : Napi::AsyncWorker(callback),
                                            inputRef(Napi::ObjectReference::New(input, 1)),
                                            inputPtr(input.Data()),
                                            quality(quality),
                                            inputSize(input_size) {}

void JxlToJpegAsyncWorker::Execute()
{

  uint8_t* output = jxlDecompress(this->inputPtr, this->inputSize, this->outputSize, quality);
  if (!output) {
    SetError("Failed to decompress JXL image");
    return;
  }

  this->outputPtr = output;
  return;
}

void JxlToJpegAsyncWorker::OnOK()
{
  Napi::HandleScope scope(Env());
  Callback().Call({Env().Undefined(), Napi::Buffer<char>::New(Env(), (char *)this->outputPtr, this->outputSize, FreeCallback)});
  this->inputRef.Unref();
}

Napi::Value jxlToJpeg(const Napi::CallbackInfo& info) {
  Napi::Object options = info[size_t(0)].As<Napi::Object>();
  Napi::Buffer<uint8_t> buffer = options.Get("buffer").As<Napi::Buffer<uint8_t>>();
  Napi::Number quality = options.Get("quality").As<Napi::Number>();
  Napi::Function cb = info[1].As<Napi::Function>();

  JxlToJpegAsyncWorker *worker = new JxlToJpegAsyncWorker(buffer, buffer.ByteLength(), (uint32_t)(quality.FloatValue()*100), cb);
  worker->Queue();

  return info.Env().Undefined();
}

BitmapToJxlAsyncWorker::BitmapToJxlAsyncWorker(Napi::Buffer<uint8_t> &input, size_t input_size, size_t width, size_t height, Napi::Function &callback) 
                                          : Napi::AsyncWorker(callback),
                                            inputRef(Napi::ObjectReference::New(input, 1)),
                                            inputPtr(input.Data()),
                                            width(width),
                                            height(height),
                                            inputSize(input_size) {}

void BitmapToJxlAsyncWorker::Execute() {
  uint8_t* output = jxlCompress(this->inputPtr, this->inputSize, this->width, this->height);
  if (!output) {
    SetError("Failed to compress JXL image");
    return;
  }

  this->outputPtr = output;
  return;
}


void BitmapToJxlAsyncWorker::OnOK()
{
  Napi::HandleScope scope(Env());
  Callback().Call({Env().Undefined(), Napi::Buffer<char>::New(Env(), (char *)this->outputPtr, this->outputSize, FreeCallback)});
  this->inputRef.Unref();
}


Napi::Value bitmapToJxl(const Napi::CallbackInfo& info) {
  Napi::Object options = info[size_t(0)].As<Napi::Object>();
  Napi::Buffer<uint8_t> buffer = options.Get("buffer").As<Napi::Buffer<uint8_t>>();
  Napi::Number width = options.Get("width").As<Napi::Number>();
  Napi::Number height = options.Get("height").As<Napi::Number>();
  Napi::Function cb = info[1].As<Napi::Function>();
  BitmapToJxlAsyncWorker *worker = new BitmapToJxlAsyncWorker(buffer, buffer.ByteLength(), (uint32_t)(width.Uint32Value()), (uint32_t)(height.Uint32Value()), cb);
  worker->Queue();

  return info.Env().Undefined();
}