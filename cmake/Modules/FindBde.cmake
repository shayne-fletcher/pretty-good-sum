#############################################################
# Accepts the following variables, as either cmake variables (priority) or environment variables
# BDE_ROOT (cmake var or env var)
# BDE_INCLUDEDIR (Takes precedence over BDE_ROOT)
# BDE_LIBRARYDIR (Takes precedence over BDE_ROOT)
# BDE_ADDITIONAL_VERSIONS to extend the list of known bde versions
# BDE_REQUESTED_LIBRARIES contains bce, bde, bdl, bae, bte, bsi, bsc, bas and bbe. Can be modified to load additional libraries
# BUILD_WITH_STATIC_RUNTIME if defined, windows solutions will use static runtime
# BDE_ADDITIONAL_VERSIONS list of versions to try, in addition to the standard ones (Windows)
# BDE_MINIMUM_VERSION (either var or env var) Any version less than this one will be discarded (Windows)
# BDE_EXACT_VERSION (either var or env var) Any version other than this one will be discarded (Windows)
# BSL_OVERRIDE_STD (either var or env var). Will add -DBSL_VERRIDE_STD definition to the project
# BDE_PATCH_ROOT. Will prepend BDE_PATCH_ROOT/BDE_VERSION/include when calling the BDE_ADD_TARGET_SUPPORT macro
#
#
# Generates the following variables
# BDE_FOUND
# BDE_VERSION
# BDE_VERSION_MAJOR
# BDE_VERSION_MINOR
# BDE_VERSION_PATCH
# BDE_INCLUDE_DIRS
# BDE_KNOWN_VERSIONS A list of all the known versions of bde (Windows specific)
# BDE_PATH_SUFFIX path under the root library dir where a specific optimized version was found (i.e. lib/vc9/debug_static_64)
# BDE_PATH_SUFFIX_DEBUG path under the root library dir 
#    where a specific debug version was found (i.e. lib/vc9/release_static_64 in Windows or lib64 if 64bit robo)
# BDE_PATH_SUFFIX_DEBUG
# BDE_LIBRARIES
# For each of the libraries requested in BDE_REQUESTED_LIBRARIES:
# BDE_<library>_FOUND
# BDE_<library>
# BDE_<library>_DEBUG
# BDE_<library>_LIBRARIES
# 
# Defines the following macros
# BDE_ADD_TARGET_SUPPORT(target). Will add BDE_PATCH_ROOT/BDE_VERSION/include (if exists) and BDE_INCLUDE_DIRS to the target_include_directories
###################################

OPTION(BSL_OVERRIDE_STD "Enables BSL_OVERRIDE_STD mode with BDE" OFF)
find_package(PackageHandleStandardArgs REQUIRED)
find_package(Robo QUIET)
find_package(PackageFinder REQUIRED) # Provides macro FIND_LIBRARY_PERMUTE

IF(BDE_FOUND)
  return()
ENDIF()
IF(ENV{BSL_OVERRIDE_STD} OR BSL_OVERRIDE_STD)
    MESSAGE("Enabling BSL_OVERRIDE_STD")
	ADD_DEFINITIONS(-DBSL_OVERRIDES_STD)
ENDIF()


if (NOT BDE_REQUESTED_LIBRARIES)
  set(BDE_REQUESTED_LIBRARIES BSI BAS BBE BTE BSC BAE BCE BDE BDL BSL)
endif()

IF(DEFINED ENV{BDE_MINIMUM_VERSION} AND NOT BDE_MINIMUM_VERSION)
    set(BDE_MINIMUM_VERSION $ENV{BDE_MINIMUM_VERSION})
endif()

IF (DEFINED ENV{BDE_EXACT_VERSION} AND NOT BDE_EXACT_VERSION)
    set(BDE_EXACT_VERSION $ENV{BDE_EXACT_VERSION})
endif()

FIND_VERSIONS(BDE 
    VERSIONS ${BDE_ADDITIONAL_VERSIONS} 2.24.0 2.23.4 2.23.3 2.23.2 2.23.1 2.22.4a 2.22.1  
    EXACT ${BDE_EXACT_VERSION}
	MINIMUM ${BDE_MINIMUM_VERSION}
	)
	

