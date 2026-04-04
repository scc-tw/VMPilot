set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/serializer/include/serializer
    ${CMAKE_SOURCE_DIR}/frontend/include/segmentator
    ${CMAKE_SOURCE_DIR}/frontend/include/region_refiner
    ${CMAKE_SOURCE_DIR}/frontend/include/capstone_wrapper
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

add_executable(test_serializer test_serializer.cpp)
suppress_thirdparty_warnings(test_serializer)
target_link_libraries(test_serializer
    GTest::gtest_main
    VMPilot_SDK_Serializer
    VMPilot_SDK_Segmentator
    VMPilot_SDK_RegionRefiner
    capstone_wrapper
    VMPilot_Common
)
gtest_discover_tests(test_serializer)
