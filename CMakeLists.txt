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
message ("Doxygen = ${Doxygen}")

# Non-standard modules (thanks Juan!)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(GoogleTest REQUIRED) #Google test

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)

if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE")
endif(MSVC)

if( (WIN32 AND CMAKE_CL_64) OR (NOT WIN32 AND CMAKE_SIZEOF_VOID_P MATCHES 8))
  SET(INSTALL_LIB_DIR_SUFFIX 64)
endif()

# Library

set(PGS_HPP 
	${CMAKE_CURRENT_SOURCE_DIR}/src/logical.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/recursive_wrapper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/recursive_union.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sum.hpp
)
set(PGS_CPP) #There are no .cpp files

#Install

INSTALL(FILES ${PGS_HPP} DESTINATION include) #Headers
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION share/doc/images) #Images

SET(PGS_TESTS_CPP 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/breathing.t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/match.t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/get.t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/list.t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/move.t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/copy.t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.t.cpp
)
ADD_EXECUTABLE(pgs_test ${PGS_TESTS_CPP})
TARGET_INCLUDE_DIRECTORIES(pgs_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
TARGET_INCLUDE_DIRECTORIES(pgs_test SYSTEM PUBLIC ${GTEST_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(pgs_test ${GTEST_LIBRARIES})

#Set header path for tests

TARGET_INCLUDE_DIRECTORIES(pgs_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
#TARGET_INCLUDE_DIRECTORIES(pgs_test SYSTEM PUBLIC ${GTEST_INCLUDE_DIRS} )
#TARGET_LINK_LIBRARIES(pgs_test ${GTEST_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

INSTALL(TARGETS pgs_test
    RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${INSTALL_LIB_DIR_SUFFIX}
	ARCHIVE DESTINATION lib${INSTALL_LIB_DIR_SUFFIX}
	)

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
