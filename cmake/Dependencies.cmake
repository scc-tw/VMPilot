# cmake/Dependencies.cmake — ALL external dependencies in one place
#
# Everything that isn't a git submodule (third_party/) is declared here.
# Git submodules: abseil-cpp, protobuf, capstone, botan, blake3, expected

# CPM package manager
include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# Shallow clones: depth 1 for all CPM/FetchContent downloads
set(CPM_USE_SHALLOW_CLONE ON)
set(FETCHCONTENT_SHALLOW ON)

# ── SDK dependencies (header-only / lightweight) ─────────────────────────────

CPMAddPackage(
    NAME ELFIO
    GITHUB_REPOSITORY "serge1/ELFIO"
    GIT_TAG "Release_3.12"
)

CPMAddPackage(
    NAME COFFI
    GITHUB_REPOSITORY "scc-tw/COFFI"
    GIT_TAG "1.1.5"
)

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
