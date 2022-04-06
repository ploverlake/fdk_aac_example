/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "m4a_writer.h"
#include <stdio.h>
#include "mp4v2/mp4v2.h"

M4aWriter::M4aWriter()
    : m4a_file_(MP4_INVALID_FILE_HANDLE), track_id_(MP4_INVALID_TRACK_ID) {}

M4aWriter::~M4aWriter() {
  if (m4a_file_ != MP4_INVALID_FILE_HANDLE) {
    Close();
  }
}

int32_t M4aWriter::Open(const char* filename,
                        int32_t aot,
                        int32_t sample_rate,
                        int32_t frame_length,
                        uint8_t* conf,
                        int32_t conf_size) {
  MP4FileHandle m4a_file = MP4_INVALID_FILE_HANDLE;

  do {
    MP4FileHandle m4a_file = MP4Create(filename);
    if (m4a_file == MP4_INVALID_FILE_HANDLE) {
      printf("Unable to open mp4 file '%s'\n", filename);
      return -1;
    }

    MP4TrackId track_id = MP4AddAudioTrack(m4a_file, sample_rate, frame_length);
    if (track_id == MP4_INVALID_TRACK_ID) {
      printf("Add audio track failed\n");
      break;
    }

    MP4SetAudioProfileLevel(m4a_file, aot);

    bool ret = MP4SetTrackESConfiguration(m4a_file, track_id, conf, conf_size);
    if (!ret) {
      printf("MP4SetTrackESConfiguration failed\n");
      break;
    }

    m4a_file_ = m4a_file;
    track_id_ = track_id;
  } while (0);

  if (m4a_file_ == MP4_INVALID_FILE_HANDLE &&
      m4a_file != MP4_INVALID_FILE_HANDLE) {
    MP4Close(m4a_file);
    return -1;
  }

  return 0;
}

int32_t M4aWriter::Write(uint8_t* data, int32_t size_in_bytes) {
  bool ret = MP4WriteSample(m4a_file_, track_id_, data, size_in_bytes);
  return ret ? 0 : -1;
}

void M4aWriter::Close() {
  if (m4a_file_ != MP4_INVALID_FILE_HANDLE) {
    MP4Close(m4a_file_);
  }
  m4a_file_ = MP4_INVALID_FILE_HANDLE;
}
