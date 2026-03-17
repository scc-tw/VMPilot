set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/segmentator
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/region_refiner
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/capstone_wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/bytecode_compiler
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/
    ${spdlog_SOURCE_DIR}/include
)

include_directories(${INCLUDE_DIRS})

# Pass test data path as compile definition
add_compile_definitions(
    TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/data/basic/bin"
)

# --- Test targets ---

add_executable(test_native_function_base test_native_function_base.cpp)
target_link_libraries(test_native_function_base GTest::gtest_main)

add_executable(test_native_symbol_table test_native_symbol_table.cpp)
target_link_libraries(test_native_symbol_table GTest::gtest_main)

add_executable(test_capstone_wrapper test_capstone_wrapper.cpp)
target_link_libraries(test_capstone_wrapper GTest::gtest_main capstone_wrapper)

add_executable(test_elf_handler test_elf_handler.cpp)
target_link_libraries(test_elf_handler GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

add_executable(test_x86_handler test_x86_handler.cpp)
target_link_libraries(test_x86_handler GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

add_executable(test_region_refiner test_region_refiner.cpp)
target_link_libraries(test_region_refiner GTest::gtest_main VMPilot_SDK_RegionRefiner)

add_executable(test_region_grouping test_region_grouping.cpp)
target_link_libraries(test_region_grouping GTest::gtest_main VMPilot_SDK_RegionRefiner)

add_executable(test_strategy test_strategy.cpp)
target_link_libraries(test_strategy GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

add_executable(test_bytecode_compiler test_bytecode_compiler.cpp)
target_link_libraries(test_bytecode_compiler GTest::gtest_main VMPilot_SDK_LIB)

add_executable(test_segmentator_integration test_segmentator_integration.cpp)
target_link_libraries(test_segmentator_integration GTest::gtest_main VMPilot_SDK_Segmentator VMPilot_SDK_RegionRefiner VMPilot_SDK_LIB capstone_wrapper opcode_table)

add_executable(test_macho_handler test_macho_handler.cpp)
target_link_libraries(test_macho_handler GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

add_executable(test_arm64_handler test_arm64_handler.cpp)
target_link_libraries(test_arm64_handler GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

add_executable(test_pe_handler test_pe_handler.cpp)
target_link_libraries(test_pe_handler GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

add_executable(test_readonly_section test_readonly_section.cpp)
target_link_libraries(test_readonly_section GTest::gtest_main)

add_executable(test_string_extraction test_string_extraction.cpp)
target_link_libraries(test_string_extraction GTest::gtest_main VMPilot_SDK_Segmentator capstone_wrapper opcode_table)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(test_native_function_base)
gtest_discover_tests(test_readonly_section)
gtest_discover_tests(test_native_symbol_table)
gtest_discover_tests(test_capstone_wrapper)
gtest_discover_tests(test_elf_handler)
gtest_discover_tests(test_x86_handler)
gtest_discover_tests(test_region_refiner)
gtest_discover_tests(test_region_grouping)
gtest_discover_tests(test_strategy)
gtest_discover_tests(test_bytecode_compiler)
gtest_discover_tests(test_macho_handler)
gtest_discover_tests(test_arm64_handler)
gtest_discover_tests(test_pe_handler)
gtest_discover_tests(test_segmentator_integration)
gtest_discover_tests(test_string_extraction)
