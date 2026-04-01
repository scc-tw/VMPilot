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
    # Core VM engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_stub_entry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_runtime_init.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/oram_strategies.cpp
    # NATIVE_CALL arg classification (platform-independent)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/classify_args.cpp
    # TLS helpers (standalone)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tls_helpers.cpp
)

# ---------------------------------------------------------------------------
# Platform-specific native call trampoline (FP + variadic + stack args)
# ---------------------------------------------------------------------------
if(MSVC)
    enable_language(ASM_MASM)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        list(APPEND RUNTIME_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_win64.asm)
    else()
        list(APPEND RUNTIME_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_x86_32.asm)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    enable_language(ASM)
    if(WIN32)
        list(APPEND RUNTIME_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_win64.S)
    else()
        list(APPEND RUNTIME_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_x86_64.S)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    enable_language(ASM)
    list(APPEND RUNTIME_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_arm64.S)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86|x86")
    enable_language(ASM)
    list(APPEND RUNTIME_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_x86_32.S)
else()
    list(APPEND RUNTIME_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_call_fallback.cpp)
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
    add_executable(runtime_smoke_test
        ${CMAKE_CURRENT_SOURCE_DIR}/test/smoke_test.cpp
    )
    target_link_libraries(runtime_smoke_test
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
                vmpilot_sanitizer
    )

    include(GoogleTest)

    # ── Kept tests (Category A: shared types, TLS, no old VM dependency) ──

    gtest_discover_tests(runtime_smoke_test)

    add_executable(test_tls_helpers
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_tls_helpers.cpp
    )
    target_link_libraries(test_tls_helpers
        PRIVATE VMPilot_Runtime
                Threads::Threads
                GTest::gtest_main
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_tls_helpers)

    add_executable(test_phase1_foundation
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase1_foundation.cpp
    )
    target_link_libraries(test_phase1_foundation
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_phase1_foundation)

    add_executable(test_phase2_state
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase2_state.cpp
    )
    target_link_libraries(test_phase2_state
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_phase2_state)

    add_executable(test_phase3_handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_phase3_handlers.cpp
    )
    target_link_libraries(test_phase3_handlers
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
                vmpilot_sanitizer
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
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_phase4_engine)

    add_executable(test_engine_comprehensive
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_engine_comprehensive.cpp
    )
    target_include_directories(test_engine_comprehensive
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    target_link_libraries(test_engine_comprehensive
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_engine_comprehensive)

    # ── Test coverage tests (doc 16 compliance + opcode coverage) ──

    set(_coverage_tests
        test_jcc_conditions
        test_width_extension
        test_shift_rotate
        test_push_pop_oram
        test_cfg_patterns
        test_load_store
        test_call_depth
        test_error_paths
        test_native_call_variants
        test_cmp_edge_cases
        test_ctx_ops
        test_policy_consistency
    )

    foreach(_t IN LISTS _coverage_tests)
        add_executable(${_t}
            ${CMAKE_CURRENT_SOURCE_DIR}/test/${_t}.cpp
        )
        target_include_directories(${_t}
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
        )
        target_link_libraries(${_t}
            PRIVATE VMPilot_Runtime
                    GTest::gtest_main
                    vmpilot_sanitizer
        )
        gtest_discover_tests(${_t})
    endforeach()

    # ── VmRunner factory tests ──

    add_executable(test_vm_runner
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_vm_runner.cpp
    )
    target_include_directories(test_vm_runner
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test
                ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(test_vm_runner
        PRIVATE VMPilot_Runtime
                GTest::gtest_main
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_vm_runner)
endif()
