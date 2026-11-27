cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(thrpy VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and configure dependencies
set(THRPY_DEPENDENCIES)
include(FetchContent)
FetchContent_Declare(
    thrpy_proto
    GIT_REPOSITORY  https://github.com/hoangcaominh/thrpy-proto.git
    GIT_TAG         7ce02ff4c69e4faed61ba929180203f803106688
)
list(APPEND THRPY_DEPENDENCIES thrpy_proto)

FetchContent_Declare(
    kaitai_struct_cpp_stl_runtime
    GIT_REPOSITORY  https://github.com/kaitai-io/kaitai_struct_cpp_stl_runtime.git
    GIT_TAG         c7009877d4b0ca804442e796c996eb4501f3203d # v0.11
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library for kaitai struct")
set(BUILD_TESTS OFF CACHE BOOL "Disable test build for kaitai struct")
if(WIN32)
    set(STRING_ENCODING_TYPE "WIN32API" CACHE STRING "Set the way strings have to be encoded (ICONV|WIN32API|NONE|...)")
endif()
list(APPEND THRPY_DEPENDENCIES kaitai_struct_cpp_stl_runtime)

FetchContent_MakeAvailable(${THRPY_DEPENDENCIES})

set(GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GEN_DIR})

# protobuf
file(GLOB PROTO_FILES ${thrpy_proto_SOURCE_DIR}/*.proto)

set(PROTO_GEN_DIR "${GEN_DIR}/proto")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

find_package(protobuf REQUIRED CONFIG)

# kaitai-struct-cpp
set(KSY_SOURCE_DIR "${CMAKE_SOURCE_DIR}/ksy")
set(KSY_GEN_DIR "${GEN_DIR}/ksy")
file(MAKE_DIRECTORY ${KSY_GEN_DIR})

file(GLOB KSY_FILES ${KSY_SOURCE_DIR}/*.ksy)
set(KSY_GEN_HEADER_LIST)
set(KSY_GEN_SOURCE_LIST)

# kaitai-struct-compiler is a .bat file on Windows
if(WIN32)
    set(KSY_COMPILER_CMD cmd /c call kaitai-struct-compiler.bat)
else()
    set(KSY_COMPILER_CMD kaitai-struct-compiler)
endif()

foreach(KSY_FILE ${KSY_FILES})
    get_filename_component(KSY_BASENAME ${KSY_FILE} NAME_WE)
    set(KSY_GEN_HEADER "${KSY_GEN_DIR}/${KSY_BASENAME}.h")
    set(KSY_GEN_SOURCE "${KSY_GEN_DIR}/${KSY_BASENAME}.cpp")

    add_custom_command(
        WORKING_DIRECTORY ${KSY_GEN_DIR}
        OUTPUT ${KSY_GEN_HEADER} ${KSY_GEN_SOURCE}
        COMMAND ${KSY_COMPILER_CMD} -t cpp_stl ${KSY_FILE}
        DEPENDS ${KSY_FILE}
    )

    list(APPEND KSY_GEN_HEADER_LIST ${KSY_GEN_HEADER})
    list(APPEND KSY_GEN_SOURCE_LIST ${KSY_GEN_SOURCE})
endforeach()

# thrpy library
option(thrpy_BUILD_TESTS "Build tests" OFF)

configure_file(config.h.in "${GEN_DIR}/config.h")

add_library(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/rpy.c
        src/detect.c
        src/crypt.c
        src/lzss.c
        src/bitstream.c
        src/bitstream.h
        src/msgutils.cpp
        src/msgutils.h
        src/rpy/thmodern.c
        src/rpy/thmodern20.c
        src/rpy/th06.cpp
        src/rpy/th07.cpp
        src/rpy/th08.cpp
        src/rpy/th10.cpp
        src/rpy/th11.cpp
        src/rpy/th12.cpp
        src/rpy/th128.cpp
        src/rpy/th13.cpp
        src/rpy/th14.cpp
        src/rpy/th15.cpp
        src/rpy/th16.cpp
        src/rpy/th17.cpp
        src/rpy/th18.cpp
        src/rpy/th20.cpp
        ${KSY_GEN_SOURCE_LIST}
        ${KSY_GEN_HEADER_LIST}
        ${PROTO_FILES}
    PUBLIC
        include/thrpy.h
        include/rpy.h
        include/detect.h
        include/crypt.h
        include/lzss.h
        include/rpy/thmodern.h
        include/rpy/thmodern20.h
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
protobuf_generate(
    TARGET ${PROJECT_NAME}
    LANGUAGE cpp
    IMPORT_DIRS "${thrpy_proto_SOURCE_DIR}"
    PROTOC_OUT_DIR ${PROTO_GEN_DIR}
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE src ${GEN_DIR}
    PUBLIC include
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        kaitai_struct_cpp_stl_runtime
        protobuf::libprotobuf
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

# Install
include(GNUInstallDirs)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
)
set_target_properties(
    ${TARGET_BIN} PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
)

install(
    TARGETS ${TARGET_BIN} ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
