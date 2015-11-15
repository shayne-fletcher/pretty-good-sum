# Compatible with FindGTest.cmake distributed with cmake
# Defines the following variables:
#    GTEST_FOUND - Found the Google Testing framework
#    GTEST_INCLUDE_DIRS - Include directories
#
# Also defines the library variables below as normal variables.  These
# contain debug/optimized keywords when a debugging library is found.
#
#    GTEST_BOTH_LIBRARIES - Both libgtest & libgtest-main
#    GTEST_LIBRARIES - libgtest
#    GTEST_MAIN_LIBRARIES - libgtest-main
#
# Accepts the following variables as input:
#
#    GTEST_INCLUDE_PATH (cmake or env variable) The location of the include file. Takes priority GTEST_ROOT
#    GTEST_LIBRARY_PATH cmake or env variable) The location of the libraries. Takes priority over GTEST_ROOT
#    BUILD_WITH_STATIC_RUNTIME If using MSVC, it will use MD (static runtime) libraries
#
# (Input variables that are compatible with FindGTest.cmake)
#
#    GTEST_ROOT - (as a CMake or environment variable)
#                 The root directory of the gtest install prefix
#
#    GTEST_MSVC_SEARCH - If compiling with MSVC, this variable can be set to
#                        "MD" or "MT" to enable searching a GTest build tree
#                        (defaults: "MD"). Overrides BUILD_WITH_STATIC_RUNTIME to be backwards compatible
#
# Example Usage:
#
#     enable_testing()
#     find_package(GoogleTest REQUIRED)
#     include_directories(${GTEST_INCLUDE_DIRS})
#

if(GTEST_FOUND)
  return()
endif()


find_package(PackageHandleStandardArgs REQUIRED)

find_path(GTEST_INCLUDE_DIRS gtest/gtest.h
    HINTS
        ENV GTEST_INCLUDE_PATH ${GTEST_INCLUDE_PATH}
        ENV GTEST_ROOT ${GTEST_ROOT}
        ${ROBO_DKPG_ROOT_DIR}
    PATH_SUFFIXES ${_include_suffixes}
    NO_DEFAULT_PATH
  )

if(GTEST_INCLUDE_DIRS)

   get_filename_component(_header_parent ${GTEST_INCLUDE_DIRS} DIRECTORY)
    
  find_library (GTEST gtest PATHS ${GTEST_LIBRARY_PATH})

    if(GTEST AND GTEST_DEBUG)
      set(GTEST_LIBRARIES optimized ${GTEST} debug ${GTEST_DEBUG} )
    elseif(GTEST)
      set(GTEST_LIBRARIES ${GTEST})
    endif()
    
    set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
    
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTEST DEFAULT_MSG GTEST_INCLUDE_DIRS GTEST_LIBRARIES)

if (GTEST_FOUND AND VERBOSE)
    message(STATUS "GTEST_FOUND=${GTEST_FOUND}")
    message(STATUS "GTEST_INCLUDE_DIRS=${GTEST_INCLUDE_DIRS}")
    message(STATUS "GTEST_LIBRARIES=${GTEST_LIBRARIES}")
    message(STATUS "GTEST_MAIN_LIBRARIES=${GTEST_MAIN_LIBRARIES}")
    message(STATUS "GTEST_BOTH_LIBRARIES=${GTEST_BOTH_LIBRARIES}")
endif()
    
