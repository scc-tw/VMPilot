set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/file_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/arch_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/segmentator
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/region_refiner
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/capstone_wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/bytecode_compiler
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

# Pass test data paths as compile definitions
add_compile_definitions(
    TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/data/basic/bin"
    TEST_DATA_MULTI_REGION="${CMAKE_SOURCE_DIR}/data/multi_region/bin"
    TEST_DATA_INLINE_HEAVY="${CMAKE_SOURCE_DIR}/data/inline_heavy/bin"
    TEST_DATA_NESTED_PROTECT="${CMAKE_SOURCE_DIR}/data/nested_protect/bin"
    TEST_DATA_DATA_REFS="${CMAKE_SOURCE_DIR}/data/data_refs/bin"
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
target_link_libraries(test_segmentator_integration GTest::gtest_main VMPilot_SDK_LIB)

add_executable(test_thread_pool test_thread_pool.cpp)
target_link_libraries(test_thread_pool GTest::gtest_main opcode_table)

add_executable(test_compilation_unit test_compilation_unit.cpp)
target_link_libraries(test_compilation_unit GTest::gtest_main)

add_executable(test_compiler_backend test_compiler_backend.cpp)
target_link_libraries(test_compiler_backend GTest::gtest_main VMPilot_SDK_Bytecode_Compiler opcode_table)

add_executable(test_compilation_orchestrator test_compilation_orchestrator.cpp)
target_link_libraries(test_compilation_orchestrator GTest::gtest_main VMPilot_SDK_Bytecode_Compiler opcode_table)

add_executable(test_compile_pipeline test_compile_pipeline.cpp)
suppress_thirdparty_warnings(test_compile_pipeline)
target_link_libraries(test_compile_pipeline
    GTest::gtest_main
    VMPilot_SDK_LIB
)

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

add_executable(test_segment_api test_segment_api.cpp)
target_link_libraries(test_segment_api GTest::gtest_main VMPilot_SDK_Segmentator VMPilot_SDK_RegionRefiner capstone_wrapper opcode_table)

include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)

add_executable(test_serializer test_serializer.cpp)
suppress_thirdparty_warnings(test_serializer)
target_include_directories(test_serializer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/serializer
)
target_link_libraries(test_serializer
    GTest::gtest_main
    VMPilot_SDK_Serializer
    VMPilot_SDK_Segmentator
    VMPilot_SDK_RegionRefiner
    capstone_wrapper
    opcode_table
)

add_executable(test_write_classification test_write_classification.cpp)
target_link_libraries(test_write_classification GTest::gtest_main capstone_wrapper)

add_executable(test_atomic_traits test_atomic_traits.cpp)
target_include_directories(test_atomic_traits PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/traits
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
)
target_link_libraries(test_atomic_traits GTest::gtest_main capstone_wrapper)

add_executable(test_phase4e test_phase4e.cpp)
target_include_directories(test_phase4e PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_phase4e GTest::gtest_main)

add_executable(test_pattern_layer test_pattern_layer.cpp)
target_include_directories(test_pattern_layer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/traits
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/layers
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_pattern_layer
    GTest::gtest_main
    VMPilot_SDK_ReferenceAnalyzer
    VMPilot_SDK_Segmentator
    capstone_wrapper
    opcode_table
)

add_executable(test_insn_layer test_insn_layer.cpp)
target_include_directories(test_insn_layer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/traits
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/layers
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_insn_layer
    GTest::gtest_main
    VMPilot_SDK_ReferenceAnalyzer
    capstone_wrapper
)

add_executable(test_relocation_layer test_relocation_layer.cpp)
target_include_directories(test_relocation_layer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/traits
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer/layers
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
)
target_link_libraries(test_relocation_layer GTest::gtest_main)

add_executable(test_sym_expr test_sym_expr.cpp)
target_include_directories(test_sym_expr PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_sym_expr
    GTest::gtest_main
    VMPilot_SDK_ReferenceAnalyzer
)

add_executable(test_symbolic_resolver test_symbolic_resolver.cpp)
target_include_directories(test_symbolic_resolver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_symbolic_resolver
    GTest::gtest_main
    VMPilot_SDK_ReferenceAnalyzer
    VMPilot_SDK_Segmentator
    capstone_wrapper
    opcode_table
)

add_executable(test_memory_model test_memory_model.cpp)
target_include_directories(test_memory_model PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_memory_model
    GTest::gtest_main
    VMPilot_SDK_ReferenceAnalyzer
)

add_executable(test_reference_analyzer test_reference_analyzer.cpp)
target_include_directories(test_reference_analyzer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/reference_analyzer
)
target_link_libraries(test_reference_analyzer
    GTest::gtest_main
    VMPilot_SDK_ReferenceAnalyzer
    VMPilot_SDK_Segmentator
    capstone_wrapper
    opcode_table
)

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
gtest_discover_tests(test_segment_api)
gtest_discover_tests(test_serializer)
gtest_discover_tests(test_thread_pool)
gtest_discover_tests(test_compilation_unit)
gtest_discover_tests(test_compiler_backend)
gtest_discover_tests(test_compilation_orchestrator)
gtest_discover_tests(test_compile_pipeline)
gtest_discover_tests(test_reference_analyzer)
gtest_discover_tests(test_write_classification)
gtest_discover_tests(test_memory_model)
gtest_discover_tests(test_sym_expr)
gtest_discover_tests(test_symbolic_resolver)
gtest_discover_tests(test_relocation_layer)
gtest_discover_tests(test_atomic_traits)
gtest_discover_tests(test_insn_layer)
gtest_discover_tests(test_pattern_layer)
gtest_discover_tests(test_phase4e)

add_executable(test_integration_pipeline test_integration_pipeline.cpp)
suppress_thirdparty_warnings(test_integration_pipeline)
target_include_directories(test_integration_pipeline PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/serializer
)
target_link_libraries(test_integration_pipeline
    GTest::gtest_main
    VMPilot_SDK_LIB
    opcode_table
)
gtest_discover_tests(test_integration_pipeline)
