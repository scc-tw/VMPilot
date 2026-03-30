/// @file anti_tamper.cpp
/// @brief Phase 9.2: Whole-blob integrity verification implementation.
///
/// Recomputes BLAKE3_keyed(integrity_key, blob) and compares against the
/// hash stored in VMContext at load time. Constant-time comparison prevents
/// timing side-channels from leaking which byte position differs.

#include <anti_tamper.hpp>
#include <vm/vm_crypto.hpp>

#include <cstdint>
#include <cstring>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VMContext;
using namespace Common::VM::Crypto;

tl::expected<void, DiagnosticCode>
verify_blob_integrity(const uint8_t* blob_data,
                      size_t blob_size,
                      const VMContext& ctx) noexcept {

    // Recompute BLAKE3_keyed(integrity_key, entire_blob)
    uint8_t computed[32];
    blake3_keyed_hash(ctx.integrity_key, blob_data, blob_size, computed, 32);

    // Constant-time comparison to prevent timing side-channels.
    // An attacker probing which byte differs must not gain information
    // from the comparison duration.
    uint8_t diff = 0;
    for (int i = 0; i < 32; ++i)
        diff |= computed[i] ^ ctx.blob_integrity_hash[i];

    if (diff != 0)
        return tl::make_unexpected(DiagnosticCode::IntegrityCheckFailed);

    return {};
}

}  // namespace VMPilot::Runtime
