#pragma once
#ifndef __COMMON_VM_ENCODING_HPP__
#define __COMMON_VM_ENCODING_HPP__

/// @file vm_encoding.hpp
/// @brief Encoding table derivation — shared deterministic logic for compiler and runtime.
///
/// Every function in this file MUST produce bit-identical output on all
/// platforms and compilers.  The compiler uses these functions to pre-encode
/// constants and compute BB MACs; the runtime uses them to reconstruct
/// encoding tables from epoch_seeds at BB entry.  Any divergence between
/// the two sides is a correctness bug that silently corrupts every value.
///
/// ──────────────────────────────────────────────────────────────────────
/// Why random bijective LUTs and not affine encoding:
///
///   Affine encoding  E(x) = a·x + b (mod 2^n)  has MinCircuit = O(n).
///   Three I/O samples suffice to solve the linear system and recover
///   (a, b).  SyntIA (USENIX Security 2021) achieves >90% handler recovery
///   against Tigress with affine encoding.  QSynth (DIMVA 2021) achieves
///   similar results via black-box I/O sampling.
///
///   Random bijective LUTs have MinCircuit = Ω(2^n / n) by Shannon's
///   counting argument: there are (2^n)! bijections on n-bit strings,
///   but only (c·n)^{O(s)} circuits of size s, so for s < 2^n / n
///   the circuit count is vanishingly small relative to the bijection
///   count.  For n = 8 this gives ≈ Ω(32) gates per byte lane — no
///   compact algebraic representation exists.
///
///   This is the Minimum Circuit Size Problem (MCSP): given a truth table,
///   decide if a circuit of size ≤ s exists.  MCSP is believed to be
///   hard (NP-intermediate), and random truth tables are the hardest
///   instances (Kabanets & Cai, Computational Complexity 2000).
///
/// ──────────────────────────────────────────────────────────────────────
/// Why PRF-seeded derivation preserves MCSP hardness:
///
///   generate_bijection(BLAKE3(epoch_seed, r, k)) produces a bijection
///   whose truth table is computationally indistinguishable from one
///   sampled uniformly at random, under the assumption that BLAKE3 in
///   keyed mode is a PRF.
///
///   Formally: for any PPT distinguisher D,
///       |Pr[D(T_PRF) = 1] − Pr[D(T_random) = 1]| ≤ negl(λ)
///   where T_PRF is a table derived via BLAKE3 and T_random is uniform.
///
///   Since MCSP hardness is a property of "most" truth tables (Shannon
///   counting), and PRF output is indistinguishable from uniform, the
///   derived bijection inherits MCSP hardness with overwhelming
///   probability.
/// ──────────────────────────────────────────────────────────────────────

#include <cstdint>
#include <cstddef>

