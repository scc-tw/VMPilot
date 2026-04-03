add_subdirectory(crypto)

find_package(Threads REQUIRED)

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
            vmpilot_options
            vmpilot_sanitizer
)

# Tests
if(ENABLE_TESTS)
    add_executable(test_hardware_rng test/test_hardware_rng.cpp)
    target_link_libraries(test_hardware_rng
        PRIVATE ${LIB_NAME}
                Threads::Threads
                GTest::gtest_main
                vmpilot_options
                vmpilot_sanitizer
    )
    include(GoogleTest)
    gtest_discover_tests(test_hardware_rng)
endif()
