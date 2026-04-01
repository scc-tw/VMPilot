file(GLOB_RECURSE RUNTIME_EXAMPLE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

foreach(_example_src IN LISTS RUNTIME_EXAMPLE_SOURCES)
    get_filename_component(_example_name "${_example_src}" NAME_WE)
    set(_example_target "${_example_name}_example")

    add_executable(${_example_target} ${_example_src})
    target_include_directories(${_example_target}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/..
                ${CMAKE_CURRENT_SOURCE_DIR}/../include
                ${CMAKE_CURRENT_SOURCE_DIR}/../test
    )
    target_link_libraries(${_example_target}
        PRIVATE VMPilot_Runtime
    )
endforeach()
