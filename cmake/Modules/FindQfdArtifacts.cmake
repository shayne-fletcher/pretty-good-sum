#############################################################
#
# Defines the following variables
# QFD_FOUND
# QFD_LIBRARY_SUFFIX the chosen compiler/runtime runtime suffix
# QFD_LIBRARY_SUFFIX_DEBUG the chosen compiler/runtime suffix (debug)
# 
# Windows only
# QFD_MSVC_MT_LIBRARY_SUFFIX the static runtime suffix
# QFD_MSVC_MT_LIBRARY_SUFFIX_DEBUG the static runtime suffix
# QFD_MSVC_MD_LIBRARY_SUFFIX
# QFD_MSVC_MD_LIBRARY_SUFFIX_DEBUG

#############################################################

find_package(PackageHandleStandardArgs REQUIRED)
set(qfd_valid_arch TRUE)

if (MSVC)
    if(MSVC10)
        set(__artifact_name -vc100)
    elseif(MSVC12)
        set(__artifact_name -vc120)
    elseif(MSVC14)
        set(__artifact_name -vc140)
    else()
        set(qfd_valid_arch)
    endif()

    set(QFD_MSVC_MT_LIBRARY_SUFFIX ${__artifact_name}-s)
    set(QFD_MSVC_MT_LIBRARY_SUFFIX_DEBUG ${__artifact_name}-sd)
    set(QFD_MSVC_MD_LIBRARY_SUFFIX ${__artifact_name})
    set(QFD_MSVC_MD_LIBRARY_SUFFIX_DEBUG ${__artifact_name}-d)

    if(BUILD_WITH_STATIC_RUNTIME)
        set(QFD_LIBRARY_SUFFIX ${QFD_MSVC_MT_LIBRARY_SUFFIX})
        set(QFD_LIBRARY_SUFFIX_DEBUG ${QFD_MSVC_MT_LIBRARY_SUFFIX_DEBUG})
    else()
        set(QFD_LIBRARY_SUFFIX ${QFD_MSVC_MD_LIBRARY_SUFFIX})
        set(QFD_LIBRARY_SUFFIX_DEBUG ${QFD_MSVC_MD_LIBRARY_SUFFIX_DEBUG})
    endif()
else()
    set(QFD_LIBRARY_SUFFIX_DEBUG "-d")
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS (QFD "Libraries for this architecture not provided by the QFD group" qfd_valid_arch QFD_LIBRARY_SUFFIX_DEBUG)

if (QFD_FOUND AND VERBOSE)
    message(STATUS "QFD_FOUND=${QFD_FOUND}")
    message(STATUS "QFD_LIBRARY_SUFFIX=${QFD_LIBRARY_SUFFIX}")
    message(STATUS "QFD_LIBRARY_SUFFIX_DEBUG=${QFD_LIBRARY_SUFFIX_DEBUG}")
endif()

