add_subdirectory(crypto)

find_package(Threads REQUIRED)

set (INCLUDE_DIRS ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vm
    ${CRYPTO_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)
include_directories (${INCLUDE_DIRS})

# setup library source name
set(LIB_NAME "opcode_table")

# Platform-specific hardware RNG implementation (one file per OS, arch handled inside)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(HARDWARE_RNG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/hardware_rng_darwin.cpp)
elseif(WIN32)
    set(HARDWARE_RNG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/hardware_rng_windows.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(HARDWARE_RNG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/hardware_rng_linux.cpp)
else()
    message(WARNING "Unsupported OS for hardware RNG — using Linux fallback")
    set(HARDWARE_RNG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/hardware_rng_linux.cpp)
endif()

# Add the source files
set (SRC_FILES ${SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opcode_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instruction_t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_type_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/macho_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/diagnostic_collector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_crypto.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_encoding.cpp
    ${HARDWARE_RNG_SRC}
)

# Add the library dependencies dir BLAKE3
set (LIBS ${LIBS}
    crypto
    Threads::Threads
)

add_library(${LIB_NAME} STATIC ${SRC_FILES})
target_link_libraries(${LIB_NAME} ${LIBS})

# Tests
if(ENABLE_TESTS)
    add_executable(test_hardware_rng
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_hardware_rng.cpp
    )
    target_link_libraries(test_hardware_rng
        PRIVATE ${LIB_NAME}
                Threads::Threads
                GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(test_hardware_rng)
endif()