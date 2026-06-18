cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(thrpy)

add_subdirectory(vendor)
add_subdirectory(ksy)

add_executable(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/main.cpp
        src/rpy.cpp
        src/impl/crypt_impl.c
        src/impl/crypt_impl.h
        src/crypt.cpp
        src/lzss.cpp
        src/rpy/th06.cpp
        src/rpy/th07.cpp
        src/rpy/th08.cpp
    PUBLIC
        include/thrpy.h
        include/rpy.h
        include/crypt.h
        include/lzss.h
        include/rpy/th06.h
        include/rpy/th07.h
        include/rpy/th08.h
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE src
    PUBLIC include
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE ksy
)