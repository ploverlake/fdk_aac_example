/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef M4A_WRITER_H_
#define M4A_WRITER_H_

#include <stdint.h>

class M4aWriter {
 public:
  M4aWriter();
  ~M4aWriter();

  int32_t Open(const char* filename,
               int32_t aot,
               int32_t sample_rate,
               int32_t frame_length,
               uint8_t* conf,
               int32_t conf_size);
  int32_t Write(uint8_t* data, int32_t size_in_bytes);
  void Close();

 private:
  void* m4a_file_;
  uint32_t track_id_;
};

#endif  // M4A_WRITER_H_
