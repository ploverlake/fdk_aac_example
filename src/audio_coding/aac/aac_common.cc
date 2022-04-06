/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "aac_common.h"
#include <stdio.h>
#include <string.h>
#include "aacdecoder_lib.h"
#include "aacenc_lib.h"

struct aot_info aac_enc_aots[] = {
    {AAC_COMMON_AOT_LC, "AAC-LC"},     {AAC_COMMON_AOT_HE, "HE-AAC"},
    {AAC_COMMON_AOT_HEv2, "HE-AACv2"}, {AAC_COMMON_AOT_LD, "AAC-LD"},
    {AAC_COMMON_AOT_ELD, "AAC-ELD"},
};

int32_t aac_enc_aots_size = sizeof(aac_enc_aots) / sizeof(aac_enc_aots[0]);

const char* get_aot_name(int32_t aot, int32_t flag) {
  if (aot == AAC_COMMON_AOT_LC) {
    if (flag & AC_PS_PRESENT) {
      aot = AAC_COMMON_AOT_HEv2;
    } else if (flag & AC_SBR_PRESENT) {
      aot = AAC_COMMON_AOT_HE;
    }
  }

  for (int32_t i = 0; i < aac_enc_aots_size; ++i) {
    if (aac_enc_aots[i].aot == aot) {
      return aac_enc_aots[i].friendly_name;
    }
  }
  return "NA";
}

void print_aac_lib_info() {
  LIB_INFO lib_info[FDK_MODULE_LAST];
  memset(lib_info, 0, sizeof(lib_info));

  int32_t err = aacEncGetLibInfo(lib_info);
  if (err) {
    printf("aacEncGetLibInfo failed, %d\n", err);
    return;
  }

  for (int32_t i = 0; i < FDK_MODULE_LAST; ++i) {
    if (FDK_AACENC == lib_info[i].module_id) {
      printf("%s: %s\n", lib_info[i].title, lib_info[i].versionStr);
      break;
    }
  }

  memset(lib_info, 0, sizeof(lib_info));

  err = aacDecoder_GetLibInfo(lib_info);
  if (err) {
    printf("aacEncGetLibInfo failed, %d\n", err);
    return;
  }

  for (int32_t i = 0; i < FDK_MODULE_LAST; ++i) {
    if (FDK_AACDEC == lib_info[i].module_id) {
      printf("%s: %s\n", lib_info[i].title, lib_info[i].versionStr);
      break;
    }
  }
}
