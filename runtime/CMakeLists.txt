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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/composition_cache.cpp
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
    # Native call bridge (Phase 8)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bridge/native_call_bridge.cpp
    # Security (Phase 9)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/security/anti_tamper.cpp
    # VM entry/exit (Phase 10)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_entry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_stub_entry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_runtime_init.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ephemeral_encoding.cpp
    # TLS helpers (L6)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tls_helpers.cpp
    # Phase 2+: new state model
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_state.cpp
    # Phase 4: pipeline functions for new state types
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline.cpp
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

    add_executable(test_handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_handlers.cpp
    )
    target_include_directories(test_handlers
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_handlers
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_dispatcher
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_dispatcher.cpp
    )
    target_include_directories(test_dispatcher
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_dispatcher
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_native_bridge
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_native_bridge.cpp
    )
    target_include_directories(test_native_bridge
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_native_bridge
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_native_call_e2e
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_native_call_e2e.cpp
    )
    target_include_directories(test_native_call_e2e
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_native_call_e2e
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_vm_entry
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_vm_entry.cpp
    )
    target_include_directories(test_vm_entry
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_vm_entry
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_vm_stub_entry
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_vm_stub_entry.cpp
    )
    target_include_directories(test_vm_stub_entry
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_vm_stub_entry
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_anti_tamper
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_anti_tamper.cpp
    )
    target_include_directories(test_anti_tamper
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_anti_tamper
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_mba_property
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_mba_property.cpp
    )
    target_include_directories(test_mba_property
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_mba_property
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_cross_bb
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_cross_bb.cpp
    )
    target_include_directories(test_cross_bb
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_cross_bb
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_full_prp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_full_prp.cpp
    )
    target_include_directories(test_full_prp
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_full_prp
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_cfg_patterns
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_cfg_patterns.cpp
    )
    target_include_directories(test_cfg_patterns
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_cfg_patterns
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_native_fp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_native_fp.cpp
    )
    target_include_directories(test_native_fp
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_native_fp
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_rekey
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_rekey.cpp
    )
    target_include_directories(test_rekey
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_rekey
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    add_executable(test_ephemeral_encoding
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ephemeral_encoding.cpp
    )
    target_include_directories(test_ephemeral_encoding
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_ephemeral_encoding
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(runtime_smoke_test)
    gtest_discover_tests(test_blob_loader)
    gtest_discover_tests(test_oram)
    gtest_discover_tests(test_encoding)
    gtest_discover_tests(test_decoder)
    gtest_discover_tests(test_handlers)
    gtest_discover_tests(test_dispatcher)
    gtest_discover_tests(test_native_bridge)
    gtest_discover_tests(test_native_call_e2e)
    gtest_discover_tests(test_vm_entry)
    gtest_discover_tests(test_vm_stub_entry)
    gtest_discover_tests(test_anti_tamper)
    gtest_discover_tests(test_mba_property)
    gtest_discover_tests(test_cross_bb)
    gtest_discover_tests(test_full_prp)
    gtest_discover_tests(test_cfg_patterns)
    gtest_discover_tests(test_native_fp)
    gtest_discover_tests(test_rekey)
    gtest_discover_tests(test_ephemeral_encoding)

    add_executable(test_tls_helpers
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_tls_helpers.cpp
    )
    target_link_libraries(test_tls_helpers
        PRIVATE VMPilot_Runtime
                Threads::Threads
                GTest::gtest_main
    )
    gtest_discover_tests(test_tls_helpers)

    add_executable(test_phase1_foundation
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase1_foundation.cpp
    )
    target_link_libraries(test_phase1_foundation
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )
    gtest_discover_tests(test_phase1_foundation)

    add_executable(test_phase2_state
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase2_state.cpp
    )
    target_link_libraries(test_phase2_state
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )
    gtest_discover_tests(test_phase2_state)

    add_executable(test_phase3_handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase3_handlers.cpp
    )
    target_link_libraries(test_phase3_handlers
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )
    gtest_discover_tests(test_phase3_handlers)

    add_executable(test_phase4_engine
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase4_engine.cpp
    )
    target_include_directories(test_phase4_engine
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_phase4_engine
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
    )
    gtest_discover_tests(test_phase4_engine)
endif()
