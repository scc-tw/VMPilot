/// @file program_factory.cpp
/// @brief Generates encrypted VM blobs for each opcode benchmark shape.

#include "program_factory.hpp"
#include "test_blob_builder.hpp"

#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>

#include <cstring>

namespace VMPilot::Bench {

using namespace Common::VM;
using Test::TestBB;
using Test::TestInstruction;

// ─── Flag helpers ───────────────────────────────────────────────────────

static uint8_t f_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static uint8_t f_r() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t f_pool() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t f_none() { return 0; }

// ─── Seed helper ────────────────────────────────────────────────────────

static void fill_seed(uint8_t seed[32], uint8_t base = 0x42) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

static void fill_epoch_seed(uint8_t out[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        out[i] = static_cast<uint8_t>(base + i);
}

// ─── Shape builders ─────────────────────────────────────────────────────

/// RegReg: LOAD_CONST r_a, 42; LOAD_CONST r_b, 3; <opcode r_a, r_b> x N; HALT
static BenchProgram build_reg_reg(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode              = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    // Setup: two LOAD_CONST via pool
    // Body : N copies of the target instruction
    // End  : HALT
    uint32_t total = 2 + N + 1;  // 2 setup + N body + 1 HALT
    prog.total_insn_count = total;

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xB0);

    bb.instructions.reserve(total);
    // Setup: LOAD_CONST r_a = pool[0] (= 42), r_b = pool[1] (= 3)
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), spec.reg_a, 0, 0});
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), spec.reg_b, 0, 1});

    // Body: opcode x N
    TestInstruction body_insn{spec.opcode, f_rr(), spec.reg_a, spec.reg_b, spec.aux};
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back(body_insn);

    // HALT
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    // Pool: [0]=42, [1]=3  (3 is safe for DIV/MOD/shift)
    std::vector<uint64_t> pool = {42, 3};

    prog.blob = Test::build_test_blob(prog.seed, {bb}, pool, false, {});
    return prog;
}

/// RegOnly: LOAD_CONST r_a, 42; <opcode r_a> x N; HALT
static BenchProgram build_reg_only(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode              = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    uint32_t total = 1 + N + 1;
    prog.total_insn_count = total;

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xB1);

    bb.instructions.reserve(total);
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), spec.reg_a, 0, 0});

    TestInstruction body_insn{spec.opcode, f_r(), spec.reg_a, 0, spec.aux};
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back(body_insn);

    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    std::vector<uint64_t> pool = {42};
    prog.blob = Test::build_test_blob(prog.seed, {bb}, pool, false, {});
    return prog;
}

/// NoOperand: <opcode> x N; HALT
static BenchProgram build_no_operand(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode              = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    uint32_t total = N + 1;
    prog.total_insn_count = total;

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xB2);

    bb.instructions.reserve(total);

    TestInstruction body_insn{spec.opcode, f_none(), 0, 0, spec.aux};
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back(body_insn);

    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.blob = Test::build_test_blob(prog.seed, {bb}, {}, false, {});
    return prog;
}

// ─── Public API ─────────────────────────────────────────────────────────

static constexpr uint32_t DEFAULT_N = 500;

BenchProgram build_opcode_program(const OpcodeBenchSpec& spec, uint32_t N) {
    if (N == 0)
        N = (spec.max_n > 0) ? spec.max_n : DEFAULT_N;

    switch (spec.shape) {
        case Shape::RegReg:    return build_reg_reg(spec, N);
        case Shape::RegOnly:   return build_reg_only(spec, N);
        case Shape::NoOperand: return build_no_operand(spec, N);
        default:
            // Phase 2+ shapes — return empty program (skipped at runtime)
            return {};
    }
}

BenchProgram build_nop_baseline(uint32_t N) {
    OpcodeBenchSpec nop_spec{VmOpcode::NOP, Shape::NoOperand, 0, 0, 0, "NOP_BASELINE", 0};
    return build_no_operand(nop_spec, N);
}

}  // namespace VMPilot::Bench
