/**
 * Copyright (c) 2022, Russell. All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <memory>
#include "aac_adts_reader.h"
#include "aac_common.h"
#include "aac_decoder.h"
#include "wav_writer.h"

#define AAC_ADTS_HEADER_SIZE 7

// 44100, stereo
// UCHAR eld_conf[] = {0xF8, 0xE8, 0x50, 0x00};
// UCHAR *conf[] = {eld_conf};
// UINT conf_size = sizeof(eld_conf);

static void usage(const char* name) {
  printf("%s [-h] [-e enc_delay] [-d] in.aac out.wav\n", name);
  printf("Only support ADTS format\n");
  printf("  -h  Show usage and exit\n");
  printf("  -d  Encode delay(samples/channel) to prune\n");
}

static void PrintDecoderInfo(const char* infile,
                             const char* outfile,
                             int32_t enc_delay,
                             AacDecoderInfo& aac_decoder_info) {
  print_aac_lib_info();
  printf("Input: '%s', %d Hz, %d ch(s), %d bps, %s\n", infile,
         aac_decoder_info.sample_rate, aac_decoder_info.channels,
         aac_decoder_info.bitrate,
         get_aot_name(aac_decoder_info.aot, aac_decoder_info.aot_flags));
  printf("Output: '%s'\n", outfile);
  printf("Frame length: %d samples/channel\n", aac_decoder_info.frame_length);
  printf("Output delay: %u samples/channel\n", aac_decoder_info.output_delay);
  printf("Aac sample rate: %d, aac channels: %d\n",
         aac_decoder_info.aac_sample_rate, aac_decoder_info.aac_channels);
  printf("Presupposed enc delay to prune: %d samples/channel\n", enc_delay);
}

static int32_t DecodeAacAdts(const char* infile,
                             const char* outfile,
                             const int32_t enc_delay) {
  auto aac_adts_reader = std::make_unique<AacAdtsReader>();
  int32_t ret = aac_adts_reader->Open(infile);
  if (ret) {
    printf("Open aac adts file failed, %s\n", infile);
    return -1;
  }

  auto aac_decoder = std::make_unique<AacDecoder>();
  ret = aac_decoder->Init(AAC_TRANSPORT_TYPE_ADTS);
  if (ret) {
    printf("Init aac adts decoder failed\n");
    return -1;
  }

  auto wav_writer = std::make_unique<WavWriter>();

  AacDecoderInfo aac_decoder_info;
  bool got_stream_info = false;

  int32_t total_delay_in_bytes = 0;
  int32_t pcm_frame_size_in_bytes = 0;

  while (1) {
    uint8_t in_buf[8192];
    int32_t in_buf_size = sizeof(in_buf);
    ret = aac_adts_reader->ReadOneFrame(in_buf, &in_buf_size);
    if (ret) {
      break;
    }

    uint8_t out_buf[8 * 2048];
    int32_t out_buf_size = sizeof(out_buf);
    ret = aac_decoder->GetDecoded(in_buf, in_buf_size, out_buf, &out_buf_size);
    if (ret) {
      printf("Decode error\n");
      break;
    } else if (out_buf_size == 0) {
      // not enough bits
      printf("hhhh\n");
      continue;
    }

    if (!got_stream_info) {
      got_stream_info = true;
      ret = aac_decoder->GetInfo(&aac_decoder_info);
      if (ret) {
        printf("Get info of aac decoder failed\n");
        break;
      }
      PrintDecoderInfo(infile, outfile, enc_delay, aac_decoder_info);

      ret = wav_writer->Open(outfile, aac_decoder_info.sample_rate,
                             aac_decoder_info.channels, 16);
      if (ret) {
        printf("Open wav file failed, %s\n", outfile);
        break;
      }

      total_delay_in_bytes = enc_delay * aac_decoder_info.channels * 2;
      pcm_frame_size_in_bytes =
          aac_decoder_info.frame_length * aac_decoder_info.channels * 2;
    }

    if (total_delay_in_bytes >= pcm_frame_size_in_bytes) {
      total_delay_in_bytes -= pcm_frame_size_in_bytes;
      continue;
    }

    uint8_t* write_buf = out_buf;
    int32_t write_size = pcm_frame_size_in_bytes;
    if (total_delay_in_bytes > 0) {
      write_buf += total_delay_in_bytes;
      write_size = pcm_frame_size_in_bytes - total_delay_in_bytes;
      total_delay_in_bytes = 0;
    }

    wav_writer->Write(write_buf, write_size);
  }

  return 0;
}

int main(int argc, char* argv[]) {
  int32_t enc_delay = 0;
  int32_t ch;
  while ((ch = getopt(argc, argv, "hd:")) != -1) {
    switch (ch) {
      case 'd':
        enc_delay = atoi(optarg);
        break;
      case 'h':
      case '?':
      default:
        usage(argv[0]);
        return -1;
    }
  }
  if (argc - optind < 2) {
    usage(argv[0]);
    return -1;
  }
  const char* infile = argv[optind];
  const char* outfile = argv[optind + 1];

  DecodeAacAdts(infile, outfile, enc_delay);
  return 0;
}
