#include "core.h"

#include <inttypes.h>
#include <jxl/decode.h>
#include <jxl/decode_cxx.h>
#include <jxl/resizable_parallel_runner.h>
#include <jxl/resizable_parallel_runner_cxx.h>

#include <cstring>
#include <memory>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "no_png.h"
#include "packed_image.h"
#include "encode.h"
#include "base/data_parallel.h"

void MaybeMakeCicp(const jxl::extras::PackedPixelFile& ppf,
                   std::vector<uint8_t>* cicp) {
  cicp->clear();
  const JxlColorEncoding& clr = ppf.color_encoding;
  uint8_t color_primaries = 0;
  uint8_t transfer_function = static_cast<uint8_t>(clr.transfer_function);

  if (clr.color_space != JXL_COLOR_SPACE_RGB) {
    return;
  }
  if (clr.primaries == JXL_PRIMARIES_P3) {
    if (clr.white_point == JXL_WHITE_POINT_D65) {
      color_primaries = 12;
    } else if (clr.white_point == JXL_WHITE_POINT_DCI) {
      color_primaries = 11;
    } else {
      return;
    }
  } else if (clr.primaries != JXL_PRIMARIES_CUSTOM &&
             clr.white_point == JXL_WHITE_POINT_D65) {
    color_primaries = static_cast<uint8_t>(clr.primaries);
  } else {
    return;
  }
  if (clr.transfer_function == JXL_TRANSFER_FUNCTION_UNKNOWN ||
      clr.transfer_function == JXL_TRANSFER_FUNCTION_GAMMA) {
    return;
  }

  cicp->resize(4);
  cicp->at(0) = color_primaries;    // Colour Primaries
  cicp->at(1) = transfer_function;  // Transfer Function
  cicp->at(2) = 0;                  // Matrix Coefficients
  cicp->at(3) = 1;                  // Video Full Range Flag
}

auto runner = JxlResizableParallelRunnerMake(nullptr);
std::unique_ptr<jxl::ThreadPool> pool_ = jxl::make_unique<jxl::ThreadPool>(JxlResizableParallelRunner, runner.get());

