#pragma once
#ifndef __COMMON_VM_XEX_SPECK64_HPP__
#define __COMMON_VM_XEX_SPECK64_HPP__

/// @file xex_speck64.hpp
/// @brief XEX tweakable encryption mode over Speck64/128 for register FPE.
///
/// WHY XEX MODE (Rogaway 2004):
///
///   Each VM register needs a DIFFERENT encryption even under the SAME key —
///   otherwise register r0 and r1 holding the same plaintext would produce
///   identical ciphertext, leaking equality information.
///
///   XEX solves this with a per-register "tweak":
///     Enc_T(P) = E_K(P ⊕ T) ⊕ T
///     Dec_T(C) = E_K^{-1}(C ⊕ T) ⊕ T
///
///   where T = α · 2^r in GF(2^64), with α = E_K(0) as the base tweak.
///   Different registers get different tweaks, so same plaintext → different
///   ciphertext.
///
/// SECURITY ARGUMENT (per-register independence):
///
///   For registers r ≠ s with the same plaintext P:
///     Enc_{T_r}(P) = E_K(P ⊕ T_r) ⊕ T_r
///     Enc_{T_s}(P) = E_K(P ⊕ T_s) ⊕ T_s
///
///   Since T_r ≠ T_s (GF elements are distinct for r ≠ s), the Speck inputs
///   (P ⊕ T_r) and (P ⊕ T_s) differ, and E_K is a PRP, so the outputs are
///   computationally independent.  An adversary seeing both ciphertexts gains
///   no information about whether the plaintexts are equal.
///
/// GF(2^64) ARITHMETIC:
///
///   We use the irreducible polynomial x^64 + x^4 + x^3 + x + 1 over GF(2).
///   The reduction constant is 0x1B (representing x^4 + x^3 + x + 1).
///
///   Doubling in GF(2^64): if the high bit of δ is 0, shift left by 1.
///   If the high bit is 1, shift left by 1 and XOR with 0x1B.
///   This is the standard XTS/XEX tweak derivation.
///
/// REFERENCE: P. Rogaway, "Efficient Instantiations of Tweakable
///   Blockciphers and Refinements to Modes OCB and PMAC", Asiacrypt 2004.

#include "speck64.hpp"

#include <cstdint>

namespace VMPilot::Common::VM::Crypto {

/// Number of register tweaks (one per VM register).
static constexpr unsigned XEX_NUM_TWEAKS = 16;

/// GF(2^64) reduction polynomial: x^64 + x^4 + x^3 + x + 1.
/// Only the low-order bits matter after reduction: x^4 + x^3 + x + 1 = 0x1B.
static constexpr uint64_t GF64_REDUCTION = 0x1B;

/// Pre-computed XEX tweak state (alpha + 16 derived tweaks).
struct XEX_Tweaks {
    uint64_t t[XEX_NUM_TWEAKS];
};

/// Double an element in GF(2^64).
///
/// CONSTANT-TIME PROOF:
///   The mask `uint64_t(0) - carry` evaluates to either 0x0 or 0xFFFFFFFFFFFFFFFF.
///   Both branches of the conditional XOR execute in the same number of cycles
///   because the XOR with (mask & constant) has no data-dependent branch.
///   The shift, subtraction, and XOR are all constant-time on all architectures.
inline uint64_t GF64_Double(uint64_t delta) noexcept {
    uint64_t carry = delta >> 63;              // 0 or 1
    delta <<= 1;
    delta ^= (uint64_t(0) - carry) & GF64_REDUCTION;  // conditional reduction
    return delta;
}

/// Compute base tweak α = E_K(0) and derive per-register tweaks.
///
/// Tweak derivation: T[0] = α, T[i] = GF64_Double(T[i-1]) for i > 0.
///
/// WHY derive from E_K(0):
///   Using the cipher itself to derive the base tweak binds the tweak space
///   to the key.  An adversary who doesn't know K cannot predict α, and
///   therefore cannot predict any T[r].  This is the standard XEX construction.
inline void XEX_ComputeTweaks(const Speck64_RoundKeys& rk,
                              XEX_Tweaks& tweaks) noexcept {
    uint64_t alpha = Speck64_Encrypt(rk, 0);   // α = E_K(0)
    tweaks.t[0] = alpha;
    for (unsigned i = 1; i < XEX_NUM_TWEAKS; ++i)
        tweaks.t[i] = GF64_Double(tweaks.t[i - 1]);
}

/// XEX encrypt: C = E_K(P ⊕ T) ⊕ T
inline uint64_t XEX_Encrypt(const Speck64_RoundKeys& rk,
                            uint64_t tweak,
                            uint64_t plaintext) noexcept {
    return Speck64_Encrypt(rk, plaintext ^ tweak) ^ tweak;
}

/// XEX decrypt: P = E_K^{-1}(C ⊕ T) ⊕ T
inline uint64_t XEX_Decrypt(const Speck64_RoundKeys& rk,
                            uint64_t tweak,
                            uint64_t ciphertext) noexcept {
    return Speck64_Decrypt(rk, ciphertext ^ tweak) ^ tweak;
}

// ─────────────────────────────────────────────────────────────────────────────
// Convenience: FPE_Encode / FPE_Decode for a single register value
// ─────────────────────────────────────────────────────────────────────────────

/// Encode a plaintext register value using the current FPE key.
///
/// This is the doc 16 equivalent of "encode_register" from the LUT model.
/// The encoding is determined by (key, register_index) — different registers
/// under the same key produce different ciphertexts for the same plaintext.
///
/// SECURITY: XEX is IND-TCPA (indistinguishable under chosen-plaintext
/// attack with adaptive tweak queries) when E_K is a secure PRP.
/// Speck64/128 with 27 rounds is a secure PRP (10-round margin over
/// best known differential attack).
inline uint64_t FPE_Encode(const Speck64_RoundKeys& rk,
                           const XEX_Tweaks& tweaks,
                           uint8_t reg,
                           uint64_t plaintext) noexcept {
    return XEX_Encrypt(rk, tweaks.t[reg & 0x0F], plaintext);
}

/// Decode an encoded register value back to plaintext.
inline uint64_t FPE_Decode(const Speck64_RoundKeys& rk,
                           const XEX_Tweaks& tweaks,
                           uint8_t reg,
                           uint64_t ciphertext) noexcept {
    return XEX_Decrypt(rk, tweaks.t[reg & 0x0F], ciphertext);
}

/// Convenience: key schedule + tweaks + encode in one call.
/// Suitable for one-off operations (e.g., initial register encoding).
/// For per-instruction use, pre-compute rk + tweaks and call FPE_Encode.
inline uint64_t FPE_Encode(const uint8_t key128[16],
                           uint8_t reg,
                           uint64_t plaintext) noexcept {
    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key128, rk);
    XEX_Tweaks tw;
    XEX_ComputeTweaks(rk, tw);
    return FPE_Encode(rk, tw, reg, plaintext);
}

/// Convenience: key schedule + tweaks + decode in one call.
inline uint64_t FPE_Decode(const uint8_t key128[16],
                           uint8_t reg,
                           uint64_t ciphertext) noexcept {
    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key128, rk);
    XEX_Tweaks tw;
    XEX_ComputeTweaks(rk, tw);
    return FPE_Decode(rk, tw, reg, ciphertext);
}

}  // namespace VMPilot::Common::VM::Crypto

#endif  // __COMMON_VM_XEX_SPECK64_HPP__