######## The logic starts here. First I'll define the paths used to look for libraries, etc
if(WIN32)
    if(MSVC90)
            set(BDE_PATH_SUFFIX_DEBUG lib/vc9/debug)
            set(BDE_PATH_SUFFIX lib/vc9/release)
        elseif(MSVC10)
            set(BDE_PATH_SUFFIX_DEBUG lib/v100/debug)
            set(BDE_PATH_SUFFIX lib/v100/release)
        elseif(MSVC11)
            set(BDE_PATH_SUFFIX_DEBUG lib/v110/debug)
        elseif(MSVC12)
            set(BDE_PATH_SUFFIX lib/v110/release)
            set(BDE_PATH_SUFFIX_DEBUG lib/v120_xp/debug)
            set(BDE_PATH_SUFFIX lib/v120_xp/release)
        elseif(MSVC14)
            message(FATAL_ERROR "MSVC14 Currently unsupported")
        endif()
        if(BUILD_WITH_STATIC_RUNTIME)
            set(BDE_PATH_SUFFIX_DEBUG ${BDE_PATH_SUFFIX_DEBUG}_static)
            set(BDE_PATH_SUFFIX ${BDE_PATH_SUFFIX}_static)
        else()
            set(BDE_PATH_SUFFIX_DEBUG ${BDE_PATH_SUFFIX_DEBUG}_dynamic)
            set(BDE_PATH_SUFFIX ${BDE_PATH_SUFFIX}_dynamic)
        endif()
        if(CMAKE_CL_64)
            set(BDE_PATH_SUFFIX_DEBUG ${BDE_PATH_SUFFIX_DEBUG}_64)
            set(BDE_PATH_SUFFIX ${BDE_PATH_SUFFIX}_64)
        else()
            set(BDE_PATH_SUFFIX_DEBUG ${BDE_PATH_SUFFIX_DEBUG}_32)
            set(BDE_PATH_SUFFIX ${BDE_PATH_SUFFIX}_32)
        endif()
        set(BDE_LIBRARY_SUFFIXES .lib )
        set(BDE_LIBRARY_SUFFIXES_DEBUG .lib)
    else() # Unix. Either lib or lib64
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(BDE_PATH_SUFFIX lib64)
            set(BDE_PATH_SUFFIX_DEBUG lib64)
            set(BDE_LIBRARY_SUFFIXES .opt_dbg_exc_mt_64.a .opt_exc_mt_64.a)
            set(BDE_LIBRARY_SUFFIXES_DEBUG .dbg_exc_mt_64_safe.a .dbg_exc_mt_64.a)
        else()
            set(BDE_PATH_SUFFIX lib)
            set(BDE_PATH_SUFFIX_DEBUG lib)
            set(BDE_LIBRARY_SUFFIXES .opt_dbg_exc_mt.a .opt_exc_mt.a)
            set(BDE_LIBRARY_SUFFIXES_DEBUG .dbg_exc_mt_safe .dbg_exc_mt.a)
        endif()
    endif()
 
set(_include_suffixes include)
foreach(version ${BDE_VERSIONS_AVAILABLE})
    list(APPEND _include_suffixes
      "${version}"
      "${version}/include"
      )
endforeach()

file(TO_CMAKE_PATH "C:/BPCDEVTOOLS/BDE" __winPath)
find_path(BDE_INCLUDE_DIRS
  NAMES         bdescm_versiontag.h
  PATHS         
    $ENV{BDE_INCLUDEDIR} ${BDE_INCLUDEDIR}
    $ENV{BDE_ROOT} ${BDE_ROOT}
    ${ROBO_DKPG_ROOT_INCLUDE_DIRS} ${__winPath} #Standard deployment locations
  PATH_SUFFIXES include ${_include_suffixes}
  NO_DEFAULT_PATH
  )

