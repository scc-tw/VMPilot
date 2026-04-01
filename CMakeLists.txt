cmake_minimum_required(VERSION 3.20)

project(VMPilot
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ─────────────────────────────────────────────────────────────────────────────
# vmpilot_options — INTERFACE library carrying project-wide compile settings.
# Every first-party target links this; third_party targets do NOT.
# ─────────────────────────────────────────────────────────────────────────────

add_library(vmpilot_options INTERFACE)

target_compile_features(vmpilot_options INTERFACE cxx_std_17)

target_compile_options(vmpilot_options INTERFACE
    # Warnings
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8>
    # Colour diagnostics
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>
    # Debug: symbols + frame pointers
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-g -fno-omit-frame-pointer -O0>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Zi /Od>
    # Release: max optimisation + gc-sections
    $<$<AND:$<CONFIG:Release>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-O3 -fdata-sections -ffunction-sections>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2 /Gy>
    # RelWithDebInfo
    $<$<AND:$<CONFIG:RelWithDebInfo>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-Og -g -fno-omit-frame-pointer>
    $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/Zi /Od>
)

target_compile_definitions(vmpilot_options INTERFACE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# LTO for Release builds (CMake 3.9+ native support)
if (CMAKE_BUILD_TYPE MATCHES "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_ok OUTPUT _ipo_err)
    if (_ipo_ok)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${_ipo_err}")
    endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# vmpilot_sanitizer — INTERFACE library for ASan + UBsan.
# Separate from vmpilot_options so third_party code is never sanitised.
# GCC 15 + UBsan rejects constexpr fn-ptr null comparisons in abseil.
# ─────────────────────────────────────────────────────────────────────────────

add_library(vmpilot_sanitizer INTERFACE)

if (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
            target_compile_options(vmpilot_sanitizer INTERFACE
                -fsanitize=address,undefined)
            target_link_options(vmpilot_sanitizer INTERFACE
                -fsanitize=address,undefined)
        endif()
    elseif (MSVC)
        target_compile_options(vmpilot_sanitizer INTERFACE /fsanitize=address)
        target_link_options(vmpilot_sanitizer INTERFACE /fsanitize=address)
    endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Dependencies
# ─────────────────────────────────────────────────────────────────────────────

include(cmake/CPM.cmake)
CPMAddPackage("gh:nlohmann/json@3.11.3")

option(ENABLE_TESTS "Build tests" OFF)
if (ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Subdirectories
# ─────────────────────────────────────────────────────────────────────────────

add_subdirectory(common)
add_subdirectory(sdk)
add_subdirectory(loader)
add_subdirectory(runtime)
