#!/usr/bin/bash

CMAKE_PATH="`cygpath -p 'C:\Program Files (x86)\CMake\bin\'`"
GS_PATH="`cygpath -p 'C:\Program Files (x86)\gs\gs9.18\bin'`"
DOXYGEN_PATH="`cygpath -p 'C:\Program Files\Doxygen\bin\'`"
PATH="$CMAKE_PATH:$DOXYGEN_PATH:$GS_PATH:$PATH"; export PATH
