set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/segmentator
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/region_refiner
    ${CMAKE_SOURCE_DIR}/common/include
)

include_directories(${INCLUDE_DIRS})

add_executable(test_segmentator test_segmentator.cpp)
target_link_libraries(test_segmentator
    VMPilot_SDK_Segmentator
    VMPilot_SDK_LIB
    capstone_wrapper
)

add_executable(test_region_refiner test_region_refiner.cpp)
target_link_libraries(test_region_refiner
    VMPilot_SDK_RegionRefiner
)
