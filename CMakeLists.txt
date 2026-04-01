cmake_minimum_required(VERSION 3.20)

project(VMPilot)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /utf-8")
endif()

# Enable debug symbols and disable optimization for debug build.
# Sanitizers are applied per-target (not globally) to avoid breaking
# third_party code — GCC 15 + UBsan rejects constexpr fn-ptr comparisons
# in abseil's hash_policy_traits.h.
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -O0")
        if (NOT (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64"))
            set(VMPILOT_SANITIZER_FLAGS "-fsanitize=address,undefined")
        else ()
            message(WARNING "Sanitizer is not supported on ARM")
            set(VMPILOT_SANITIZER_FLAGS "")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if (CMAKE_CXX_FLAGS MATCHES ".*(/sdl|-sdl).*")
            set(VMPILOT_SANITIZER_FLAGS "/fsanitize=address")
        else ()
            message(WARNING "Your MSVC version does not support sanitizer")
            set(VMPILOT_SANITIZER_FLAGS "")
        endif ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /Od")
    endif ()
    add_compile_definitions(DEBUG)
endif ()


# If it is release build, we will enable optimization, also link time optimization if it is supported
if (CMAKE_BUILD_TYPE MATCHES "Release")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=auto")
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin")
        endif()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
        if (CMAKE_CXX_FLAGS MATCHES ".*(/GL|-GL).*") # If it is support link time optimization, we will enable it
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL")
        else ()
            message(WARNING "Your MSVC version does not support link time optimization")
        endif()
    endif()
    # Define NDEBUG macro
    add_compile_definitions(NDEBUG)
endif()

# Shrink symbol table if it is release build
if (CMAKE_BUILD_TYPE MATCHES "Release")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections -ffunction-sections")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Gy")
    endif()
endif()

# RelWithDebInfo: optimised build with debug symbols and per-target sanitizers.
# Overrides CMAKE_CXX_FLAGS_RELWITHDEBINFO (not CMAKE_CXX_FLAGS) so that
# sanitisers stay per-target via vmpilot_sanitizer, keeping third-party code
# (abseil, protobuf) clean.  Usage:
#   cmake -S . -B build-diag -DCMAKE_BUILD_TYPE=RelWithDebInfo
if (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Og -g -fno-omit-frame-pointer -fno-lto"
            CACHE STRING "Flags for RelWithDebInfo" FORCE)
        if (NOT (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64"))
            set(VMPILOT_SANITIZER_FLAGS "-fsanitize=address,undefined")
        else ()
            message(WARNING "Sanitizer is not supported on ARM")
            set(VMPILOT_SANITIZER_FLAGS "")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /Od"
            CACHE STRING "Flags for RelWithDebInfo" FORCE)
        if (CMAKE_CXX_FLAGS MATCHES ".*(/sdl|-sdl).*")
            set(VMPILOT_SANITIZER_FLAGS "/fsanitize=address")
        else ()
            message(WARNING "Your MSVC version does not support sanitizer")
            set(VMPILOT_SANITIZER_FLAGS "")
        endif ()
    endif ()
endif()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif ()

# add dependencies
include(cmake/CPM.cmake)
CPMAddPackage("gh:nlohmann/json@3.11.3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Testing support — only fetch gtest when ENABLE_TESTS is ON
option(ENABLE_TESTS "Build tests" OFF)
if (ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    # Prevent gtest from overriding our compiler/linker options
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

# Sanitizer interface target — link this to get ASan+UBsan.
# Kept separate from CMAKE_CXX_FLAGS so third_party code (abseil, protobuf)
# is never compiled with sanitizers.  GCC 15 + UBsan rejects constexpr
# fn-ptr null comparisons in abseil's hash_policy_traits.h.
add_library(vmpilot_sanitizer INTERFACE)
if (VMPILOT_SANITIZER_FLAGS)
    target_compile_options(vmpilot_sanitizer INTERFACE ${VMPILOT_SANITIZER_FLAGS})
    target_link_options(vmpilot_sanitizer INTERFACE ${VMPILOT_SANITIZER_FLAGS})
endif()

add_subdirectory(common)
add_subdirectory(sdk)
add_subdirectory(loader)
add_subdirectory(runtime)
