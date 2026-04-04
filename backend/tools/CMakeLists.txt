set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/backend/include
    ${CMAKE_SOURCE_DIR}/common/include/core
    ${CMAKE_SOURCE_DIR}/frontend/include/file_handler
    ${CMAKE_SOURCE_DIR}/frontend/include/arch_handler
    ${CMAKE_SOURCE_DIR}/frontend/include/segmentator
    ${CMAKE_SOURCE_DIR}/frontend/include/region_refiner
    ${CMAKE_SOURCE_DIR}/serializer/include/serializer
    ${CMAKE_SOURCE_DIR}/backend/include/bytecode_compiler
    ${CMAKE_SOURCE_DIR}/frontend/include/capstone_wrapper
    ${CMAKE_SOURCE_DIR}/frontend/include/reference_analyzer
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

include_directories(${INCLUDE_DIRS})

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
target_link_libraries(dump_compile VMPilot_Frontend VMPilot_Backend VMPilot_SDK_Serializer VMPilot_Common)

add_executable(verify_roundtrip verify_roundtrip.cpp)
suppress_thirdparty_warnings(verify_roundtrip)
target_link_libraries(verify_roundtrip VMPilot_Frontend VMPilot_Backend VMPilot_SDK_Serializer VMPilot_Common)
