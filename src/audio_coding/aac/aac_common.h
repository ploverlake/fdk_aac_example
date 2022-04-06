/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#ifndef AAC_COMMON_H_
#define AAC_COMMON_H_

#include <stdint.h>

#define AAC_TRANSPORT_TYPE_RAW 0
#define AAC_TRANSPORT_TYPE_ADTS 1

#define AAC_COMMON_AOT_LC 2
#define AAC_COMMON_AOT_HE 5
#define AAC_COMMON_AOT_HEv2 29
#define AAC_COMMON_AOT_LD 23
#define AAC_COMMON_AOT_ELD 39

#ifdef __cplusplus
extern "C" {
#endif

struct aot_info {
  int32_t aot;
  const char* friendly_name;
};
extern struct aot_info aac_enc_aots[];
extern int32_t aac_enc_aots_size;

const char* get_aot_name(int32_t aot, int32_t flag);
void print_aac_lib_info();

#ifdef __cplusplus
}
#endif

#endif  // AAC_COMMON_H_
