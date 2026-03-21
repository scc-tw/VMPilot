set(TARGET_NAME VMPilot_SDK_ReferenceAnalyzer)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ReferenceAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SymExpr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/layers/InsnLayer.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/reference_analyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/arch_handler
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/capstone_wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/segmentator
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
    ${CMAKE_SOURCE_DIR}/third_party/capstone/include
    ${spdlog_SOURCE_DIR}/include
    # Internal headers (SectionLookup.hpp, etc.)
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/traits
    ${CMAKE_CURRENT_SOURCE_DIR}/layers
)

include_directories(${INCLUDE_DIRS})

add_library(${TARGET_NAME} STATIC ${SRC_FILES})
target_link_libraries(${TARGET_NAME} capstone_wrapper spdlog)
