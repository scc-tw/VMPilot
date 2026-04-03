/// @file program_factory.cpp
/// @brief Build Doc 19 dispatch-unit benchmark programs.
///
/// Each program = setup BBs (untimed) + K measured BBs.
/// Each BB = exactly N instructions (1 real + N-1 NOP), mimicking a
/// Doc 19 fixed-width dispatch unit.
///
/// All BBs across all benchmarks have the same length N, guaranteeing
/// identical max_bb_insn_count and therefore identical verify_bb_mac cost.

#include "program_factory.hpp"
#include "test_blob_builder.hpp"

#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>

namespace VMPilot::Bench {

using namespace Common::VM;
using Test::TestBB;
using Test::TestInstruction;
using Test::TestNativeCall;

// ─── Helpers ───────────────────────────────────────────────────────────

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

static void fill_seed(uint8_t seed[32], uint8_t base = 0x42) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

static TestBB make_bb(uint32_t id, uint8_t epoch_base) {
    TestBB bb{};
    bb.bb_id = id; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    for (int i = 0; i < 32; ++i)
        bb.epoch_seed[i] = static_cast<uint8_t>(epoch_base + i);
    return bb;
}

/// Build one measured BB: [real_opcode, NOP×(N-1)] or [NOP×(N-1), real_opcode] for branches.
static TestBB make_measured_bb(uint32_t bb_id, uint8_t epoch_base,
                               const TestInstruction& real_insn,
                               uint32_t N, bool branch_at_end) {
    auto bb = make_bb(bb_id, epoch_base);
    if (branch_at_end) {
        // Branch opcodes go at position N-1 (last in DU)
        for (uint32_t i = 0; i + 1 < N; ++i)
            bb.instructions.push_back({VmOpcode::NOP, f_none(), 0, 0, 0});
        bb.instructions.push_back(real_insn);
    } else {
        // Non-branch opcodes go at position 0
        bb.instructions.push_back(real_insn);
        for (uint32_t i = 1; i < N; ++i)
            bb.instructions.push_back({VmOpcode::NOP, f_none(), 0, 0, 0});
    }
    return bb;
}

/// Build one NOP-only BB of length N.
static TestBB make_nop_bb(uint32_t bb_id, uint8_t epoch_base, uint32_t N) {
    auto bb = make_bb(bb_id, epoch_base);
    for (uint32_t i = 0; i < N; ++i)
        bb.instructions.push_back({VmOpcode::NOP, f_none(), 0, 0, 0});
    return bb;
}

/// Build setup BBs of exactly N insns each.
///
/// For N≥2: one BB with setup_insns + NOP padding + JMP (fits in N insns).
/// For N=1: one BB per setup instruction (no JMP — fallthrough handles it).
///
/// Returns the BBs (may be 1 or many) and updates next_bb_id.
static std::vector<TestBB> make_setup_bbs(uint32_t& next_bb_id,
                                          uint8_t epoch_base,
                                          const std::vector<TestInstruction>& setup_insns,
                                          uint32_t target_bb_id, uint32_t N) {
    std::vector<TestBB> result;

    if (N >= 2) {
        // All setup insns + NOPs + JMP fit in one N-insn BB
        auto bb = make_bb(next_bb_id++, epoch_base);
        for (const auto& insn : setup_insns)
            bb.instructions.push_back(insn);
        while (bb.instructions.size() + 1 < N)
            bb.instructions.push_back({VmOpcode::NOP, f_none(), 0, 0, 0});
        bb.instructions.push_back({VmOpcode::JMP, f_none(), 0, 0, target_bb_id});
        result.push_back(std::move(bb));
    } else {
        // N=1: each setup insn is its own 1-insn BB (fallthrough to next)
        for (const auto& insn : setup_insns) {
            auto bb = make_bb(next_bb_id++, epoch_base);
            bb.instructions.push_back(insn);
            result.push_back(std::move(bb));
        }
    }

    return result;
}

// ─── Trivial native for NATIVE_CALL benchmarks ────────────────────────

static uint64_t trivial_native(uint64_t, uint64_t, uint64_t, uint64_t,
                                uint64_t, uint64_t, uint64_t, uint64_t) {
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════

DUBenchProgram build_du_program(const OpcodeBenchSpec& spec,
                                uint32_t K, uint32_t N) {
    DUBenchProgram prog;
    prog.opcode = spec.opcode;
    prog.du_count = K;
    prog.du_width = N;
    fill_seed(prog.seed);

    std::vector<TestBB> bbs;
    std::vector<uint64_t> pool;
    std::vector<TestNativeCall> ncs;
    uint32_t next_bb_id = 1;
    const bool is_branch = (spec.setup == Setup::Branch);

    // ── Determine the real instruction ──────────────────────────────
    TestInstruction real_insn{};
    real_insn.opcode = spec.opcode;
    real_insn.reg_a  = spec.reg_a;
    real_insn.reg_b  = spec.reg_b;
    real_insn.aux    = spec.aux;

    switch (spec.setup) {
        case Setup::None:
        case Setup::CtxRead:
            real_insn.flags = f_none();
            if (spec.opcode == VmOpcode::LOAD_CTX)
                real_insn.flags = f_r();
            break;
        case Setup::Reg1:
            real_insn.flags = f_r();
            break;
        case Setup::Reg2:
            real_insn.flags = f_rr();
            break;
        case Setup::Memory:
            real_insn.flags = f_rm();
            real_insn.aux = 0;  // guest mem offset 0
            prog.needs_guest_memory = true;
            break;
        case Setup::OramPush:
            real_insn.flags = f_r();
            break;
        case Setup::OramPop:
            real_insn.flags = f_r();
            break;
        case Setup::Pool:
            real_insn.flags = f_pool();
            break;
        case Setup::CtxWrite:
            real_insn.flags = f_r();
            break;
        case Setup::Branch:
            real_insn.flags = f_none();
            // aux = target BB ID, set per measured BB below
            break;
        case Setup::NativeCall:
            real_insn.flags = f_none();
            prog.needs_native = true;
            prog.native_fn = reinterpret_cast<uint64_t>(&trivial_native);
            break;
    }

    // ── Build setup BBs (untimed) ───────────────────────────────────
    uint32_t first_measured_id = 0;  // set after setup BBs

    // Helper: add setup BBs and update setup_du_count.
    auto add_setup = [&](const std::vector<TestInstruction>& insns) {
        auto sbs = make_setup_bbs(next_bb_id, 0xA0, insns, 0, N);
        prog.setup_du_count += static_cast<uint32_t>(sbs.size());
        for (auto& sb : sbs)
            bbs.push_back(std::move(sb));
    };

    switch (spec.setup) {
        case Setup::Reg1:
            pool.push_back(42);
            add_setup({{VmOpcode::LOAD_CONST, f_pool(), spec.reg_a, 0, 0}});
            break;

        case Setup::Reg2:
            pool.push_back(42);
            pool.push_back(3);
            add_setup({{VmOpcode::LOAD_CONST, f_pool(), spec.reg_a, 0, 0},
                       {VmOpcode::LOAD_CONST, f_pool(), spec.reg_b, 0, 1}});
            break;

        case Setup::Memory:
            pool.push_back(42);
            add_setup({{VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0},
                       {VmOpcode::STORE, f_rm(), 0, 0, 0}});
            break;

        case Setup::OramPush:
            pool.push_back(42);
            add_setup({{VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0}});
            break;

        case Setup::OramPop: {
            // Setup: 1 LOAD_CONST BB + K PUSH BBs, all fallthrough (no JMP).
            // The LOAD_CONST BB must NOT use add_setup() because add_setup's
            // JMP gets fixed to first_measured_id, skipping the PUSH BBs.
            pool.push_back(42);
            {
                TestInstruction lc{VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0};
                bbs.push_back(make_measured_bb(next_bb_id++, 0xA0,
                                               lc, N, false));
                prog.setup_du_count++;
            }
            // K PUSH BBs (each is a DU with 1 PUSH + N-1 NOP)
            for (uint32_t i = 0; i < K; ++i) {
                TestInstruction push_insn{VmOpcode::PUSH, f_r(), 0, 0, 0};
                bbs.push_back(make_measured_bb(next_bb_id++, 0xA1,
                                               push_insn, N, false));
                prog.setup_du_count++;
            }
            break;
        }

        case Setup::Pool:
            for (uint32_t i = 0; i < K; ++i)
                pool.push_back(i + 100);
            break;

        case Setup::CtxWrite:
            pool.push_back(0x800);
            add_setup({{VmOpcode::LOAD_CONST, f_pool(), 0, 0, 0}});
            break;

        case Setup::NativeCall:
            break;

        default:
            break;
    }

    // Fix setup BB JMP targets → first measured BB (N≥2 only; N=1 uses fallthrough)
    first_measured_id = next_bb_id;
    for (auto& sb : bbs) {
        if (!sb.instructions.empty()) {
            auto& last = sb.instructions.back();
            if (last.opcode == VmOpcode::JMP)
                last.aux = first_measured_id;
        }
    }

    // ── Build K measured BBs ────────────────────────────────────────
    for (uint32_t i = 0; i < K; ++i) {
        uint32_t bb_id = next_bb_id++;
        uint8_t epoch_base = static_cast<uint8_t>(0xB0 + (i & 0x0F));

        if (is_branch) {
            // Branch at end of DU, target = next BB
            TestInstruction br = real_insn;
            br.aux = (i + 1 < K) ? (bb_id + 1) : bb_id;  // last BB: self (will HALT)
            bbs.push_back(make_measured_bb(bb_id, epoch_base, br, N, true));
        } else if (spec.setup == Setup::Pool) {
            TestInstruction lc = real_insn;
            lc.aux = i % static_cast<uint32_t>(pool.size());
            bbs.push_back(make_measured_bb(bb_id, epoch_base, lc, N, false));
        } else if (spec.setup == Setup::NativeCall) {
            TestInstruction nc_insn = real_insn;
            nc_insn.aux = 0;  // all use transition entry 0
            bbs.push_back(make_measured_bb(bb_id, epoch_base, nc_insn, N, false));
        } else {
            bbs.push_back(make_measured_bb(bb_id, epoch_base, real_insn, N, false));
        }
    }

    // Replace last measured BB's last instruction with HALT
    if (!bbs.empty()) {
        auto& last_bb = bbs.back();
        last_bb.instructions.back() = {VmOpcode::HALT, f_none(), 0, 0, 0};
    }

    // ── Build native call transition entries ─────────────────────────
    if (spec.setup == Setup::NativeCall) {
        TestNativeCall nc{};
        nc.call_site_ip = 0;  // placeholder, not used for timing
        nc.arg_count = 0;
        nc.target_addr = prog.native_fn;
        ncs.push_back(nc);
    }

    // ── Assemble blob ───────────────────────────────────────────────
    prog.blob = Test::build_test_blob(prog.seed, bbs, pool, false, ncs);
    return prog;
}

DUBenchProgram build_du_baseline(uint32_t K, uint32_t N) {
    DUBenchProgram prog;
    prog.opcode = VmOpcode::NOP;
    prog.du_count = K;
    prog.du_width = N;
    fill_seed(prog.seed);

    std::vector<TestBB> bbs;
    for (uint32_t i = 0; i < K; ++i) {
        uint32_t bb_id = i + 1;
        uint8_t epoch_base = static_cast<uint8_t>(0xB0 + (i & 0x0F));
        bbs.push_back(make_nop_bb(bb_id, epoch_base, N));
    }

    // Last BB: replace last NOP with HALT
    if (!bbs.empty())
        bbs.back().instructions.back() = {VmOpcode::HALT, f_none(), 0, 0, 0};

    prog.blob = Test::build_test_blob(prog.seed, bbs, {}, false, {});
    return prog;
}

}  // namespace VMPilot::Bench
