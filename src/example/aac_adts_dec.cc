/**
 * Copyright (c) 2022, Russell. All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <string>
#include "aac_adts_reader.h"
#include "aac_common.h"
#include "aac_decoder.h"
#include "args.hxx"
#include "wav_writer.h"

#define AAC_ADTS_HEADER_SIZE 7

static void PrintDecoderInfo(const char* infile,
                             const char* outfile,
                             int32_t encoder_delay,
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
  printf("Presupposed encoder delay to prune: %d samples/channel\n",
         encoder_delay);
}

static int32_t DecodeAacAdts(const char* infile,
                             const char* outfile,
                             const int32_t encoder_delay) {
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
      continue;
    }

    if (!got_stream_info) {
      got_stream_info = true;
      ret = aac_decoder->GetInfo(&aac_decoder_info);
      if (ret) {
        printf("Get info of aac decoder failed\n");
        break;
      }
      PrintDecoderInfo(infile, outfile, encoder_delay, aac_decoder_info);

      ret = wav_writer->Open(outfile, aac_decoder_info.sample_rate,
                             aac_decoder_info.channels, 16);
      if (ret) {
        printf("Open wav file failed, %s\n", outfile);
        break;
      }

      total_delay_in_bytes = encoder_delay * aac_decoder_info.channels * 2;
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
  args::ArgumentParser parser(
      "Decode AAC with ADTS format to WAV file.\nOnly support 1 or 2 "
      "channel(s)");
  parser.helpParams.progindent = 0;
  parser.helpParams.addDefault = true;
  parser.helpParams.addChoices = true;
  parser.helpParams.useValueNameOnce = true;

  args::Positional<std::string> aac_file(parser, "Input", "AAC file",
                                         args::Options::Required);
  args::Positional<std::string> wav_file(parser, "Output", "WAV file",
                                         args::Options::Required);
  args::HelpFlag help(parser, "help", "Show usage and exit", {'h', "help"});
  args::ValueFlag<int32_t> encoder_delay(
      parser, "delay", "Encoder delay(samples/channel) to prune",
      {'d', "delay"}, 0);

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

  if (aac_file.GetError() != args::Error::None) {
    std::cout << aac_file.GetErrorMsg() << std::endl;
    return -1;
  } else if (wav_file.GetError() != args::Error::None) {
    std::cout << wav_file.GetErrorMsg() << std::endl;
    return -1;
  }

  DecodeAacAdts(aac_file.Get().c_str(), wav_file.Get().c_str(),
                encoder_delay.Get());
  return 0;
}
