/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef AAC_ENCODER_H_
#define AAC_ENCODER_H_

#include <stdint.h>
#include "aac_common.h"

struct AacEncoderInfo {
  int32_t frame_length;  // samples per channel
  int32_t delay;         // samples per channel
  int32_t delay_core;
  uint8_t conf[64];
  int32_t conf_size;
};

class AacEncoder {
 public:
  AacEncoder();
  ~AacEncoder();

  int32_t Init(int32_t transport_type,
               int32_t aot,
               int32_t sample_rate,
               int32_t channels,
               int32_t bitrate);
  int32_t GetInfo(AacEncoderInfo* info);
  int32_t GetEncoded(uint8_t* in_buffer,
                     int32_t in_size_bytes,
                     uint8_t* out_buffer,
                     int32_t* out_size_bytes);
  void Uninit();

 private:
  int32_t ChannelMode(int32_t channels);

 private:
  void* aac_encoder_handle_;
};

#endif  // AAC_ENCODER_H_
