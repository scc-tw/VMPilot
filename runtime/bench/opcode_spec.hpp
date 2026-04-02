#pragma once
/// @file opcode_spec.hpp
/// @brief Declarative opcode benchmark specifications.
///
/// To add a benchmark for a new opcode, append one entry to SPECS[].
/// The ProgramFactory reads the Shape and auto-generates the test program.

#include <vm/vm_opcode.hpp>

#include <cstdint>

namespace VMPilot::Bench {

using Common::VM::VmOpcode;

/// What kind of operand setup does the opcode need?
enum class Shape : uint8_t {
    RegReg,       ///< dst = dst OP src   (ADD, SUB, CMP, AND, SHL ...)
    RegOnly,      ///< dst = OP(dst)      (NEG, NOT, SEXT8, ZEXT16 ...)
    NoOperand,    ///< no operands        (NOP, FENCE, CHECK_INTEGRITY ...)
    // ── Shapes below are Phase 2+ ──
    PoolReg,      ///< dst = pool[aux]    (LOAD_CONST)
    Memory,       ///< dst = mem[aux]     (LOAD, STORE, LOCK_ADD ...)
    Oram,         ///< PUSH / POP
    CtxAccess,    ///< LOAD_CTX / STORE_CTX
    Custom,       ///< hand-written factory (JMP, JCC, NATIVE_CALL ...)
};

struct OpcodeBenchSpec {
    VmOpcode    opcode;
    Shape       shape;
    uint8_t     reg_a;      ///< destination / first register
    uint8_t     reg_b;      ///< source / second register (RegReg only)
    uint32_t    aux;        ///< aux field value
    const char* name;       ///< display name (nullptr → to_string(opcode))
    uint32_t    max_n;      ///< 0 → use default; e.g. PUSH/POP → 200
};

/// Master table — all 55 opcodes.  Shapes that cannot be auto-generated
/// from the spec table alone (Custom) are built via hand-written factories
/// in program_factory.cpp.
// clang-format off
inline constexpr OpcodeBenchSpec SPECS[] = {
    // ── Cat 0: Data Movement ────────────────────────────────────────
    {VmOpcode::MOVE,       Shape::RegReg,    0, 1, 0, nullptr,   0},
    {VmOpcode::LOAD,       Shape::Memory,    0, 0, 0, nullptr,   0},
    {VmOpcode::STORE,      Shape::Memory,    0, 0, 0, nullptr,   0},
    {VmOpcode::PUSH,       Shape::Oram,      0, 0, 0, nullptr, 200},
    {VmOpcode::POP,        Shape::Oram,      0, 0, 0, nullptr, 200},
    {VmOpcode::LOAD_CONST, Shape::PoolReg,   0, 0, 0, nullptr,   0},
    {VmOpcode::LOAD_CTX,   Shape::CtxAccess, 0, 0, 0, nullptr,   0},  // aux=0 → vm_ip
    {VmOpcode::STORE_CTX,  Shape::CtxAccess, 0, 0, 1, nullptr,   0},  // aux=1 → vm_sp

    // ── Cat 1: Arithmetic ───────────────────────────────────────────
    {VmOpcode::ADD,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::SUB,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::MUL,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::IMUL,       Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::DIV,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::IDIV,       Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::NEG,        Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::MOD,        Shape::RegReg,    0, 1, 0, nullptr, 0},

    // ── Cat 2: Logic ────────────────────────────────────────────────
    {VmOpcode::AND,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::OR,         Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::XOR,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::NOT,        Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::SHL,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::SHR,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::SAR,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::ROL,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::ROR,        Shape::RegReg,    0, 1, 0, nullptr, 0},

    // ── Cat 3: Comparison ───────────────────────────────────────────
    {VmOpcode::CMP,        Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::TEST,       Shape::RegReg,    0, 1, 0, nullptr, 0},
    {VmOpcode::SET_FLAG,   Shape::NoOperand, 0, 0, 1, nullptr, 0},
    {VmOpcode::GET_FLAG,   Shape::RegOnly,   0, 0, 0, nullptr, 0},

    // ── Cat 4: Control Flow ─────────────────────────────────────────
    {VmOpcode::JMP,         Shape::Custom, 0, 0, 0, nullptr,  50},
    {VmOpcode::JCC,         Shape::Custom, 0, 0, 0, nullptr,  50},
    {VmOpcode::NATIVE_CALL, Shape::Custom, 0, 0, 0, nullptr, 200},
    // CALL_VM, RET_VM, HALT — skipped (nesting/termination constraints)

    // ── Cat 5: Width/Extension ──────────────────────────────────────
    {VmOpcode::SEXT8,      Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::SEXT16,     Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::SEXT32,     Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::ZEXT8,      Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::ZEXT16,     Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::ZEXT32,     Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::TRUNC8,     Shape::RegOnly,   0, 0, 0, nullptr, 0},
    {VmOpcode::TRUNC16,    Shape::RegOnly,   0, 0, 0, nullptr, 0},

    // ── Cat 6: Atomic ───────────────────────────────────────────────
    {VmOpcode::LOCK_ADD,    Shape::Memory, 0, 1, 0, nullptr, 0},
    {VmOpcode::XCHG,        Shape::Memory, 0, 1, 0, nullptr, 0},
    {VmOpcode::CMPXCHG,     Shape::Memory, 0, 1, 0, nullptr, 0},
    {VmOpcode::FENCE,       Shape::NoOperand, 0, 0, 0, nullptr, 0},
    {VmOpcode::ATOMIC_LOAD, Shape::Memory, 0, 0, 0, nullptr, 0},

    // ── Cat 7: VM Internal ──────────────────────────────────────────
    {VmOpcode::NOP,             Shape::NoOperand, 0, 0, 0, nullptr, 0},
    {VmOpcode::CHECK_INTEGRITY, Shape::NoOperand, 0, 0, 0, nullptr, 0},
    {VmOpcode::CHECK_DEBUG,     Shape::NoOperand, 0, 0, 0, nullptr, 0},
    {VmOpcode::MUTATE_ISA,      Shape::NoOperand, 0, 0, 0, nullptr, 0},
    // REKEY, SAVE_EPOCH, RESYNC — skipped (require enc_state tracking / nesting)
};
// clang-format on

inline constexpr size_t SPEC_COUNT = sizeof(SPECS) / sizeof(SPECS[0]);

}  // namespace VMPilot::Bench
