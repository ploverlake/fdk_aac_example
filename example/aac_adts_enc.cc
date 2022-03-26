/**
 * Copyright (c) 2022, Russell. All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <memory>
#include "aac_encoder.h"
#include "wav_reader.h"

static void Usage(const char* name) {
  printf("%s [-h] [-t aot] [-r bitrate] in.wav out.aac\n", name);
  printf("Only support 1 or 2 channel(s)\n");
  printf("  -h  Show usage and exit\n");
  printf("  -t  Encode AOT(LC: %d, HE: %d, HEv2: %d)\n", AAC_COMMON_AOT_LC,
         AAC_COMMON_AOT_HE, AAC_COMMON_AOT_HEv2);
  printf("  -r  Encode bitrate\n");
}

static void PrintEncoderInfo(const char* infile,
                             const char* outfile,
                             int32_t aot,
                             int32_t bitrate,
                             WavFileInfo& wav_file_info,
                             AacEncoderInfo& aac_encoder_info) {
  print_aac_lib_info();
  printf("Input: '%s', %d Hz, %d ch(s), %d bits/sample\n", infile,
         wav_file_info.sample_rate, wav_file_info.channels,
         wav_file_info.bits_per_sample);
  printf("Output: '%s', %s, %d bps\n", outfile, get_aot_name(aot, 0), bitrate);
  printf("Frame length: %u samples/channel\n", aac_encoder_info.frame_length);
  printf("Delay: %u samples/channel\n", aac_encoder_info.delay);
  printf("Delay core: %u samples/channel\n", aac_encoder_info.delay_core);
  printf("Conf: {");
  for (int32_t i = 0; i < aac_encoder_info.conf_size; ++i) {
    printf("0x%02X", static_cast<int32_t>(aac_encoder_info.conf[i]));
    if (i < aac_encoder_info.conf_size - 1) {
      printf(", ");
    }
  }
  printf("}\n");
}

static bool IsAotValid(int32_t aot) {
  if (aot == AAC_COMMON_AOT_LC || aot == AAC_COMMON_AOT_HE ||
      aot == AAC_COMMON_AOT_HEv2) {
    return true;
  }
  return false;
}

static int32_t EncodeAacAdts(const char* infile,
                             const char* outfile,
                             int32_t aot,
                             int32_t bitrate) {
  auto wav_reader = std::make_unique<WavReader>();
  int32_t ret = wav_reader->Open(infile);
  if (ret) {
    printf("Open wav file failed, %s\n", infile);
    return -1;
  }

  WavFileInfo wav_file_info = {0};
  ret = wav_reader->GetInfo(&wav_file_info);
  if (ret) {
    printf("Get info of wav file failed\n");
    return -1;
  }

  std::unique_ptr<FILE, decltype(&fclose)> out(fopen(outfile, "wb"), &fclose);
  if (out == nullptr) {
    printf("Open output file failed, %s\n", outfile);
    return -1;
  }

  auto aac_encoder = std::make_unique<AacEncoder>();
  ret =
      aac_encoder->Init(AAC_TRANSPORT_TYPE_ADTS, aot, wav_file_info.sample_rate,
                        wav_file_info.channels, bitrate);
  if (ret) {
    printf("Init aac adts encoder failed\n");
    return -1;
  }

  AacEncoderInfo aac_encoder_info;
  ret = aac_encoder->GetInfo(&aac_encoder_info);
  if (ret) {
    printf("Get info of aac encoder failed\n");
    return -1;
  }

  PrintEncoderInfo(infile, outfile, aot, bitrate, wav_file_info,
                   aac_encoder_info);

  int32_t frame_size_in_bytes =
      wav_file_info.channels * 2 * aac_encoder_info.frame_length;

  auto input_buf = std::make_unique<uint8_t[]>(frame_size_in_bytes);
  auto output_buf = std::make_unique<uint8_t[]>(frame_size_in_bytes);

  while (1) {
    int32_t read_bytes = wav_reader->Read(input_buf.get(), frame_size_in_bytes);
    if (read_bytes < 0) {
      printf("Read wav file failed\n");
      break;
    }

    int32_t out_size_bytes = frame_size_in_bytes;
    int32_t ret = aac_encoder->GetEncoded(input_buf.get(), read_bytes,
                                          output_buf.get(), &out_size_bytes);
    if (ret) {
      break;
    } else if (out_size_bytes == 0) {
      continue;
    }
    fwrite(output_buf.get(), 1, out_size_bytes, out.get());
  }

  return 0;
}

int main(int argc, char* argv[]) {
  int32_t aot = -1;
  int32_t bitrate = -1;
  int32_t ch;
  while ((ch = getopt(argc, argv, "ht:r:")) != -1) {
    switch (ch) {
      case 't':
        aot = atoi(optarg);
        break;
      case 'r':
        bitrate = atoi(optarg);
        break;
      case 'h':
      case '?':
      default:
        Usage(argv[0]);
        return -1;
    }
  }
  if (argc - optind < 2) {
    Usage(argv[0]);
    return -1;
  }
  const char* infile = argv[optind];
  const char* outfile = argv[optind + 1];

  if (!IsAotValid(aot)) {
    printf("Invalid AOT: %d\n", aot);
    Usage(argv[0]);
    return -1;
  }

  if (bitrate <= 0) {
    printf("Invalid bitrate: %d\n", bitrate);
    Usage(argv[0]);
    return -1;
  }

  EncodeAacAdts(infile, outfile, aot, bitrate);
  return 0;
}
