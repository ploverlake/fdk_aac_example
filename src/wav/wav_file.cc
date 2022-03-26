/**
 * Copyright (c) 2022, Russell. All rights reserved.
 */

/* ------------------------------------------------------------------
 * Copyright (C) 2009 Martin Storsjo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#include "wav_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WAV_FILE_HEADER_SIZE 44
#define TAG(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

struct wav_handler {
  FILE* wav;
  int32_t format;
  int32_t sample_rate;
  int32_t bits_per_sample;
  int32_t channels;
  int32_t byte_rate;
  int32_t block_align;
  int32_t data_length;
};

static uint32_t read_tag(struct wav_handler* wh) {
  uint32_t tag = 0;
  tag = (tag << 8) | fgetc(wh->wav);
  tag = (tag << 8) | fgetc(wh->wav);
  tag = (tag << 8) | fgetc(wh->wav);
  tag = (tag << 8) | fgetc(wh->wav);
  return tag;
}

static uint32_t read_uint32(struct wav_handler* wh) {
  uint32_t value = 0;
  value |= fgetc(wh->wav) << 0;
  value |= fgetc(wh->wav) << 8;
  value |= fgetc(wh->wav) << 16;
  value |= fgetc(wh->wav) << 24;
  return value;
}

static uint16_t read_uint16(struct wav_handler* wh) {
  uint16_t value = 0;
  value |= fgetc(wh->wav) << 0;
  value |= fgetc(wh->wav) << 8;
  return value;
}

static void write_tag(struct wav_handler* wh, uint32_t tag) {
  fputc((tag & 0xFF000000) >> 24, wh->wav);
  fputc((tag & 0xFF0000) >> 16, wh->wav);
  fputc((tag & 0xFF00) >> 8, wh->wav);
  fputc(tag & 0xFF, wh->wav);
}

static void write_uint32(struct wav_handler* wh, uint32_t value) {
  fputc(value & 0xFF, wh->wav);
  value >>= 8;
  fputc(value & 0xFF, wh->wav);
  value >>= 8;
  fputc(value & 0xFF, wh->wav);
  value >>= 8;
  fputc(value & 0xFF, wh->wav);
  value >>= 8;
}

static void write_uint16(struct wav_handler* wh, uint16_t value) {
  fputc(value & 0xFF, wh->wav);
  value >>= 8;
  fputc(value & 0xFF, wh->wav);
  value >>= 8;
}

static void wav_write_header(struct wav_handler* wh) {
  int32_t chunk_size = wh->data_length + WAV_FILE_HEADER_SIZE - 8;
  int32_t block_align = (wh->bits_per_sample >> 3) * wh->channels;
  int32_t avg_bytes_per_sec = wh->sample_rate * block_align;
  uint16_t format_code = (wh->bits_per_sample == 16 ? 1 : 3);

  fseek(wh->wav, 0, SEEK_SET);

  write_tag(wh, TAG('R', 'I', 'F', 'F'));
  write_uint32(wh, chunk_size);
  write_tag(wh, TAG('W', 'A', 'V', 'E'));
  write_tag(wh, TAG('f', 'm', 't', ' '));
  write_uint32(wh, 16);
  write_uint16(wh, format_code);
  write_uint16(wh, wh->channels);
  write_uint32(wh, wh->sample_rate);
  write_uint32(wh, avg_bytes_per_sec);
  write_uint16(wh, block_align);
  write_uint16(wh, wh->bits_per_sample);
  write_tag(wh, TAG('d', 'a', 't', 'a'));
  write_uint32(wh, wh->data_length);
}

void* wav_read_open(const char* filename) {
  struct wav_handler* wh = (struct wav_handler*)malloc(sizeof(*wh));
  if (wh == NULL) {
    return NULL;
  }
  memset(wh, 0, sizeof(*wh));

  wh->wav = fopen(filename, "rb");
  if (wh->wav == NULL) {
    free(wh);
    return NULL;
  }

  long data_pos = 0;
  while (1) {
    uint32_t tag = read_tag(wh);
    if (feof(wh->wav)) {
      break;
    }

    uint32_t length = read_uint32(wh);
    if (tag != TAG('R', 'I', 'F', 'F') || length < 4) {
      fseek(wh->wav, length, SEEK_CUR);
      continue;
    }

    uint32_t tag2 = read_tag(wh);
    length -= 4;
    if (tag2 != TAG('W', 'A', 'V', 'E')) {
      fseek(wh->wav, length, SEEK_CUR);
      continue;
    }

    // RIFF chunk found, iterate through it
    while (length >= 8) {
      uint32_t subtag, sublength;
      subtag = read_tag(wh);
      if (feof(wh->wav)) {
        break;
      }

      sublength = read_uint32(wh);
      length -= 8;
      if (length < sublength) {
        break;
      }

      if (subtag == TAG('f', 'm', 't', ' ')) {
        if (sublength < 16) {
          // Insufficient data for 'fmt '
          break;
        }
        wh->format = read_uint16(wh);
        wh->channels = read_uint16(wh);
        wh->sample_rate = read_uint32(wh);
        wh->byte_rate = read_uint32(wh);
        wh->block_align = read_uint16(wh);
        wh->bits_per_sample = read_uint16(wh);
        fseek(wh->wav, sublength - 16, SEEK_CUR);
      } else if (subtag == TAG('d', 'a', 't', 'a')) {
        data_pos = ftell(wh->wav);
        wh->data_length = sublength;
        fseek(wh->wav, sublength, SEEK_CUR);
      } else {
        fseek(wh->wav, sublength, SEEK_CUR);
      }

      length -= sublength;
    }

    if (length > 0) {
      // Bad chunk?
      fseek(wh->wav, length, SEEK_CUR);
    }
  }

  fseek(wh->wav, data_pos, SEEK_SET);
  return wh;
}

void wav_read_close(void* obj) {
  struct wav_handler* wh = (struct wav_handler*)obj;
  if (wh != NULL) {
    if (wh->wav != NULL) {
      fclose(wh->wav);
    }
    free(wh);
  }
}

int32_t wav_get_header(void* obj,
                       int32_t* format,
                       int32_t* sample_rate,
                       int32_t* channels,
                       int32_t* bits_per_sample,
                       int32_t* data_length) {
  struct wav_handler* wh = (struct wav_handler*)obj;
  if (wh == NULL) {
    return -1;
  }

  if (format) {
    *format = wh->format;
  }
  if (sample_rate) {
    *sample_rate = wh->sample_rate;
  }
  if (channels) {
    *channels = wh->channels;
  }
  if (bits_per_sample) {
    *bits_per_sample = wh->bits_per_sample;
  }
  if (data_length) {
    *data_length = wh->data_length;
  }

  if (wh->format && wh->sample_rate && wh->channels && wh->bits_per_sample) {
    return 0;
  }
  return -1;
}

int32_t wav_read_data(void* obj, void* data, int32_t length) {
  struct wav_handler* wh = (struct wav_handler*)obj;
  if (wh == NULL || wh->wav == NULL) {
    return -1;
  }
  if (length > wh->data_length) {
    length = wh->data_length;
  }
  int32_t n = (int32_t)fread(data, 1, length, wh->wav);
  wh->data_length -= n;
  return n;
}

void* wav_write_open(const char* filename,
                     int32_t sample_rate,
                     int32_t channels,
                     int32_t bits_per_sample) {
  if (bits_per_sample != 16 && bits_per_sample != 32) {
    return NULL;
  }

  struct wav_handler* wh = (struct wav_handler*)malloc(sizeof(*wh));
  if (wh == NULL) {
    return NULL;
  }
  memset(wh, 0, sizeof(*wh));

  wh->wav = fopen(filename, "wb");
  if (wh->wav == NULL) {
    free(wh);
    return NULL;
  }

  wh->sample_rate = sample_rate;
  wh->channels = channels;
  wh->bits_per_sample = bits_per_sample;

  wav_write_header(wh);
  return wh;
}

void wav_write_close(void* obj) {
  struct wav_handler* wh = (struct wav_handler*)obj;
  if (wh != NULL) {
    if (wh->wav != NULL) {
      wav_write_header(wh);
      fclose(wh->wav);
    }
    free(wh);
  }
}

int32_t wav_write_data(void* obj, void* data, int32_t length) {
  struct wav_handler* wh = (struct wav_handler*)obj;
  if (wh == NULL || wh->wav == NULL) {
    return -1;
  }

  int32_t n = (int32_t)fwrite(data, 1, length, wh->wav);
  wh->data_length += n;
  return n;
}
