#pragma once
#ifndef __COMMON_VM_CRYPTO_HPP__
#define __COMMON_VM_CRYPTO_HPP__

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace VMPilot::Common::VM::Crypto {

// -------------------------------------------------------------------------
// SipHash-2-4  (header-only, standalone implementation)
// -------------------------------------------------------------------------

namespace detail {

/// Left-rotate a 64-bit value by N bits.
template <unsigned N>
[[nodiscard]] constexpr uint64_t rotl(uint64_t x) noexcept {
    static_assert(N < 64, "rotation amount must be < 64");
    return (x << N) | (x >> (64 - N));
}

/// One SipRound.
inline void sip_round(uint64_t& v0, uint64_t& v1,
                      uint64_t& v2, uint64_t& v3) noexcept {
    v0 += v1;  v2 += v3;
    v1 = rotl<13>(v1);  v3 = rotl<16>(v3);
    v1 ^= v0;  v3 ^= v2;
    v0 = rotl<32>(v0);
    v2 += v1;  v0 += v3;
    v1 = rotl<17>(v1);  v3 = rotl<21>(v3);
    v1 ^= v2;  v3 ^= v0;
    v2 = rotl<32>(v2);
}

/// Read a little-endian uint64_t from a byte pointer.
[[nodiscard]] inline uint64_t le64(const uint8_t* p) noexcept {
    uint64_t v = 0;
    std::memcpy(&v, p, 8);
    // On big-endian systems this would need a byte swap, but in practice
    // VMPilot targets little-endian (x86/ARM).
    return v;
}

}  // namespace detail

/// Core SipHash-2-4: returns 64-bit hash of an arbitrary-length message.
///
/// @param key  16-byte key
/// @param msg  pointer to message bytes (may be nullptr if len == 0)
/// @param len  message length in bytes
/// @return     64-bit SipHash digest
[[nodiscard]] inline uint64_t siphash_2_4(const uint8_t key[16],
                                          const uint8_t* msg,
                                          size_t len) noexcept {
    // Key setup
    const uint64_t k0 = detail::le64(key);
    const uint64_t k1 = detail::le64(key + 8);

    uint64_t v0 = k0 ^ UINT64_C(0x736F6D6570736575);
    uint64_t v1 = k1 ^ UINT64_C(0x646F72616E646F6D);
    uint64_t v2 = k0 ^ UINT64_C(0x6C7967656E657261);
    uint64_t v3 = k1 ^ UINT64_C(0x7465646279746573);

    // Process full 8-byte blocks
    const size_t blocks = len / 8;
    for (size_t i = 0; i < blocks; ++i) {
        uint64_t m = detail::le64(msg + i * 8);
        v3 ^= m;
        detail::sip_round(v0, v1, v2, v3);
        detail::sip_round(v0, v1, v2, v3);
        v0 ^= m;
    }

    // Last partial block + length byte
    uint64_t last = static_cast<uint64_t>(len & 0xFF) << 56;
    const uint8_t* tail = msg + blocks * 8;
    const size_t remaining = len & 7;

    switch (remaining) {
        case 7: last |= static_cast<uint64_t>(tail[6]) << 48; [[fallthrough]];
        case 6: last |= static_cast<uint64_t>(tail[5]) << 40; [[fallthrough]];
        case 5: last |= static_cast<uint64_t>(tail[4]) << 32; [[fallthrough]];
        case 4: last |= static_cast<uint64_t>(tail[3]) << 24; [[fallthrough]];
        case 3: last |= static_cast<uint64_t>(tail[2]) << 16; [[fallthrough]];
        case 2: last |= static_cast<uint64_t>(tail[1]) << 8;  [[fallthrough]];
        case 1: last |= static_cast<uint64_t>(tail[0]);        break;
        case 0: break;
    }

    v3 ^= last;
    detail::sip_round(v0, v1, v2, v3);
    detail::sip_round(v0, v1, v2, v3);
    v0 ^= last;

    // Finalization
    v2 ^= 0xFF;
    detail::sip_round(v0, v1, v2, v3);
    detail::sip_round(v0, v1, v2, v3);
    detail::sip_round(v0, v1, v2, v3);
    detail::sip_round(v0, v1, v2, v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

/// Pipeline step 2: keystream for instruction decryption.
///
/// keystream = SipHash(fast_key XOR enc_state, j)
/// where j is encoded as an 8-byte little-endian value.
///
/// @param key        16-byte fast_key
/// @param enc_state  current encryption state
/// @param index      instruction index j within the basic block
/// @return           64-bit keystream word
[[nodiscard]] inline uint64_t siphash_keystream(const uint8_t key[16],
                                                uint64_t enc_state,
                                                uint32_t index) noexcept {
    // XOR enc_state into the key
    uint8_t tweaked[16];
    std::memcpy(tweaked, key, 16);

    // XOR enc_state into the first 8 bytes of the key
    uint64_t k0;
    std::memcpy(&k0, tweaked, 8);
    k0 ^= enc_state;
    std::memcpy(tweaked, &k0, 8);

    // Encode index as 8-byte LE message
    uint8_t msg[8];
    uint64_t idx64 = static_cast<uint64_t>(index);
    std::memcpy(msg, &idx64, 8);

    return siphash_2_4(tweaked, msg, 8);
}

/// ORAM: expand SipHash into 64 bytes (8 x uint64_t) for cache line keystream.
///
/// Each output word: SipHash(key, nonce || line || sub_index)
///
/// @param key    16-byte ORAM key
/// @param nonce  ORAM nonce
/// @param line   cache line index
/// @param out    output array of 8 uint64_t words (64 bytes total)
inline void siphash_expand(const uint8_t key[16],
                           uint64_t nonce,
                           uint64_t line,
                           uint64_t out[8]) noexcept {
    for (uint64_t i = 0; i < 8; ++i) {
        // Build a 24-byte message: nonce(8) || line(8) || sub_index(8)
        uint8_t msg[24];
        std::memcpy(msg,      &nonce, 8);
        std::memcpy(msg + 8,  &line,  8);
        std::memcpy(msg + 16, &i,     8);
        out[i] = siphash_2_4(key, msg, 24);
    }
}

// -------------------------------------------------------------------------
// BLAKE3 wrappers  (implemented in vm_crypto.cpp, linked against BLAKE3 C)
// -------------------------------------------------------------------------

/// BLAKE3 KDF: derive key material from seed + context string.
///
/// Uses BLAKE3 derive_key mode internally.
///
/// @param seed         32-byte seed material
/// @param context      context string (domain separation)
/// @param context_len  length of context string
/// @param out          output buffer for derived key material
/// @param out_len      number of bytes to derive
void blake3_kdf(const uint8_t seed[32], const char* context, size_t context_len,
                uint8_t* out, size_t out_len) noexcept;

/// BLAKE3 keyed hash (for BB MAC verification).
///
/// @param key       32-byte key
/// @param data      input data
/// @param data_len  length of input data
/// @param out       output buffer for hash
/// @param out_len   number of output bytes
void blake3_keyed_hash(const uint8_t key[32], const uint8_t* data, size_t data_len,
                       uint8_t* out, size_t out_len) noexcept;

// -------------------------------------------------------------------------
// BLAKE3_KEYED with 128-bit key  (doc 16 forward-secrecy extension)
// -------------------------------------------------------------------------

/// BLAKE3 keyed hash using a 128-bit (16-byte) key.
///
/// WHY 128-BIT KEY:
///
///   The Speck FPE key is 128 bits (matching Speck64/128's key size).
///   Doc 16's key ratchet and fingerprint operations need a keyed hash
///   using this 128-bit key.  BLAKE3's keyed mode requires 32 bytes.
///
///   Extension strategy: repeat the 16-byte key → [K || K] (32 bytes).
///   This provides 128-bit security (not 256-bit), which is sufficient
///   because the entire FPE chain is 128-bit security (Speck64/128).
///   Doubling the key provides no additional entropy — the effective
///   keyspace is still 2^128, matching the cipher's security level.
///
/// CLEANUP: The 32-byte extended key is zeroed after use to prevent
///   the (redundant) copy from lingering in stack memory.
///
/// @param key128    16-byte key (Speck FPE key)
/// @param data      input data
/// @param data_len  length of input data
/// @param out       output buffer for hash
/// @param out_len   number of output bytes
void blake3_keyed_128(const uint8_t key128[16],
                      const uint8_t* data, size_t data_len,
                      uint8_t* out, size_t out_len) noexcept;

/// Fingerprint all 16 encoded registers using BLAKE3_KEYED_128.
///
/// WHY FINGERPRINT ALL REGISTERS:
///
///   Doc 16 §4, Phase F: after each instruction, the VM computes
///   fingerprint = BLAKE3_KEYED_128(insn_fpe_key, encoded_regs[0..15]).
///   This fingerprint feeds into the key ratchet (Phase G), entangling
///   the next instruction's FPE key with ALL current register state.
///
///   Result: modifying any register value (even one not involved in the
///   current instruction) alters the fingerprint, which alters the next
///   key, which corrupts all subsequent decryptions.  This is the core
///   of doc 16's "entanglement" property.
///
/// @param key128       16-byte FPE key
/// @param encoded_regs array of 16 × uint64_t encoded register values
/// @param out128       16-byte output fingerprint
void blake3_keyed_fingerprint(const uint8_t key128[16],
                              const uint64_t encoded_regs[16],
                              uint8_t out128[16]) noexcept;

}  // namespace VMPilot::Common::VM::Crypto

#endif  // __COMMON_VM_CRYPTO_HPP__
