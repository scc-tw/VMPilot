set(TARGET_NAME VMPilot_Linker)

add_library(${TARGET_NAME} STATIC
    Loader.cpp
    BinaryEditor.cpp
    StubEmitter.cpp
    X86_64StubEmitter.cpp
    ARM64StubEmitter.cpp
    X86_32StubEmitter.cpp
    PayloadBuilder.cpp
    ELFEditor.cpp
    MachOEditor.cpp
    PEEditor.cpp
    strategies/elf_dep_strategies.cpp
    strategies/pe_dep_strategies.cpp
    strategies/macho_dep_strategies.cpp
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/strategies
    PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

target_link_libraries(${TARGET_NAME}
    PRIVATE VMPilot_Common
            vmpilot_options
            vmpilot_sanitizer
            coffi-modern
            elfio-modern
)

# Suppress third-party warnings (ELFIO/COFFI headers)
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)
suppress_thirdparty_warnings(${TARGET_NAME})
