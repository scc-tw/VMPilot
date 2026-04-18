#ifndef VMPILOT_RUNTIME_TRUST_ROOT_HPP
#define VMPILOT_RUNTIME_TRUST_ROOT_HPP

#include <cstddef>
#include <cstdint>

// Build-time-embedded vendor trust root.
//
// The runtime verifies every PackageBindingRecord against this public key
// before accepting any unit for execution. The struct is POD with fixed-size
// fields so the linker can place it in a read-only, named section that
// never appears in `.data` and is easy to audit post-link.

namespace VMPilot::Runtime {

// Fixed four-byte identifier. Reading `'V','M','T','R'` in a hex dump is the
// quickest way to confirm the struct has not been stripped or mis-placed.
// Little-endian spelling of the ASCII bytes so the bytes at the section
// start read literally as "VMTR".
inline constexpr std::uint32_t kVendorTrustRootMagic =
    (static_cast<std::uint32_t>('V'))        |
    (static_cast<std::uint32_t>('M') <<  8)  |
    (static_cast<std::uint32_t>('T') << 16)  |
    (static_cast<std::uint32_t>('R') << 24);

inline constexpr std::uint32_t kVendorTrustRootVersion = 1;

inline constexpr std::size_t kEd25519PublicKeySize = 32;

// Fixed-size character arrays so the struct stays POD and the whole record
// fits in the linker-pinned section without heap indirection. Every field is
// null-terminated; anything longer than (size - 1) bytes is rejected at
// build time by the configure step.
struct VendorTrustRoot {
    std::uint32_t magic;
    std::uint32_t version;
    std::uint64_t root_epoch;
    std::uint8_t  public_key[kEd25519PublicKeySize];
    char          root_key_id[32];
    char          signature_alg_id[32];
    char          key_usage[32];
    std::uint8_t  reserved[16];
};

static_assert(sizeof(VendorTrustRoot) == 4 + 4 + 8 + 32 + 32 + 32 + 32 + 16,
              "VendorTrustRoot layout drifted — section consumers depend on it");

// Canonical string constants for field values; exposing them keeps callers
// from hard-coding literals that might drift.
inline constexpr const char* kTrustRootSignatureAlgEd25519 = "ed25519-pure-v1";
inline constexpr const char* kTrustRootKeyUsage           = "artifact_binding_root";

// Returns a reference to the build-time-embedded trust root. Reading the
// reference costs zero code on the hot path; the compiler folds the load
// behind a symbol that the linker resolves to the `.vmpltr` section.
const VendorTrustRoot& trust_root() noexcept;

// Quick structural sanity check. Returns false if the trust root looks
// stripped, rewritten, or otherwise tampered with post-link. Callers should
// treat a false result as fail-closed and abort startup.
bool trust_root_is_well_formed() noexcept;

}  // namespace VMPilot::Runtime

#endif  // VMPILOT_RUNTIME_TRUST_ROOT_HPP
