#pragma once
#ifndef __RUNTIME_PIPELINE_HPP__
#define __RUNTIME_PIPELINE_HPP__

/// @file pipeline.hpp
/// @brief Non-template pipeline functions for the doc 16 forward-secrecy dispatch.
///
/// These functions implement the heavy-lifting parts of the pipeline
/// (fetch, decrypt, decode, resolve, FPE decode, advance, MAC verify,
/// BB transition, FPE key derivation) using the 4-way state split.
///
/// Doc 16 changes vs doc 15:
///
///   - resolve_operands: MEM operands now go through GlobalMemTables (LUT decode)
///     instead of per-BB DomainTables.  REG operands return FPE-encoded values.
///     POOL operands are plaintext (pool is decrypted at blob load time).
///
///   - enter_basic_block: derives the initial insn_fpe_key for the new BB
///     via derive_bb_fpe_key(), re-encodes all live registers from old FPE key
///     to new FPE key, and sanitises dead registers.  DomainTables derivation
///     is removed (doc 16 has no per-BB store/load tables).
///
///   - derive_bb_fpe_key: new function.  Computes the initial FPE key for a
///     basic block: key = BLAKE3_KEYED_128(epoch_seed, bb_chain_state).
///
/// The only template-dependent part of the pipeline is handler dispatch,
/// which stays in vm_engine.hpp.

#include "decoded_insn.hpp"
#include "vm_state.hpp"

#include <diagnostic.hpp>
#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime::pipeline {

using Common::DiagnosticCode;

/// Steps 1-3: Fetch encrypted instruction, decrypt via SipHash, decode opcode.
///
/// Reads 8 bytes from blob at vm_ip, XORs with keystream derived from
/// enc_state, resolves opcode through PRP inverse + alias LUT.
/// Does NOT update enc_state (that's step 9, advance_enc_state).
[[nodiscard]] tl::expected<DecodedInsn, DiagnosticCode>
fetch_decrypt_decode(const VmImmutable& imm,
                     const VmExecution& exec,
                     const VmEpoch& epoch) noexcept;

/// Step 4: Resolve both operands (always both -- D3 uniformity).
///
/// Converts operand type tags to values:
///   REG  -> exec.regs[reg_x]          (FPE-encoded RegVal, decode happens later)
///   POOL -> constant_pool[aux]        (plaintext, stored as RegVal bits)
///   MEM  -> read guest memory -> GlobalMemTables::decode -> plaintext as RegVal
///   NONE -> RegVal(0)
///
/// Also fills plain_a/plain_b:
///   REG  -> FPE_Decode(insn_fpe_key, tweaks, reg, resolved.bits)
///   POOL -> resolved.bits (already plaintext)
///   MEM  -> resolved.bits (already decoded through GlobalMemTables)
///   NONE -> 0
///
/// WHY FPE decode is inside resolve_operands (not a separate step):
///   The pipeline always resolves+decodes as a unit.  Splitting them would
///   require the caller to track which operands need FPE decode (only REG).
///   Doing it here keeps the step() function clean.
void resolve_operands(const VmImmutable& imm,
                      const VmExecution& exec,
                      const VmEpoch& epoch,
                      DecodedInsn& insn) noexcept;

/// Step 9: Advance enc_state chain.
///
/// enc_state = SipHash(enc_state_as_16byte_key, opcode(2) || aux(4))
/// Also increments insn_index_in_bb.
void advance_enc_state(VmExecution& exec,
                       uint16_t plaintext_opcode,
                       uint32_t plaintext_aux) noexcept;

/// BB transition: enter a new basic block.
///
/// 1. Find BBMetadata for target_bb_id
/// 2. Derive bb_enc_seed, reset enc_state and insn_index_in_bb
/// 3. Set vm_ip = entry_ip
/// 4. If epoch changed: apply opcode permutation via VmEpoch::enter_bb()
///    (doc 16 has no RegTables/DomainTables to re-derive on epoch change)
/// 5. Derive new insn_fpe_key = BLAKE3_KEYED_128(epoch_seed, bb_chain_state)
/// 6. Re-encode all live registers: FPE_Decode(old_key) -> FPE_Encode(new_key)
/// 7. Sanitise dead registers: FPE_Encode(new_key, tweak, 0)
/// 8. Evolve bb_chain_state = BLAKE3(stored_seed, old_chain_state || bb_id)
/// 9. Update current_bb_id, current_bb_index, current_epoch
[[nodiscard]] tl::expected<void, DiagnosticCode>
enter_basic_block(VmExecution& exec,
                  VmEpoch& epoch,
                  const VmImmutable& imm,
                  uint32_t target_bb_id) noexcept;

/// Derive the initial insn_fpe_key for a basic block.
///
/// key = BLAKE3_KEYED_128(epoch_seed, bb_chain_state)[0:16]
///
/// WHY BLAKE3_KEYED with epoch_seed (not stored_seed):
///   The epoch_seed changes per epoch, so the FPE key space is partitioned
///   by epoch.  Two BBs in different epochs with the same chain state still
///   get different FPE keys.  This prevents cross-epoch key correlation.
///
/// WHY bb_chain_state as input:
///   The chain state accumulates the history of all previously visited BBs.
///   This means the FPE key depends on the ENTIRE execution path, not just
///   the current BB identity.  Two visits to the same BB through different
///   paths get different FPE keys -- this is the core forward-secrecy property.
///
/// @param bb              BB metadata (contains epoch_seed)
/// @param bb_chain_state  32-byte accumulated chain state
/// @param out_key         16-byte output FPE key (Speck64/128 key size)
void derive_bb_fpe_key(const BBMetadata& bb,
                       const uint8_t bb_chain_state[32],
                       uint8_t out_key[16]) noexcept;

/// Step 11: Verify BB MAC at basic-block boundary.
///
/// Re-decrypts the entire BB's instructions from scratch and computes
/// BLAKE3_keyed(integrity_key, all_plaintext_insns)[0:8], then compares
/// against the stored MAC in constant time.
[[nodiscard]] tl::expected<void, DiagnosticCode>
verify_bb_mac(const VmImmutable& imm,
              const VmExecution& exec,
              const VmEpoch& epoch) noexcept;

/// Replay enc_state SipHash chain from BB entry to a specific instruction index.
/// Used by RET_VM to resume execution after CALL_VM.
void replay_enc_state(VmExecution& exec, const VmEpoch& epoch,
                      const VmImmutable& imm, uint32_t target_insn_idx) noexcept;

/// Get the current BB's instruction count (for boundary detection).
[[nodiscard]] uint32_t current_bb_insn_count(
    const VmImmutable& imm,
    const VmExecution& exec) noexcept;

}  // namespace VMPilot::Runtime::pipeline

#endif  // __RUNTIME_PIPELINE_HPP__
