#!/usr/bin/bash

source ../bash/common.sh

rm -rf CMake*
cmake -G "Visual Studio 14 2015 Win64" \
  -DGTEST_INCLUDE_PATH=${GTEST_ROOT}/include \
  -DGTEST_LIBRARY_PATH=${GTEST_ROOT}/build/Release \
  -DCMAKE_INSTALL_PREFIX:PATH=. -DVERBOSE=1 -DBUILD_DOCUMENTATION=1 .. 
