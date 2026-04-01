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
    ${spdlog_SOURCE_DIR}/include
)

add_executable(dump_regions dump_regions.cpp)
target_link_libraries(dump_regions
    VMPilot_SDK_Segmentator
    VMPilot_SDK_RegionRefiner
    capstone_wrapper
    VMPilot_Common
)

include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)

add_executable(dump_compile dump_compile.cpp)
suppress_thirdparty_warnings(dump_compile)
target_link_libraries(dump_compile VMPilot_SDK_LIB VMPilot_Common)

add_executable(verify_roundtrip verify_roundtrip.cpp)
suppress_thirdparty_warnings(verify_roundtrip)
target_link_libraries(verify_roundtrip VMPilot_SDK_LIB VMPilot_Common)
