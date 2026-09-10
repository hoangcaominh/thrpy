cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(thrpy VERSION 0.0.1)

set(TARGET_BIN "${PROJECT_NAME}-bin")
set(TARGET_TEST "${PROJECT_NAME}-test")

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 063de7e9578f82b369302001269680b4b1553359
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

configure_file(config.h.in config.h)

add_subdirectory(vendor)
# add_subdirectory(ksy)

add_library(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        # src/main.c
        src/rpy.c
        src/detect.c
        src/crypt.c
        src/lzss.c
        src/bitstream.c
        src/bitstream.h
        src/rpy/thmodern.c
        src/rpy/thmodern.h
        src/rpy/thmodern20.c
        src/rpy/thmodern20.h
        src/rpy/th06.c
        src/rpy/th07.c
        src/rpy/th08.c
        src/rpy/th10.c
        src/rpy/th11.c
        src/rpy/th12.c
        src/rpy/th128.c
        src/rpy/th13.c
        src/rpy/th14.c
        src/rpy/th15.c
        src/rpy/th16.c
        src/rpy/th17.c
        src/rpy/th18.c
        src/rpy/th20.c
    PUBLIC
        include/thrpy.h
        include/rpy.h
        include/detect.h
        include/crypt.h
        include/lzss.h
        include/rpy/th06.h
        include/rpy/th07.h
        include/rpy/th08.h
        include/rpy/th10.h
        include/rpy/th11.h
        include/rpy/th12.h
        include/rpy/th128.h
        include/rpy/th13.h
        include/rpy/th14.h
        include/rpy/th15.h
        include/rpy/th16.h
        include/rpy/th17.h
        include/rpy/th18.h
        include/rpy/th20.h
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE src ${PROJECT_BINARY_DIR}
    PUBLIC include
)
# target_link_libraries(
#     ${PROJECT_NAME}
#     PRIVATE ksy
# )

# Executable
add_executable(${TARGET_BIN} src/main.c)
set_target_properties(${TARGET_BIN} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_include_directories(
    ${TARGET_BIN}
    PRIVATE src ${PROJECT_BINARY_DIR}
    PUBLIC include
)
target_link_libraries(
    ${TARGET_BIN}
    PRIVATE ${PROJECT_NAME}
)

# Test
enable_testing()
add_executable(
    ${TARGET_TEST}
    tests/gtest/helper.h
    tests/gtest/test_th06.cpp
    tests/gtest/test_th07.cpp
    tests/gtest/test_th08.cpp
    tests/gtest/test_th10.cpp
    tests/gtest/test_th11.cpp
    tests/gtest/test_th12.cpp
    tests/gtest/test_th128.cpp
    tests/gtest/test_th13.cpp
    tests/gtest/test_th14.cpp
    tests/gtest/test_th15.cpp
    tests/gtest/test_th16.cpp
    tests/gtest/test_th17.cpp
    tests/gtest/test_th18.cpp
    tests/gtest/test_th20.cpp
)
target_include_directories(
    ${TARGET_TEST}
    PRIVATE ${PROJECT_BINARY_DIR}
    PUBLIC include
)
target_link_libraries(
    ${TARGET_TEST}
    PRIVATE ${PROJECT_NAME} GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(${TARGET_TEST})
