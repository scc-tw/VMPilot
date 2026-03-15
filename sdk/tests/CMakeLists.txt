set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/segmentator
    ${CMAKE_SOURCE_DIR}/common/include
)

include_directories(${INCLUDE_DIRS})

add_executable(test_segmentator test_segmentator.cpp)
target_link_libraries(test_segmentator
    VMPilot_SDK_Segmentator
    VMPilot_SDK_LIB
    capstone_wrapper
)
