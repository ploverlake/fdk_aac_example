/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "wav_reader.h"
#include <stdio.h>
#include <string.h>
#include "wav_file.h"

WavReader::WavReader() : wav_file_(nullptr) {
  memset(&wav_file_info_, 0, sizeof(wav_file_info_));
}

WavReader::~WavReader() {
  if (wav_file_) {
    Close();
  }
}

int32_t WavReader::Open(const char* filename) {
  void* wav_file = wav_read_open(filename);
  if (wav_file == nullptr) {
    printf("Unable to open wav file '%s'\n", filename);
    return -1;
  }

  WavFileInfo info;
  int32_t ret =
      wav_get_header(wav_file, &info.format, &info.sample_rate, &info.channels,
                     &info.bits_per_sample, &info.data_length);
  if (ret) {
    printf("Can not get header, bad wav file %s\n", filename);
    wav_read_close(wav_file);
    return -1;
  }

  wav_file_ = wav_file;
  wav_file_info_ = info;
  return 0;
}

int32_t WavReader::GetInfo(WavFileInfo* info) {
  if (info == nullptr) {
    printf("Invalid param, %p", info);
    return -1;
  }

  memcpy(info, &wav_file_info_, sizeof(wav_file_info_));
  return 0;
}

int32_t WavReader::Read(uint8_t* data, int32_t size_in_bytes) {
  if (wav_file_ == nullptr) {
    return -1;
  }
  return wav_read_data(wav_file_, data, size_in_bytes);
}

void WavReader::Close() {
  wav_read_close(wav_file_);
  wav_file_ = nullptr;
}
