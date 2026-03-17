set(TARGET_NAME VMPilot_SDK_RegionRefiner)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/RegionRefiner.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/region_refiner
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core
    ${spdlog_SOURCE_DIR}/include
)

include_directories(${INCLUDE_DIRS})

add_library(${TARGET_NAME} STATIC ${SRC_FILES})
target_link_libraries(${TARGET_NAME} spdlog)
