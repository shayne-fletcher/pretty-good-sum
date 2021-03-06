IF(QDLA_FOUND)
  RETURN()
ENDIF()
IF(NOT WIN32)
  return()
ENDIF()

GET_FILENAME_COMPONENT(QDLA_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Bloomberg L.P.\\QDLA;InstallDir]" ABSOLUTE CACHE)


find_package(PackageHandleStandardArgs REQUIRED)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QDLA DEFAULT_MSG QDLA_ROOT)

IF(NOT EXISTS "${QDLA_ROOT}/")
    message(FATAL_ERROR "QDLA_ROOT doesn't exist at location ${QDLA_ROOT}")
ENDIF()

if(MSVC12)
    set(QDLA_PATH_SUFFIX_DEBUG lib/v120_xp/debug)
    set(QDLA_PATH_SUFFIX       lib/v120_xp/release)
elseif(MSVC14)
    set(QDLA_PATH_SUFFIX_DEBUG lib/v140_xp/debug)
    set(QDLA_PATH_SUFFIX       lib/v140_xp/release)
else()
    message(FATAL_ERROR "MSVC ${CMAKE_CXX_COMPILER_VERSION} version currently unsupported")
endif()

if(BUILD_WITH_STATIC_RUNTIME)
    set(QDLA_PATH_SUFFIX_DEBUG ${QDLA_PATH_SUFFIX_DEBUG}_static)
    set(QDLA_PATH_SUFFIX ${QDLA_PATH_SUFFIX}_static)
else()
    set(QDLA_PATH_SUFFIX_DEBUG ${QDLA_PATH_SUFFIX_DEBUG}_dynamic)
    set(QDLA_PATH_SUFFIX ${QDLA_PATH_SUFFIX}_dynamic)
endif()

if(CMAKE_CL_64)
    set(QDLA_PATH_SUFFIX_DEBUG ${QDLA_PATH_SUFFIX_DEBUG}_64)
    set(QDLA_PATH_SUFFIX ${QDLA_PATH_SUFFIX}_64)
else()
    set(QDLA_PATH_SUFFIX_DEBUG ${QDLA_PATH_SUFFIX_DEBUG}_32)
    set(QDLA_PATH_SUFFIX ${QDLA_PATH_SUFFIX}_32)
endif()


message(STATUS "QDLA_ROOT=${QDLA_ROOT}")
message(STATUS "QDLA_PATH_SUFFIX=${QDLA_PATH_SUFFIX}")
message(STATUS "QDLA_PATH_SUFFIX_DEBUG=${QDLA_PATH_SUFFIX_DEBUG}")
