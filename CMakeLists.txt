cmake_minimum_required(VERSION 3.1)
project(pretty_good_sum CXX C)

message (STATUS "env var = $ENV{GTEST_ROOT}")

# The project version number.

set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# Standard modules

find_package(Doxygen)

# Non-standard modules (thanks Juan!)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

message (CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH})
find_package(Robo)
find_package(GoogleTest REQUIRED) #Google test

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
# With Clang we force C++11 + C99
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE")
endif(MSVC)

if( (WIN32 AND CMAKE_CL_64) OR (NOT WIN32 AND CMAKE_SIZEOF_VOID_P MATCHES 8))
  SET(INSTALL_LIB_DIR_SUFFIX 64)
endif()

# Library

set(PGS_HPP 
    src/pgs/logical.hpp
    src/pgs/recursive_wrapper.hpp
    src/pgs/recursive_union.hpp
    src/pgs/sum_type.hpp
)

#Install

INSTALL(FILES ${PGS_HPP} DESTINATION include) #Headers
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION share/doc/images) #Images

ADD_SUBDIRECTORY(tests)

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    ADD_CUSTOM_TARGET(doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        #WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)	
endif()
