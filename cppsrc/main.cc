/* cppsrc/main.cpp */
#include <napi.h>
#include "wrapper.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  exports.Set("jxlToJpeg", Napi::Function::New(env, jxlToJpeg));
  exports.Set("bitmapToJxl", Napi::Function::New(env, bitmapToJxl));
  return exports;
}

NODE_API_MODULE(libjxl, InitAll)