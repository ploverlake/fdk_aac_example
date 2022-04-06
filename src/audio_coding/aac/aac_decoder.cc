/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "aac_decoder.h"
#include <stdio.h>
#include "aacdecoder_lib.h"

AacDecoder::AacDecoder() : aac_decoder_handle_(nullptr) {}

AacDecoder::~AacDecoder() {
  if (aac_decoder_handle_) {
    Uninit();
  }
}

int32_t AacDecoder::Init(int32_t transport_type) {
  HANDLE_AACDECODER aac_decoder_handle = nullptr;
  AAC_DECODER_ERROR err = AAC_DEC_OK;

  do {
    TRANSPORT_TYPE transmux = TT_UNKNOWN;
    if (transport_type == AAC_TRANSPORT_TYPE_RAW) {
      transmux = TT_MP4_RAW;
    } else if (transport_type == AAC_TRANSPORT_TYPE_ADTS) {
      transmux = TT_MP4_ADTS;
    }
    if (transmux == TT_UNKNOWN) {
      printf("Unsupported transport type, %d\n", transport_type);
      break;
    }

    aac_decoder_handle = aacDecoder_Open(transmux, 1);
    if (aac_decoder_handle == nullptr) {
      printf("Unable to initialize decoder\n");
      break;
    }

    err = aacDecoder_SetParam(aac_decoder_handle, AAC_CONCEAL_METHOD, 0);
    if (err) {
      printf("Unable to set concealment(0)\n");
      break;
    }

    err = aacDecoder_SetParam(aac_decoder_handle, AAC_PCM_LIMITER_ENABLE, 0);
    if (err) {
      printf("Unable to set limiter(0)\n");
      break;
    }

    aac_decoder_handle_ = aac_decoder_handle;
  } while (0);

  if (err || aac_decoder_handle_ == nullptr) {
    if (aac_decoder_handle != nullptr) {
      aacDecoder_Close(aac_decoder_handle);
    }
    aac_decoder_handle_ = nullptr;
  }

  return (aac_decoder_handle_ != nullptr ? 0 : -1);
}

int32_t AacDecoder::GetDecoded(uint8_t* in_buffer,
                               int32_t in_size_bytes,
                               uint8_t* out_buffer,
                               int32_t* out_size_bytes) {
  HANDLE_AACDECODER aac_decoder_handle =
      static_cast<HANDLE_AACDECODER>(aac_decoder_handle_);
  if (!aac_decoder_handle) {
    printf("Invalid aac decoder\n");
    return -1;
  }

  if (!in_buffer || !in_size_bytes || !out_buffer || !out_size_bytes ||
      !*out_size_bytes) {
    printf("Invalid params\n");
    return -1;
  }

  UCHAR* in_buf_ptr = in_buffer;
  UINT in_buf_size = in_size_bytes;
  UINT bytes_valid = in_size_bytes;
  AAC_DECODER_ERROR err = aacDecoder_Fill(aac_decoder_handle, &in_buf_ptr,
                                          &in_buf_size, &bytes_valid);
  if (err) {
    printf("aacDecoder_Fill failed.\n");
    return -1;
  }

  err = aacDecoder_DecodeFrame(aac_decoder_handle, (INT_PCM*)out_buffer,
                               *out_size_bytes, 0);
  if (err == AAC_DEC_NOT_ENOUGH_BITS) {
    *out_size_bytes = 0;
  } else if (err) {
    printf("aacDecoder_DecodeFrame failed %d\n", err);
    return -1;
  }

  return 0;
}

int32_t AacDecoder::GetInfo(AacDecoderInfo* info) {
  HANDLE_AACDECODER aac_decoder_handle =
      static_cast<HANDLE_AACDECODER>(aac_decoder_handle_);
  if (!aac_decoder_handle) {
    printf("Invalid aac decoder\n");
    return -1;
  }

  if (!info) {
    printf("Invalid param\n");
    return -1;
  }

  CStreamInfo* stream_info = aacDecoder_GetStreamInfo(aac_decoder_handle);
  if (stream_info == nullptr) {
    printf("Unable to get stream info\n");
    return -1;
  }

  info->aot = stream_info->aot;
  info->aot_flags = stream_info->flags;
  info->sample_rate = stream_info->sampleRate;
  info->aac_sample_rate = stream_info->aacSampleRate;
  info->channels = stream_info->numChannels;
  info->aac_channels = stream_info->aacNumChannels;
  info->frame_length = stream_info->frameSize;
  info->bitrate = stream_info->bitRate;
  info->output_delay = stream_info->outputDelay;

  return 0;
}

void AacDecoder::Uninit() {
  HANDLE_AACDECODER aac_decoder_handle =
      static_cast<HANDLE_AACDECODER>(aac_decoder_handle_);
  if (aac_decoder_handle) {
    aacDecoder_Close(aac_decoder_handle);
  }
  aac_decoder_handle_ = nullptr;
}
