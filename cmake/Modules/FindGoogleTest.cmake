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

find_package(Robo QUIET)
find_package(Qdla QUIET)

set(__gtest_includedir_hint)
# 1 GTEST_INCLUDEDIR
LIST(APPEND __gtest_includedir_hint ${GTEST_INCLUDEDIR})
# 2 Now ENV GTEST_INCLUDEDIR
LIST(APPEND __gtest_includedir_hint $ENV{GTEST_INCLUDEDIR})
# 3 Now GTEST_ROOT
LIST(APPEND __gtest_includedir_hint ${GTEST_ROOT})
# 4 ENV{GTEST_ROOT}
LIST(APPEND __gtest_includedir_hint $ENV{GTEST_ROOT})
# 5 Qdla infra (Windows)
IF(QDLA_FOUND)
  LIST(APPEND __gtest_includedir_hint ${QDLA_ROOT}/gtest/include)
ENDIF()
# 6 Now Robo
IF(ROBO_FOUND)
  LIST(APPEND __gtest_includedir_hint ${ROBO_DKPG_ROOT_INCLUDE_DIRS})
ENDIF()

find_path(GTEST_INCLUDE_DIRS gtest/gtest.h
    HINTS
        ${__gtest_includedir_hint}
    PATH_SUFFIXES
        include
    NO_DEFAULT_PATH
  )

IF(GTEST_INCLUDE_DIRS)

    get_filename_component(_header_parent ${GTEST_INCLUDE_DIRS} DIRECTORY)

    set(__header_relative lib)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_header_relative lib64)
    ENDIF()
    
    set( _gtest_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set( _gtest_ORIG_CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES})

        set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib ${CMAKE_FIND_LIBRARY_SUFFIXES})
    IF(WIN32)
      set(CMAKE_FIND_LIBRARY_PREFIXES "")
    ENDIF()

      
    FIND_LIBRARY(__gtest_lib NAMES gtest
        HINTS
            ${GTEST_LIBRARYDIR} ENV GTEST_LIBRARYDIR
            ${_header_parent}
        PATH_SUFFIXES
            ${_header_relative}
            ${QDLA_PATH_SUFFIX}
    )

    FIND_LIBRARY(__gtest_main_lib NAMES gtest_main
        HINTS
            ${GTEST_LIBRARYDIR} ENV GTEST_LIBRARYDIR
            ${_header_parent}
        PATH_SUFFIXES
            ${_header_relative}
            ${QDLA_PATH_SUFFIX}
    )


    FIND_LIBRARY(__gtest_debug_lib NAMES gtestd gtest
        HINTS
            ${GTEST_LIBRARYDIR} ENV GTEST_LIBRARYDIR
            ${_header_parent}
        PATH_SUFFIXES
            ${_header_relative}
            ${QDLA_PATH_SUFFIX_DEBUG}
    )

    FIND_LIBRARY(__gtest_main_debug_lib NAMES gtesti_maind gtest_main
        HINTS
            ${GTEST_LIBRARYDIR} ENV GTEST_LIBRARYDIR
            ${_header_parent}
        PATH_SUFFIXES
            ${_header_relative}
            ${QDLA_PATH_SUFFIX_DEBUG}
    )

    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_gtest_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_PREFIXES ${_gtest_ORIG_CMAKE_FIND_LIBRARY_PREFIXES})
             
    if(__gtest_lib AND __gtest_debug_lib)
      set(GTEST_LIBRARIES optimized ${__gtest_lib} debug ${__gtest_debug_lib})
    elseif(__gtest_lib)
      set(GTEST_LIBRARIES ${__gtest_lib})
    endif()

    if(__gtest_main_lib AND __gtest_main_debug_lib)
      set(GTEST_MAIN_LIBRARIES optimized ${__gtest_main_lib} debug ${__gtest_main_debug_lib})
    elseif(__gtest_main_lib)
      set(GTEST_MAIN_LIBRARIES ${__gtest_main_lib})
    endif()


endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTEST DEFAULT_MSG GTEST_INCLUDE_DIRS GTEST_LIBRARIES GTEST_MAIN_LIBRARIES)

SET(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

if (GTEST_FOUND AND VERBOSE)
    message(STATUS "GTEST_FOUND=${GTEST_FOUND}")
    message(STATUS "GTEST_INCLUDE_DIRS=${GTEST_INCLUDE_DIRS}")
    message(STATUS "GTEST_LIBRARIES=${GTEST_LIBRARIES}")
    message(STATUS "GTEST_MAIN_LIBRARIES=${GTEST_MAIN_LIBRARIES}")
    message(STATUS "GTEST_BOTH_LIBRARIES=${GTEST_BOTH_LIBRARIES}")
endif()
    

