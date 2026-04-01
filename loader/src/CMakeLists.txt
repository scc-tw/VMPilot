set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/strategies
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

# Third-party header-only libs — mark SYSTEM to suppress warnings
include_directories(SYSTEM
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/
)

set(TARGET_NAME VMPilot_Loader)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BinaryEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StubEmitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/X86_64StubEmitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ARM64StubEmitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/X86_32StubEmitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PayloadBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ELFEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MachOEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PEEditor.cpp
    # Phase 5: FallbackChain strategies
    ${CMAKE_CURRENT_SOURCE_DIR}/strategies/elf_dep_strategies.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/strategies/pe_dep_strategies.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/strategies/macho_dep_strategies.cpp
)

include_directories(${INCLUDE_DIRS})

add_library(${TARGET_NAME} STATIC ${SRC_FILES})

target_link_libraries(${TARGET_NAME} opcode_table vmpilot_sanitizer)

# Suppress third-party warnings
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)
suppress_thirdparty_warnings(${TARGET_NAME})
