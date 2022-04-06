/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef AAC_ADTS_READER_H_
#define AAC_ADTS_READER_H_

#include <stdint.h>
#include <stdio.h>

class AacAdtsReader {
 public:
  AacAdtsReader();
  ~AacAdtsReader();

  int32_t Open(const char* filename);
  int32_t ReadOneFrame(uint8_t* data, int32_t* size_in_bytes);
  void Close();

 private:
  FILE* aac_adts_file_;
};

#endif  // AAC_ADTS_READER_H_
