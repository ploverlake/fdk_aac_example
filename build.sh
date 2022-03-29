#!/bin/bash
# shellcheck disable=SC2164
PROG_DIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1 && pwd -P)"

MAKE_PROG="$(command which make)"
if [ ! -x "$MAKE_PROG" ]; then
  echo "Error: no make"
  exit 1
fi

CMAKE_PROG="$(command which cmake)"
if [ ! -x "$CMAKE_PROG" ]; then
  echo "Error: no cmake"
  exit 1
fi

cd "$PROG_DIR"
cmake_cmd="$CMAKE_PROG -H. -Bbuild"
eval "$cmake_cmd"
if [ $? -ne 0 ]; then
  echo "Error: cmake failed"
  exit 1
fi

cd build
$MAKE_PROG VERBOSE=1
