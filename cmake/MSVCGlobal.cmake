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
endif()

# TODO: fix the underlying alignment issues and remove these suppressions.
#   C4324: struct padded due to alignas (runtime VmExecution, VmOramState, PlatformCallDesc)
#   C4146: unary minus on unsigned type (runtime NEG handler, ORAM strategies)
add_compile_options(
    $<$<COMPILE_LANGUAGE:C,CXX>:/wd4324>
    $<$<COMPILE_LANGUAGE:C,CXX>:/wd4146>
)
