/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef WAV_WRITER_H_
#define WAV_WRITER_H_

#include <stdint.h>

class WavWriter {
 public:
  WavWriter();
  ~WavWriter();

  int32_t Open(const char* filename,
               int32_t sample_rate,
               int32_t channels,
               int32_t bits_per_sample);
  int32_t Write(uint8_t* data, int32_t size_in_bytes);
  void Close();

 private:
  void* wav_file_;
};

#endif  // WAV_WRITER_H_
