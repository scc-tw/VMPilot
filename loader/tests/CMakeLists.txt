set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

include_directories(${INCLUDE_DIRS})

# Suppress warnings-as-errors from third-party headers (coffi C4754 on MSVC)
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)

# Unit tests
set(TEST_NAME test_loader)
add_executable(${TEST_NAME} test_loader.cpp)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader VMPilot_Common GTest::gtest_main)
gtest_discover_tests(${TEST_NAME})

# Integration / handover tests
set(TEST_NAME test_handover)
add_executable(${TEST_NAME} test_handover.cpp)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader VMPilot_Common GTest::gtest_main)
gtest_discover_tests(${TEST_NAME})

# .text extension tests
set(TEST_NAME test_extend_text)
add_executable(${TEST_NAME} test_extend_text.cpp)
target_include_directories(${TEST_NAME} SYSTEM PRIVATE
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader VMPilot_Common GTest::gtest_main)
suppress_thirdparty_warnings(${TEST_NAME})
gtest_discover_tests(${TEST_NAME})

# End-to-end patch() integration tests
set(TEST_NAME test_patch_e2e)
add_executable(${TEST_NAME} test_patch_e2e.cpp)
target_include_directories(${TEST_NAME} SYSTEM PRIVATE
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader VMPilot_Common GTest::gtest_main)
suppress_thirdparty_warnings(${TEST_NAME})
gtest_discover_tests(${TEST_NAME})

# Editor permission tests (W^X compliance)
set(TEST_NAME test_editor_permissions)
add_executable(${TEST_NAME} test_editor_permissions.cpp)
target_include_directories(${TEST_NAME} SYSTEM PRIVATE
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader VMPilot_Common GTest::gtest_main)
suppress_thirdparty_warnings(${TEST_NAME})
gtest_discover_tests(${TEST_NAME})

# Phase 5: FallbackChain + CRTP framework tests
set(TEST_NAME test_phase5_framework)
add_executable(${TEST_NAME} test_phase5_framework.cpp)
target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/strategies)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader VMPilot_Common GTest::gtest_main)
gtest_discover_tests(${TEST_NAME})
