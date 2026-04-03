# cmake/Sanitizers.cmake — vmpilot_sanitizer INTERFACE target

add_library(vmpilot_sanitizer INTERFACE)

if (VMPILOT_ENABLE_SANITIZERS)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # ASan + UBsan work on x86_64, Apple Silicon (arm64), and x86.
        # Only skip embedded ARM Linux where sanitizer runtime may be absent.
        if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND
            CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
            message(WARNING "Sanitizers not supported on ARM Linux — skipping")
        else()
            target_compile_options(vmpilot_sanitizer INTERFACE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer)
            target_link_options(vmpilot_sanitizer INTERFACE
                -fsanitize=address,undefined)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # MSVC ASan compile flags (/fsanitize=address) and /INCREMENTAL:NO
        # are set globally in root CMakeLists.txt so that ALL targets
        # (including third-party) share the same annotation metadata.
        # This target is kept as a no-op on MSVC for interface compatibility.
    endif()
endif()
