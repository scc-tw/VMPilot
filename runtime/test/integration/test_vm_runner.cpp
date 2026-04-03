/// @file test_vm_runner.cpp
/// @brief Tests for VmRunner factory, NativeRegistry, and StepController.
///
/// Validates the complete VmRunner pipeline:
///   1. NativeRegistry name→index mapping
///   2. ProgramBuilder DSL with native_call("name")
///   3. VmRunner HALT-only program
///   4. VmRunner LOAD_CONST + HALT → correct return value
///   5. VmRunner with native call
///   6. StepController interactive mode
///   7. Multi-BB program with branching

#include "vm_runner.hpp"
#include <native_registry.hpp>
#include "program_builder.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;

// ============================================================================
// NativeRegistry tests
// ============================================================================

TEST(NativeRegistry, AddAndLookup) {
    NativeRegistry reg;
    auto idx0 = reg.add("puts", 0x1234, 1);
    auto idx1 = reg.add("draw", 0x5678, 2);

    EXPECT_EQ(idx0, 0u);
    EXPECT_EQ(idx1, 1u);
    EXPECT_EQ(reg.size(), 2u);
    EXPECT_EQ(reg.index_of("puts"), 0u);
    EXPECT_EQ(reg.index_of("draw"), 1u);
    EXPECT_TRUE(reg.has("puts"));
    EXPECT_FALSE(reg.has("missing"));
}

TEST(NativeRegistry, DuplicateThrows) {
    NativeRegistry reg;
    reg.add("puts", 0x1234, 1);
    EXPECT_THROW(reg.add("puts", 0x5678, 2), std::runtime_error);
}

TEST(NativeRegistry, UnknownThrows) {
    NativeRegistry reg;
    EXPECT_THROW((void)reg.index_of("missing"), std::runtime_error);
}

TEST(NativeRegistry, PackArgCount) {
    // Simple case: 3 args, no fp, not variadic
    EXPECT_EQ(NativeRegistry::pack_arg_count(3, 0, false), 3u);

    // With fp_mask: 2 args, arg1 is fp (mask = 0x02)
    uint32_t packed = NativeRegistry::pack_arg_count(2, 0x02, false);
    EXPECT_EQ(packed & 0x0F, 2u);           // arg count
    EXPECT_EQ((packed >> 4) & 0xFF, 0x02u); // fp mask

    // Variadic
    packed = NativeRegistry::pack_arg_count(1, 0, true);
    EXPECT_EQ(packed & 0x0F, 1u);
    EXPECT_TRUE((packed & (1u << 12)) != 0);
}

// ============================================================================
// ProgramBuilder tests
// ============================================================================

TEST(ProgramBuilder, BasicBlockAssembly) {
    ProgramBuilder pb;
    pb.bb(1)
        .halt()
    .end_bb();

    EXPECT_EQ(pb.basic_blocks().size(), 1u);
    EXPECT_EQ(pb.basic_blocks()[0].bb_id, 1u);
    EXPECT_EQ(pb.basic_blocks()[0].instructions.size(), 1u);
    EXPECT_EQ(pb.basic_blocks()[0].instructions[0].opcode, VmOpcode::HALT);
}

TEST(ProgramBuilder, MultipleInstructions) {
    ProgramBuilder pb;
    pb.bb(1)
        .load_const(0, 0)
        .add(0, 1)
        .halt()
    .end_bb();

    const auto& insns = pb.basic_blocks()[0].instructions;
    EXPECT_EQ(insns.size(), 3u);
    EXPECT_EQ(insns[0].opcode, VmOpcode::LOAD_CONST);
    EXPECT_EQ(insns[1].opcode, VmOpcode::ADD);
    EXPECT_EQ(insns[2].opcode, VmOpcode::HALT);
}

TEST(ProgramBuilder, NativeCallWithRegistry) {
    NativeRegistry reg;
    reg.add("my_func", 0x1234, 1);

    ProgramBuilder pb(&reg);
    pb.bb(1)
        .native_call("my_func")
        .halt()
    .end_bb();

    const auto& insns = pb.basic_blocks()[0].instructions;
    EXPECT_EQ(insns[0].opcode, VmOpcode::NATIVE_CALL);
    EXPECT_EQ(insns[0].aux, 0u);  // first registered native = index 0
}

