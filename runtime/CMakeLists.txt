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
    src/vm_intrinsics.cpp
    src/blob_builder.cpp
    src/program_builder.cpp
    src/handler_detail.cpp
    src/trust_root_embed.cpp
    src/envelope/outer.cpp
    src/binding/package.cpp
    src/binding/inner_partition.cpp
    src/binding/unit.cpp
    src/registry/registry.cpp
    ${PLATFORM_ASM_SOURCES}
)

include(${CMAKE_SOURCE_DIR}/cmake/TrustRoot.cmake)
vmpilot_configure_trust_root(VMPilot_Runtime)

target_include_directories(VMPilot_Runtime
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/common/include
            ${CMAKE_SOURCE_DIR}/common/include/vm
            ${CMAKE_SOURCE_DIR}/third_party/expected/include
    PRIVATE ${CRYPTO_INCLUDE_DIR}
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

    # ──────────────────────────────────────────────────────────────────────
    # Test-only fixture generator (Stage 0 of the runtime redesign migration)
    #
    # Produces signed, canonically-encoded fixtures (PackageBindingRecord,
    # etc.) for consumption by later-stage acceptance tests. The signing path
    # talks to Botan directly on purpose so the production crypto adaptor
    # never ships a `Sign_Ed25519` entry point. Only linked into test
    # binaries that explicitly opt in via `WITH_FIXTURES`.
    # ──────────────────────────────────────────────────────────────────────
    add_library(vmpilot_runtime_test_fixtures STATIC
        test/fixtures/sign_ed25519_testonly.cpp
        test/fixtures/fixture_generator.cpp
    )
    # Botan 3 headers require C++20; crypto submodule already compiles at
    # that level. Fixture TUs include Botan headers directly so they also
    # need C++20. Test binaries that link the fixtures but don't include
    # Botan headers themselves (no direct #include <botan/...>) keep C++17.
    target_compile_features(vmpilot_runtime_test_fixtures PUBLIC cxx_std_20)
    target_include_directories(vmpilot_runtime_test_fixtures
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/test
                ${CMAKE_SOURCE_DIR}/common/include
                ${CMAKE_SOURCE_DIR}/common/include/vm
        PRIVATE ${CRYPTO_INCLUDE_DIR}
    )
    target_link_libraries(vmpilot_runtime_test_fixtures
        PUBLIC  VMPilot_Common
        PRIVATE vmpilot_options
                vmpilot_sanitizer
    )

    # Helper: define a runtime test with standard boilerplate.
    function(vmpilot_add_runtime_test name)
        cmake_parse_arguments(ARG "WITH_FIXTURES" "" "SOURCES;EXTRA_INCLUDES" ${ARGN})
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
        if(ARG_WITH_FIXTURES)
            target_include_directories(${name} PRIVATE ${CRYPTO_INCLUDE_DIR})
            target_link_libraries(${name} PRIVATE vmpilot_runtime_test_fixtures)
        endif()
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
        test_tls_intrinsic
    )
        vmpilot_add_runtime_test(${_t} SOURCES test/integration/${_t}.cpp)
    endforeach()

    vmpilot_add_runtime_test(test_vm_runner
        SOURCES test/integration/test_vm_runner.cpp
        EXTRA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # ── fixtures/ — Stage 0 infrastructure smoke test ──

    vmpilot_add_runtime_test(test_fixture_smoke
        SOURCES test/fixtures/test_fixture_smoke.cpp
        WITH_FIXTURES)

    vmpilot_add_runtime_test(test_verify_ed25519
        SOURCES test/fixtures/test_verify_ed25519.cpp
        WITH_FIXTURES)

    vmpilot_add_runtime_test(test_trust_root
        SOURCES test/fixtures/test_trust_root.cpp
        WITH_FIXTURES)

    vmpilot_add_runtime_test(test_outer_envelope
        SOURCES test/fixtures/test_outer_envelope.cpp
        WITH_FIXTURES)

    vmpilot_add_runtime_test(test_accept_package
        SOURCES test/fixtures/test_accept_package.cpp
        WITH_FIXTURES)

    vmpilot_add_runtime_test(test_registry_dispatch
        SOURCES test/fixtures/test_registry_dispatch.cpp
        WITH_FIXTURES)

    vmpilot_add_runtime_test(test_accept_unit_entry
        SOURCES test/fixtures/test_accept_unit_entry.cpp
        WITH_FIXTURES)

    # ── security/ — Doc 15-19 cryptographic invariants ──

    foreach(_t IN ITEMS
        test_encryption_chain
        test_forward_secrecy
        test_timing_invariants
        test_mac_integrity
        test_execution_independence
        test_verify_bb_mac
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
