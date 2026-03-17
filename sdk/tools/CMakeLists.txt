set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/file_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/arch_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/segmentator
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/region_refiner
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/capstone_wrapper
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/
    ${spdlog_SOURCE_DIR}/include
)

include_directories(${INCLUDE_DIRS})

add_executable(dump_regions dump_regions.cpp)
target_link_libraries(dump_regions
    VMPilot_SDK_Segmentator
    VMPilot_SDK_RegionRefiner
    capstone_wrapper
    opcode_table
)
