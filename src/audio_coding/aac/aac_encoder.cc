/**
 * Copyright (c) 2022 Russell. All rights reserved.
 */

#include "aac_encoder.h"
#include <stdio.h>
#include <string.h>
#include "aacenc_lib.h"

AacEncoder::AacEncoder() : aac_encoder_handle_(nullptr) {}

AacEncoder::~AacEncoder() {
  if (aac_encoder_handle_) {
    Uninit();
  }
}

int32_t AacEncoder::Init(int32_t transport_type,
                         int32_t aot,
                         int32_t sample_rate,
                         int32_t channels,
                         int32_t bitrate) {
  HANDLE_AACENCODER aac_encoder_handle = nullptr;
  AACENC_ERROR err = AACENC_OK;

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

    int32_t mode = ChannelMode(channels);
    if (mode == MODE_INVALID) {
      printf("Unsupported channels %d\n", channels);
      break;
    }

    err = aacEncOpen(&aac_encoder_handle, 0, channels);
    if (err) {
      printf("Unable to open encoder, %d\n", err);
      break;
    }

    err = aacEncoder_SetParam(aac_encoder_handle, AACENC_AOT, aot);
    if (err) {
      printf("Unable to set the AOT, %d\n", err);
      break;
    }

    if (aot == AAC_COMMON_AOT_ELD) {
      err = aacEncoder_SetParam(aac_encoder_handle, AACENC_SBR_MODE, 1);
      if (err) {
        printf("Unable to set SBR mode for ELD, %d\n", err);
        break;
      }
    }

    err =
        aacEncoder_SetParam(aac_encoder_handle, AACENC_SAMPLERATE, sample_rate);
    if (err) {
      printf("Unable to set the sample rate, %d\n", err);
      break;
    }

    err = aacEncoder_SetParam(aac_encoder_handle, AACENC_CHANNELMODE, mode);
    if (err) {
      printf("Unable to set the channel mode, %d\n", err);
      break;
    }

    err = aacEncoder_SetParam(aac_encoder_handle, AACENC_CHANNELORDER, 1);
    if (err) {
      printf("Unable to set the channel order, %d\n", err);
      break;
    }

    err = aacEncoder_SetParam(aac_encoder_handle, AACENC_BITRATE, bitrate);
    if (err) {
      printf("Unable to set the bitrate, %d\n", err);
      break;
    }

    err = aacEncoder_SetParam(aac_encoder_handle, AACENC_TRANSMUX, transmux);
    if (err) {
      printf("Unable to set the transport type, %d\n", err);
      break;
    }

    err = aacEncoder_SetParam(aac_encoder_handle, AACENC_AFTERBURNER, 1);
    if (err) {
      printf("Unable to enable afterburner, %d\n", err);
      break;
    }

    err = aacEncEncode(aac_encoder_handle, nullptr, nullptr, nullptr, nullptr);
    if (err) {
      printf("Unable to initialize encoder, %d\n", err);
      break;
    }

    aac_encoder_handle_ = static_cast<void*>(aac_encoder_handle);
  } while (0);

  if (err || aac_encoder_handle_ == nullptr) {
    if (aac_encoder_handle != nullptr) {
      aacEncClose(&aac_encoder_handle);
    }
    aac_encoder_handle_ = nullptr;
  }

  return (aac_encoder_handle_ != nullptr ? 0 : -1);
}

int32_t AacEncoder::GetInfo(AacEncoderInfo* info) {
  HANDLE_AACENCODER aac_encoder_handle =
      static_cast<HANDLE_AACENCODER>(aac_encoder_handle_);
  if (!aac_encoder_handle) {
    printf("Invalid aac encoder\n");
    return -1;
  }

  if (!info) {
    printf("Invalid param\n");
    return -1;
  }

  AACENC_InfoStruct enc_info = {0};
  AACENC_ERROR err = aacEncInfo(aac_encoder_handle, &enc_info);
  if (err) {
    printf("Unable to get encoder info\n");
    return -1;
  }

  info->frame_length = enc_info.frameLength;
  info->delay = enc_info.nDelay;
  info->delay_core = enc_info.nDelayCore;
  info->conf_size = enc_info.confSize;
  memcpy(info->conf, enc_info.confBuf, enc_info.confSize);

  return 0;
}

int32_t AacEncoder::GetEncoded(uint8_t* in_buffer,
                               int32_t in_size_bytes,
                               uint8_t* out_buffer,
                               int32_t* out_size_bytes) {
  HANDLE_AACENCODER aac_encoder_handle =
      static_cast<HANDLE_AACENCODER>(aac_encoder_handle_);
  if (!aac_encoder_handle) {
    printf("Invalid aac encoder\n");
    return -1;
  }

  if (!in_buffer || !out_buffer || !out_size_bytes || !*out_size_bytes) {
    printf("Invalid params\n");
    return -1;
  }

  int32_t in_identifier = IN_AUDIO_DATA;
  int32_t in_elem_size = 2;

  AACENC_BufDesc in_buf = {0};
  in_buf.numBufs = 1;
  in_buf.bufs = (void**)&in_buffer;
  in_buf.bufferIdentifiers = &in_identifier;
  in_buf.bufSizes = &in_size_bytes;
  in_buf.bufElSizes = &in_elem_size;

  int32_t out_identifier = OUT_BITSTREAM_DATA;
  int32_t out_size = *out_size_bytes;
  int32_t out_elem_size = 1;

  AACENC_BufDesc out_buf = {0};
  out_buf.numBufs = 1;
  out_buf.bufs = (void**)&out_buffer;
  out_buf.bufferIdentifiers = &out_identifier;
  out_buf.bufSizes = &out_size;
  out_buf.bufElSizes = &out_elem_size;

  AACENC_InArgs in_args = {0};
  in_args.numInSamples = (in_size_bytes <= 0) ? -1 : (in_size_bytes / 2);

  AACENC_OutArgs out_args = {0};
  AACENC_ERROR err =
      aacEncEncode(aac_encoder_handle, &in_buf, &out_buf, &in_args, &out_args);
  if (err != AACENC_OK) {
    if (err == AACENC_ENCODE_EOF) {
      printf("EOF\n");
    } else {
      printf("Encoding failed, %d\n", err);
    }
    return -1;
  }

  *out_size_bytes = out_args.numOutBytes;
  return 0;
}

void AacEncoder::Uninit() {
  HANDLE_AACENCODER aac_encoder_handle =
      static_cast<HANDLE_AACENCODER>(aac_encoder_handle_);
  if (aac_encoder_handle) {
    aacEncClose(&aac_encoder_handle);
  }
  aac_encoder_handle_ = nullptr;
}

int32_t AacEncoder::ChannelMode(int32_t channels) {
  CHANNEL_MODE mode = MODE_INVALID;
  switch (channels) {
    case 1:
      mode = MODE_1;
      break;
    case 2:
      mode = MODE_2;
      break;
    default:
      break;
  }
  return static_cast<int32_t>(mode);
}
