IF(NOT MSVC)
  return()
ENDIF()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO" )
  
set(flag_var
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL 
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
)

foreach(var ${flag_var})
    string(REGEX REPLACE "/MP" "" ${var} "${${var}}")
    string(REGEX REPLACE "/Z[iI7]" "" ${var} "${${var}}")
endforeach(var)
  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE")

if(BUILD_WITH_STATIC_RUNTIME)
    foreach(var ${flag_var})
        if(${var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
        endif()
    endforeach(var)
endif()
