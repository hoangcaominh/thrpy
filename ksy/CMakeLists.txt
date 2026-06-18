set(KSY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(KSY_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB KSY_FILES ${KSY_SOURCE_DIR}/*.ksy)
set(KSY_GEN_HEADER_LIST)
set(KSY_GEN_SOURCE_LIST)
foreach(KSY_FILE ${KSY_FILES})
    get_filename_component(KSY_BASENAME ${KSY_FILE} NAME_WE)
    set(KSY_GEN_HEADER "${KSY_GEN_DIR}/${KSY_BASENAME}.h")
    set(KSY_GEN_SOURCE "${KSY_GEN_DIR}/${KSY_BASENAME}.cpp")

    add_custom_command(
        OUTPUT ${KSY_GEN_HEADER} ${KSY_GEN_SOURCE}
        COMMAND kaitai-struct-compiler -t cpp_stl ${KSY_FILE}
    )

    list(APPEND KSY_GEN_HEADER_LIST ${KSY_GEN_HEADER})
    list(APPEND KSY_GEN_HEADER_LIST ${KSY_GEN_SOURCE})
endforeach()

add_library(ksy)
target_sources(
    ksy
    PRIVATE ${KSY_GEN_SOURCE_LIST}
    PUBLIC ${KSY_GEN_HEADER_LIST}
)
target_include_directories(ksy PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(ksy PUBLIC kaitai_struct_cpp_stl_runtime)
