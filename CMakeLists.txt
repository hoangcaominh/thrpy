cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(thrpy VERSION 0.0.1)

set(GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GEN_DIR})

# Options
option(thrpy_BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(config.h.in "${GEN_DIR}/config.h")

# protobuf
set(PROTO_GEN_DIR "${GEN_DIR}/proto")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/vendor")
set(absl_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor/protobuf/lib/cmake/absl")
set(utf8_range_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor/protobuf/lib/cmake/utf8_range")

find_package(protobuf REQUIRED CONFIG)

add_subdirectory(vendor)
# add_subdirectory(ksy)

# kaitai-struct-cpp
set(KSY_SOURCE_DIR "${CMAKE_SOURCE_DIR}/ksy")
set(KSY_GEN_DIR "${GEN_DIR}/ksy")
file(MAKE_DIRECTORY ${KSY_GEN_DIR})

file(GLOB KSY_FILES ${KSY_SOURCE_DIR}/*.ksy)
set(KSY_GEN_HEADER_LIST)
set(KSY_GEN_SOURCE_LIST)
foreach(KSY_FILE ${KSY_FILES})
    get_filename_component(KSY_BASENAME ${KSY_FILE} NAME_WE)
    set(KSY_GEN_HEADER "${KSY_GEN_DIR}/${KSY_BASENAME}.h")
    set(KSY_GEN_SOURCE "${KSY_GEN_DIR}/${KSY_BASENAME}.cpp")

    add_custom_command(
        WORKING_DIRECTORY ${KSY_GEN_DIR}
        OUTPUT ${KSY_GEN_HEADER} ${KSY_GEN_SOURCE}
        COMMAND kaitai-struct-compiler -t cpp_stl ${KSY_FILE}
    )

    list(APPEND KSY_GEN_HEADER_LIST ${KSY_GEN_HEADER})
    list(APPEND KSY_GEN_SOURCE_LIST ${KSY_GEN_SOURCE})
endforeach()

# thrpy library
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
        ${KSY_GEN_SOURCE_LIST}
        proto/th06.proto
        proto/thdef.proto
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
        ${KSY_GEN_HEADER_LIST}
)
protobuf_generate(
    TARGET ${PROJECT_NAME}
    LANGUAGE cpp
    IMPORT_DIRS "${CMAKE_SOURCE_DIR}/proto"
    PROTOC_OUT_DIR ${PROTO_GEN_DIR}
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE src ${GEN_DIR}
    PUBLIC include
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE vendor
)

# Executable
set(TARGET_BIN "${PROJECT_NAME}-bin")
add_executable(${TARGET_BIN} src/main.c)
set_target_properties(${TARGET_BIN} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_include_directories(
    ${TARGET_BIN}
    PRIVATE src ${GEN_DIR}
)
target_link_libraries(
    ${TARGET_BIN}
    PRIVATE ${PROJECT_NAME}
)

# Test
if (thrpy_BUILD_TESTS)
    add_subdirectory(tests)
endif()
