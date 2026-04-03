#pragma once
#ifndef __COMMON_VM_BLOB_HPP__
#define __COMMON_VM_BLOB_HPP__

/// @file vm_blob.hpp
/// @brief Bytecode blob format v1 — shared contract between compiler and runtime.
///
/// The blob carries encrypted bytecodes and metadata but NOT encoding tables.
/// Encoding tables are derived at runtime from epoch_seeds embedded in the
/// encrypted BB metadata, using the shared generate_bijection() function in
/// vm_encoding.hpp.
///
/// Why no encoding tables in the blob:
///
///   1. MCSP Hardness is preserved under PRF-based derivation.
///      Shannon's counting argument shows a random bijection on {0..255}
///      has MinCircuit = Ω(2^n / n) ≈ Ω(32) gates for n = 8.
///      Because BLAKE3 is a PRF, the output of
///          BLAKE3(epoch_seed, r, k) → table_seed → Fisher-Yates
///      is computationally indistinguishable from a truly random bijection
///      for any PPT adversary. Therefore the derived tables inherit the
///      same MCSP hardness as if they were sampled uniformly at random.
///
///   2. Composition tables for Class A (bitwise) operations are built lazily
///      at the first use of each unique (op, dst, src_a, src_b) register
///      triple within a BB, then cached for the BB's duration.
///      The build process iterates over all 256×256 input combinations —
///      no actual data values appear — so the "zero plaintext" property
///      of Class A operations (spec §3.4) is maintained.
///
///   3. The epoch_seed for each BB is stored inside encrypted BB metadata.
///      Metadata encryption with meta_key = BLAKE3(stored_seed, "meta")
///      is the critical barrier in the security model's circular dependency
///      chain (spec §3.3): without stored_seed an attacker cannot reach
///      epoch_seeds, and therefore cannot derive encoding tables.

#include <cstdint>

