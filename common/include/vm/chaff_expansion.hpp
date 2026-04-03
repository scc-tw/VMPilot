#pragma once
#ifndef __COMMON_VM_CHAFF_EXPANSION_HPP__
#define __COMMON_VM_CHAFF_EXPANSION_HPP__

/// @file chaff_expansion.hpp
/// @brief Compiler-side chaff NOP insertion for Doc 19 fixed-width SO.
///
/// These functions transform a BB's instruction sequence into a fixed-width
/// dispatch-unit-aligned stream by inserting cryptographically random chaff
/// NOPs after each real instruction.
///
/// WHY chaff (Doc 19 §1):
///   Every dispatch unit must contain exactly N sub-instructions so that
///   the runtime executes the same number of crypto pipeline iterations
///   regardless of opcode mix.  The chaff NOPs are cryptographically
///   "welded" to the instruction stream via the Doc 17 plaintext-driven
///   ratchet: removing or reordering any chaff NOP changes the enc_state
///   chain, corrupting all subsequent instruction decryptions.
///
/// WHY randomized fields (Doc 19 Appendix D):
///   Chaff NOPs must have random reg_a, reg_b, flags, and aux fields.
///   Without randomization, the enc_state evolution for NOP slots would be
///   deterministic given the opcode — an attacker who knows the PRP
///   permutation could predict the ST chain and identify chaff positions.
///   CSPRNG-seeded fields make each chaff NOP's contribution to the
///   ratchet unpredictable.

#include <vm/builder_types.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Common::VM {

/// Expand a BB's instruction sequence to fixed-width dispatch units.
///
/// After each real instruction, inserts (N-1) chaff NOP instructions with
/// CSPRNG-derived random fields.  The result has exactly N× the original
/// instruction count.
///
/// @param insns      instruction sequence (modified in-place)
/// @param N          dispatch unit width (Policy::fusion_granularity)
/// @param rng_seed   64-bit seed for the SplitMix64 PRNG that generates
///                   chaff field randomness.  Typically derived from
///                   BLAKE3(stored_seed, "chaff" || bb_id).
void expand_to_fixed_width(std::vector<BuilderInstruction>& insns,
                           uint8_t N, uint64_t rng_seed) noexcept;

/// Pad a BB's instruction count to a multiple of (N × Q_D) dispatch units.
///
/// Appends full chaff-NOP dispatch units (N NOPs each) until the total
/// instruction count is a multiple of N × Q_D.  This quantizes BB length
/// to prevent leaking the real instruction count via blob size.
///
/// @param insns      instruction sequence (modified in-place, already expanded)
/// @param N          dispatch unit width
/// @param Q_D        dispatch quantum (typically 8)
/// @param rng_seed   64-bit seed for chaff generation
void pad_bb_dispatch_units(std::vector<BuilderInstruction>& insns,
                           uint8_t N, uint8_t Q_D,
                           uint64_t rng_seed) noexcept;

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_CHAFF_EXPANSION_HPP__
