add_library(usermod_ulab INTERFACE)

file(GLOB_RECURSE ULAB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c)

target_sources(usermod_ulab INTERFACE
    ${ULAB_SOURCES}
)

target_include_directories(usermod_ulab INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_ulab INTERFACE
    MODULE_ULAB_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_ulab)

