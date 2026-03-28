find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------
set(RUNTIME_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/include/vm
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
set(RUNTIME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/encoding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/oram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers.cpp
    # Handler categories
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/data_movement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/class_a_bitwise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/class_b_mba.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/class_c_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/comparison.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/control_flow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/width_extension.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/atomic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/vm_internal.cpp
)

# ---------------------------------------------------------------------------
# Platform-specific ASM support (placeholders for future .S files)
# ---------------------------------------------------------------------------
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    # Future: add x86_64 native bridge assembly here
    # list(APPEND RUNTIME_SOURCES src/arch/x86_64_bridge.S)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    # Future: add AArch64 native bridge assembly here
    # list(APPEND RUNTIME_SOURCES src/arch/aarch64_bridge.S)
endif()

# ---------------------------------------------------------------------------
# Static library
# ---------------------------------------------------------------------------
add_library(VMPilot_Runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(VMPilot_Runtime
    PUBLIC  ${RUNTIME_INCLUDE_DIRS}
)

target_link_libraries(VMPilot_Runtime
    PUBLIC  nlohmann_json::nlohmann_json
            opcode_table
    PRIVATE Threads::Threads
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(ENABLE_TESTS)
    add_executable(runtime_smoke_test
        ${CMAKE_CURRENT_SOURCE_DIR}/test/smoke_test.cpp
    )
    target_link_libraries(runtime_smoke_test
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_blob_loader
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_blob_loader.cpp
    )
    target_include_directories(test_blob_loader
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_blob_loader
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_oram
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_oram.cpp
    )
    target_include_directories(test_oram
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_oram
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_encoding
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_encoding.cpp
    )
    target_include_directories(test_encoding
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_encoding
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_decoder
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_decoder.cpp
    )
    target_include_directories(test_decoder
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_decoder
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(runtime_smoke_test)
    gtest_discover_tests(test_blob_loader)
    gtest_discover_tests(test_oram)
    gtest_discover_tests(test_encoding)
    gtest_discover_tests(test_decoder)
endif()
