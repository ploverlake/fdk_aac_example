/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "wav_writer.h"
#include <stdio.h>
#include "wav_file.h"

WavWriter::WavWriter() : wav_file_(nullptr) {}

WavWriter::~WavWriter() {
  if (wav_file_ != nullptr) {
    Close();
  }
}

int32_t WavWriter::Open(const char* filename,
                        int32_t sample_rate,
                        int32_t channels,
                        int32_t bits_per_sample) {
  void* wav_file =
      wav_write_open(filename, sample_rate, channels, bits_per_sample);
  if (wav_file == nullptr) {
    printf("Unable to open wav file '%s'\n", filename);
    return -1;
  }

  wav_file_ = wav_file;
  return 0;
}

int32_t WavWriter::Write(uint8_t* data, int32_t size_in_bytes) {
  if (wav_file_ == nullptr) {
    return -1;
  }
  return wav_write_data(wav_file_, data, size_in_bytes);
}

void WavWriter::Close() {
  wav_write_close(wav_file_);
  wav_file_ = nullptr;
}