bool decodeJpegXlOneShot(const uint8_t* jxl, size_t size,
                         std::vector<uint8_t>* pixels, size_t* xsize, size_t* ysize, 
                         std::vector<uint8_t>* icc_profile, jxl::extras::PackedPixelFile* ppf, 
                         uint8_t *&result, uint32_t &output_size, uint32_t quality) {
  // Multi-threaded parallel runner.

  auto dec = JxlDecoderMake(nullptr);
  
 // runner.get()
  // auto thread_pool = JxlThreadParallelRunnerMake(nullptr, 4);
  // void* runner = thread_pool.get();

  if (JXL_DEC_SUCCESS !=
      JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO |
                                               JXL_DEC_FULL_IMAGE |
                                               JXL_DEC_COLOR_ENCODING )) {
    fprintf(stderr, "JxlDecoderSubscribeEvents failed\n");
    return false;
  }

  if (JXL_DEC_SUCCESS != JxlDecoderSetParallelRunner(dec.get(),
                                                     JxlResizableParallelRunner,
                                                     runner.get())) {
    fprintf(stderr, "JxlDecoderSetParallelRunner failed\n");
    return false;
  }

  JxlBasicInfo info;
  JxlPixelFormat format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
  JxlDecoderSetInput(dec.get(), jxl, size);
  JxlDecoderCloseInput(dec.get());

  for (;;) {
    JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());
    if (status == JXL_DEC_ERROR) {
      fprintf(stderr, "Decoder error\n");
      return false;
    } else if (status == JXL_DEC_NEED_MORE_INPUT) {
      fprintf(stderr, "Error, already provided all input\n");
      return false;
    } else if (status == JXL_DEC_BASIC_INFO) {

      if (JXL_DEC_SUCCESS != JxlDecoderGetBasicInfo(dec.get(), &ppf->info)) {
        fprintf(stderr, "JxlDecoderGetBasicInfo failed\n");
        return false;
      }

      info = ppf->info;
      ppf->info.bits_per_sample = ppf->info.bits_per_sample > 8? 8 : ppf->info.bits_per_sample;
      ppf->info.exponent_bits_per_sample = 0;
      *xsize = info.xsize;
      *ysize = info.ysize;

      JxlResizableParallelRunnerSetThreads(
          runner.get(),
          JxlResizableParallelRunnerSuggestThreads(info.xsize, info.ysize));

      // JxlResizableParallelRunnerSetThreads(
      //     runner.get(),
      //     JxlResizableParallelRunnerSuggestThreads(info.xsize, info.ysize));
    } else if (status == JXL_DEC_COLOR_ENCODING) {
      // Get the ICC color profile of the pixel data

      size_t icc_size;
      if (JXL_DEC_SUCCESS !=
          JxlDecoderGetICCProfileSize(dec.get(), JXL_COLOR_PROFILE_TARGET_DATA,
                                      &icc_size)) {
        fprintf(stderr, "JxlDecoderGetICCProfileSize failed\n");
        return false;
      }
      ppf->icc.resize(icc_size);
      if (JXL_DEC_SUCCESS != JxlDecoderGetColorAsICCProfile(
                                 dec.get(), JXL_COLOR_PROFILE_TARGET_DATA, 
                                 ppf->icc.data(), icc_size)) {
        fprintf(stderr, "JxlDecoderGetColorAsICCProfile failed\n");
        return false;
      }

    } else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
      size_t buffer_size;
      if (JXL_DEC_SUCCESS !=
          JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size)) {
        fprintf(stderr, "JxlDecoderImageOutBufferSize failed\n");
        return false;
      }
      if (buffer_size != *xsize * *ysize * 3) {
        fprintf(stderr, "Invalid out buffer size %" PRIu64 " %" PRIu64 "\n",
                static_cast<uint64_t>(buffer_size),
                static_cast<uint64_t>(*xsize * *ysize * 3));
        return false;
      }
      pixels->resize(*xsize * *ysize * 3);
      void* pixels_buffer = (void*)pixels->data();
      size_t pixels_buffer_size = pixels->size() * sizeof(uint8_t);
      if (JXL_DEC_SUCCESS != JxlDecoderSetImageOutBuffer(dec.get(), &format,
                                                         pixels_buffer,
                                                         pixels_buffer_size)) {
        fprintf(stderr, "JxlDecoderSetImageOutBuffer failed\n");
        return false;
      }
    } else if (status == JXL_DEC_FULL_IMAGE) {
      // Nothing to do. Do not yet return. If the image is an animation, more
      // full frames may be decoded. This example only keeps the last one.
    } else if (status == JXL_DEC_SUCCESS) {
      // All decoding successfully finished.
      // It's not required to call JxlDecoderReleaseInput(dec.get()) here since
      // the decoder will be destroyed.
      jxl::extras::PackedFrame packedFrame(info.xsize, info.ysize, format);
      ppf->frames.clear();
      ppf->frames.emplace_back(std::move(packedFrame));

      jxl::extras::PackedImage& color = ppf->frames.back().color;
      uint8_t* pixels_buffer = reinterpret_cast<uint8_t*>(color.pixels());
      std::memcpy(pixels_buffer, pixels->data(), pixels->size());

      std::unique_ptr<jxl::extras::Encoder> encoder = jxl::extras::Encoder::FromExtension(".jpg");
      jxl::extras::EncodedImage encoded_image;
      encoder->SetOption("jpeg_encoder", "libjpeg");
      encoder->SetOption("jpeg_quality", std::to_string(quality));
      encoder->SetOption("q", std::to_string(quality));

      if (!encoder->Encode(*ppf, &encoded_image, pool_.get())) {
        printf("Encoding failed\n");
        return false;
      }
      output_size = encoded_image.bitstreams[0].size();
      result = new uint8_t[output_size];
      std::memcpy(result, encoded_image.bitstreams[0].data(),
              encoded_image.bitstreams[0].size());

      return true;
    } else {
      return false;
    }
  }
}

uint8_t* jxlDecompress(const uint8_t* input, size_t input_size, uint32_t &output_size, uint32_t quality) {
  std::vector<uint8_t> pixels;
  std::vector<uint8_t> icc_profile;
  size_t xsize = 0, ysize = 0;

  jxl::extras::PackedPixelFile ppf;
  uint8_t *result = nullptr;
  if (!decodeJpegXlOneShot(input, input_size, &pixels, &xsize, &ysize,
                           &icc_profile, &ppf, result, output_size, quality)) {
    fprintf(stderr, "Error while decoding the jxl file\n");
    return nullptr;
  }
  
  return result;
}