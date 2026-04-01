#pragma once
/// @file program_builder.hpp
/// @brief Fluent builder for constructing VM bytecode programs in examples.
///
/// Wraps test_blob_builder.hpp with a human-readable DSL:
///
///   auto prog = ProgramBuilder(seed)
///       .bb(1)
///           .load_const(0, 42)     // r0 = 42
///           .load_const(1, 58)     // r1 = 58
///           .add(0, 1)             // r0 += r1
///           .halt()
///       .end_bb()
///       .build();
///
///   auto engine = VmEngine<DebugPolicy, DirectOram>::create(
///       prog.data(), prog.size(), seed);

#include "test_blob_builder.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

namespace VMPilot::Example {

using namespace Common::VM;
using namespace Test;

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────────────────────────────────

class ProgramBuilder;

// ─────────────────────────────────────────────────────────────────────────────
// BBBuilder — builds a single basic block with method chaining
// ─────────────────────────────────────────────────────────────────────────────

class BBBuilder {
public:
    BBBuilder(ProgramBuilder& parent, uint32_t bb_id, uint16_t live_regs)
        : parent_(parent)
    {
        bb_.bb_id = bb_id;
        bb_.epoch = 0;
        bb_.live_regs_bitmap = live_regs;
        bb_.flags = 0;
        // Deterministic epoch seed from bb_id (reproducible across runs)
        for (int i = 0; i < 32; ++i)
            bb_.epoch_seed[i] = static_cast<uint8_t>(bb_id * 37 + i);
    }

    /// Set epoch (for multi-epoch programs)
    BBBuilder& epoch(uint32_t e) { bb_.epoch = e; return *this; }

    /// Set custom epoch seed
    BBBuilder& epoch_seed(const uint8_t seed[32]) {
        std::memcpy(bb_.epoch_seed, seed, 32);
        return *this;
    }

    // ── Data movement ───────────────────────────────────────────────────

    /// LOAD_CONST reg, value — loads a 64-bit constant into register
    BBBuilder& load_const(uint8_t reg, uint64_t value) {
        uint32_t pool_idx = next_pool_idx_();
        pool_.push_back({value, bb_index_(), reg});
        return emit_(VmOpcode::LOAD_CONST, flags_(VM_OPERAND_POOL, VM_OPERAND_NONE),
                      reg, 0, pool_idx);
    }

    /// MOVE dst, src — copy register
    BBBuilder& move(uint8_t dst, uint8_t src) {
        return emit_rr_(VmOpcode::MOVE, dst, src);
    }

    /// LOAD dst, aux_addr — load from guest memory
    BBBuilder& load(uint8_t dst, uint32_t addr) {
        return emit_(VmOpcode::LOAD, flags_(VM_OPERAND_REG, VM_OPERAND_MEM),
                      dst, 0, addr);
    }

    /// STORE src, aux_addr — store to guest memory
    BBBuilder& store(uint8_t src, uint32_t addr) {
        return emit_(VmOpcode::STORE, flags_(VM_OPERAND_REG, VM_OPERAND_MEM),
                      src, 0, addr);
    }

    /// PUSH src — push to ORAM stack
    BBBuilder& push(uint8_t src) {
        return emit_(VmOpcode::PUSH, flags_(VM_OPERAND_REG, VM_OPERAND_NONE),
                      src, 0, 0);
    }

    /// POP dst — pop from ORAM stack
    BBBuilder& pop(uint8_t dst) {
        return emit_(VmOpcode::POP, flags_(VM_OPERAND_REG, VM_OPERAND_NONE),
                      dst, 0, 0);
    }

    // ── Arithmetic (dst = dst OP src) ───────────────────────────────────

