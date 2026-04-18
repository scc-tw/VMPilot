add_subdirectory(crypto)

find_package(Threads REQUIRED)

# ───────────────────────────────────────────────────────────────────────────
# libcbor — vendored via third_party/ (follows BLAKE3 integration precedent).
#
# Used today as a build-time dependency only; the strict decoder in
# common/src/cbor/strict.cpp is hand-rolled so that production runtime has a
# tight, auditable binary footprint. libcbor stays linked so tests and future
# tooling can cross-validate against an independent implementation.
# ───────────────────────────────────────────────────────────────────────────
function(add_libcbor)
    set(LIBCBOR_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/libcbor)

    if(MSVC)
        set(_saved_C_FLAGS "${CMAKE_C_FLAGS}")
        set(_saved_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "")
        set(CMAKE_CXX_FLAGS "")
    endif()

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(WITH_TESTS OFF CACHE BOOL "" FORCE)
    set(WITH_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(CBOR_PRETTY_PRINTER OFF CACHE BOOL "" FORCE)

    add_subdirectory(${LIBCBOR_SOURCE_DIR} ${CMAKE_BINARY_DIR}/libcbor-build EXCLUDE_FROM_ALL)

    if(MSVC)
        set(CMAKE_C_FLAGS "${_saved_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${_saved_CXX_FLAGS}")
    endif()
endfunction()

add_libcbor()

set(LIB_NAME VMPilot_Common)

# Platform-specific hardware RNG implementation
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(HARDWARE_RNG_SRC src/vm/hardware_rng_darwin.cpp)
elseif(WIN32)
    set(HARDWARE_RNG_SRC src/vm/hardware_rng_windows.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(HARDWARE_RNG_SRC src/vm/hardware_rng_linux.cpp)
else()
    message(WARNING "Unsupported OS for hardware RNG — using Linux fallback")
    set(HARDWARE_RNG_SRC src/vm/hardware_rng_linux.cpp)
endif()

add_library(${LIB_NAME} STATIC
    src/file_type_parser.cpp
    src/utilities.cpp
    src/macho_parser.cpp
    src/thread_pool.cpp
    src/diagnostic_collector.cpp
    src/native_registry.cpp
    src/vm/vm_crypto.cpp
    src/vm/vm_encoding.cpp
    src/vm/chaff_expansion.cpp
    src/cbor/strict.cpp
    ${HARDWARE_RNG_SRC}
)

target_include_directories(${LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vm
        ${CMAKE_SOURCE_DIR}/third_party/expected/include
    PRIVATE
        ${CRYPTO_INCLUDE_DIR}
)

target_link_libraries(${LIB_NAME}
    PUBLIC  crypto
    PRIVATE Threads::Threads
            cbor
            vmpilot_options
            vmpilot_sanitizer
)

# Tests
if(ENABLE_TESTS)
    include(GoogleTest)

    add_executable(test_hardware_rng test/test_hardware_rng.cpp)
    target_link_libraries(test_hardware_rng
        PRIVATE ${LIB_NAME}
                Threads::Threads
                GTest::gtest_main
                vmpilot_options
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_hardware_rng)

    add_executable(test_cbor_strict test/test_cbor_strict.cpp)
    target_link_libraries(test_cbor_strict
        PRIVATE ${LIB_NAME}
                GTest::gtest_main
                vmpilot_options
                vmpilot_sanitizer
    )
    gtest_discover_tests(test_cbor_strict)
endif()
