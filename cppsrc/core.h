// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef TOOLS_WASM_DEMO_JXL_DECOMPRESSOR_H_
#define TOOLS_WASM_DEMO_JXL_DECOMPRESSOR_H_

#include <stddef.h>
#include <stdint.h>
#include "encode.h"


/*
  Returns (as uint32_t):
    0 - OOM
    1 - decoding JXL failed
    2 - encoding PNG failed
    >=4 - OK
 */

uint8_t* jxlDecompress(const uint8_t* input, size_t input_size, uint32_t &output_size, uint32_t quality);

#endif  // TOOLS_WASM_DEMO_JXL_DECOMPRESSOR_H_
