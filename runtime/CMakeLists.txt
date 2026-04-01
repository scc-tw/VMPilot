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
# Static library
# ---------------------------------------------------------------------------
add_library(VMPilot_Runtime STATIC
    src/vm_stub_entry.cpp
    src/vm_runtime_init.cpp
    src/vm_state.cpp
    src/pipeline.cpp
    src/oram_strategies.cpp
    src/classify_args.cpp
    src/tls_helpers.cpp
    ${PLATFORM_ASM_SOURCES}
)

target_include_directories(VMPilot_Runtime
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/common/include
            ${CMAKE_SOURCE_DIR}/common/include/vm
            ${CMAKE_SOURCE_DIR}/third_party/expected/include
)

target_link_libraries(VMPilot_Runtime
    PUBLIC  nlohmann_json::nlohmann_json
            opcode_table
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

    # ── Core runtime tests ──

    vmpilot_add_runtime_test(runtime_smoke_test     SOURCES test/smoke_test.cpp)
    vmpilot_add_runtime_test(test_phase1_foundation SOURCES test/test_phase1_foundation.cpp)
    vmpilot_add_runtime_test(test_phase2_state      SOURCES test/test_phase2_state.cpp)
    vmpilot_add_runtime_test(test_phase3_handlers   SOURCES test/test_phase3_handlers.cpp)
    vmpilot_add_runtime_test(test_phase4_engine     SOURCES test/test_phase4_engine.cpp)
    vmpilot_add_runtime_test(test_engine_comprehensive
        SOURCES test/test_engine_comprehensive.cpp)

    # TLS test needs Threads
    add_executable(test_tls_helpers test/test_tls_helpers.cpp)
    target_include_directories(test_tls_helpers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
    target_link_libraries(test_tls_helpers
        PRIVATE VMPilot_Runtime Threads::Threads GTest::gtest_main
                vmpilot_options vmpilot_sanitizer)
    gtest_discover_tests(test_tls_helpers)

    # ── Opcode coverage tests (doc 16 compliance) ──

    foreach(_t IN ITEMS
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
        vmpilot_add_runtime_test(${_t} SOURCES test/${_t}.cpp)
    endforeach()

    # ── VmRunner factory tests ──

    vmpilot_add_runtime_test(test_vm_runner
        SOURCES test/test_vm_runner.cpp
        EXTRA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
