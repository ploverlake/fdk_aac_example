cmake_minimum_required(VERSION 3.13)
project(fdk_aac_example)

if (POLICY CMP0115)
  # Suppress warning from mp4v2
  set(CMAKE_POLICY_DEFAULT_CMP0115 OLD)
endif ()

# C standard requirements.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Available configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Global compiling flags
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -DARGS_NOEXCEPT=1 -Wall -fvisibility=hidden"
)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -DARGS_NOEXCEPT=1 -Wall -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden"
)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG=1 -g -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG=1 -g -O2")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/example")
