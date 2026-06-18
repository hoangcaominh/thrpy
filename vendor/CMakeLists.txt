set(BUILD_TESTS OFF CACHE BOOL "Disable test build")
add_subdirectory(kaitai_struct_cpp_stl_runtime)

add_library(vendor INTERFACE)
target_include_directories(
    vendor
    INTERFACE
)
target_link_libraries(
    vendor
    INTERFACE
    kaitai_struct_cpp_stl_runtime
)
