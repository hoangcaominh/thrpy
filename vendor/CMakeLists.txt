set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library for kaitai struct")
set(BUILD_TESTS OFF CACHE BOOL "Disable test build for kaitai struct")

if(WIN32)
    set(STRING_ENCODING_TYPE "WIN32API" CACHE STRING "Set the way strings have to be encoded (ICONV|WIN32API|NONE|...)")
endif()

add_subdirectory(kaitai_struct_cpp_stl_runtime)

if(WIN32)
    target_compile_options(
        kaitai_struct_cpp_stl_runtime PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/WX->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-error>
    )
endif()

add_library(vendor INTERFACE)
target_link_libraries(
    vendor
    INTERFACE
    kaitai_struct_cpp_stl_runtime
    protobuf::libprotobuf
)
