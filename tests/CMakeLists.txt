
enable_testing()
set(TARGET_TEST "${PROJECT_NAME}-test")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 063de7e9578f82b369302001269680b4b1553359
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    ${TARGET_TEST}
    gtest/helper.h
    gtest/test_th06.cpp
    gtest/test_th07.cpp
    gtest/test_th08.cpp
    gtest/test_th10.cpp
    gtest/test_th11.cpp
    gtest/test_th12.cpp
    gtest/test_th128.cpp
    gtest/test_th13.cpp
    gtest/test_th14.cpp
    gtest/test_th15.cpp
    gtest/test_th16.cpp
    gtest/test_th17.cpp
    gtest/test_th18.cpp
    gtest/test_th20.cpp
    gtest/test_identical.cpp
)
target_include_directories(
    ${TARGET_TEST}
    PRIVATE ${GEN_DIR}
)
target_link_libraries(
    ${TARGET_TEST}
    PRIVATE ${PROJECT_NAME} GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(${TARGET_TEST})
