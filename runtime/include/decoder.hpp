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
/// @param ctx  the VMContext whose enc_state and insn_index_in_bb are updated
void advance_enc_state(Common::VM::VMContext& ctx) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_DECODER_HPP__
