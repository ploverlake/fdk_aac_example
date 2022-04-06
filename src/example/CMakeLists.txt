cmake_minimum_required(VERSION 3.13)
project(fdk_aac_example)

set(EXTRA_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../audio_coding/wav"
    "${CMAKE_CURRENT_SOURCE_DIR}/../audio_coding/aac"
    "${CMAKE_CURRENT_SOURCE_DIR}/../audio_coding/m4a"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/args"
)

set(EXTRA_LINK_LIBS audio_coding args)

# aac_adts_enc
set(AAC_ADTS_ENC_EXAMPLE aac_adts_enc)
set(AAC_ADTS_ENC_SOURCE_FILES aac_adts_enc.cc)
add_executable("${AAC_ADTS_ENC_EXAMPLE}" "${AAC_ADTS_ENC_SOURCE_FILES}")

target_include_directories(
  "${AAC_ADTS_ENC_EXAMPLE}" PRIVATE "${EXTRA_INCLUDE_DIRS}"
)
target_link_libraries("${AAC_ADTS_ENC_EXAMPLE}" PRIVATE "${EXTRA_LINK_LIBS}")

# aac_adts_dec
set(AAC_ADTS_DEC_EXAMPLE aac_adts_dec)
set(AAC_ADTS_DEC_SOURCE_FILES aac_adts_dec.cc)
add_executable("${AAC_ADTS_DEC_EXAMPLE}" "${AAC_ADTS_DEC_SOURCE_FILES}")

target_include_directories(
  "${AAC_ADTS_DEC_EXAMPLE}" PRIVATE "${EXTRA_INCLUDE_DIRS}"
)
target_link_libraries("${AAC_ADTS_DEC_EXAMPLE}" PRIVATE "${EXTRA_LINK_LIBS}")

# aac_m4a_enc
set(AAC_M4A_ENC_EXAMPLE aac_m4a_enc)
set(AAC_M4A_ENC_SOURCE_FILES aac_m4a_enc.cc)
add_executable("${AAC_M4A_ENC_EXAMPLE}" "${AAC_M4A_ENC_SOURCE_FILES}")

target_include_directories(
  "${AAC_M4A_ENC_EXAMPLE}" PRIVATE "${EXTRA_INCLUDE_DIRS}"
)
target_link_libraries("${AAC_M4A_ENC_EXAMPLE}" PRIVATE "${EXTRA_LINK_LIBS}")

add_subdirectory(
  "${CMAKE_CURRENT_SOURCE_DIR}/../audio_coding"
  "${CMAKE_CURRENT_BINARY_DIR}/audio_coding"
)

# args
set(ARGS_BUILD_EXAMPLE
    OFF
    CACHE BOOL "Do not build example" FORCE
)
set(ARGS_BUILD_UNITTESTS
    OFF
    CACHE BOOL "Do not build unittests" FORCE
)
add_subdirectory(
  "${CMAKE_CURRENT_SOURCE_DIR}/../../deps/args"
  "${CMAKE_CURRENT_BINARY_DIR}/deps/args"
)
