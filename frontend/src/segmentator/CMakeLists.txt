set (INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/frontend/include/
    ${CMAKE_SOURCE_DIR}/frontend/include/file_handler/
    ${CMAKE_SOURCE_DIR}/frontend/include/arch_handler/
    ${CMAKE_SOURCE_DIR}/frontend/include/segmentator/
    ${CMAKE_SOURCE_DIR}/frontend/include/region_refiner/
    ${CMAKE_SOURCE_DIR}/common/include/
    ${CMAKE_SOURCE_DIR}/common/include/core/
    ${CMAKE_SOURCE_DIR}/third_party/expected/include/
    ${spdlog_SOURCE_DIR}/include
)

set (TARGER_NAME VMPilot_SDK_Segmentator)

set (SRC_FILES ${SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/segmentator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HandlerRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../file_handler/ELFHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../file_handler/PEHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../file_handler/MachOHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../file_handler/ELFSectionViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch_handler/ArchHandlerCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch_handler/X86Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch_handler/ARM64Handler.cpp
)

include_directories(${INCLUDE_DIRS})

add_library(${TARGER_NAME} STATIC ${SRC_FILES})

target_link_libraries(${TARGER_NAME} PUBLIC VMPilot_Common capstone_wrapper coffi-modern elfio-modern)

# Suppress warnings-as-errors from third-party headers (coffi C4754 on MSVC)
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)
suppress_thirdparty_warnings(${TARGER_NAME})
