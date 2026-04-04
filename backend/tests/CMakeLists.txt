set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/backend/include
    ${CMAKE_SOURCE_DIR}/backend/include/bytecode_compiler
    ${CMAKE_SOURCE_DIR}/frontend/include
    ${CMAKE_SOURCE_DIR}/frontend/include/segmentator
    ${CMAKE_SOURCE_DIR}/frontend/include/region_refiner
    ${CMAKE_SOURCE_DIR}/frontend/include/reference_analyzer
    ${CMAKE_SOURCE_DIR}/frontend/include/capstone_wrapper
    ${CMAKE_SOURCE_DIR}/serializer/include/serializer
    ${CMAKE_SOURCE_DIR}/common/include/core
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

include_directories(${INCLUDE_DIRS})
include_directories(SYSTEM ${spdlog_SOURCE_DIR}/include)

add_compile_definitions(
    TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/data/basic/bin"
    TEST_DATA_MULTI_REGION="${CMAKE_SOURCE_DIR}/data/multi_region/bin"
    TEST_DATA_INLINE_HEAVY="${CMAKE_SOURCE_DIR}/data/inline_heavy/bin"
    TEST_DATA_NESTED_PROTECT="${CMAKE_SOURCE_DIR}/data/nested_protect/bin"
    TEST_DATA_DATA_REFS="${CMAKE_SOURCE_DIR}/data/data_refs/bin"
)

include(GoogleTest)
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)

add_executable(test_bytecode_compiler test_bytecode_compiler.cpp)
target_link_libraries(test_bytecode_compiler GTest::gtest_main VMPilot_Frontend VMPilot_Backend VMPilot_SDK_Serializer VMPilot_Common)
gtest_discover_tests(test_bytecode_compiler)

add_executable(test_compiler_backend test_compiler_backend.cpp)
target_link_libraries(test_compiler_backend GTest::gtest_main VMPilot_SDK_Bytecode_Compiler VMPilot_Common)
gtest_discover_tests(test_compiler_backend)

add_executable(test_compilation_orchestrator test_compilation_orchestrator.cpp)
target_link_libraries(test_compilation_orchestrator GTest::gtest_main VMPilot_SDK_Bytecode_Compiler VMPilot_Common)
gtest_discover_tests(test_compilation_orchestrator)

add_executable(test_compile_pipeline test_compile_pipeline.cpp)
suppress_thirdparty_warnings(test_compile_pipeline)
target_link_libraries(test_compile_pipeline GTest::gtest_main VMPilot_Frontend VMPilot_Backend VMPilot_SDK_Serializer VMPilot_Common)
gtest_discover_tests(test_compile_pipeline)

add_executable(test_integration_pipeline test_integration_pipeline.cpp)
suppress_thirdparty_warnings(test_integration_pipeline)
target_include_directories(test_integration_pipeline PRIVATE
    ${CMAKE_SOURCE_DIR}/serializer/include/serializer
)
target_link_libraries(test_integration_pipeline GTest::gtest_main VMPilot_Frontend VMPilot_Backend VMPilot_SDK_Serializer VMPilot_Common)
gtest_discover_tests(test_integration_pipeline)
