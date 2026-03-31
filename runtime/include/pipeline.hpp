#pragma once
#ifndef __RUNTIME_PIPELINE_HPP__
#define __RUNTIME_PIPELINE_HPP__

/// @file pipeline.hpp
/// @brief Non-template pipeline functions for the 12-step uniform dispatch.
///
/// These functions implement the heavy-lifting parts of the pipeline
/// (fetch, decrypt, decode, resolve, advance, MAC verify, BB transition)
/// using the new 4-way state split.  They are NOT templates — compiled
/// once and called by VmEngine<Policy, Oram>::step().
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

/// Step 4: Resolve both operands (always both — D3 uniformity).
///
/// Converts operand type tags to RegVal values:
///   REG  → exec.regs[reg_x]
///   POOL → constant_pool[aux] (pre-encoded, cast to RegVal)
///   MEM  → read guest memory → load_convert → RegVal
///   NONE → RegVal(0)
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
/// 4. If epoch changed: apply RE_TABLE via VmEpoch::transition_regs(),
///    then VmEpoch::enter_bb() to derive new tables
/// 5. If same epoch: just VmEpoch::enter_bb()
/// 6. Update current_bb_id, current_bb_index, current_epoch
[[nodiscard]] tl::expected<void, DiagnosticCode>
enter_basic_block(VmExecution& exec,
                  VmEpoch& epoch,
                  const VmImmutable& imm,
                  uint32_t target_bb_id) noexcept;

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
