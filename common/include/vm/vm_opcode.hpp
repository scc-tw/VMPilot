#pragma once
#ifndef __COMMON_VM_OPCODE_HPP__
#define __COMMON_VM_OPCODE_HPP__

#include <cstdint>

namespace VMPilot::Common::VM {

/// 55 semantic opcodes in 8 categories (ISA Design v1, Section 4.1).
enum class VmOpcode : uint8_t {
    // Cat 0: Data Movement (8)
    MOVE       = 0,
    LOAD       = 1,
    STORE      = 2,
    PUSH       = 3,
    POP        = 4,
    LOAD_CONST = 5,
    LOAD_CTX   = 6,
    STORE_CTX  = 7,

    // Cat 1: Arithmetic (8)
    ADD  = 8,
    SUB  = 9,
    MUL  = 10,
    IMUL = 11,
    DIV  = 12,
    IDIV = 13,
    NEG  = 14,
    MOD  = 15,

    // Cat 2: Logic (9)
    AND = 16,
    OR  = 17,
    XOR = 18,
    NOT = 19,
    SHL = 20,
    SHR = 21,
    SAR = 22,
    ROL = 23,
    ROR = 24,

    // Cat 3: Comparison (4)
    CMP      = 25,
    TEST     = 26,
    SET_FLAG = 27,
    GET_FLAG = 28,

    // Cat 4: Control Flow (6)
    JMP         = 29,
    JCC         = 30,
    CALL_VM     = 31,
    RET_VM      = 32,
    NATIVE_CALL = 33,
    HALT        = 34,

    // Cat 5: Width/Extension (8)
    SEXT8   = 35,
    SEXT16  = 36,
    SEXT32  = 37,
    ZEXT8   = 38,
    ZEXT16  = 39,
    ZEXT32  = 40,
    TRUNC8  = 41,
    TRUNC16 = 42,

    // Cat 6: Atomic (5)
    LOCK_ADD    = 43,
    XCHG        = 44,
    CMPXCHG     = 45,
    FENCE       = 46,
    ATOMIC_LOAD = 47,

