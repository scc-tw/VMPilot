#pragma once
/// @file opcode_spec.hpp
/// @brief Declarative opcode benchmark specifications — Doc 19 DU model.
///
/// Each opcode is benchmarked as a Doc 19 dispatch unit:
///   [real_opcode, NOP, NOP, ..., NOP]  (N = Policy::fusion_granularity)
///
/// Setup requirements (register init, ORAM stack fill, guest memory)
/// are handled by setup BBs stepped through untimed before measurement.

#include <vm/vm_opcode.hpp>

#include <cstddef>
#include <cstdint>

namespace VMPilot::Bench {

using Common::VM::VmOpcode;

/// What setup does this opcode need before measurement?
enum class Setup : uint8_t {
    None,         ///< No setup (NOP, FENCE, CHECK_*)
    Reg1,         ///< 1 LOAD_CONST (NEG, NOT, SEXT*, ZEXT*, TRUNC*, GET_FLAG)
    Reg2,         ///< 2 LOAD_CONST (ADD, SUB, MUL, CMP, AND, SHL, MOVE ...)
    Memory,       ///< LOAD_CONST + STORE to guest mem (LOAD, STORE, atomics)
    OramPush,     ///< LOAD_CONST (PUSH reads from reg)
    OramPop,      ///< LOAD_CONST + N PUSHes to fill stack (POP reads from ORAM)
    Pool,         ///< Constant pool entries (LOAD_CONST)
    CtxRead,      ///< No setup (LOAD_CTX reads vm_ip/sp/bb_id/epoch)
    CtxWrite,     ///< 1 LOAD_CONST (STORE_CTX writes to vm_sp)
    Branch,       ///< JMP/JCC — opcode placed at DU end, targets next BB
    NativeCall,   ///< NATIVE_CALL — needs transition entry
};

struct OpcodeBenchSpec {
    VmOpcode    opcode;
    Setup       setup;
    uint8_t     reg_a;
    uint8_t     reg_b;
    uint32_t    aux;
    const char* name;       ///< nullptr → to_string(opcode)
};

// clang-format off
inline constexpr OpcodeBenchSpec SPECS[] = {
    // ── Cat 0: Data Movement ────────────────────────────────────────
    {VmOpcode::MOVE,       Setup::Reg2,      0, 1, 0, nullptr},
    {VmOpcode::LOAD,       Setup::Memory,    0, 0, 0, nullptr},
    {VmOpcode::STORE,      Setup::Memory,    0, 0, 0, nullptr},
    {VmOpcode::PUSH,       Setup::OramPush,  0, 0, 0, nullptr},
    {VmOpcode::POP,        Setup::OramPop,   0, 0, 0, nullptr},
    {VmOpcode::LOAD_CONST, Setup::Pool,      0, 0, 0, nullptr},
    {VmOpcode::LOAD_CTX,   Setup::CtxRead,   0, 0, 0, nullptr},
    {VmOpcode::STORE_CTX,  Setup::CtxWrite,  0, 0, 1, nullptr},

    // ── Cat 1: Arithmetic ───────────────────────────────────────────
    {VmOpcode::ADD,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::SUB,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::MUL,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::IMUL,       Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::DIV,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::IDIV,       Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::NEG,        Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::MOD,        Setup::Reg2, 0, 1, 0, nullptr},

    // ── Cat 2: Logic ────────────────────────────────────────────────
    {VmOpcode::AND,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::OR,         Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::XOR,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::NOT,        Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::SHL,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::SHR,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::SAR,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::ROL,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::ROR,        Setup::Reg2, 0, 1, 0, nullptr},

    // ── Cat 3: Comparison ───────────────────────────────────────────
    {VmOpcode::CMP,        Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::TEST,       Setup::Reg2, 0, 1, 0, nullptr},
    {VmOpcode::SET_FLAG,   Setup::None, 0, 0, 1, nullptr},
    {VmOpcode::GET_FLAG,   Setup::Reg1, 0, 0, 0, nullptr},

    // ── Cat 4: Control Flow ─────────────────────────────────────────
    {VmOpcode::JMP,         Setup::Branch,     0, 0, 0, nullptr},
    {VmOpcode::JCC,         Setup::Branch,     0, 0, 0, nullptr},
    {VmOpcode::NATIVE_CALL, Setup::NativeCall, 0, 0, 0, nullptr},

    // ── Cat 5: Width/Extension ──────────────────────────────────────
    {VmOpcode::SEXT8,      Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::SEXT16,     Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::SEXT32,     Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::ZEXT8,      Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::ZEXT16,     Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::ZEXT32,     Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::TRUNC8,     Setup::Reg1, 0, 0, 0, nullptr},
    {VmOpcode::TRUNC16,    Setup::Reg1, 0, 0, 0, nullptr},

    // ── Cat 6: Atomic ───────────────────────────────────────────────
    {VmOpcode::LOCK_ADD,    Setup::Memory, 0, 1, 0, nullptr},
    {VmOpcode::XCHG,       Setup::Memory, 0, 1, 0, nullptr},
    {VmOpcode::CMPXCHG,    Setup::Memory, 0, 1, 0, nullptr},
    {VmOpcode::FENCE,      Setup::None,   0, 0, 0, nullptr},
    {VmOpcode::ATOMIC_LOAD, Setup::Memory, 0, 0, 0, nullptr},

    // ── Cat 7: VM Internal ──────────────────────────────────────────
    {VmOpcode::NOP,             Setup::None, 0, 0, 0, nullptr},
    {VmOpcode::CHECK_INTEGRITY, Setup::None, 0, 0, 0, nullptr},
    {VmOpcode::CHECK_DEBUG,     Setup::None, 0, 0, 0, nullptr},
    {VmOpcode::MUTATE_ISA,      Setup::None, 0, 0, 0, nullptr},
};
// clang-format on

inline constexpr size_t SPEC_COUNT = sizeof(SPECS) / sizeof(SPECS[0]);

}  // namespace VMPilot::Bench