    BBBuilder& add(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::ADD, dst, src); }
    BBBuilder& sub(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::SUB, dst, src); }
    BBBuilder& mul(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::MUL, dst, src); }
    BBBuilder& div(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::DIV, dst, src); }
    BBBuilder& mod(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::MOD, dst, src); }
    BBBuilder& neg(uint8_t dst)              { return emit_r_(VmOpcode::NEG, dst); }

    // ── Bitwise (dst = dst OP src) ──────────────────────────────────────

    BBBuilder& xor_(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::XOR, dst, src); }
    BBBuilder& and_(uint8_t dst, uint8_t src) { return emit_rr_(VmOpcode::AND, dst, src); }
    BBBuilder& or_(uint8_t dst, uint8_t src)  { return emit_rr_(VmOpcode::OR,  dst, src); }
    BBBuilder& not_(uint8_t dst)              { return emit_r_(VmOpcode::NOT, dst); }
    BBBuilder& shl(uint8_t dst, uint8_t src)  { return emit_rr_(VmOpcode::SHL, dst, src); }
    BBBuilder& shr(uint8_t dst, uint8_t src)  { return emit_rr_(VmOpcode::SHR, dst, src); }

    // ── Comparison ──────────────────────────────────────────────────────

    BBBuilder& cmp(uint8_t a, uint8_t b) { return emit_rr_(VmOpcode::CMP, a, b); }
    BBBuilder& test(uint8_t a, uint8_t b) { return emit_rr_(VmOpcode::TEST, a, b); }

    // ── Control flow ────────────────────────────────────────────────────

    BBBuilder& jmp(uint32_t target_bb_id) {
        return emit_(VmOpcode::JMP, flags_(VM_OPERAND_NONE, VM_OPERAND_NONE),
                      0, 0, target_bb_id);
    }

    BBBuilder& jcc(uint8_t condition, uint32_t target_bb_id) {
        return emit_(VmOpcode::JCC,
                      flags_(VM_OPERAND_NONE, VM_OPERAND_NONE, condition),
                      0, 0, target_bb_id);
    }

    BBBuilder& halt() {
        return emit_(VmOpcode::HALT, 0, 0, 0, 0);
    }

    BBBuilder& nop() {
        return emit_(VmOpcode::NOP, 0, 0, 0, 0);
    }

    /// NATIVE_CALL — calls native function at transition table index
    BBBuilder& native_call(uint32_t transition_index) {
        return emit_(VmOpcode::NATIVE_CALL, flags_(VM_OPERAND_NONE, VM_OPERAND_NONE),
                      0, 0, transition_index);
    }

    // ── Width extension ─────────────────────────────────────────────────

    BBBuilder& sext8(uint8_t dst)  { return emit_r_(VmOpcode::SEXT8, dst); }
    BBBuilder& sext16(uint8_t dst) { return emit_r_(VmOpcode::SEXT16, dst); }
    BBBuilder& sext32(uint8_t dst) { return emit_r_(VmOpcode::SEXT32, dst); }
    BBBuilder& zext8(uint8_t dst)  { return emit_r_(VmOpcode::ZEXT8, dst); }
    BBBuilder& zext16(uint8_t dst) { return emit_r_(VmOpcode::ZEXT16, dst); }
    BBBuilder& zext32(uint8_t dst) { return emit_r_(VmOpcode::ZEXT32, dst); }

    // ── Builder control ─────────────────────────────────────────────────

    /// Finish this BB and return to the program builder.
    ProgramBuilder& end_bb();

    /// Access the built TestBB (for advanced use)
    const TestBB& test_bb() const { return bb_; }
    const std::vector<TestPoolEntry>& pool_entries() const { return pool_; }

private:
    ProgramBuilder& parent_;
    TestBB bb_{};
    std::vector<TestPoolEntry> pool_;

    static uint8_t flags_(uint8_t op_a, uint8_t op_b, uint8_t cond = 0) {
        return static_cast<uint8_t>((op_a << 6) | (op_b << 4) | (cond & 0x0F));
    }

    /// Emit a generic instruction
    BBBuilder& emit_(VmOpcode op, uint8_t fl, uint8_t ra, uint8_t rb, uint32_t aux) {
        TestInstruction insn{};
        insn.opcode = op;
        insn.flags  = fl;
        insn.reg_a  = ra;
        insn.reg_b  = rb;
        insn.aux    = aux;
        bb_.instructions.push_back(insn);
        return *this;
    }

    /// reg-reg instruction shorthand
    BBBuilder& emit_rr_(VmOpcode op, uint8_t dst, uint8_t src) {
        return emit_(op, flags_(VM_OPERAND_REG, VM_OPERAND_REG), dst, src, 0);
    }

    /// reg-only instruction shorthand (unary: NOT, NEG, SEXT, etc.)
    BBBuilder& emit_r_(VmOpcode op, uint8_t dst) {
        return emit_(op, flags_(VM_OPERAND_REG, VM_OPERAND_NONE), dst, 0, 0);
    }

    uint32_t bb_index_() const;       // defined after ProgramBuilder
    uint32_t next_pool_idx_() const;  // defined after ProgramBuilder
};