namespace VMPilot::Common::VM::Encoding {

// ─────────────────────────────────────────────────────────────────────────────
// Bijection primitives
// ─────────────────────────────────────────────────────────────────────────────

/// Generate a random bijection (permutation of {0..255}) from a 32-byte seed.
///
/// Algorithm: Fisher-Yates shuffle consuming pseudorandom bytes from
/// BLAKE3-XOF(seed || "bijection").  Fisher-Yates is the unique O(n)
/// algorithm that samples uniformly from S_n when given uniform random
/// swaps (Knuth, TAOCP Vol 2 §3.4.2).  BLAKE3-XOF extends the PRF
/// guarantee to arbitrary output length.
///
/// Modulo bias: for divisor d ≤ 256 and 16-bit random word, the bias
/// is at most 256 / 2^16 ≈ 0.4%, which does not affect MCSP hardness
/// (the counting argument tolerates constant-factor deviations from
/// uniform).
///
/// @param seed   32-byte seed (typically BLAKE3(epoch_seed, r, k))
/// @param table  output: 256-byte permutation table
void generate_bijection(const uint8_t seed[32],
                        uint8_t table[256]) noexcept;

/// Compute the inverse of a bijection.
///
/// For a bijection T, its inverse T^{-1} satisfies T^{-1}(T(x)) = x.
/// The inverse is also a random bijection under the same distribution,
/// so MCSP hardness applies equally.
///
/// @param table    input bijection (256 entries)
/// @param inverse  output inverse bijection (256 entries)
void invert_bijection(const uint8_t table[256],
                      uint8_t inverse[256]) noexcept;

/// Compose two bijections:  result[x] = outer[inner[x]].
///
/// Bijection composition is closed: the composition of two bijections
/// is itself a bijection.  If either operand is a random bijection
/// independent of the other, the composition is also uniformly random
/// over S_256.  This guarantees that RE_TABLE, STORE_TABLE, and
/// LOAD_TABLE inherit MCSP hardness from their constituent bijections
/// (spec §11.5).
///
/// @param outer   outer bijection  (256 entries)
/// @param inner   inner bijection  (256 entries)
/// @param result  output composed bijection (256 entries)
void compose_bijections(const uint8_t outer[256],
                        const uint8_t inner[256],
                        uint8_t result[256]) noexcept;

// ─────────────────────────────────────────────────────────────────────────────
// Per-BB register encoding derivation
// ─────────────────────────────────────────────────────────────────────────────

/// Derive per-BB register encoding tables from an epoch_seed.
///
/// For each live register r and byte lane k:
///     table_seed = BLAKE3(epoch_seed, to_bytes(r) || to_bytes(k))
///     encode[r][k] = generate_bijection(table_seed)
///     decode[r][k] = invert_bijection(encode[r][k])
///
/// Tables for dead registers (bits not set in live_regs_bitmap) are
/// left untouched.  The liveness optimisation is spec §3.7: dead
/// registers carry no semantic value, so encoding them would waste
/// cycles without improving security.
///
/// @param epoch_seed        32-byte random seed from BB metadata
/// @param live_regs_bitmap  bit i set ⇒ register i is live
/// @param encode            output encode tables [16][8][256]
/// @param decode            output decode tables [16][8][256]
void derive_register_tables(const uint8_t epoch_seed[32],
                            uint16_t live_regs_bitmap,
                            uint8_t encode[16][8][256],
                            uint8_t decode[16][8][256]) noexcept;

/// Derive global memory encoding tables from stored_seed.
///
/// Memory encoding uses a single global domain because memory is
/// cross-BB persistent: a value STORE'd in BB_1 must be LOADable
/// in BB_4.  A per-BB memory domain would require re-encrypting
/// all of guest memory at every BB transition, which is infeasible.
///
/// The global domain is derived as:
///     mem_seed = BLAKE3(stored_seed, "mem")
///     For each lane k:
///         table_seed = BLAKE3(mem_seed, to_bytes(k))
///         mem_encode[k] = generate_bijection(table_seed)
///         mem_decode[k] = invert_bijection(mem_encode[k])
///
/// @param stored_seed  32-byte root secret
/// @param mem_encode   output: global memory encode tables [8][256]
/// @param mem_decode   output: global memory decode tables [8][256]
void derive_memory_tables(const uint8_t stored_seed[32],
                          uint8_t mem_encode[8][256],
                          uint8_t mem_decode[8][256]) noexcept;

// ─────────────────────────────────────────────────────────────────────────────
// Domain conversion tables
// ─────────────────────────────────────────────────────────────────────────────

/// Derive STORE and LOAD domain-conversion tables for live registers.
///
///     STORE_TABLE[r][k] = mem_encode[k] ∘ reg_decode[r][k]
///     LOAD_TABLE[r][k]  = reg_encode[r][k] ∘ mem_decode[k]
///
/// Why composition instead of decode-then-encode at runtime:
///   A two-step "decode to plaintext, then encode to target domain"
///   exposes 8 bits of plaintext per byte lane per operation in a CPU
///   register.  The composed table maps directly between domains with
///   no plaintext intermediate.  Since composition of two independent
///   random bijections is itself a random bijection (uniform over S_256),
///   STORE/LOAD tables are MCSP-hard (spec §11.5).
///
/// @param reg_encode        per-register encode tables [16][8][256]
/// @param reg_decode        per-register decode tables [16][8][256]
/// @param mem_encode        global memory encode tables [8][256]
/// @param mem_decode        global memory decode tables [8][256]
/// @param live_regs_bitmap  which registers to process
/// @param store_tables      output: STORE tables [16][8][256]
/// @param load_tables       output: LOAD tables [16][8][256]
void derive_store_load_tables(
    const uint8_t reg_encode[16][8][256],
    const uint8_t reg_decode[16][8][256],
    const uint8_t mem_encode[8][256],
    const uint8_t mem_decode[8][256],
    uint16_t live_regs_bitmap,
    uint8_t store_tables[16][8][256],
    uint8_t load_tables[16][8][256]) noexcept;

/// Derive RE_TABLE for re-encoding live registers across an epoch change.
///
///     RE_TABLE[r][k] = encode_new[r][k] ∘ decode_old[r][k]
///
/// This is the "encoding phi node" at CFG joins (spec §3.6): all
/// incoming edges apply their respective RE_TABLE so that registers
/// converge to the join-BB's encoding domain.
///
/// Algebraically, RE_TABLE is a bijection (composition of bijections)
/// and preserves the encoded value's semantic identity:
///     RE(E_old(x)) = E_new(x)
/// No plaintext x ever materialises.
///
/// @param decode_old        old epoch's decode tables [16][8][256]
/// @param encode_new        new epoch's encode tables [16][8][256]
/// @param live_regs_bitmap  which registers to re-encode
/// @param re_tables         output: RE tables [16][8][256]
void derive_re_tables(
    const uint8_t decode_old[16][8][256],
    const uint8_t encode_new[16][8][256],
    uint16_t live_regs_bitmap,
    uint8_t re_tables[16][8][256]) noexcept;

// ─────────────────────────────────────────────────────────────────────────────
// Class A composition tables (built lazily per unique register triple)
// ─────────────────────────────────────────────────────────────────────────────

/// Build a Class A homomorphic composition table for a specific
/// (bitwise_op, dst, src_a, src_b) register triple.
///
///     comp[k][a][b] = encode_dst[k][ decode_src_a[k][a]  OP  decode_src_b[k][b] ]
///
/// This table maps (encoded_a, encoded_b) → encoded_result directly,
/// with no plaintext intermediate during instruction execution.
/// The build process iterates over all 256 × 256 input pairs per lane
/// (a generic precomputation, not dependent on actual data values),
/// so the Class A "zero plaintext" guarantee (spec §3.4) holds.
///
/// Why the composition table is MCSP-hard even though we know
/// encode_dst and decode_src:
///   The composition table is a random function from {0..255}² to {0..255}
///   (not a bijection), but its circuit complexity is lower-bounded by
///   the circuit complexity of decode_src (which is a random bijection,
///   MCSP-hard).  An attacker who recovers the composition table still
///   cannot factor it into (decode, op, encode) without solving MCSP
///   for the constituent bijections.
///
/// @param op_fn        pointwise bitwise operation (e.g. XOR, AND, OR)
/// @param encode_dst   encode table for destination register [8][256]
/// @param decode_src_a decode table for source register a    [8][256]
/// @param decode_src_b decode table for source register b    [8][256]
/// @param comp         output: composition table [8][256][256]
void build_composition_table(
    uint8_t (*op_fn)(uint8_t, uint8_t),
    const uint8_t encode_dst[8][256],
    const uint8_t decode_src_a[8][256],
    const uint8_t decode_src_b[8][256],
    uint8_t comp[8][256][256]) noexcept;

/// Build a unary Class A composition table (e.g. NOT).
///
///     comp[k][a] = encode_dst[k][ OP(decode_src[k][a]) ]
///
/// @param op_fn        pointwise unary operation (e.g. bitwise NOT)
/// @param encode_dst   encode table for destination register [8][256]
/// @param decode_src   decode table for source register      [8][256]
/// @param comp         output: composition table [8][256]
void build_unary_composition_table(
    uint8_t (*op_fn)(uint8_t),
    const uint8_t encode_dst[8][256],
    const uint8_t decode_src[8][256],
    uint8_t comp[8][256]) noexcept;

// ─────────────────────────────────────────────────────────────────────────────
// Opcode permutation (D4)
// ─────────────────────────────────────────────────────────────────────────────

/// Derive the per-epoch opcode permutation from epoch_seed.
///
///     perm_seed = BLAKE3(epoch_seed, "perm")
///     perm = Fisher-Yates(identity_256, perm_seed)
///
/// This is Layer 2 of the two-layer PRP (spec §4.2, §11.4).
/// For any semantic opcodes s₁, s₂ and observed ciphertext value v:
///     Pr[π(s₁) = v] = Pr[π(s₂) = v] = 1/256
/// which follows from PRP indistinguishability (Luby-Rackoff theorem)
/// when the permutation is derived from a PRF (BLAKE3).
///
/// This defeats the chosen-instruction attack (NDSS 2022, Biernat et al.)
/// because the mapping between semantic opcodes and observed byte values
/// is uniformly random and changes at every epoch boundary.
///
/// @param epoch_seed  32-byte BB epoch seed
/// @param perm        output: permutation table [256]
/// @param perm_inv    output: inverse permutation table [256]
void derive_opcode_permutation(const uint8_t epoch_seed[32],
                               uint8_t perm[256],
                               uint8_t perm_inv[256]) noexcept;

}  // namespace VMPilot::Common::VM::Encoding

#endif  // __COMMON_VM_ENCODING_HPP__
