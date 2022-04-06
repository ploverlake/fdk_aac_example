cmake_minimum_required(VERSION 3.13)
project(audio_coding)

# cmake-format: off
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

set(M4A_SOURCE_FILES
    m4a/m4a_writer.cc
    m4a/m4a_writer.h
)

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

# fdk-aac
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "Build fdk-aac as a static library" FORCE
)
set(BUILD_PROGRAMS
    OFF
    CACHE BOOL "Do not build aac-enc utility" FORCE
)
set(FDK_AAC_INSTALL_CMAKE_CONFIG_MODULE
    OFF
    CACHE BOOL "Do not install CMake package configuration file" FORCE
)
set(FDK_AAC_INSTALL_PKGCONFIG_MODULE
    OFF
    CACHE BOOL "Do not install pkg-config .pc file" FORCE
)
add_subdirectory(
  "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/fdk-aac"
  "${CMAKE_CURRENT_BINARY_DIR}/deps/fdk-aac"
)

# mp4v2
set(BUILD_SHARED
    OFF
    CACHE BOOL "Build libmp4v2 as a static library" FORCE
)
set(BUILD_UTILS
    OFF
    CACHE BOOL "Do not build MP4v2 auxiliary tools" FORCE
)
add_subdirectory(
  "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/mp4v2"
  "${CMAKE_CURRENT_BINARY_DIR}/deps/mp4v2"
)

# audio_coding
get_target_property(mp4v2_INCLUDE_DIRECTORIES mp4v2 INCLUDE_DIRECTORIES)
target_include_directories(
  "${PROJECT_NAME}"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/aac"
          "${CMAKE_CURRENT_SOURCE_DIR}/m4a"
          "${CMAKE_CURRENT_SOURCE_DIR}/wav"
          "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/fdk-aac/libSYS/include"
          "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/fdk-aac/libAACenc/include"
          "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/fdk-aac/libAACdec/include"
          "${mp4v2_INCLUDE_DIRECTORIES}"
)

target_link_libraries("${PROJECT_NAME}" PUBLIC fdk-aac mp4v2)
