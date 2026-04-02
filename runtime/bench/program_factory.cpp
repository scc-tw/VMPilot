/// @file program_factory.cpp
/// @brief Generates encrypted VM blobs for each opcode benchmark shape.

#include "program_factory.hpp"
#include "test_blob_builder.hpp"

#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>

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
static uint8_t f_rm() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_MEM << 4));
}
static uint8_t f_none() { return 0; }

// ─── Seed helpers ───────────────────────────────────────────────────────

static void fill_seed(uint8_t seed[32], uint8_t base = 0x42) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

static void fill_epoch_seed(uint8_t out[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        out[i] = static_cast<uint8_t>(base + i);
}

static TestBB make_bb(uint32_t id, uint8_t epoch_base) {
    TestBB bb{};
    bb.bb_id = id; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, epoch_base);
    return bb;
}

// ═══════════════════════════════════════════════════════════════════════
// Shape builders
// ═══════════════════════════════════════════════════════════════════════

// ── RegReg ──────────────────────────────────────────────────────────────
static BenchProgram build_reg_reg(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB0);
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), spec.reg_a, 0, 0});
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), spec.reg_b, 0, 1});
    TestInstruction body{spec.opcode, f_rr(), spec.reg_a, spec.reg_b, spec.aux};
    for (uint32_t i = 0; i < N; ++i) bb.instructions.push_back(body);
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {42, 3}, false, {});
    return prog;
}

// ── RegOnly ─────────────────────────────────────────────────────────────
static BenchProgram build_reg_only(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB1);
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), spec.reg_a, 0, 0});
    TestInstruction body{spec.opcode, f_r(), spec.reg_a, 0, spec.aux};
    for (uint32_t i = 0; i < N; ++i) bb.instructions.push_back(body);
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {42}, false, {});
    return prog;
}

// ── NoOperand ───────────────────────────────────────────────────────────
static BenchProgram build_no_operand(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB2);
    TestInstruction body{spec.opcode, f_none(), 0, 0, spec.aux};
    for (uint32_t i = 0; i < N; ++i) bb.instructions.push_back(body);
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {}, false, {});
    return prog;
}

// ── Memory (LOAD, STORE, atomics) ───────────────────────────────────────
static BenchProgram build_memory(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode = spec.opcode;
    prog.measured_insn_count = N;
    prog.needs_guest_memory = true;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB3);
    // Setup: load a value and store it to mem[0] so reads have valid data
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0});
    bb.instructions.push_back({VmOpcode::STORE, f_rm(), 0, 0, 0});

    TestInstruction body{spec.opcode, f_rm(), spec.reg_a, spec.reg_b, 0};
    for (uint32_t i = 0; i < N; ++i) bb.instructions.push_back(body);
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {42}, false, {});
    return prog;
}

// ── Oram (PUSH / POP) ──────────────────────────────────────────────────
static BenchProgram build_push(uint32_t N) {
    BenchProgram prog;
    prog.opcode = VmOpcode::PUSH;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB4);
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0});
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back({VmOpcode::PUSH, f_r(), 0, 0, 0});
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {42}, false, {});
    return prog;
}

static BenchProgram build_pop(uint32_t N) {
    BenchProgram prog;
    prog.opcode = VmOpcode::POP;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB5);
    bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0});
    // Setup: push N items
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back({VmOpcode::PUSH, f_r(), 0, 0, 0});
    // Measured: pop N items
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back({VmOpcode::POP, f_r(), 0, 0, 0});
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {42}, false, {});
    return prog;
}

// ── PoolReg (LOAD_CONST) ────────────────────────────────────────────────
static BenchProgram build_pool_reg(uint32_t N) {
    BenchProgram prog;
    prog.opcode = VmOpcode::LOAD_CONST;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB6);
    std::vector<uint64_t> pool;
    pool.reserve(N);
    for (uint32_t i = 0; i < N; ++i) {
        pool.push_back(i + 100);
        bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), 0, 0, i});
    }
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, pool, false, {});
    return prog;
}

// ── CtxAccess (LOAD_CTX / STORE_CTX) ────────────────────────────────────
static BenchProgram build_ctx_access(const OpcodeBenchSpec& spec, uint32_t N) {
    BenchProgram prog;
    prog.opcode = spec.opcode;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xB7);
    if (spec.opcode == VmOpcode::STORE_CTX)
        bb.instructions.push_back({VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0});

    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back({spec.opcode, f_r(), 0, 0, spec.aux});
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    std::vector<uint64_t> pool;
    if (spec.opcode == VmOpcode::STORE_CTX) pool.push_back(0x800);
    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, pool, false, {});
    return prog;
}

