#pragma once
#ifndef __RUNTIME_DECODED_INSN_HPP__
#define __RUNTIME_DECODED_INSN_HPP__

/// @file decoded_insn.hpp
/// @brief Decoded instruction with phantom-typed resolved operands.
///
/// After the fetch-decrypt-decode pipeline (steps 1-3) and operand resolution
/// (step 4), the dispatcher produces a DecodedInsn with:
///   - Semantic VmOpcode (resolved through alias_lut + opcode_perm_inv)
///   - Operand type tags (REG/POOL/MEM/NONE)
///   - Register indices
///   - Resolved operand values as RegVal (phantom-typed Domain::Register)
///
/// All resolved operands are in Register Domain regardless of their source:
///   - OPERAND_REG:  exec.regs[reg_x]  → RegVal (already in register domain)
///   - OPERAND_POOL: constant_pool[aux] → RegVal (pre-encoded by compiler)
///   - OPERAND_MEM:  guest memory → load_convert() → RegVal
///   - OPERAND_NONE: RegVal(0)
///
/// This guarantees that handlers always work with RegVal inputs, and the
/// phantom type system prevents accidentally passing a MemVal or PlainVal.

#include <vm/encoded_value.hpp>
#include <vm/vm_opcode.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

using Common::VM::VmOpcode;
using Common::VM::RegVal;

/// Decoded and operand-resolved instruction, ready for handler dispatch.
struct DecodedInsn {
    /// Resolved semantic opcode (after alias LUT + PRP inverse).
    VmOpcode opcode;

    /// Operand type tags (VM_OPERAND_NONE/REG/POOL/MEM).
    uint8_t operand_a_type;
    uint8_t operand_b_type;

    /// Condition/width/variant nibble from flags[3:0].
    uint8_t condition;

    /// Register indices (from reg_pack field).
    uint8_t reg_a;     ///< destination / first source
    uint8_t reg_b;     ///< second source

    /// Raw opcode from decrypted VmInsn (for enc_state update).
    uint16_t plaintext_opcode;

    /// Auxiliary field (pool index / memory offset / BB target / padding).
    uint32_t aux;

    /// Resolved operand values — ALL in Register Domain.
    ///
    /// The dispatcher resolves operands before calling the handler:
    ///   REG  → exec.regs[reg_x]        (already RegVal)
    ///   POOL → constant_pool[aux]       (pre-encoded as RegVal by compiler)
    ///   MEM  → load_convert(mem_value)  (converted to RegVal)
    ///   NONE → RegVal(0)
    ///
    /// Handlers receive typed RegVal, not raw uint64_t.
    RegVal resolved_a;
    RegVal resolved_b;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_DECODED_INSN_HPP__
