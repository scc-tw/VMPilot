# Tpm2Tss.cmake — tpm2-tss integration for Stage B TPM-backed state provider.
#
# Exposes VMPILOT_ENABLE_TPM (default OFF) and a single helper function
# `vmpilot_configure_tpm2_tss(<target>)` that, when the option is on:
#
#   * finds tss2-esys + tss2-tctildr via pkg-config (Linux / FreeBSD)
#   * links them PRIVATE against <target>
#   * defines VMPILOT_HAS_TPM2_TSS=1 PRIVATE on <target>
#
# Every test or consumer that wants to compile tpm2-tss-guarded code
# must call the helper too — the compile define does not propagate
# through the static lib (see VMPilot_Runtime wiring below).
#
# Platform policy (doc 17a §2):
#   Linux   — supported. Pkg-config resolves the libs from the distro.
#   macOS   — NOT supported. Fail-fast at configure time with a pointer
#             to Stage C (AppleSecureEnclaveKeyProvider / AppAttest).
#   Windows — NOT supported in this PR. Windows TBS integration is
#             a later PR; set VMPILOT_ENABLE_TPM=OFF (the default) to
#             keep the build clean on Windows developer machines.

option(VMPILOT_ENABLE_TPM
    "Enable TPM-backed persistent state provider (Linux only for now)"
    OFF)

function(vmpilot_configure_tpm2_tss target)
    if(NOT VMPILOT_ENABLE_TPM)
        return()
    endif()

    if(APPLE)
        message(FATAL_ERROR
            "VMPILOT_ENABLE_TPM=ON is not supported on macOS.\n"
            "macOS exposes no public TPM2 NV monotonic counter to "
            "third-party runtimes (doc 17 §2.3 / doc 17a §2.3). Use "
            "the Stage C Apple providers "
            "(AppleSecureEnclaveKeyProvider / AppleAppAttestStateProvider) "
            "for highsec on macOS.")
    endif()

    if(WIN32)
        message(FATAL_ERROR
            "VMPILOT_ENABLE_TPM=ON on Windows is deferred.\n"
            "Windows TPM Base Services (TBS) integration is a later PR. "
            "Set VMPILOT_ENABLE_TPM=OFF (the default) to build the "
            "runtime without TPM support on Windows.")
    endif()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TSS2_ESYS    REQUIRED IMPORTED_TARGET tss2-esys)
    pkg_check_modules(TSS2_TCTILDR REQUIRED IMPORTED_TARGET tss2-tctildr)

    target_link_libraries(${target} PRIVATE
        PkgConfig::TSS2_ESYS
        PkgConfig::TSS2_TCTILDR)
    # PUBLIC define — downstream consumers (test binaries, example
    # binaries) must also see VMPILOT_HAS_TPM2_TSS so their
    # #ifdef-guarded test code compiles. The library linkage above is
    # PRIVATE because tpm2-tss symbols are called only from this
    # target's TUs.
    target_compile_definitions(${target} PUBLIC VMPILOT_HAS_TPM2_TSS=1)
endfunction()