namespace VMPilot::Common::VM {

// ─────────────────────────────────────────────────────────────────────────────
// Constants
// ─────────────────────────────────────────────────────────────────────────────

/// "VMP1" in little-endian — identifies the blob format.
constexpr uint32_t VM_BLOB_MAGIC   = 0x31504D56;

/// Blob format version.  Breaking changes increment this.
constexpr uint16_t VM_BLOB_VERSION = 2;  // v2: Doc 17 full-instruction ratchet (8-byte enc_state input)

// ─────────────────────────────────────────────────────────────────────────────
// Header (32 bytes)
// ─────────────────────────────────────────────────────────────────────────────

/// Fixed-size header at offset 0 of every blob.
///
/// All subsequent section offsets are computable from these fields —
/// no section directory is needed.  Every section is naturally 8-byte
/// aligned because every item size is a multiple of 8.
struct BlobHeader {
    uint32_t magic;               ///< VM_BLOB_MAGIC
    uint16_t version;             ///< VM_BLOB_VERSION
    uint16_t flags;               ///< bit 0: debug_mode
    uint32_t total_size;          ///< blob size in bytes including this header
    uint32_t insn_count;          ///< N_total (real + chaff ≈ 1.3× real)
    uint32_t bb_count;            ///< number of basic blocks
    uint32_t pool_entry_count;    ///< constant-pool entries (each 8 bytes)
    uint32_t native_call_count;   ///< NATIVE_CALL transition entries (0 in v1)
    uint32_t reserved;            ///< must be 0
};
static_assert(sizeof(BlobHeader) == 32, "BlobHeader must be exactly 32 bytes");

// ─────────────────────────────────────────────────────────────────────────────
// Per-BB metadata (64 bytes, stored encrypted in the blob)
// ─────────────────────────────────────────────────────────────────────────────

/// Serialised form of one BB's metadata inside the blob.
///
/// Encrypted with  meta_key = BLAKE3(stored_seed, "meta")[0:16]
/// using position-dependent SipHash XOR (word index as nonce).
///
/// Why encrypt:
///   epoch_seed is the sole non-derivable secret per BB.
///   It is the root from which all per-BB encoding tables are derived
///   (spec §3.2).  If metadata were plaintext, the circular-dependency
///   defence collapses into a linear pipeline:
///       stored_seed → epoch_seeds → encoding tables → decode all values.
///   Encrypting metadata forces the attacker to possess stored_seed
///   before reaching epoch_seeds, adding one mandatory step (spec §3.3).
///
/// bb_enc_seed is NOT stored here — it is deterministically derivable as
///   BLAKE3(stored_seed, "enc", bb_id)[0:8]   (spec §13.5)
/// so storing it would be redundant.
struct SerializedBBMeta {
    uint32_t bb_id;               ///< unique BB identifier
    uint32_t epoch;               ///< epoch number (encoding evolution, spec §3.2)
    uint32_t entry_ip;            ///< global instruction index where this BB starts
    uint32_t insn_count_in_bb;    ///< instruction count (real + chaff in this BB)
    uint16_t live_regs_bitmap;    ///< bit i set ⇒ register i is live (D2 liveness, spec §3.7)
    uint16_t flags;               ///< bit 0: epoch differs from predecessor → apply RE_TABLE
    uint8_t  epoch_seed[32];      ///< random, per-BB encoding seed (D2, spec §3.2)
    uint32_t reserved_0;           ///< must be zero
    uint32_t reserved_1;           ///< must be zero
    uint32_t reserved_2;           ///< must be zero (pads struct to 64 bytes)
};
static_assert(sizeof(SerializedBBMeta) == 64, "SerializedBBMeta must be exactly 64 bytes");

/// Flags within SerializedBBMeta::flags.
constexpr uint16_t BB_FLAG_EPOCH_CHANGED = 0x0001;

// ─────────────────────────────────────────────────────────────────────────────
// Transition entry (32 bytes, for NATIVE_CALL)
// ─────────────────────────────────────────────────────────────────────────────

/// Per-NATIVE_CALL metadata: target, arg layout, and calling convention.
///
/// arg_count is bit-packed (v2 layout):
///
///   [4:0]   = actual_arg_count (0–31)
///   [12:5]  = fp_arg_mask (bit i set ⇒ arg i is double/float)
///   [13]    = is_variadic (x86-64 SysV: sets AL = fp arg count)
///   [14]    = returns_fp  (result in xmm0/d0/ST(0) instead of rax/x0/eax)
///   [15]    = returns_struct (hidden first pointer arg)
///   [17:16] = calling_convention:
///       00 = platform-default (cdecl on x86-32, SysV on Linux x64, MS on Win x64)
///       01 = stdcall (Windows x86-32: callee cleans stack)
///       10 = thiscall (reserved — issue #13)
///       11 = fastcall (reserved — issue #13)
///   [31:18] = struct_return_size (14 bits, max 16383 bytes)
struct TransitionEntry {
    uint32_t call_site_ip;        ///< instruction index of the NATIVE_CALL
    uint32_t arg_count;           ///< bit-packed: see above
    uint64_t target_offset;       ///< native function offset from load base
    uint64_t coeff_a;             ///< reserved (was affine coefficient, now unused)
    uint64_t coeff_b;             ///< reserved
};
static_assert(sizeof(TransitionEntry) == 32, "TransitionEntry must be exactly 32 bytes");

/// Helpers for TransitionEntry.arg_count bit-field extraction (v2 layout).
constexpr uint8_t te_arg_count(const TransitionEntry& e) noexcept {
    return static_cast<uint8_t>(e.arg_count & 0x1Fu);  // 5 bits (0-31)
}
constexpr uint8_t te_fp_mask(const TransitionEntry& e) noexcept {
    return static_cast<uint8_t>((e.arg_count >> 5) & 0xFFu);  // 8 bits
}
constexpr bool te_is_variadic(const TransitionEntry& e) noexcept {
    return (e.arg_count & (1u << 13)) != 0;
}
constexpr bool te_returns_fp(const TransitionEntry& e) noexcept {
    return (e.arg_count & (1u << 14)) != 0;
}
constexpr bool te_returns_struct(const TransitionEntry& e) noexcept {
    return (e.arg_count & (1u << 15)) != 0;
}
constexpr uint8_t te_convention(const TransitionEntry& e) noexcept {
    return static_cast<uint8_t>((e.arg_count >> 16) & 0x03u);  // 2 bits
}
constexpr uint16_t te_struct_size(const TransitionEntry& e) noexcept {
    return static_cast<uint16_t>(e.arg_count >> 18);  // 14 bits
}

/// Calling convention constants for te_convention().
constexpr uint8_t TE_CONV_DEFAULT  = 0;  ///< cdecl (x86-32) / SysV (Linux x64) / MS (Win x64)
constexpr uint8_t TE_CONV_STDCALL  = 1;  ///< Windows x86-32: callee cleans stack
constexpr uint8_t TE_CONV_THISCALL = 2;  ///< reserved (issue #13)
constexpr uint8_t TE_CONV_FASTCALL = 3;  ///< reserved (issue #13)

/// Build a v2 arg_count value from components.
constexpr uint32_t te_pack_arg_count(
    uint8_t argc, uint8_t fp_mask, bool is_variadic, bool returns_fp,
    bool returns_struct, uint8_t convention = 0,
    uint16_t struct_size = 0) noexcept
{
    return (static_cast<uint32_t>(argc & 0x1F))
         | (static_cast<uint32_t>(fp_mask)          << 5)
         | (static_cast<uint32_t>(is_variadic)      << 13)
         | (static_cast<uint32_t>(returns_fp)        << 14)
         | (static_cast<uint32_t>(returns_struct)    << 15)
         | (static_cast<uint32_t>(convention & 0x03) << 16)
         | (static_cast<uint32_t>(struct_size)       << 18);
}

// ─────────────────────────────────────────────────────────────────────────────
// Section offset helpers
// ─────────────────────────────────────────────────────────────────────────────

/// All section offsets are computable from header fields alone.
/// Each item is 8 bytes so alignment is automatic.

constexpr uint32_t blob_section_insn(const BlobHeader& h) noexcept {
    (void)h;
    return static_cast<uint32_t>(sizeof(BlobHeader));  // 32
}

constexpr uint32_t blob_section_pool(const BlobHeader& h) noexcept {
    return blob_section_insn(h) + 8u * h.insn_count;
}

constexpr uint32_t blob_section_meta(const BlobHeader& h) noexcept {
    return blob_section_pool(h) + 8u * h.pool_entry_count;
}

constexpr uint32_t blob_section_mac(const BlobHeader& h) noexcept {
    return blob_section_meta(h) + 64u * h.bb_count;
}

constexpr uint32_t blob_section_trans(const BlobHeader& h) noexcept {
    return blob_section_mac(h) + 8u * h.bb_count;
}

constexpr uint32_t blob_section_alias(const BlobHeader& h) noexcept {
    return blob_section_trans(h) + 32u * h.native_call_count;
}

constexpr uint32_t blob_section_config(const BlobHeader& h) noexcept {
    return blob_section_alias(h) + 256u;
}

/// Expected total blob size.  Must equal header.total_size for a valid blob.
constexpr uint32_t blob_expected_size(const BlobHeader& h) noexcept {
    return blob_section_config(h) + 8u;
}

// ─────────────────────────────────────────────────────────────────────────────
// Blob flags
// ─────────────────────────────────────────────────────────────────────────────

constexpr uint16_t BLOB_FLAG_DEBUG = 0x0001;

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_BLOB_HPP__