    // Cat 7: VM Internal (7)
    NOP             = 48,
    CHECK_INTEGRITY = 49,
    CHECK_DEBUG     = 50,
    MUTATE_ISA      = 51,
    REKEY           = 52,
    SAVE_EPOCH      = 53,
    RESYNC          = 54,
};

/// Total number of semantic opcodes.
constexpr uint8_t VM_OPCODE_COUNT = 55;

/// Return the category index (0-7) for a given opcode.
[[nodiscard]] constexpr uint8_t vm_opcode_category(VmOpcode op) noexcept {
    const auto v = static_cast<uint8_t>(op);
    if (v <=  7) return 0;  // Data Movement
    if (v <= 15) return 1;  // Arithmetic
    if (v <= 24) return 2;  // Logic
    if (v <= 28) return 3;  // Comparison
    if (v <= 34) return 4;  // Control Flow
    if (v <= 42) return 5;  // Width/Extension
    if (v <= 47) return 6;  // Atomic
    if (v <= 54) return 7;  // VM Internal
    return 0xFF;            // invalid
}

/// Human-readable name for a VmOpcode.
inline const char* to_string(VmOpcode op) noexcept {
    switch (op) {
        // Cat 0
        case VmOpcode::MOVE:       return "MOVE";
        case VmOpcode::LOAD:       return "LOAD";
        case VmOpcode::STORE:      return "STORE";
        case VmOpcode::PUSH:       return "PUSH";
        case VmOpcode::POP:        return "POP";
        case VmOpcode::LOAD_CONST: return "LOAD_CONST";
        case VmOpcode::LOAD_CTX:   return "LOAD_CTX";
        case VmOpcode::STORE_CTX:  return "STORE_CTX";
        // Cat 1
        case VmOpcode::ADD:  return "ADD";
        case VmOpcode::SUB:  return "SUB";
        case VmOpcode::MUL:  return "MUL";
        case VmOpcode::IMUL: return "IMUL";
        case VmOpcode::DIV:  return "DIV";
        case VmOpcode::IDIV: return "IDIV";
        case VmOpcode::NEG:  return "NEG";
        case VmOpcode::MOD:  return "MOD";
        // Cat 2
        case VmOpcode::AND: return "AND";
        case VmOpcode::OR:  return "OR";
        case VmOpcode::XOR: return "XOR";
        case VmOpcode::NOT: return "NOT";
        case VmOpcode::SHL: return "SHL";
        case VmOpcode::SHR: return "SHR";
        case VmOpcode::SAR: return "SAR";
        case VmOpcode::ROL: return "ROL";
        case VmOpcode::ROR: return "ROR";
        // Cat 3
        case VmOpcode::CMP:      return "CMP";
        case VmOpcode::TEST:     return "TEST";
        case VmOpcode::SET_FLAG: return "SET_FLAG";
        case VmOpcode::GET_FLAG: return "GET_FLAG";
        // Cat 4
        case VmOpcode::JMP:         return "JMP";
        case VmOpcode::JCC:         return "JCC";
        case VmOpcode::CALL_VM:     return "CALL_VM";
        case VmOpcode::RET_VM:      return "RET_VM";
        case VmOpcode::NATIVE_CALL: return "NATIVE_CALL";
        case VmOpcode::HALT:        return "HALT";
        // Cat 5
        case VmOpcode::SEXT8:   return "SEXT8";
        case VmOpcode::SEXT16:  return "SEXT16";
        case VmOpcode::SEXT32:  return "SEXT32";
        case VmOpcode::ZEXT8:   return "ZEXT8";
        case VmOpcode::ZEXT16:  return "ZEXT16";
        case VmOpcode::ZEXT32:  return "ZEXT32";
        case VmOpcode::TRUNC8:  return "TRUNC8";
        case VmOpcode::TRUNC16: return "TRUNC16";
        // Cat 6
        case VmOpcode::LOCK_ADD:    return "LOCK_ADD";
        case VmOpcode::XCHG:        return "XCHG";
        case VmOpcode::CMPXCHG:     return "CMPXCHG";
        case VmOpcode::FENCE:       return "FENCE";
        case VmOpcode::ATOMIC_LOAD: return "ATOMIC_LOAD";
        // Cat 7
        case VmOpcode::NOP:             return "NOP";
        case VmOpcode::CHECK_INTEGRITY: return "CHECK_INTEGRITY";
        case VmOpcode::CHECK_DEBUG:     return "CHECK_DEBUG";
        case VmOpcode::MUTATE_ISA:      return "MUTATE_ISA";
        case VmOpcode::REKEY:           return "REKEY";
        case VmOpcode::SAVE_EPOCH:      return "SAVE_EPOCH";
        case VmOpcode::RESYNC:          return "RESYNC";
    }
    return "UNKNOWN";
}

// ─────────────────────────────────────────────────────────────────────────────
// opcode_writes_reg — doc 16 Phase E decision table
// ─────────────────────────────────────────────────────────────────────────────

/// Determine whether an opcode's handler writes a plaintext result to regs[reg_a].
///
/// Doc 16 Phase E needs to know which handlers left plaintext in regs[reg_a]
/// (needing FPE-encode) vs which left the existing FPE-encoded value untouched.
/// Encoding an already-encoded value would produce garbage, so we must
/// distinguish the two cases.
///
/// The list of non-writing opcodes is exhaustive: any new opcode that writes
/// to regs must NOT appear in the false cases.
constexpr bool opcode_writes_reg(VmOpcode op) noexcept {
    switch (op) {
        // Control flow: these modify IP / flags / branch state, not regs.
        case VmOpcode::JMP:
        case VmOpcode::JCC:
        case VmOpcode::HALT:
        case VmOpcode::NOP:

        // Comparison: writes to vm_flags, not regs.
        case VmOpcode::CMP:
        case VmOpcode::TEST:
        case VmOpcode::SET_FLAG:

        // Memory store: writes to guest memory or ORAM, not regs.
        case VmOpcode::STORE:
        case VmOpcode::PUSH:
        case VmOpcode::STORE_CTX:

        // Synchronisation: issues a fence, no register write.
        case VmOpcode::FENCE:

        // VM internal: integrity/debug checks produce no register output.
        case VmOpcode::CHECK_INTEGRITY:
        case VmOpcode::CHECK_DEBUG:
        case VmOpcode::MUTATE_ISA:

        // REKEY: triggers key ratchet externally, handler writes no register.
        case VmOpcode::REKEY:

        // SAVE_EPOCH / RESYNC: snapshot/restore mechanics, not a reg write.
        // NOTE: RESYNC does overwrite regs[] with a saved snapshot, but those
        // values are already FPE-encoded (under the snapshot-time key).
        // Treating RESYNC as "not writing" avoids double-encoding.
        // The ratchet's Phase H will re-encode under the new key, which is
        // wrong for snapshot values -- RESYNC needs redesign for doc 16.
        case VmOpcode::SAVE_EPOCH:
        case VmOpcode::RESYNC:

        // CALL_VM / RET_VM: shadow stack manipulation, not plaintext register writes.
        // CALL_VM saves already-FPE-encoded regs[] to shadow_stack (no reg write).
        // RET_VM restores FPE-encoded snapshot into regs[] and restores insn_fpe_key
        // to the saved key, so regs[] are valid ciphertext under the restored key.
        // Phase E must NOT re-encode them (double-encoding produces garbage).
        // Phase H's re-encoding from restored key → ratcheted key handles the
        // domain transition correctly.
        case VmOpcode::CALL_VM:
        case VmOpcode::RET_VM:
            return false;

        // Everything else writes a plaintext result to regs[reg_a]:
        // MOVE, LOAD, POP, LOAD_CONST, LOAD_CTX, GET_FLAG,
        // ADD..MOD, AND..ROR, SEXT8..TRUNC16, NATIVE_CALL,
        // LOCK_ADD, XCHG, CMPXCHG, ATOMIC_LOAD
        default:
            return true;
    }
}

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_OPCODE_HPP__
