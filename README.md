# pretty-good-sum

Pretty good sum type library implementing a modern C++ sum type.

![Image of Larry](images/prettygood.png)

A CMake build solution is provided, googletest is required.

My build procedure looks something like this:
```bash

  git clone ...
  cd pretty-good-sum.git
  mkdir build
  cd build

```

From the build directory I generate a build system with a script like
the following (run_cmake.sh)

```bash

  CMAKE_PATH="`cygpath -p 'C:\Program Files (x86)\CMake\bin\'`"
  PATH="$CMAKE_PATH:$PATH"; export PATH

  GTEST_ROOT="..."; export GTEST_ROOT

  rm -rf CMake*
  cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX:PATH=. -DVERBOSE=1 -DBUILD_DOCUMENTATION=1 .. 

```

To build the tests and generate the documentaion I execute a script
along these lines (run_cpp.sh)

```bash

CMAKE_PATH="`cygpath -p 'C:\Program Files (x86)\CMake\bin\'`"
GS_PATH="`cygpath -p 'C:\Program Files (x86)\gs\gs9.18\bin'`"
PATH="$GS_PATH:$CMAKE_PATH:$PATH"; export PATH
cmake.exe --build . --target install --config Release --clean-first

```
