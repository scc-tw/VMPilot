add_executable(vmpilot_bench
    bench_main.cpp
    program_factory.cpp
)

target_include_directories(vmpilot_bench
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/..
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
            ${CMAKE_CURRENT_SOURCE_DIR}/../test
)

target_link_libraries(vmpilot_bench
    PRIVATE VMPilot_Runtime
            vmpilot_options
)

set_target_properties(vmpilot_bench PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
