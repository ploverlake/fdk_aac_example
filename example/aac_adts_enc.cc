/**
 * Copyright (c) 2022, Russell. All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <string>
#include "aac_encoder.h"
#include "args.hxx"
#include "wav_reader.h"

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
  args::ArgumentParser parser(
      "Encode AAC with ADTS format.\nOnly support 1 or 2 channel(s)");
  parser.helpParams.progindent = 0;
  parser.helpParams.addDefault = true;
  parser.helpParams.addChoices = true;
  parser.helpParams.useValueNameOnce = true;

  args::Positional<std::string> wav_file(parser, "Input", "WAV file",
                                         args::Options::Required);
  args::Positional<std::string> aac_file(parser, "Output", "AAC file",
                                         args::Options::Required);
  args::HelpFlag help(parser, "help", "Show usage and exit", {'h', "help"});

  args::MapFlag<std::string, int> aot(
      parser, "AOT", "Audio Object Type", {'a', "aot"},
      {{std::to_string(AAC_COMMON_AOT_LC), AAC_COMMON_AOT_LC},
       {std::to_string(AAC_COMMON_AOT_HE), AAC_COMMON_AOT_HE},
       {std::to_string(AAC_COMMON_AOT_HEv2), AAC_COMMON_AOT_HEv2}},
      AAC_COMMON_AOT_LC);
  aot.HelpChoices({std::to_string(AAC_COMMON_AOT_LC) + "(LC)",
                   std::to_string(AAC_COMMON_AOT_HE) + "(HE)",
                   std::to_string(AAC_COMMON_AOT_HEv2) + "(HEv2)"});
  aot.HelpDefault(std::to_string(AAC_COMMON_AOT_LC));

  args::ValueFlag<int32_t> bitrate(parser, "bitrate", "Encode bitrate(bps)",
                                   {'b', "bitrate"}, 64000);

  bool ret = parser.ParseCLI(argc, argv);
  if (!ret) {
    if (parser.GetError() != args::Error::None) {
      std::cout << parser.GetErrorMsg() << std::endl << std::endl;
    }
    std::cout << parser.Help();
    return -1;
  } else if (help.Get()) {
    std::cout << parser.Help();
    return 0;
  }

  if (wav_file.GetError() != args::Error::None) {
    std::cout << wav_file.GetErrorMsg() << std::endl;
    return -1;
  } else if (aac_file.GetError() != args::Error::None) {
    std::cout << aac_file.GetErrorMsg() << std::endl;
    return -1;
  } else if (aot.GetError() != args::Error::None) {
    std::cout << aot.GetErrorMsg() << std::endl;
    return -1;
  }

  EncodeAacAdts(wav_file.Get().c_str(), aac_file.Get().c_str(), aot.Get(),
                bitrate.Get());
  return 0;
}
