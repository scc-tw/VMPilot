#pragma once
#ifndef __RUNTIME_PROGRAM_BUILDER_HPP__
#define __RUNTIME_PROGRAM_BUILDER_HPP__

/// @file program_builder.hpp
/// @brief ProgramBuilder — fluent DSL for assembling VM bytecode programs.
///
/// Provides a human-friendly interface for constructing basic blocks and
/// instructions.  Used by VmRunner to let users write programs like:
///
///   p.bb(1)
///       .load_const(0, pool_idx)
///       .native_call("puts")
///       .halt()
///   .end_bb();
///
/// The builder produces TestBB/TestInstruction vectors compatible with
/// the blob builder.  When a NativeRegistry is injected (by VmRunner),
/// `native_call("name")` resolves the name to a transition entry index.

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>

namespace VMPilot::Runtime {

// Forward declaration
class NativeRegistry;

/// Instruction descriptor (mirrors TestInstruction from test_blob_builder.hpp).
struct BuilderInstruction {
    Common::VM::VmOpcode opcode;
    uint8_t  flags;
    uint8_t  reg_a;
    uint8_t  reg_b;
    uint32_t aux;
};

/// Basic block descriptor (mirrors TestBB from test_blob_builder.hpp).
struct BuilderBB {
    uint32_t bb_id;
    uint32_t epoch;
    uint16_t live_regs_bitmap;
    uint16_t flags;
    uint8_t  epoch_seed[32];
    std::vector<BuilderInstruction> instructions;
};

/// Fluent builder for a single basic block.
///
/// Returned by ProgramBuilder::bb(). All instruction methods return *this
/// for chaining. end_bb() finalizes the block and returns the parent builder.
class BBBuilder {
public:
    class ProgramBuilder& parent_;
    BuilderBB bb_;

    BBBuilder(class ProgramBuilder& parent, uint32_t bb_id,
              uint32_t epoch, uint8_t epoch_seed_base);

    // ── Instruction emission ────────────────────────────────────────────

    /// Raw instruction with all fields.
    BBBuilder& insn(Common::VM::VmOpcode op, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint32_t aux) {
        bb_.instructions.push_back({op, flags, ra, rb, aux});
        return *this;
    }

    /// HALT — stop execution.
    BBBuilder& halt() {
        return insn(Common::VM::VmOpcode::HALT, 0, 0, 0, 0);
    }

    /// LOAD_CONST reg_a, pool[aux] — load constant from pool.
    BBBuilder& load_const(uint8_t reg, uint32_t pool_index) {
        uint8_t f = static_cast<uint8_t>(
            (Common::VM::VM_OPERAND_POOL << 6) |
            (Common::VM::VM_OPERAND_NONE << 4));
        return insn(Common::VM::VmOpcode::LOAD_CONST, f, reg, 0, pool_index);
    }

    /// MOVE dst, src — register to register copy.
    BBBuilder& move(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::MOVE, f, dst, src, 0);
    }

    /// ADD dst, src — encoded addition.
    BBBuilder& add(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::ADD, f, dst, src, 0);
    }

    /// SUB dst, src — encoded subtraction.
    BBBuilder& sub(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::SUB, f, dst, src, 0);
    }

    /// MUL dst, src — multiplication (Class C).
    BBBuilder& mul(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::MUL, f, dst, src, 0);
    }

    /// AND dst, src — bitwise AND (Class A).
    BBBuilder& bitand_(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::AND, f, dst, src, 0);
    }

    /// OR dst, src — bitwise OR (Class A).
    BBBuilder& bitor_(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::OR, f, dst, src, 0);
    }

    /// XOR dst, src — bitwise XOR (Class A).
    BBBuilder& xor_(uint8_t dst, uint8_t src) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::XOR, f, dst, src, 0);
    }

    /// CMP a, b — compare two registers, set flags.
    BBBuilder& cmp(uint8_t a, uint8_t b) {
        uint8_t f = flags_rr();
        return insn(Common::VM::VmOpcode::CMP, f, a, b, 0);
    }

    /// JMP target_bb — unconditional branch.
    BBBuilder& jmp(uint32_t target_bb_id) {
        return insn(Common::VM::VmOpcode::JMP, 0, 0, 0, target_bb_id);
    }

    /// JCC target_bb, condition — conditional branch.
    BBBuilder& jcc(uint32_t target_bb_id, uint8_t condition) {
        return insn(Common::VM::VmOpcode::JCC, condition, 0, 0, target_bb_id);
    }

    /// NATIVE_CALL by name — resolves via injected NativeRegistry.
    /// The aux field is set to the transition entry index.
    BBBuilder& native_call(const std::string& name);

    /// NATIVE_CALL by raw transition entry index.
    BBBuilder& native_call_raw(uint32_t transition_index) {
        uint8_t f = static_cast<uint8_t>(
            (Common::VM::VM_OPERAND_NONE << 6) |
            (Common::VM::VM_OPERAND_NONE << 4));
        return insn(Common::VM::VmOpcode::NATIVE_CALL, f, 0, 0, transition_index);
    }

    /// NOP — no operation.
    BBBuilder& nop() {
        return insn(Common::VM::VmOpcode::NOP, 0, 0, 0, 0);
    }

    /// PUSH reg — push register to ORAM stack.
    BBBuilder& push(uint8_t reg) {
        uint8_t f = static_cast<uint8_t>(
            (Common::VM::VM_OPERAND_REG << 6) |
            (Common::VM::VM_OPERAND_NONE << 4));
        return insn(Common::VM::VmOpcode::PUSH, f, reg, 0, 0);
    }

    /// POP reg — pop from ORAM stack to register.
    BBBuilder& pop(uint8_t reg) {
        uint8_t f = static_cast<uint8_t>(
            (Common::VM::VM_OPERAND_NONE << 6) |
            (Common::VM::VM_OPERAND_REG << 4));
        return insn(Common::VM::VmOpcode::POP, f, reg, 0, 0);
    }

    /// CALL_VM target_bb — push shadow stack, jump to VM subroutine.
    BBBuilder& call_vm(uint32_t target_bb_id) {
        return insn(Common::VM::VmOpcode::CALL_VM, 0, 0, 0, target_bb_id);
    }

    /// RET_VM — return from VM subroutine.
    BBBuilder& ret_vm() {
        return insn(Common::VM::VmOpcode::RET_VM, 0, 0, 0, 0);
    }

    /// Finalize this BB and return to the parent ProgramBuilder.
    class ProgramBuilder& end_bb();

