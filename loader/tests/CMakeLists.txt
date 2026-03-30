set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

include_directories(${INCLUDE_DIRS})

# Unit tests (no external dependencies)
set(TEST_NAME test_loader)
add_executable(${TEST_NAME} test_loader.cpp)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader opcode_table GTest::gtest_main)
gtest_discover_tests(${TEST_NAME})

# Handover verification tests (real Mach-O binary, decode patched bytes)
set(HANDOVER_TEST test_handover)
add_executable(${HANDOVER_TEST} test_handover.cpp)
target_link_libraries(${HANDOVER_TEST} PRIVATE VMPilot_Loader opcode_table GTest::gtest_main)
gtest_discover_tests(${HANDOVER_TEST})
