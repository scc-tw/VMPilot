set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/sdk/include/core
    ${CMAKE_SOURCE_DIR}/sdk/include/bytecode_compiler
    ${CMAKE_SOURCE_DIR}/sdk/include/segmentator
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

# Third-party header-only libs — mark SYSTEM to suppress warnings
include_directories(SYSTEM
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/
)

set(TARGET_NAME VMPilot_Loader)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/BinaryPatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BlobSerializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StubGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ELFPatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PEPatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MachOPatcher.cpp
)

include_directories(${INCLUDE_DIRS})

add_library(${TARGET_NAME} STATIC ${SRC_FILES})

target_link_libraries(${TARGET_NAME} opcode_table)

# Suppress third-party warnings
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)
suppress_thirdparty_warnings(${TARGET_NAME})
