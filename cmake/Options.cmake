# cmake/Options.cmake — project-level options
#
# All user-facing knobs live here. CMakePresets.json sets these;
# manual cmake invocations can override with -D flags.

# ── Build type ───────────────────────────────────────────────────────────────
# Default to Debug if not specified (matches README quick-start).
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
        STRINGS Debug Release RelWithDebInfo MinSizeRel)
    message(STATUS "No CMAKE_BUILD_TYPE specified — defaulting to Debug")
endif()

# ── Feature toggles ─────────────────────────────────────────────────────────
option(VMPILOT_ENABLE_TESTS       "Build test targets"                          OFF)
option(VMPILOT_ENABLE_SANITIZERS  "Enable ASan + UBsan on first-party targets"  OFF)
option(VMPILOT_ENABLE_LTO         "Enable link-time optimisation (Release)"     OFF)

# Back-compat alias (some CI scripts use the old name)
if (ENABLE_TESTS AND NOT VMPILOT_ENABLE_TESTS)
    set(VMPILOT_ENABLE_TESTS ON)
endif()
# Forward the canonical name so existing if(ENABLE_TESTS) guards still work
set(ENABLE_TESTS ${VMPILOT_ENABLE_TESTS} CACHE BOOL "" FORCE)
