/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef WAV_READER_H_
#define WAV_READER_H_

#include <stdint.h>

struct WavFileInfo {
  int32_t format;
  int32_t sample_rate;
  int32_t channels;
  int32_t bits_per_sample;
  int32_t data_length;
};

class WavReader {
 public:
  WavReader();
  ~WavReader();

  int32_t Open(const char* filename);
  int32_t GetInfo(WavFileInfo* info);
  int32_t Read(uint8_t* data, int32_t size_in_bytes);
  void Close();

 private:
  void* wav_file_;
  WavFileInfo wav_file_info_;
};

#endif  // WAV_READER_H_
