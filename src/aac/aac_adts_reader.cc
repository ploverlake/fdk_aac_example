/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "aac_adts_reader.h"
#include <stdio.h>

#define AAC_ADTS_HEADER_SIZE 7

AacAdtsReader::AacAdtsReader() : aac_adts_file_(nullptr) {}

AacAdtsReader::~AacAdtsReader() {
  if (aac_adts_file_) {
    Close();
  }
}

int32_t AacAdtsReader::Open(const char* filename) {
  FILE* aac_adts_file = fopen(filename, "rb");
  if (aac_adts_file == nullptr) {
    printf("Unable to open aac adts file '%s'\n", filename);
    return -1;
  }
  aac_adts_file_ = aac_adts_file;
  return 0;
}

int32_t AacAdtsReader::ReadOneFrame(uint8_t* data, int32_t* size_in_bytes) {
  if (aac_adts_file_ == nullptr) {
    return -1;
  }

  if (!data || !size_in_bytes || *size_in_bytes < AAC_ADTS_HEADER_SIZE) {
    printf("Invalid param");
    return -1;
  }

  int32_t n = fread(data, 1, AAC_ADTS_HEADER_SIZE, aac_adts_file_);
  if (n != AAC_ADTS_HEADER_SIZE) {
    return -1;
  }

  if (data[0] != 0xff || (data[1] & 0xf0) != 0xf0) {
    printf("Invalid ADTS header.\n");
    return -1;
  }

  int32_t frame_size =
      ((data[3] & 0x03) << 11) | (data[4] << 3) | (data[5] >> 5);
  if (*size_in_bytes < frame_size) {
    printf("Buffer size is not enough\n");
    return -1;
  }

  n = fread(data + AAC_ADTS_HEADER_SIZE, 1, frame_size - AAC_ADTS_HEADER_SIZE,
            aac_adts_file_);
  if (n != (frame_size - AAC_ADTS_HEADER_SIZE)) {
    printf("Read ADTS frame failed.\n");
    return -1;
  }

  *size_in_bytes = frame_size;
  return 0;
}

void AacAdtsReader::Close() {
  if (aac_adts_file_) {
    fclose(aac_adts_file_);
  }
  aac_adts_file_ = nullptr;
}
