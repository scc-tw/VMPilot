find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# Platform-specific native call trampoline
# ---------------------------------------------------------------------------
set(PLATFORM_ASM_SOURCES "")
if(MSVC)
    enable_language(ASM_MASM)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        list(APPEND PLATFORM_ASM_SOURCES src/platform_call_win64.asm)
    else()
        list(APPEND PLATFORM_ASM_SOURCES src/platform_call_x86_32.asm)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    enable_language(ASM)
    if(WIN32)
        list(APPEND PLATFORM_ASM_SOURCES src/platform_call_win64.S)
    else()
        list(APPEND PLATFORM_ASM_SOURCES src/platform_call_x86_64.S)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    enable_language(ASM)
    list(APPEND PLATFORM_ASM_SOURCES src/platform_call_arm64.S)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86|x86")
    enable_language(ASM)
    list(APPEND PLATFORM_ASM_SOURCES src/platform_call_x86_32.S)
else()
    list(APPEND PLATFORM_ASM_SOURCES src/platform_call_fallback.cpp)
endif()

# ---------------------------------------------------------------------------
# Platform-specific runtime initialization (payload section discovery)
# ---------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(RUNTIME_INIT_SRC src/vm_runtime_init_darwin.cpp)
elseif(WIN32)
    set(RUNTIME_INIT_SRC src/vm_runtime_init_windows.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(RUNTIME_INIT_SRC src/vm_runtime_init_linux.cpp)
else()
    message(WARNING "Unsupported OS for runtime init — payload auto-discovery disabled")
    set(RUNTIME_INIT_SRC "")
endif()

# ---------------------------------------------------------------------------
# Static library
# ---------------------------------------------------------------------------
add_library(VMPilot_Runtime STATIC
    src/vm_stub_entry.cpp
    ${RUNTIME_INIT_SRC}
    src/vm_state.cpp
    src/vm_engine.cpp
    src/vm_runner.cpp
    src/pipeline.cpp
    src/oram_strategies.cpp
    src/classify_args.cpp
    src/tls_helpers.cpp
    src/blob_builder.cpp
    src/program_builder.cpp
    src/handler_detail.cpp
    ${PLATFORM_ASM_SOURCES}
)

target_include_directories(VMPilot_Runtime
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/common/include
            ${CMAKE_SOURCE_DIR}/common/include/vm
            ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

target_link_libraries(VMPilot_Runtime
    PUBLIC  VMPilot_Common
    PRIVATE Threads::Threads
            vmpilot_options
            vmpilot_sanitizer
)

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------
add_subdirectory(example)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(ENABLE_TESTS)
    include(GoogleTest)

    # Helper: define a runtime test with standard boilerplate.
    function(vmpilot_add_runtime_test name)
        cmake_parse_arguments(ARG "" "" "SOURCES;EXTRA_INCLUDES" ${ARGN})
        add_executable(${name} ${ARG_SOURCES})
        target_include_directories(${name}
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
                    ${ARG_EXTRA_INCLUDES}
        )
        target_link_libraries(${name}
            PRIVATE VMPilot_Runtime
                    GTest::gtest_main
                    vmpilot_options
                    vmpilot_sanitizer
        )
        gtest_discover_tests(${name})
    endfunction()

    # ══════════════════════════════════════════════════════════════════════
    # New test structure (organized by concern, not by implementation phase)
    # ══════════════════════════════════════════════════════════════════════

    # ── unit/ — isolated component tests ──

    foreach(_t IN ITEMS
        test_crypto_primitives
        test_blob_view
        test_oram_strategies
        test_state_types
        test_pipeline_functions
    )
        vmpilot_add_runtime_test(${_t} SOURCES test/unit/${_t}.cpp)
    endforeach()

    # ── opcode/ — per-opcode correctness (happy path + edge case) ──

    foreach(_t IN ITEMS
        test_data_movement
        test_arithmetic
        test_logic_bitwise
        test_comparison_branch
        test_width_extension
        test_control_flow
    )
        vmpilot_add_runtime_test(${_t} SOURCES test/opcode/${_t}.cpp)
    endforeach()

    # ── integration/ — cross-component interactions ──

    foreach(_t IN ITEMS
        test_cfg_patterns
        test_native_call
        test_policy_matrix
    )
        vmpilot_add_runtime_test(${_t} SOURCES test/integration/${_t}.cpp)
    endforeach()

    vmpilot_add_runtime_test(test_vm_runner
        SOURCES test/integration/test_vm_runner.cpp
        EXTRA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # ── security/ — Doc 15-19 cryptographic invariants ──

    foreach(_t IN ITEMS
        test_encryption_chain
        test_forward_secrecy
        test_timing_invariants
        test_mac_integrity
        test_execution_independence
    )
        vmpilot_add_runtime_test(${_t} SOURCES test/security/${_t}.cpp)
    endforeach()

    # ── robustness/ — error paths + boundary values ──

    foreach(_t IN ITEMS
        test_error_paths
        test_boundary_values
        test_stack_limits
    )
        vmpilot_add_runtime_test(${_t} SOURCES test/robustness/${_t}.cpp)
    endforeach()

    # ── concurrency/ — thread safety + reentrancy (needs Threads) ──

    foreach(_t IN ITEMS test_parallel_engines test_reentrancy)
        add_executable(${_t} test/concurrency/${_t}.cpp)
        target_include_directories(${_t}
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
        target_link_libraries(${_t}
            PRIVATE VMPilot_Runtime Threads::Threads GTest::gtest_main
                    vmpilot_options vmpilot_sanitizer)
        gtest_discover_tests(${_t})
    endforeach()

    # ── platform/ — platform-specific (TLS needs Threads) ──

    add_executable(test_tls_helpers test/platform/test_tls_helpers.cpp)
    target_include_directories(test_tls_helpers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
    target_link_libraries(test_tls_helpers
        PRIVATE VMPilot_Runtime Threads::Threads GTest::gtest_main
                vmpilot_options vmpilot_sanitizer)
    gtest_discover_tests(test_tls_helpers)

    # Old test files deleted — all migrated to subdirectories above
endif()

# ── Benchmarks (opt-in) ─────────────────────────────────────────────────
if(VMPILOT_ENABLE_BENCHMARKS)
    add_subdirectory(bench)
endif()
