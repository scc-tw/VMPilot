#pragma once
#ifndef __COMMON_VM_SPECK64_HPP__
#define __COMMON_VM_SPECK64_HPP__

/// @file speck64.hpp
/// @brief Speck64/128 lightweight block cipher — header-only, constant-time.
///
/// WHY HEADER-ONLY:
///   Speck64 key schedule, encrypt, and decrypt are called on every VM
///   instruction during Phases E-H (FPE encode, key ratchet re-encode).
///   Each step() invocation performs at minimum 2 key schedules + 32+
///   block cipher calls.  Inlining eliminates per-call overhead and lets
///   the compiler unroll the 27-round loop.  Pure ARX computation, no
///   platform-specific code, no branches — constant-time by construction.
///
/// WHY SPECK64/128:
///
///   Doc 16 (Forward-Secrecy Extension) requires a 64-bit block cipher for
///   XEX-mode Format-Preserving Encryption (FPE) of register values.
///
///   Speck64/128 is chosen over AES because:
///     1. 64-bit block matches the 64-bit register width exactly — no padding,
///        no ciphertext expansion, no format mismatch.
///     2. ARX-only construction (Add-Rotate-XOR) is inherently constant-time
///        on all CPUs — no S-boxes, no table lookups, no cache-timing leaks.
///     3. 27 rounds with 128-bit key gives 128-bit security level, matching
///        the Speck FPE key size and the BLAKE3_KEYED ratchet output width.
///
/// CONSTANT-TIME ARGUMENT:
///
///   Every operation in Speck is either addition mod 2^32, bitwise rotation
///   by a fixed amount, or XOR.  None of these operations have data-dependent
///   timing on any modern CPU architecture (x86, ARM, RISC-V).  There are no
///   branches, no table lookups, and no data-dependent memory accesses.
///   Therefore, Speck64/128 is constant-time by construction.
///
/// REFERENCE: R. Beaulieu et al., "The SIMON and SPECK Families of
///   Lightweight Block Ciphers", NSA, 2013.  Speck64/128: n=32, m=4, T=27.

#include <cstdint>
#include <cstring>

namespace VMPilot::Common::VM::Crypto {

/// Number of rounds for Speck64/128 (block=64, key=128, word=32, words=4).
static constexpr unsigned SPECK64_ROUNDS = 27;

/// Speck64/128 expanded round keys (27 × 32-bit words).
struct Speck64_RoundKeys {
    uint32_t k[SPECK64_ROUNDS];
};

namespace detail {

/// Right-rotate a 32-bit word by `n` bits.
/// WHY constexpr: enables compile-time evaluation in key schedule unrolling.
constexpr uint32_t rotr32(uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32 - n));
}

/// Left-rotate a 32-bit word by `n` bits.
constexpr uint32_t rotl32(uint32_t x, unsigned n) noexcept {
    return (x << n) | (x >> (32 - n));
}

/// One Speck round function (encryption direction).
///
/// The round function is: x = (ROR(x, 8) + y) ^ k;  y = ROL(y, 3) ^ x
///
/// WHY this specific rotation pair (8, 3):
///   For word size n=32, Speck uses (alpha, beta) = (8, 3).
///   This maximises differential trail weight per round — Beaulieu et al.
///   showed empirically that (8, 3) achieves the best security margin
///   for 32-bit words among all tested rotation constants.
constexpr void speck_round(uint32_t& x, uint32_t& y, uint32_t k) noexcept {
    x = rotr32(x, 8);
    x += y;
    x ^= k;
    y = rotl32(y, 3);
    y ^= x;
}

/// Inverse round function (decryption direction).
constexpr void speck_round_inv(uint32_t& x, uint32_t& y, uint32_t k) noexcept {
    y ^= x;
    y = rotr32(y, 3);
    x ^= k;
    x -= y;
    x = rotl32(x, 8);
}

}  // namespace detail

/// Expand 128-bit key into 27 round keys.
///
/// KEY SCHEDULE STRUCTURE (Speck64/128, m=4 key words):
///
///   The key schedule uses the same round function as encryption, applied to
///   the key words.  Starting from K = (l[2], l[1], l[0], k[0]):
///
///     For i = 0..25:
///       l[i+3] = (ROR(l[i], 8) + k[i]) ^ i
///       k[i+1] = ROL(k[i], 3) ^ l[i+3]
///
///   Round key i = k[i].
///
///   WHY the round function for key schedule:
///     Using the same ARX structure for both data path and key schedule means
///     Speck has no "independent subkeys" — changing any key bit cascades
///     through all subsequent round keys.  This is desirable for FPE because
///     it means the per-instruction key ratchet (BLAKE3_KEYED output) fully
///     determines the Speck permutation with no weak-key classes.
inline void Speck64_KeySchedule(const uint8_t key128[16],
                                Speck64_RoundKeys& rk) noexcept {
    // Speck64/128 key words: K = (l[2], l[1], l[0], k[0]) in little-endian.
    // key bytes [0..3] = k[0], [4..7] = l[0], [8..11] = l[1], [12..15] = l[2]
    uint32_t l[SPECK64_ROUNDS + 3];  // l[0..28]
    uint32_t k_sch;                  // running key word

    std::memcpy(&k_sch, key128 + 0, 4);
    std::memcpy(&l[0],  key128 + 4, 4);
    std::memcpy(&l[1],  key128 + 8, 4);
    std::memcpy(&l[2],  key128 + 12, 4);

    rk.k[0] = k_sch;

    for (unsigned i = 0; i < SPECK64_ROUNDS - 1; ++i) {
        // Key schedule round: same structure as encryption round with
        // round counter `i` as the "key" (prevents slide attacks).
        l[i + 3] = (detail::rotr32(l[i], 8) + k_sch) ^ i;
        k_sch    = detail::rotl32(k_sch, 3) ^ l[i + 3];
        rk.k[i + 1] = k_sch;
    }
}

/// Encrypt a 64-bit block with Speck64/128.
///
/// Block = (x, y) where x is the "left" (high) word and y is the "right"
/// (low) word, both 32 bits.  Input/output is a single uint64_t:
///   plaintext = (x << 32) | y
///
/// SECURITY: 27 rounds provide full diffusion.  Best known attack on
/// Speck64/128 covers 17/27 rounds (Biryukov et al., differential),
/// leaving a 10-round security margin.
inline uint64_t Speck64_Encrypt(const Speck64_RoundKeys& rk,
                                uint64_t plaintext) noexcept {
    uint32_t x = static_cast<uint32_t>(plaintext >> 32);
    uint32_t y = static_cast<uint32_t>(plaintext);

    for (unsigned i = 0; i < SPECK64_ROUNDS; ++i)
        detail::speck_round(x, y, rk.k[i]);

    return (static_cast<uint64_t>(x) << 32) | y;
}

/// Decrypt a 64-bit block with Speck64/128.
inline uint64_t Speck64_Decrypt(const Speck64_RoundKeys& rk,
                                uint64_t ciphertext) noexcept {
    uint32_t x = static_cast<uint32_t>(ciphertext >> 32);
    uint32_t y = static_cast<uint32_t>(ciphertext);

    for (int i = SPECK64_ROUNDS - 1; i >= 0; --i)
        detail::speck_round_inv(x, y, rk.k[i]);

    return (static_cast<uint64_t>(x) << 32) | y;
}

}  // namespace VMPilot::Common::VM::Crypto

#endif  // __COMMON_VM_SPECK64_HPP__
