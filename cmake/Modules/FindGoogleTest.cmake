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
find_package(QfdArtifacts REQUIRED)
find_package(PackageFinder REQUIRED) # Provides macro FIND_LIBRARY_PERMUTE

IF(DEFINED ENV{GTEST_MINIMUM_VERSION} AND NOT GTEST_MINIMUM_VERSION)
    set(GTEST_MINIMUM_VERSION $ENV{GTEST_MINIMUM_VERSION})
endif()
IF (DEFINED ENV{GTEST_EXACT_VERSION} AND NOT GTEST_EXACT_VERSION)
    set(GTEST_EXACT_VERSION $ENV{GTEST_EXACT_VERSION})
endif()

FIND_VERSIONS(GTEST 
    VERSIONS ${GTEST_ADDITIONAL_VERSIONS} 1.7.0 1.6.0  
    EXACT ${GTEST_EXACT_VERSION}
    MINIMUM ${GTEST_MINIMUM_VERSION}
)

set(_include_suffixes include)
foreach(version ${GTEST_VERSIONS_AVAILABLE})
    list(APPEND _include_suffixes
      "${version}"
      "${version}/include"
      )
endforeach()

message ("GTEST_ROOT=${GTEST_ROOT} and env var = $ENV{GTEST_ROOT}")

find_path(GTEST_INCLUDE_DIRS gtest/gtest.h
    HINTS
        ENV GTEST_INCLUDE_PATH ${GTEST_INCLUDE_PATH}
        ENV GTEST_ROOT ${GTEST_ROOT}
        ${ROBO_DKPG_ROOT_DIR}
    PATH_SUFFIXES ${_include_suffixes}
    NO_DEFAULT_PATH
  )

if(MSVC)
      # This tree structure made to me somewhat compatible with FindGTest.cmake
      SET(GTEST_COMPAT_PATH_SUFFIX msvc/gtest/Release)
      SET(GTEST_COMPAT_PATH_SUFFIX_DEBUG msvc/gtest/Debug)
      if(GTEST_MSVC_SEARCH STREQUAL "MD" OR (NOT GTEST_MSVC_SEARCH AND BUILD_WITH_STATIC_RUNTIME))
          SET(GTEST_COMPAT_PATH_SUFFIX msvc/gtest-md/Release)
          SET(GTEST_COMPAT_PATH_SUFFIX_DEBUG msvc/gtest-md/Debug)
      endif()
endif()

message ("GTEST_INCLUDE_DIRS=${GTEST_INCLUDE_DIRS}")

if(GTEST_INCLUDE_DIRS)

    get_filename_component(_header_parent ${GTEST_INCLUDE_DIRS} DIRECTORY)
    
    if( (WIN32 AND CMAKE_CL_64) OR (NOT WIN32 AND CMAKE_SIZEOF_VOID_P MATCHES 8))
        set(__pathSuffix lib64)
    else()
        set(__pathSuffix lib)
    endif()
    

    set(_lib_path_suffixes ${__pathSuffix})
    set(_lib_path_suffixes_debug ${__pathSuffix})
    foreach(version ${GTEST_VERSIONS_AVAILABLE})
      list(APPEND _lib_path_suffixes "${version}/${__pathSuffix}") # 1.7.0/lib64 or 1.7.0/msvc/gtest/Release
      list(APPEND _lib_path_suffixes_debug "${version}/${__pathSuffix}")
      if(GTEST_COMPAT_PATH_SUFFIX)
        list(APPEND _lib_path_suffixes "${version}/${GTEST_COMPAT_PATH_SUFFIX}")
        list(APPEND _lib_path_suffixes_debug "${version}/${GTEST_COMPAT_PATH_SUFFIX_DEBUG}")
      endif()
      
    endforeach()

    
    # GTEST RELEASE MODE
    FIND_LIBRARY_PERMUTE(GTEST  NAMES gtest
      PATHS         
        ENV GTEST_LIBRARY_PATH ${GTEST_LIBRARY_PATH}
        ENV GTEST_ROOT ${GTEST_ROOT}
        ${ROBO_DKPG_ROOT_LIBRARY_DIR}
        ${_header_parent}    
      PATH_SUFFIXES ${_lib_path_suffixes}
      SUFFIXES ${QFD_LIBRARY_SUFFIX} # i.e. vc120-sd in windows or -d in Linux
      NO_DEFAULT_PATH
    )
    # GTEST DEBUG MODE
    FIND_LIBRARY_PERMUTE(GTEST_DEBUG NAMES gtestd gtest
      PATHS         
        ENV GTEST_LIBRARY_PATH ${GTEST_LIBRARY_PATH}
        ENV GTEST_ROOT ${GTEST_ROOT}
        ${ROBO_DKPG_ROOT_LIBRARY_DIR}
        ${_header_parent}
      PATH_SUFFIXES ${_lib_path_suffixes_debug}
      SUFFIXES ${QFD_LIBRARY_SUFFIX_DEBUG}
    )
    
    if(GTEST AND GTEST_DEBUG)
      set(GTEST_LIBRARIES optimized ${GTEST} debug ${GTEST_DEBUG} )
    elseif(GTEST)
      set(GTEST_LIBRARIES ${GTEST})
    endif()
    
    FIND_LIBRARY_PERMUTE(GTEST_MAIN  NAMES gtest_main
      PATHS         
        ENV GTEST_LIBRARY_PATH ${GTEST_LIBRARY_PATH}
        ENV GTEST_ROOT ${GTEST_ROOT}
        ${ROBO_DKPG_ROOT_LIBRARY_DIR}
        ${_header_parent}    
      PATH_SUFFIXES ${_lib_path_suffixes}
      SUFFIXES ${QFD_LIBRARY_SUFFIX}
      NO_DEFAULT_PATH
    )

    FIND_LIBRARY_PERMUTE(GTEST_MAIN_DEBUG NAMES gtest_maind gtest_main
      PATHS         
        ENV GTEST_LIBRARY_PATH ${GTEST_LIBRARY_PATH}
        ENV GTEST_ROOT ${GTEST_ROOT}
        ${ROBO_DKPG_ROOT_LIBRARY_DIR}
        ${_header_parent}
      PATH_SUFFIXES ${_lib_path_suffixes_debug}
      SUFFIXES ${QFD_LIBRARY_SUFFIX_DEBUG}
    )
    if(GTEST_MAIN AND GTEST_MAIN_DEBUG)
      set(GTEST_MAIN_LIBRARIES optimized ${GTEST_MAIN} debug ${GTEST_MAIN_DEBUG} )
    elseif(GTEST_MAIN)
      set(GTEST_MAIN_LIBRARIES ${GTEST_MAIN})
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
    
