#pragma once
#ifndef __RUNTIME_DECODER_HPP__
#define __RUNTIME_DECODER_HPP__

#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_opcode.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Decoded instruction with resolved opcode and plaintext operands.
struct DecodedInsn {
    Common::VM::VmOpcode opcode;  ///< resolved semantic opcode
    uint8_t  operand_a_type;      ///< VM_OPERAND_* constant
    uint8_t  operand_b_type;      ///< VM_OPERAND_* constant
    uint8_t  condition;           ///< condition/width/variant nibble
    uint8_t  reg_a;               ///< first register index (0-15)
    uint8_t  reg_b;               ///< second register index (0-15)
    uint16_t plaintext_opcode;    ///< raw opcode from VmInsn (before alias/perm)
    uint32_t aux;                 ///< immediate / pool index / offset
};

/// Fetch, decrypt, and decode the instruction at the current vm_ip.
///
/// Performs:
///   1. Read encrypted 8-byte VmInsn from bytecodes[vm_ip]
///   2. Decrypt via SipHash keystream (enc_state chain)
///   3. Resolve opcode through alias_lut + opcode_perm_inv
///   4. Unpack fields into DecodedInsn
///
/// @param ctx  the current VMContext (read-only for fetch; enc_state updated)
/// @return     decoded instruction on success, DiagnosticCode on failure
[[nodiscard]] tl::expected<DecodedInsn, Common::DiagnosticCode>
fetch_decrypt_decode(Common::VM::VMContext& ctx) noexcept;

/// Advance the encryption state after processing one instruction.
///
/// Updates enc_state = SipHash(enc_state_as_16byte_key, opcode(2) || aux(4))
/// and increments insn_index_in_bb.
///
/// @param ctx              the VMContext whose enc_state and insn_index_in_bb are updated
/// @param plaintext_opcode raw 16-bit opcode from the decrypted VmInsn
/// @param plaintext_aux    raw 32-bit aux from the decrypted VmInsn
void advance_enc_state(Common::VM::VMContext& ctx,
                       uint16_t plaintext_opcode,
                       uint32_t plaintext_aux) noexcept;

/// Verify the BB MAC at a basic-block boundary.
///
/// Re-decrypts the entire BB's instructions from scratch and computes
/// BLAKE3_keyed(integrity_key, all_plaintext_insns)[0:8], then compares
/// against the stored MAC (constant-time).
///
/// @param ctx  the VMContext (must have bb_metadata, bytecodes, bb_macs populated)
/// @return     success on match, BBMacVerificationFailed on mismatch
[[nodiscard]] tl::expected<void, Common::DiagnosticCode>
verify_bb_mac(const Common::VM::VMContext& ctx) noexcept;

/// Enter a new basic block (BB transition for JMP/JCC/CALL/RET).
///
/// Performs:
///   1. Find BBMetadata for target_bb_id
///   2. Derive bb_enc_seed and reset enc_state
///   3. Reset insn_index_in_bb = 0, set vm_ip = entry_ip
///   4. If epoch changed: re-derive encoding tables and apply RE_TABLE
///   5. Update current_bb_id and current_epoch
///
/// @param ctx           the VMContext to update
/// @param target_bb_id  the BB to enter
/// @return              success or InvalidBBTransition if bb_id not found
[[nodiscard]] tl::expected<void, Common::DiagnosticCode>
enter_basic_block(Common::VM::VMContext& ctx, uint32_t target_bb_id) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_DECODER_HPP__
