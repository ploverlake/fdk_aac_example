cmake_minimum_required(VERSION 3.13)
project(fdk_aac_example)

set(EXTRA_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/wav"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/aac"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/m4a"
    "${CMAKE_CURRENT_SOURCE_DIR}/../deps/args"
)

set(EXTRA_LINK_LIBS audio_coding args fdk-aac mp4v2)

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
