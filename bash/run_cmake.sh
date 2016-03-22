#!/usr/bin/bash

source ../bash/common.sh

rm -rf CMake*
cmake -G "Visual Studio 14 2015 Win64" \
  -DCMAKE_INSTALL_PREFIX:PATH=. -DVERBOSE=1 -DBUILD_DOCUMENTATION=1 .. 
