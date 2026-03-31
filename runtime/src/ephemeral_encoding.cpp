#include <ephemeral_encoding.hpp>
#include <encoding.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime {

using namespace Common::VM;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

void generate_ephemeral_tables(
    VMContext& ctx,
    const char* domain, size_t domain_len,
    const uint8_t* extra, size_t extra_len,
    EphemeralTables& out) noexcept
{
    const uint64_t nonce = ctx.class_c_nonce++;

    // Build context: domain || extra || nonce_le64
    uint8_t context[64];  // max reasonable size
    size_t ctx_len = 0;
    if (domain_len > 0 && domain_len <= 8) {
        std::memcpy(context, domain, domain_len);
        ctx_len += domain_len;
    }
    if (extra_len > 0 && extra_len <= 16) {
        std::memcpy(context + ctx_len, extra, extra_len);
        ctx_len += extra_len;
    }
    std::memcpy(context + ctx_len, &nonce, 8);
    ctx_len += 8;

    uint8_t trans_seed[32];
    blake3_kdf(ctx.stored_seed, reinterpret_cast<const char*>(context),
               ctx_len, trans_seed, 32);

    for (uint8_t k = 0; k < VM_BYTE_LANES; ++k) {
        uint8_t lane_seed[32];
        blake3_keyed_hash(trans_seed, &k, 1, lane_seed, 32);
        generate_bijection(lane_seed, out.enc[k]);
        invert_bijection(out.enc[k], out.dec[k]);
    }
}

uint64_t ephemeral_decode(
    const VMContext& ctx,
    uint8_t reg, uint64_t encoded,
    const EphemeralTables& et) noexcept
{
    // decode from register domain → plaintext → re-encode with ephemeral LUT.
    // Plaintext exists in a GPR for exactly ONE instruction.
    uint64_t plain = decode_register(ctx, reg, encoded);
    return apply_byte_lane_lut(et.enc, plain);
}

void ephemeral_zero(EphemeralTables& et) noexcept {
    volatile uint8_t* p = reinterpret_cast<volatile uint8_t*>(&et);
    for (size_t i = 0; i < sizeof(et); ++i) p[i] = 0;
}

}  // namespace VMPilot::Runtime
