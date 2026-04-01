# cmake/Dependencies.cmake — ALL external dependencies in one place
#
# Git submodules (third_party/):
#   abseil-cpp, protobuf, capstone, botan, blake3, expected,
#   coffi-modern, elfio-modern
#
# CPM (fetched at configure time):
#   spdlog, toml++
#
# FetchContent (fetched at configure time, test-only):
#   googletest

# CPM package manager
include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# Shallow clones: depth 1 for all CPM/FetchContent downloads
set(CPM_USE_SHALLOW_CLONE ON)
set(FETCHCONTENT_SHALLOW ON)

# ── Submodule-based header-only libraries ────────────────────────────────────

# coffi-modern (PE/COFF parsing) — header-only INTERFACE target
set(COFFI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(COFFI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/coffi-modern
                 ${CMAKE_BINARY_DIR}/third_party/coffi-modern
                 EXCLUDE_FROM_ALL)

# elfio-modern (ELF parsing) — header-only INTERFACE target
set(ELFIO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ELFIO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/elfio-modern
                 ${CMAKE_BINARY_DIR}/third_party/elfio-modern
                 EXCLUDE_FROM_ALL)

# ── CPM dependencies ────────────────────────────────────────────────────────

CPMAddPackage("gh:gabime/spdlog@1.17.0")

CPMAddPackage(
    NAME tomlplusplus
    GITHUB_REPOSITORY "marzer/tomlplusplus"
    GIT_TAG "v3.4.0"
)

# ── Testing ──────────────────────────────────────────────────────────────────

if (VMPILOT_ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW ON
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    include(GoogleTest)
endif()
