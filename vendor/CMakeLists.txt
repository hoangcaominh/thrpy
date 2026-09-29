set(BUILD_TESTS OFF CACHE BOOL "Disable test build")

if(WIN32)
    set(STRING_ENCODING_TYPE "WIN32API" CACHE STRING "Set the way strings have to be encoded (ICONV|WIN32API|NONE|...)")
endif()

add_subdirectory(kaitai_struct_cpp_stl_runtime)

add_library(vendor INTERFACE)
target_link_libraries(
    vendor
    INTERFACE
    kaitai_struct_cpp_stl_runtime
    protobuf::libprotobuf
)