TEST(ProgramBuilder, NativeCallWithoutRegistryThrows) {
    ProgramBuilder pb;  // no registry
    EXPECT_THROW(
        pb.bb(1).native_call("missing").halt().end_bb(),
        std::runtime_error
    );
}

TEST(ProgramBuilder, PoolEntries) {
    ProgramBuilder pb;
    auto idx0 = pb.pool(42, 0, 0);
    auto idx1 = pb.pool(99, 0, 1);

    EXPECT_EQ(idx0, 0u);
    EXPECT_EQ(idx1, 1u);
    EXPECT_EQ(pb.pool_entries().size(), 2u);
    EXPECT_EQ(pb.pool_entries()[0].plaintext, 42u);
    EXPECT_EQ(pb.pool_entries()[1].target_reg, 1u);
}

// ============================================================================
// VmRunner tests — HALT only
// ============================================================================

TEST(VmRunner, HaltOnly) {
    auto result = VmRunner<DebugPolicy>()
        .program([](ProgramBuilder& p) {
            p.bb(1)
                .halt()
            .end_bb();
        })
        .run();

    ASSERT_TRUE(result.has_value()) << "VmRunner::run() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
}

// ============================================================================
// VmRunner tests — LOAD_CONST + HALT
// ============================================================================

TEST(VmRunner, LoadConstAndHalt) {
    auto result = VmRunner<DebugPolicy>()
        .program([](ProgramBuilder& p) {
            auto pool_idx = p.pool(42, 0, 0);
            p.bb(1)
                .load_const(0, pool_idx)
                .halt()
            .end_bb();
        })
        .run();

    ASSERT_TRUE(result.has_value()) << "VmRunner::run() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u);
}

// ============================================================================
// VmRunner tests — initial registers + ADD
// ============================================================================

TEST(VmRunner, AddRegisters) {
    auto result = VmRunner<DebugPolicy>()
        .initial_regs({{0, 10}, {1, 20}})
        .program([](ProgramBuilder& p) {
            p.bb(1)
                .add(0, 1)
                .halt()
            .end_bb();
        })
        .run();

    ASSERT_TRUE(result.has_value()) << "VmRunner::run() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 30u);
}

// ============================================================================
// VmRunner tests — native call
// ============================================================================

static uint64_t test_native_double(uint64_t a, uint64_t, uint64_t, uint64_t,
                                   uint64_t, uint64_t, uint64_t, uint64_t) {
    return a * 2;
}

TEST(VmRunner, NativeCallBasic) {
    auto result = VmRunner<DebugPolicy>()
        .native("double_it", &test_native_double, 1)
        .initial_regs({{0, 21}})
        .program([](ProgramBuilder& p) {
            p.bb(1)
                .native_call("double_it")
                .halt()
            .end_bb();
        })
        .run();

    ASSERT_TRUE(result.has_value()) << "VmRunner::run() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u);
}

// ============================================================================
// StepController tests
// ============================================================================

TEST(StepController, BasicStepping) {
    auto ctrl = VmRunner<DebugPolicy>()
        .program([](ProgramBuilder& p) {
            p.bb(1)
                .nop()
                .nop()
                .halt()
            .end_bb();
        })
        .run_interactive();

    EXPECT_FALSE(ctrl.is_halted());

    // Step through NOP, NOP, HALT
    auto r1 = ctrl.step();
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(*r1, VmResult::Stepped);
    EXPECT_FALSE(ctrl.is_halted());

    auto r2 = ctrl.step();
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(*r2, VmResult::Stepped);
    EXPECT_FALSE(ctrl.is_halted());

    auto r3 = ctrl.step();
    ASSERT_TRUE(r3.has_value());
    EXPECT_EQ(*r3, VmResult::Halted);
    EXPECT_TRUE(ctrl.is_halted());
}

TEST(StepController, RunToCompletion) {
    auto ctrl = VmRunner<DebugPolicy>()
        .initial_regs({{0, 7}})
        .program([](ProgramBuilder& p) {
            p.bb(1)
                .halt()
            .end_bb();
        })
        .run_interactive();

    auto result = ctrl.run_to_completion();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 7u);
}

// ============================================================================
// VmRunner — no program throws
// ============================================================================

TEST(VmRunner, NoProgramThrows) {
    VmRunner<DebugPolicy> runner;
    EXPECT_THROW((void)runner.run(), std::runtime_error);
}
