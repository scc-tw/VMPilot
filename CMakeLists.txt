cmake_minimum_required(VERSION 3.20)

project(VMPilot
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ─────────────────────────────────────────────────────────────────────────────
# Project configuration (cmake/ modules)
# ─────────────────────────────────────────────────────────────────────────────

include(cmake/Options.cmake)            # VMPILOT_ENABLE_TESTS, _SANITIZERS, _LTO
include(cmake/CompilerWarnings.cmake)   # vmpilot_options INTERFACE target
include(cmake/Sanitizers.cmake)         # vmpilot_sanitizer INTERFACE target
include(cmake/LTO.cmake)               # CMAKE_INTERPROCEDURAL_OPTIMIZATION
include(cmake/Dependencies.cmake)       # googletest (conditional)

# ─────────────────────────────────────────────────────────────────────────────
# Subdirectories
# ─────────────────────────────────────────────────────────────────────────────

add_subdirectory(common)
add_subdirectory(sdk)
add_subdirectory(loader)
add_subdirectory(runtime)
