/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef AAC_DECODER_H_
#define AAC_DECODER_H_

#include <stdint.h>
#include "aac_common.h"

struct AacDecoderInfo {
  int32_t aot;
  int32_t aot_flags;
  int32_t sample_rate;
  int32_t aac_sample_rate;
  int32_t channels;
  int32_t aac_channels;
  int32_t frame_length;  // samples per channel
  int32_t bitrate;
  int32_t output_delay;
};

class AacDecoder {
 public:
  AacDecoder();
  ~AacDecoder();

  int32_t Init(int32_t transport_type);
  int32_t GetDecoded(uint8_t* in_buffer,
                     int32_t in_size_bytes,
                     uint8_t* out_buffer,
                     int32_t* out_size_bytes);
  int32_t GetInfo(AacDecoderInfo* info);
  void Uninit();

 private:
  void* aac_decoder_handle_;
};

#endif  // AAC_DECODER_H_
