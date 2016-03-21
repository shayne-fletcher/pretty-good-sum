find_package(Robo)
find_package(GoogleTest REQUIRED) #Google test

# Library

SET(PGS_TESTS_CPP 
#    expression.t.cpp
#    match.t.cpp
#    get.t.cpp
#    list.t.cpp
#    tree.t.cpp
#    array.t.cpp
#    sexpr.t.cpp
#    option.t.cpp
#    move.t.cpp
#    copy.t.cpp
#    main.t.cpp
     simple.cpp
)
ADD_EXECUTABLE(pgs_test ${PGS_TESTS_CPP})
TARGET_INCLUDE_DIRECTORIES(pgs_test PUBLIC ${CMAKE_SOURCE_DIR}/src)
TARGET_INCLUDE_DIRECTORIES(pgs_test SYSTEM PRIVATE ${GTEST_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(pgs_test ${GTEST_LIBRARIES})

INSTALL(TARGETS pgs_test
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${INSTALL_LIB_DIR_SUFFIX}
    ARCHIVE DESTINATION lib${INSTALL_LIB_DIR_SUFFIX}
)

