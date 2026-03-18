set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/file_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/arch_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/segmentator
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/region_refiner
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/serializer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/bytecode_compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/capstone_wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

include_directories(${INCLUDE_DIRS})

# Third-party header-only libs — mark SYSTEM to suppress warnings
include_directories(SYSTEM
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/
    ${spdlog_SOURCE_DIR}/include
)

add_executable(dump_regions dump_regions.cpp)
target_link_libraries(dump_regions
    VMPilot_SDK_Segmentator
    VMPilot_SDK_RegionRefiner
    capstone_wrapper
    opcode_table
)

add_executable(dump_compile dump_compile.cpp)
if (MSVC)
    target_compile_options(dump_compile PRIVATE /WX-)
endif()
target_link_libraries(dump_compile VMPilot_SDK_LIB opcode_table)

add_executable(verify_roundtrip verify_roundtrip.cpp)
if (MSVC)
    target_compile_options(verify_roundtrip PRIVATE /WX-)
endif()
target_link_libraries(verify_roundtrip VMPilot_SDK_LIB opcode_table)
