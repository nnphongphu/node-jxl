// Copyright 2013 Lovell Fuller and others.
// SPDX-License-Identifier: Apache-2.0

#ifndef SRC_THUMBNAIL_H_
#define SRC_THUMBNAIL_H_

#include <napi.h>
#include <string.h>
#include "encode.h"

class JxlToJpegAsyncWorker : public Napi::AsyncWorker
{
    public:
        JxlToJpegAsyncWorker(Napi::Buffer<uint8_t> &input, size_t input_size, Napi::Function &callback);
        void Execute();
        void OnOK();

    private:
      Napi::ObjectReference inputRef;
      uint8_t *inputPtr;
      uint8_t *outputPtr;
      size_t inputSize;
      uint32_t outputSize;
};

Napi::Value jxlToJpeg(const Napi::CallbackInfo& info);

#endif  // SRC_THUMBNAIL_H_