private:
    static uint8_t flags_rr() {
        return static_cast<uint8_t>(
            (Common::VM::VM_OPERAND_REG << 6) |
            (Common::VM::VM_OPERAND_REG << 4));
    }
};

/// Pool entry descriptor for per-register encoding.
struct BuilderPoolEntry {
    uint64_t plaintext;
    uint32_t target_bb_index;  ///< which BB's encoding tables to use
    uint8_t  target_reg;       ///< which register's encoding to use
};

/// Fluent program builder — assembles basic blocks and pool entries.
///
/// A NativeRegistry reference can be injected so that BBBuilder::native_call()
/// resolves names at build time.
class ProgramBuilder {
public:
    ProgramBuilder() : registry_(nullptr), epoch_counter_(0) {}
    explicit ProgramBuilder(NativeRegistry* reg)
        : registry_(reg), epoch_counter_(0) {}

    /// Start a new basic block with the given ID.
    /// epoch_seed_base is used to deterministically seed the epoch
    /// (different values per BB give different encoding tables).
    BBBuilder bb(uint32_t bb_id, uint8_t epoch_seed_base = 0) {
        if (epoch_seed_base == 0) {
            // Auto-assign based on BB count for uniqueness
            epoch_seed_base = static_cast<uint8_t>(0xA0 + bbs_.size() * 0x10);
        }
        return BBBuilder(*this, bb_id, epoch_counter_++, epoch_seed_base);
    }

    /// Add a constant pool entry with per-register encoding info.
    uint32_t pool(uint64_t value, uint32_t target_bb_index, uint8_t target_reg) {
        uint32_t idx = static_cast<uint32_t>(pool_entries_.size());
        pool_entries_.push_back({value, target_bb_index, target_reg});
        return idx;
    }

    /// Add a constant pool entry for the default (BB 0, reg 0).
    uint32_t pool(uint64_t value) {
        return pool(value, 0, 0);
    }

    /// Access the injected NativeRegistry (may be null).
    [[nodiscard]] NativeRegistry* registry() const noexcept { return registry_; }

    /// Get the assembled basic blocks.
    [[nodiscard]] const std::vector<BuilderBB>& basic_blocks() const noexcept {
        return bbs_;
    }

    /// Get the pool entries.
    [[nodiscard]] const std::vector<BuilderPoolEntry>& pool_entries() const noexcept {
        return pool_entries_;
    }

    // Internal: called by BBBuilder::end_bb() to push a completed BB.
    void push_bb(BuilderBB&& bb) {
        bbs_.push_back(std::move(bb));
    }

private:
    NativeRegistry* registry_;
    uint32_t epoch_counter_;
    std::vector<BuilderBB> bbs_;
    std::vector<BuilderPoolEntry> pool_entries_;
};

// ── BBBuilder implementation ────────────────────────────────────────────────

inline BBBuilder::BBBuilder(ProgramBuilder& parent, uint32_t bb_id,
                            uint32_t epoch, uint8_t epoch_seed_base)
    : parent_(parent)
{
    bb_.bb_id = bb_id;
    bb_.epoch = epoch;
    bb_.live_regs_bitmap = 0xFFFF;  // all registers live by default
    bb_.flags = 0;
    for (int i = 0; i < 32; ++i)
        bb_.epoch_seed[i] = static_cast<uint8_t>(epoch_seed_base + i);
}

inline BBBuilder& BBBuilder::native_call(const std::string& name) {
    NativeRegistry* reg = parent_.registry();
    if (!reg)
        throw std::runtime_error("ProgramBuilder: native_call(\"" + name +
                                 "\") requires a NativeRegistry");
    uint32_t idx = reg->index_of(name);
    return native_call_raw(idx);
}

inline ProgramBuilder& BBBuilder::end_bb() {
    parent_.push_bb(std::move(bb_));
    return parent_;
}

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_PROGRAM_BUILDER_HPP__
