// Build-time-embedded vendor trust root.
//
// See runtime/include/trust_root.hpp for the struct contract and the linker
// section rationale. Placing the record in a named read-only section lets
// post-link audit tools find it by name and lets platform hardening (W^X,
// DEP, signed-section policy) apply without extra runtime plumbing.
//
// Development builds embed the RFC 8032 §7.1 Test 1 public key so that
// fixtures produced by runtime/test/fixtures/ verify through the Stage 2
// adaptor without extra configuration. Production builds override the
// contents by regenerating this file from cmake/TrustRoot.cmake before
// linking.

#include "trust_root.hpp"

#include <cstring>

namespace {

// Section placement. Section names differ per-format so the output of `size`
// / `objdump -h` / `dumpbin /headers` always contains a recognisable token
// regardless of target.
#if defined(_WIN32)
    // PE section names are limited to 8 bytes. `.vmpltr` matches the naming
    // convention used by the existing `.vmpltt` payload section.
    #pragma section(".vmpltr", read)
    #define VMPILOT_TRUST_ROOT_ATTR __declspec(allocate(".vmpltr"))
#elif defined(__APPLE__)
    // Mach-O sections are `__SEG,__section`, 16-byte limit on each.
    #define VMPILOT_TRUST_ROOT_ATTR \
        __attribute__((used, section("__DATA,__vmpilot_tr")))
#else
    // ELF: straight .section directive via attribute.
    #define VMPILOT_TRUST_ROOT_ATTR \
        __attribute__((used, section(".vmpilot_trust_root")))
#endif

}  // namespace

// Placement of the literal itself. `extern "C"` so the symbol is stable
// across C++ mangling schemes, `const` so the toolchain places it in the
// read-only (or at least un-writable) segment backing the named section.
extern "C" VMPILOT_TRUST_ROOT_ATTR
const ::VMPilot::Runtime::VendorTrustRoot g_vmpilot_vendor_trust_root = {
    /* magic */             ::VMPilot::Runtime::kVendorTrustRootMagic,
    /* version */           ::VMPilot::Runtime::kVendorTrustRootVersion,
    /* root_epoch */        1,
    /* public_key */        {
        // RFC 8032 §7.1 Test 1. Development default; see file header.
        0xd7, 0x5a, 0x98, 0x01, 0x82, 0xb1, 0x0a, 0xb7,
        0xd5, 0x4b, 0xfe, 0xd3, 0xc9, 0x64, 0x07, 0x3a,
        0x0e, 0xe1, 0x72, 0xf3, 0xda, 0xa6, 0x23, 0x25,
        0xaf, 0x02, 0x1a, 0x68, 0xf7, 0x07, 0x51, 0x1a,
    },
    /* root_key_id */       "vmpilot-dev-rfc8032-test1",
    /* signature_alg_id */  "ed25519-pure-v1",
    /* key_usage */         "artifact_binding_root",
    /* reserved */          {0},
};

namespace VMPilot::Runtime {

const VendorTrustRoot& trust_root() noexcept {
    return g_vmpilot_vendor_trust_root;
}

bool trust_root_is_well_formed() noexcept {
    const auto& r = g_vmpilot_vendor_trust_root;
    if (r.magic != kVendorTrustRootMagic) return false;
    if (r.version != kVendorTrustRootVersion) return false;
    if (r.root_epoch == 0) return false;

    // Fixed-size name fields must be null-terminated within their array —
    // tolerate otherwise-mangled values but refuse to accept them as
    // trust anchor material.
    if (std::memchr(r.root_key_id, '\0', sizeof(r.root_key_id)) == nullptr) return false;
    if (std::memchr(r.signature_alg_id, '\0', sizeof(r.signature_alg_id)) == nullptr) return false;
    if (std::memchr(r.key_usage, '\0', sizeof(r.key_usage)) == nullptr) return false;

    if (std::strcmp(r.key_usage, kTrustRootKeyUsage) != 0) return false;

    // Public key must not be all-zero. An all-zero Ed25519 point is not the
    // identity in the group used by Ed25519, but keeping it as "uninitialised
    // sentinel" is a useful build-time guard and costs a single pass.
    bool any_nonzero = false;
    for (auto b : r.public_key) {
        if (b != 0) { any_nonzero = true; break; }
    }
    if (!any_nonzero) return false;

    return true;
}

}  // namespace VMPilot::Runtime
