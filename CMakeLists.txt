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
        src/bitstream.h
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