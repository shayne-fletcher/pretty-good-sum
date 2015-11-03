function(__findPackagePermute output_name libname libprefix libsuffix)
  IF(NOT ${output_name})
    set(__libname ${libprefix}${libname}${libsuffix})
    find_library(${output_name} ${__libname}
      ${ARGN}
      NO_DEFAULT_PATH
    )
	IF(${output_name})
      set(${output_name}_FOUND TRUE)
    ENDIF()
  endif()
endfunction()

function(__findPackagePermute_specific_prefix output_name libname libprefix)
	IF(NOT FIND_LIBRARY_PERMUTE_SUFFIXES)
      __findPackagePermute (${output_name} ${libname} "${libprefix}" "" ${ARGN})
	endif()
    foreach(libsuffix ${FIND_LIBRARY_PERMUTE_SUFFIXES})
      __findPackagePermute (${output_name} ${libname} "${libprefix}" "${libsuffix}" ${ARGN})
    endforeach()
endfunction()

# IF we have NAMES, then I'll remove them from ARGN and treat them individually
function(FIND_LIBRARY_PERMUTE output_name)

  # first set all result variables to empty/FALSE
  set(${output_name})
  set(${output_name}_FOUND FALSE)

  set(options )
  set(oneValueArgs )
  set(multiValueArgs PATHS HINTS NAMES PATH_SUFFIXES PREFIXES SUFFIXES)
  cmake_parse_arguments(FIND_LIBRARY_PERMUTE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  SET(FIND_LIBRARY_PERMUTE_PREFIXES ${FIND_LIBRARY_PERMUTE_PREFIXES} ${CMAKE_FIND_LIBRARY_PREFIXES})
  SET(FIND_LIBRARY_PERMUTE_SUFFIXES ${FIND_LIBRARY_PERMUTE_SUFFIXES} ${CMAKE_FIND_LIBRARY_SUFFIXES})
  if(FIND_LIBRARY_PERMUTE_PREFIXES)
    LIST(REMOVE_DUPLICATES FIND_LIBRARY_PERMUTE_PREFIXES)
  endif()
  if(FIND_LIBRARY_PERMUTE_SUFFIXES)
    LIST(REMOVE_DUPLICATES FIND_LIBRARY_PERMUTE_SUFFIXES)
  endif()
  
  # Let's remove NAMES from the list of arguments, if any
  set(ARGN_COPY ${ARGN})
  if (NOT FIND_LIBRARY_PERMUTE_NAMES)
    LIST(GET ARGN_COPY 0 FIND_LIBRARY_PERMUTE_NAMES)
  	LIST(REMOVE_AT ARGN_COPY 0)
  else()
    LIST(FIND ARGN NAMES index)
	LIST(LENGTH FIND_LIBRARY_PERMUTE_NAMES len)
	LIST(REMOVE_AT ARGN_COPY ${index})
	WHILE( len GREATER 0)
	    LIST(REMOVE_AT ARGN_COPY ${index})
	    MATH( EXPR len "${len} - 1" ) # decrement len
	ENDWHILE()
  endif()
  foreach(name ${FIND_LIBRARY_PERMUTE_NAMES})
      if(NOT ${output_name})
        if(NOT FIND_LIBRARY_PERMUTE_PREFIXES)
          __findPackagePermute_specific_prefix(${output_name} ${name} "" ${ARGN_COPY})
        endif()
        foreach(libprefix ${FIND_LIBRARY_PERMUTE_PREFIXES})
          __findPackagePermute_specific_prefix(${output_name} ${name} ${libprefix} ${ARGN_COPY})
        endforeach()
	  endif()
  endforeach()
  if(${output_name})
    set(${output_name}_FOUND TRUE)
  endif()
  
	# propagate the result variables to the caller:
	set(${output_name} ${${output_name}} PARENT_SCOPE)
	set(${output_name}_FOUND ${${output_name}_FOUND} PARENT_SCOPE)
	  
endfunction()

###############
#
###############
function(FIND_VERSIONS prefix)
  set(options )
  set(oneValueArgs EXACT MINIMUM)
  set(multiValueArgs VERSIONS)
  cmake_parse_arguments(FIND_VERSIONS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  LIST(REMOVE_DUPLICATES FIND_VERSIONS_VERSIONS)
  IF(FIND_VERSIONS_EXACT)
    set(${prefix}_VERSIONS_AVAILABLE ${FIND_VERSION_EXACT})
  else()
    # The user has not requested an exact list of versions.  Among known
    # versions, find those that are acceptable to the user request.
    set(${prefix}_VERSIONS_AVAILABLE)
    # Select acceptable versions.
    foreach(version ${FIND_VERSIONS_VERSIONS})
        if(NOT FIND_VERSIONS_MINIMUM OR NOT "${version}" VERSION_LESS "${FIND_VERSIONS_MINIMUM}")
            # This version is high enough.
            list(APPEND ${prefix}_VERSIONS_AVAILABLE "${version}")
        endif()
    endforeach()
  endif()
  # propagate the result variables to the caller:
  if(${prefix}_VERSIONS_AVAILABLE)
	set(${prefix}_VERSIONS_FOUND TRUE PARENT_SCOPE)
    set(${prefix}_VERSIONS_AVAILABLE ${${prefix}_VERSIONS_AVAILABLE} PARENT_SCOPE)
	set(${prefix}_VERSIONS_TOTAL ${FIND_VERSIONS_VERSIONS} PARENT_SCOPE)
	set(${prefix}_VERSIONS_EXACT ${FIND_VERSIONS_EXACT} PARENT_SCOPE)
	set(${prefix}_VERSIONS_MINIMUM ${FIND_VERSIONS_MINIMUM} PARENT_SCOPE)
  endif()
endfunction()
