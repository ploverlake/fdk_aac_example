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
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/fdk-aac")

# mp4v2
set(BUILD_SHARED
    OFF
    CACHE BOOL "Build libmp4v2 as a static library" FORCE
)
set(BUILD_UTILS
    OFF
    CACHE BOOL "Do not build MP4v2 auxiliary tools" FORCE
)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/mp4v2")

# args
set(ARGS_BUILD_EXAMPLE
    OFF
    CACHE BOOL "Do not build example" FORCE
)
set(ARGS_BUILD_UNITTESTS
    OFF
    CACHE BOOL "Do not build unittests" FORCE
)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/args")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/example")