// ─────────────────────────────────────────────────────────────────────────────
// ProgramBuilder — assembles multiple BBs into a complete blob
// ─────────────────────────────────────────────────────────────────────────────

class ProgramBuilder {
public:
    explicit ProgramBuilder(const uint8_t seed[32]) {
        std::memcpy(seed_, seed, 32);
    }

    /// Start a new basic block.
    /// @param bb_id      unique BB identifier
    /// @param live_regs  register liveness bitmap (default: all 16 live)
    BBBuilder& bb(uint32_t bb_id, uint16_t live_regs = 0xFFFF) {
        bbs_.emplace_back(*this, bb_id, live_regs);
        return bbs_.back();
    }

    /// Register a native call target (for NATIVE_CALL instructions).
    /// @param call_site_ip  instruction index of the NATIVE_CALL
    /// @param arg_count     number of arguments (r0..rN-1)
    /// @param target_fn     function pointer (cast to uint64_t)
    ProgramBuilder& native(uint32_t call_site_ip, uint32_t arg_count,
                           uint64_t target_fn) {
        TestNativeCall nc{};
        nc.call_site_ip = call_site_ip;
        nc.arg_count    = arg_count;
        nc.target_addr  = target_fn;
        natives_.push_back(nc);
        return *this;
    }

    /// Build the complete encrypted blob.
    std::vector<uint8_t> build() {
        std::vector<TestBB> test_bbs;
        std::vector<TestPoolEntry> all_pool;

        for (auto& b : bbs_) {
            test_bbs.push_back(b.test_bb());
            for (auto& p : b.pool_entries())
                all_pool.push_back(p);
        }

        // build_test_blob_ex handles per-register pool encoding (TestPoolEntry)
        // build_test_blob handles raw uint64_t pool + native calls
        if (all_pool.empty()) {
            return build_test_blob(seed_, test_bbs, {}, false, natives_);
        } else {
            // build_test_blob_ex doesn't take native_calls — use it for pool,
            // and assert no natives (native_call examples use build_test_blob
            // with raw pool values instead).
            return build_test_blob_ex(seed_, test_bbs, all_pool, false);
        }
    }

    /// Access the seed (for VmEngine::create)
    const uint8_t* seed() const { return seed_; }

    // Internal: used by BBBuilder
    uint32_t bb_count() const { return static_cast<uint32_t>(bbs_.size()); }
    uint32_t total_pool_before(uint32_t bb_index) const {
        uint32_t total = 0;
        for (uint32_t i = 0; i < bb_index && i < bbs_.size(); ++i)
            total += static_cast<uint32_t>(bbs_[i].pool_entries().size());
        return total;
    }

private:
    uint8_t seed_[32];
    std::vector<BBBuilder> bbs_;
    std::vector<TestNativeCall> natives_;

    friend class BBBuilder;
};

// ─────────────────────────────────────────────────────────────────────────────
// BBBuilder deferred definitions (need ProgramBuilder to be complete)
// ─────────────────────────────────────────────────────────────────────────────

inline ProgramBuilder& BBBuilder::end_bb() { return parent_; }

inline uint32_t BBBuilder::bb_index_() const {
    return parent_.bb_count() - 1;  // this BB is always the last added
}

inline uint32_t BBBuilder::next_pool_idx_() const {
    return parent_.total_pool_before(bb_index_())
         + static_cast<uint32_t>(pool_.size());
}

}  // namespace VMPilot::Example
