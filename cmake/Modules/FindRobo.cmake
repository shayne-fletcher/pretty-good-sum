#############################################################
# Accepts the following variables, as either cmake variables (priority) or environment variables
# ROBO_ROOT (cmake var or env var) 
# ROBO_DPKG_ROOT (cmake var or env var). If not defined, and if ROBO_ROOT is defined, will point to ROBO_ROOT/lib/dpkgroot
# Will bypass the call and set robo according to that
#
# Defines the following variables:
# ROBO_FOUND 
# ROBO_ROOT_DIR
# ROBO_DKPG_ROOT_DIR
# 

find_package(PackageHandleStandardArgs REQUIRED)

IF(ROBO_FOUND OR WIN32)
  return()
ENDIF()

IF(NOT ROBO_ROOT)

    IF(ENV{ROBO_ROOT})
        SET(ROBO_ROOT $ENV{ROBO_ROOT})
    ELSE()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
          set(__flags --bits=64)
        else()
          set(__flags --bits=32)
        endif()
        execute_process(
          COMMAND /bbsrc/bin/prod/bin/robolibs-config --stdout ${__flags}
          OUTPUT_VARIABLE ROBOLIBS_OUTPUT 
          OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        STRING(REGEX REPLACE ".*ROBO_OBJS_PATH=(.+)/lib/objs.*" "\\1" ROBO_ROOT ${ROBOLIBS_OUTPUT})

        IF(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
            SET(ROBO_ROOT /bb/bigstorq3/qfdlibci/qfd/big2015.47)
        ENDIF()

    ENDIF()

ENDIF()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ROBO DEFAULT_MSG ROBO_ROOT)
SET(ROBO_DPKG_ROOT ${ROBO_ROOT}/lib/dpkgroot)
SET(ROBO_DKPG_ROOT_INCLUDE_DIRS ${ROBO_DPKG_ROOT}/opt/bb/include)
SET(ROBO_DKPG_ROOT_LIBRARY_DIR ${ROBO_DPKG_ROOT}/opt/bb/lib)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(ROBO_DKPG_ROOT_LIBRARY_DIR ${ROBO_DKPG_ROOT_LIBRARY_DIR}64)
ENDIF()


if (VERBOSE)
    message(STATUS "ROBO_FOUND=${ROBO_FOUND}")
    message(STATUS "ROBO_ROOT=${ROBO_ROOT}")
    message(STATUS "ROBO_DPKG_ROOT=${ROBO_DPKG_ROOT}")
    message(STATUS "ROBO_DKPG_ROOT_INCLUDE_DIRS=${ROBO_DKPG_ROOT_INCLUDE_DIRS}")
    message(STATUS "ROBO_DKPG_ROOT_LIBRARY_DIR=${ROBO_DKPG_ROOT_LIBRARY_DIR}")
endif()

