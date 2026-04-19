# cmake/MSVCGlobal.cmake — MSVC-wide flags applied to ALL targets
#
# These flags must be global (not per-target) so that every translation unit
# shares the same encoding and sanitizer annotation metadata.  A mismatch
# between any two .obj files causes LNK2038 (annotate_string / annotate_vector)
# or C4819 (codepage) errors.

if (NOT MSVC)
    return()
endif()

# UTF-8 source encoding: avoids C4819 on non-English codepages (e.g. Big5/950).
# Guarded with COMPILE_LANGUAGE so MASM (ml64.exe) is not affected.
add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>)

# ASan must be consistent across every .obj that links together.
# Setting it globally prevents annotate_string / annotate_vector mismatches (LNK2038).
# /fsanitize=address is compile-only; the linker does not accept it.
# Incremental linking is incompatible with ASan.
if (VMPILOT_ENABLE_SANITIZERS)
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/fsanitize=address>)
    add_link_options(/INCREMENTAL:NO)

    # Copy MSVC ASan runtime DLLs to the output directory so tests can find
    # them without requiring a Developer Command Prompt or manual PATH setup.
    get_filename_component(_msvc_tool_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB _asan_dlls "${_msvc_tool_dir}/clang_rt.asan*.dll")
    foreach(_dll IN LISTS _asan_dlls)
        file(COPY "${_dll}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach()
endif()

# TODO: fix the underlying alignment issues and remove these suppressions.
#   C4324: struct padded due to alignas (runtime VmExecution, VmOramState, PlatformCallDesc)
#   C4146: unary minus on unsigned type (runtime NEG handler, ORAM strategies)
add_compile_options(
    $<$<COMPILE_LANGUAGE:C,CXX>:/wd4324>
    $<$<COMPILE_LANGUAGE:C,CXX>:/wd4146>
)

# Warning-parity with clang/gcc -Werror=unused-function.
# MSVC's default /W4 places C4505 (unreferenced local function removed) at
# the W4 warning level but does NOT elevate it to an error under /WX unless
# we explicitly promote it. Without this, dead helpers land on dev-win,
# compile clean, and break CI only when clang-18 / gcc-14 / apple-clang
# see them with -Werror=unused-function enabled. Promoting closes the gap.
add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/we4505>)
