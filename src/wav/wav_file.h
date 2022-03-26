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

#ifndef WAV_FILE_H_
#define WAV_FILE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* wav_read_open(const char* filename);
void wav_read_close(void* obj);
int32_t wav_get_header(void* obj,
                       int32_t* format,
                       int32_t* sample_rate,
                       int32_t* channels,
                       int32_t* bits_per_sample,
                       int32_t* data_length);
int32_t wav_read_data(void* obj, void* data, int32_t length);

void* wav_write_open(const char* filename,
                     int32_t sample_rate,
                     int32_t channels,
                     int32_t bits_per_sample);
void wav_write_close(void* obj);
int32_t wav_write_data(void* obj, void* data, int32_t length);

#ifdef __cplusplus
}
#endif

#endif  // WAV_FILE_H_
