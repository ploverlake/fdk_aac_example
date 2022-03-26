cmake_minimum_required(VERSION 3.13)
project(audio_coding)

get_target_property(mp4v2_INCLUDE_DIRECTORIES mp4v2 INCLUDE_DIRECTORIES)

set(AAC_SOURCE_FILES
    aac/aac_adts_reader.cc
    aac/aac_adts_reader.h
    aac/aac_common.cc
    aac/aac_common.h
    aac/aac_encoder.h
    aac/aac_encoder.cc
    aac/aac_decoder.h
    aac/aac_decoder.cc
)

set(M4A_SOURCE_FILES m4a/m4a_writer.cc m4a/m4a_writer.h)

# cmake-format: off
set(WAV_SOURCE_FILES
    wav/wav_file.cc
    wav/wav_file.h
    wav/wav_reader.cc
    wav/wav_reader.h
    wav/wav_writer.cc
    wav/wav_writer.h
)
# cmake-format: on

set(SOURCE_FILES "${AAC_SOURCE_FILES}" "${M4A_SOURCE_FILES}"
                 "${WAV_SOURCE_FILES}"
)

add_library("${PROJECT_NAME}" STATIC "${SOURCE_FILES}")

target_include_directories(
  "${PROJECT_NAME}"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/aac"
          "${CMAKE_CURRENT_SOURCE_DIR}/m4a"
          "${CMAKE_CURRENT_SOURCE_DIR}/wav"
          "${CMAKE_CURRENT_SOURCE_DIR}/../deps/fdk-aac/libSYS/include"
          "${CMAKE_CURRENT_SOURCE_DIR}/../deps/fdk-aac/libAACenc/include"
          "${CMAKE_CURRENT_SOURCE_DIR}/../deps/fdk-aac/libAACdec/include"
          "${mp4v2_INCLUDE_DIRECTORIES}"
)