function(_Bde_FIND_LIBRARY name)
    STRING(TOUPPER ${name} __upperName)
    STRING(TOLOWER ${name} name)
    if(BDE_INCLUDE_DIRS AND (NOT BDE_${__upperName}_FOUND)) 

        get_filename_component(_header_parent ${BDE_INCLUDE_DIRS} DIRECTORY)

        set(_lib_path_suffixes ${BDE_PATH_SUFFIX})
        set(_lib_path_suffixes_debug ${BDE_PATH_SUFFIX_DEBUG})
        foreach(version ${BDE_VERSIONS_AVAILABLE})
            list(APPEND _lib_path_suffixes "${version}/${BDE_PATH_SUFFIX}") # 2.23.3/lib/vc9/release_static_64
          list(APPEND _lib_path_suffixes_debug "${version}/${BDE_PATH_SUFFIX_DEBUG}") # 2.23.3/lib/vc9/debug_static_64
        endforeach()

        FIND_LIBRARY_PERMUTE(BDE_${__upperName} ${name}
      PATHS         
        $ENV{BDE_LIBRARYDIR} ${BDE_LIBRARYDIR}
        $ENV{BDE_ROOT} ${BDE_ROOT}
        ${ROBO_DKPG_ROOT_LIBRARY_DIR} ${__winPath}
      PATH_SUFFIXES ${_lib_path_suffixes}
        SUFFIXES ${BDE_LIBRARY_SUFFIXES}
    )
        FIND_LIBRARY_PERMUTE(BDE_${__upperName}_DEBUG ${name}
      PATHS         
        $ENV{BDE_LIBRARYDIR} ${BDE_LIBRARYDIR}
        $ENV{BDE_ROOT} ${BDE_ROOT}
        ${ROBO_DKPG_ROOT_LIBRARY_DIR} ${__winPath} #Standard deployment locations
      PATH_SUFFIXES ${_lib_path_suffixes_debug}
        SUFFIXES ${BDE_LIBRARY_SUFFIXES_DEBUG}
    )
        if(BDE_${__upperName} AND BDE_${__upperName}_DEBUG)
            set(BDE_${__upperName}_LIBRARIES optimized ${BDE_${__upperName}} debug ${BDE_${__upperName}_DEBUG} )
        elseif(BDE_${__upperName})
            set(BDE_${__upperName}_LIBRARIES ${BDE_${__upperName}} )
        endif()

        if(BDE_${__upperName}_LIBRARIES)
            list(APPEND BDE_LIBRARIES ${BDE_${__upperName}_LIBRARIES})
            list(APPEND BDE_AVAILABLE_LIBRARIES ${__upperName})
            set(BDE_${__upperName}_FOUND TRUE)
        endif()
    
        # propagate the result variables to the caller:
        set(BDE_${__upperName}_FOUND  ${BDE_${__upperName}_FOUND} PARENT_SCOPE)
        set(BDE_${__upperName}  ${BDE_${__upperName}} PARENT_SCOPE)
        set(BDE_${__upperName}_DEBUG  ${BDE_${__upperName}_DEBUG} PARENT_SCOPE)
        set(BDE_${__upperName}_LIBRARIES  ${BDE_${__upperName}_LIBRARIES} PARENT_SCOPE)
        set(BDE_LIBRARIES  ${BDE_LIBRARIES} PARENT_SCOPE)
        set(BDE_AVAILABLE_LIBRARIES ${BDE_AVAILABLE_LIBRARIES} PARENT_SCOPE)
    endif()
endfunction()

STRING(TOUPPER "${BDE_REQUESTED_LIBRARIES}" BDE_REQUESTED_LIBRARIES)
LIST(REMOVE_DUPLICATES BDE_REQUESTED_LIBRARIES)

foreach (_lib ${BDE_REQUESTED_LIBRARIES})
  _Bde_FIND_LIBRARY(${_lib})
endforeach()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(BDE DEFAULT_MSG BDE_INCLUDE_DIRS BDE_LIBRARIES)
  # Extract version

FILE(READ ${BDE_INCLUDE_DIRS}/bdescm_versiontag.h __data)
STRING(REGEX REPLACE ".*#define BDE_VERSION_MAJOR +([0-9]+).*" "\\1" BDE_VERSION_MAJOR "${__data}")
STRING(REGEX REPLACE ".*#define BDE_VERSION_MINOR +([0-9]+).*" "\\1" BDE_VERSION_MINOR "${__data}")
FILE(READ ${BDE_INCLUDE_DIRS}/bdescm_patchversion.h __data)
STRING(REGEX REPLACE ".*#define BDESCM_PATCHVERSION_PATCH +([0-9]+).*" "\\1" BDE_VERSION_PATCH "${__data}")
set(BDE_VERSION "${BDE_VERSION_MAJOR}.${BDE_VERSION_MINOR}.${BDE_VERSION_PATCH}")


macro(BDE_ADD_TARGET_SUPPORT tag)
    IF(BDE_PATCH_ROOT AND IS_DIRECTORY ${BDE_PATCH_ROOT}/${BDE_VERSION}/include)
        TARGET_INCLUDE_DIRECTORIES(${tag}
          SYSTEM PRIVATE
            ${BDE_PATCH_ROOT}/${BDE_VERSION}/include
        )
	ENDIF()
    TARGET_INCLUDE_DIRECTORIES(${tag}
      SYSTEM PRIVATE
        ${BDE_INCLUDE_DIRS}
    )
    TARGET_LINK_LIBRARIES(${tag} ${BDE_LIBRARIES})
endmacro(BDE_ADD_TARGET_SUPPORT)

if(VERBOSE)
    message(STATUS "BDE_FOUND=${BDE_FOUND}")
	message(STATUS "BDE_VERSION=${BDE_VERSION}")
    message(STATUS "BDE_AVAILABLE_LIBRARIES=${BDE_AVAILABLE_LIBRARIES}")
    message(STATUS "BDE_INCLUDE_DIRS=${BDE_INCLUDE_DIRS}")
    message(STATUS "BDE_LIBRARIES=${BDE_LIBRARIES}")
    foreach(lib ${BDE_AVAILABLE_LIBRARIES})
        message(STATUS "BDE_${lib}_DEBUG=${BDE_${lib}_DEBUG}")
    endforeach()
endif()
