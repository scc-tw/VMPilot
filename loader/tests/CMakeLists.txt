set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/sdk/include/core
    ${CMAKE_SOURCE_DIR}/sdk/include/bytecode_compiler
    ${CMAKE_SOURCE_DIR}/sdk/include/segmentator
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

include_directories(${INCLUDE_DIRS})

set(TEST_NAME test_loader)
add_executable(${TEST_NAME} test_loader.cpp)
target_link_libraries(${TEST_NAME} PRIVATE VMPilot_Loader opcode_table GTest::gtest_main)
gtest_discover_tests(${TEST_NAME})
