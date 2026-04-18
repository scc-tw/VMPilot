# TrustRoot.cmake
#
# Controls which vendor trust root gets embedded into the runtime binary.
#
# By default the runtime ships with the RFC 8032 §7.1 Test 1 Ed25519 public
# key so fixtures produced under runtime/test/fixtures/ verify end-to-end in
# dev builds. Production builds override this by setting VMPILOT_VENDOR_PUBKEY
# on the CMake command line — either a 64-character hex string, or a path to
# a 32-byte binary blob — and optionally VMPILOT_VENDOR_KEY_ID / epoch.
#
# Regardless of the source, the bytes end up in a generated
# trust_root_embed.cpp placed in ${CMAKE_BINARY_DIR}/generated/ and pinned to
# a named read-only section (.vmpltr on PE, __DATA,__vmpilot_tr on Mach-O,
# .vmpilot_trust_root on ELF) — see runtime/include/trust_root.hpp.
#
# For now, the override path is a TODO tracked against Stage 3 completion.
# The default file runtime/src/trust_root_embed.cpp is compiled as-is when
# no override is requested.

function(vmpilot_configure_trust_root target)
    if(DEFINED VMPILOT_VENDOR_PUBKEY)
        message(FATAL_ERROR
            "VMPILOT_VENDOR_PUBKEY override is not implemented yet. "
            "Remove the flag to fall back to the development trust root, "
            "or file against Stage 3 follow-up work to add configure_file "
            "generation from a hex / binary pubkey input.")
    endif()

    # Nothing to inject in the default path — the static default lives in
    # runtime/src/trust_root_embed.cpp and is already in the target's
    # source list.
    set_property(TARGET ${target} APPEND PROPERTY
        COMPILE_DEFINITIONS "VMPILOT_TRUST_ROOT_DEFAULT=1")
endfunction()
