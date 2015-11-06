# pretty-good-sum

Pretty good sum type library implementing a modern C++ sum type.

![Image of Larry](/images/prettygood.png)

A CMake build solution is provided, googletest is required. You can
get a variety of builds with layout and naming conventions expected by
the build system by installing [qfd_infrastructure.exe]
(https://cms.prod.bloomberg.com/team/download/attachments/330372301/qfd_infrastructure.exe)

My build procedure looks something like this:
```bash

  git clone https://bbgithub.dev.bloomberg.com/sfletche/pretty-good-sum.git pretty-good-sum.git
  cd pretty-good-sum.git
  mkdir build
  cd build

```

From the build directory I generate a build system with a script like
the following (run_cmake.sh)

```bash

  CMAKE_PATH="`cygpath -p 'C:\Program Files (x86)\CMake\bin\'`"
  PATH="$CMAKE_PATH:$PATH"; export PATH

  GTEST_ROOT="C:/project/qfd/qfd_external.git/libs/qfd_infra/1.0.0"; export GTEST_ROOT

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
