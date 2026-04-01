#pragma once
#ifndef __RUNTIME_DECODED_INSN_HPP__
#define __RUNTIME_DECODED_INSN_HPP__

/// @file decoded_insn.hpp
/// @brief Decoded instruction with resolved operands and plaintext values.
///
/// After the fetch-decrypt-decode pipeline (steps 1-3) and operand resolution
/// (step 4), the dispatcher produces a DecodedInsn with:
///   - Semantic VmOpcode (resolved through alias_lut + opcode_perm_inv)
///   - Operand type tags (REG/POOL/MEM/NONE)
///   - Register indices
///   - Resolved operand values as RegVal (raw domain values)
///   - Plaintext operand values (FPE-decoded by pipeline, ready for handlers)
///
/// Doc 16 encoding model (replaces per-BB LUT model from doc 15):
///
///   - OPERAND_REG:  exec.regs[reg_x]  -> RegVal (FPE-encoded under insn_fpe_key)
///                   plain_x = FPE_Decode(insn_fpe_key, tweaks, reg, resolved_x)
///
///   - OPERAND_POOL: constant_pool[aux] -> RegVal (PLAINTEXT stored as RegVal bits)
///                   plain_x = resolved_x.bits  (pool is plaintext after decryption)
///
///   - OPERAND_MEM:  guest memory -> GlobalMemTables::decode -> PlainVal -> RegVal bits
///                   plain_x = resolved_x.bits  (mem LUT decode yields plaintext)
///
///   - OPERAND_NONE: RegVal(0), plain_x = 0
///
/// WHY two separate fields (resolved_x and plain_x):
///
///   resolved_x holds the "raw" value in its source domain:
///     REG = FPE-encoded bits  (needed for fingerprinting in Phase F)
///     POOL/MEM/NONE = plaintext stored as RegVal
///
///   plain_x holds the decoded plaintext for ALL operand types uniformly.
///   Handlers use plain_x for ALU operations, then FPE_Encode the result.
///
///   Keeping resolved_a/b allows the key ratchet's fingerprint (Phase F) to
///   hash the *encoded* register file, which is essential for forward secrecy.

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

    /// Resolved operand values — domain depends on operand type.
    ///
    /// For REG operands: FPE-encoded value from exec.regs[] (register domain).
    /// For POOL operands: plaintext from constant pool (stored as RegVal bits).
    /// For MEM operands: plaintext after GlobalMemTables decode (stored as RegVal bits).
    /// For NONE: RegVal(0).
    ///
    /// These hold the pre-decode values.  The pipeline uses them for:
    ///   1. FPE decode (REG operands only)
    ///   2. Register fingerprinting (Phase F of the key ratchet)
    RegVal resolved_a;
    RegVal resolved_b;

    /// Plaintext operand values — filled by the pipeline AFTER FPE decoding,
    /// BEFORE handler dispatch.
    ///
    /// For ALL operand types, plain_a/plain_b contain the decoded plaintext
    /// as a raw uint64_t.  Handlers operate on these values directly.
    ///
    /// WHY uint64_t not PlainVal:
    ///   Handlers need raw arithmetic (ADD, SUB, MUL, CMP).  PlainVal would
    ///   require .bits access everywhere.  Using uint64_t makes handler code
    ///   natural: result = insn.plain_a + insn.plain_b.
    uint64_t plain_a = 0;   ///< Plaintext value of operand A (FPE-decoded by pipeline)
    uint64_t plain_b = 0;   ///< Plaintext value of operand B (FPE-decoded by pipeline)
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_DECODED_INSN_HPP__
