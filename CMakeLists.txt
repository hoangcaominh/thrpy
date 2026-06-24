cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(thrpy VERSION 0.0.1)

configure_file(config.h.in config.h)

add_subdirectory(vendor)
# add_subdirectory(ksy)

add_executable(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/main.c
        src/rpy.c
        src/detect.c
        src/crypt.c
        src/lzss.c
        src/bitstream.c
        src/rpy/th06.c
        src/rpy/th07.c
        src/rpy/th08.c
    PUBLIC
        include/thrpy.h
        include/rpy.h
        include/detect.h
        include/crypt.h
        include/lzss.h
        src/bitstream.h
        include/rpy/th06.h
        include/rpy/th07.h
        include/rpy/th08.h
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