// ── Custom: JMP / JCC (measures BB transition cost) ─────────────────────
static BenchProgram build_jmp_bench(VmOpcode op, uint32_t N) {
    BenchProgram prog;
    prog.opcode = op;
    prog.measured_insn_count = N;
    fill_seed(prog.seed);

    // BB1: setup counter, JMP → BB2
    // BB2: SUB r0, r1; CMP r0, r1; JCC NE → BB2; JMP → BB3
    // BB3: HALT
    auto bb1 = make_bb(1, 0xC0);
    auto bb2 = make_bb(2, 0xC1);
    auto bb3 = make_bb(3, 0xC2);

    bb1.instructions = {
        {VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, f_pool(), 1, 0, 1},
        {VmOpcode::JMP, f_none(), 0, 0, 2},
    };

    uint8_t jcc_ne = static_cast<uint8_t>(
        (VM_OPERAND_NONE << 6) | (VM_OPERAND_NONE << 4) | 1);
    bb2.instructions = {
        {VmOpcode::SUB, f_rr(), 0, 1, 0},
        {VmOpcode::CMP, f_rr(), 0, 1, 0},
        {VmOpcode::JCC, jcc_ne, 0, 0, 2},
        {VmOpcode::JMP, f_none(), 0, 0, 3},
    };

    bb3.instructions = {{VmOpcode::HALT, f_none(), 0, 0, 0}};

    prog.total_insn_count = 3 + N * 4 + 1;
    prog.blob = Test::build_test_blob(prog.seed, {bb1, bb2, bb3},
                                       {static_cast<uint64_t>(N), 1},
                                       false, {});
    return prog;
}

// ── Custom: NATIVE_CALL ─────────────────────────────────────────────────
static uint64_t trivial_native(uint64_t, uint64_t, uint64_t, uint64_t,
                                uint64_t, uint64_t, uint64_t, uint64_t) {
    return 0;
}

static BenchProgram build_native_call(uint32_t N) {
    BenchProgram prog;
    prog.opcode = VmOpcode::NATIVE_CALL;
    prog.measured_insn_count = N;
    prog.needs_native = true;
    prog.native_fn = reinterpret_cast<uint64_t>(&trivial_native);
    fill_seed(prog.seed);

    auto bb = make_bb(1, 0xC3);
    std::vector<Test::TestNativeCall> ncs;
    for (uint32_t i = 0; i < N; ++i) {
        bb.instructions.push_back({VmOpcode::NATIVE_CALL, f_none(), 0, 0, i});
        Test::TestNativeCall nc{};
        nc.call_site_ip = i;
        nc.arg_count = 0;
        nc.target_addr = prog.native_fn;
        ncs.push_back(nc);
    }
    bb.instructions.push_back({VmOpcode::HALT, f_none(), 0, 0, 0});

    prog.total_insn_count = static_cast<uint32_t>(bb.instructions.size());
    prog.blob = Test::build_test_blob(prog.seed, {bb}, {}, false, ncs);
    return prog;
}

// ═══════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════

static constexpr uint32_t DEFAULT_N = 500;

BenchProgram build_opcode_program(const OpcodeBenchSpec& spec, uint32_t N) {
    if (N == 0)
        N = (spec.max_n > 0) ? spec.max_n : DEFAULT_N;

    switch (spec.shape) {
        case Shape::RegReg:    return build_reg_reg(spec, N);
        case Shape::RegOnly:   return build_reg_only(spec, N);
        case Shape::NoOperand: return build_no_operand(spec, N);
        case Shape::Memory:    return build_memory(spec, N);
        case Shape::PoolReg:   return build_pool_reg(N);
        case Shape::CtxAccess: return build_ctx_access(spec, N);
        case Shape::Oram:
            return (spec.opcode == VmOpcode::POP) ? build_pop(N) : build_push(N);
        case Shape::Custom:
            if (spec.opcode == VmOpcode::JMP)         return build_jmp_bench(VmOpcode::JMP, N);
            if (spec.opcode == VmOpcode::JCC)         return build_jmp_bench(VmOpcode::JCC, N);
            if (spec.opcode == VmOpcode::NATIVE_CALL) return build_native_call(N);
            return {};
    }
    return {};
}

BenchProgram build_nop_baseline(uint32_t N) {
    OpcodeBenchSpec nop{VmOpcode::NOP, Shape::NoOperand, 0, 0, 0, "NOP_BASELINE", 0};
    return build_no_operand(nop, N);
}

}  // namespace VMPilot::Bench
