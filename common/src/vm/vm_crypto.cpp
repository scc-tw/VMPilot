#include <vm/vm_crypto.hpp>
#include <vm/secure_zero.hpp>

#include <blake3.h>
#include <cstring>

namespace VMPilot::Common::VM::Crypto {

void blake3_kdf(const uint8_t seed[32], const char* context, size_t context_len,
                uint8_t* out, size_t out_len) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init_derive_key_raw(&hasher, context, context_len);
    blake3_hasher_update(&hasher, seed, 32);
    blake3_hasher_finalize(&hasher, out, out_len);
}

void blake3_keyed_hash(const uint8_t key[32], const uint8_t* data, size_t data_len,
                       uint8_t* out, size_t out_len) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, key);
    blake3_hasher_update(&hasher, data, data_len);
    blake3_hasher_finalize(&hasher, out, out_len);
}

void blake3_preexpand_128(const uint8_t key128[16],
                          uint8_t out256[32]) noexcept {
    std::memcpy(out256, key128, 16);
    std::memcpy(out256 + 16, key128, 16);
}

void blake3_keyed_preexpanded(const uint8_t expanded_key[32],
                              const uint8_t* data, size_t data_len,
                              uint8_t* out, size_t out_len) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, expanded_key);
    blake3_hasher_update(&hasher, data, data_len);
    blake3_hasher_finalize(&hasher, out, out_len);
}

void blake3_keyed_fingerprint_preexpanded(const uint8_t expanded_key[32],
                                          const uint64_t encoded_regs[16],
                                          uint8_t out128[16]) noexcept {
    blake3_keyed_preexpanded(expanded_key,
                             reinterpret_cast<const uint8_t*>(encoded_regs),
                             16 * sizeof(uint64_t),
                             out128, 16);
}

void blake3_keyed_128(const uint8_t key128[16],
                      const uint8_t* data, size_t data_len,
                      uint8_t* out, size_t out_len) noexcept {
    // Extend 128-bit key to 256-bit by repeating: [K || K].
    // Security level remains 128-bit (matching Speck64/128 key size).
    uint8_t extended[32];
    blake3_preexpand_128(key128, extended);

    blake3_keyed_preexpanded(extended, data, data_len, out, out_len);

    // Zero the extended key — prevent the redundant copy from persisting
    // in stack memory after this function returns.
    Common::VM::secure_zero(extended, 32);
}

void blake3_keyed_fingerprint(const uint8_t key128[16],
                              const uint64_t encoded_regs[16],
                              uint8_t out128[16]) noexcept {
    // Hash all 16 registers (128 bytes) into a 16-byte fingerprint.
    // The fingerprint feeds into the key ratchet (doc 16 §4, Phase G),
    // entangling the next FPE key with the full register state.
    blake3_keyed_128(key128,
                     reinterpret_cast<const uint8_t*>(encoded_regs),
                     16 * sizeof(uint64_t),
                     out128, 16);
}

}  // namespace VMPilot::Common::VM::Crypto
