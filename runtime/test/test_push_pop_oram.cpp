/// @file test_push_pop_oram.cpp
/// @brief Tests for PUSH/POP with ORAM stack.
///
/// Tests basic push/pop, LIFO ordering, multiple values, and stack overflow/underflow.
/// Both DirectOram and RollingKeyOram are tested.

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_state.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ============================================================================
// Helpers
// ============================================================================

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(base + i);
}

static uint8_t pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static uint8_t flags_none() { return 0; }

template<typename Policy = DebugPolicy, typename Oram = DirectOram>
auto single_bb_engine(const uint8_t seed[32], uint8_t epoch_base,
                      const std::vector<TestInstruction>& insns,
                      const std::vector<TestPoolEntry>& pool = {}) {
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, epoch_base);
    bb.instructions = insns;
    auto blob = pool.empty()
        ? build_test_blob(seed, {bb})
        : build_test_blob_ex(seed, {bb}, pool);
    return VmEngine<Policy, Oram>::create(
        blob.data(), blob.size(), seed);
}

// ============================================================================
// Basic PUSH/POP roundtrip
// ============================================================================

TEST(PushPopOram, BasicRoundtrip_DirectOram) {
    uint8_t seed[32]; fill_seed(seed);
    // r0=42, PUSH r0, LOAD_CONST r0=0, POP r0, HALT
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 0
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(PushPopOram, BasicRoundtrip_RollingKeyOram) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine<DebugPolicy, RollingKeyOram>(seed, 0xA0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ============================================================================
// LIFO order: push A, push B, pop → B, pop → A
// ============================================================================

TEST(PushPopOram, LIFOOrder) {
    uint8_t seed[32]; fill_seed(seed);
    // r0=10 (A), PUSH r0, LOAD r0=20 (B), PUSH r0, POP r0 → should be 20 (B)
    // Then POP r1 → should be 10 (A), but we verify via r0.
    // Strategy: push 10, push 20, pop r0 (=20), HALT
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA1,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
         {VmOpcode::PUSH, rr(), 0, 0, 0},                // push 10
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},   // r0 = 20
         {VmOpcode::PUSH, rr(), 0, 0, 0},                // push 20
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},   // r0 = 0 (clear)
         {VmOpcode::POP, rr(), 0, 0, 0},                 // pop → r0 = 20 (LIFO)
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{10, 0, 0}, {20, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "LIFO: first pop should return last pushed";
}

// ============================================================================
// Multiple values: push 3 values, pop them in reverse order
// ============================================================================

TEST(PushPopOram, MultipleValues) {
    uint8_t seed[32]; fill_seed(seed);
    // Push 10, 20, 30. Pop three times. Last pop (third) gives 10 in r0.
    // Sequence: push 10, push 20, push 30, pop (30→r0), pop (20→r0), pop (10→r0)
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA2,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 30
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::POP, rr(), 0, 0, 0},                // r0 = 30
         {VmOpcode::POP, rr(), 0, 0, 0},                // r0 = 20
         {VmOpcode::POP, rr(), 0, 0, 0},                // r0 = 10
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{10, 0, 0}, {20, 0, 0}, {30, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "After 3 pushes and 3 pops, r0 should be first pushed value";
}

// ============================================================================
// Stack underflow: POP on empty stack
// ============================================================================

TEST(PushPopOram, StackUnderflow) {
    uint8_t seed[32]; fill_seed(seed);
    // POP without PUSH → StackUnderflow error
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA3,
        {{VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_FALSE(r.has_value()) << "POP on empty stack should fail";
    EXPECT_EQ(r.error(), DiagnosticCode::StackUnderflow);
}

// ============================================================================
// Stack overflow: exhaust the 4KB ORAM workspace (512 pushes of 8 bytes)
// ============================================================================

TEST(PushPopOram, StackOverflow) {
    uint8_t seed[32]; fill_seed(seed);
    // 4096 / 8 = 512 pushes fill the stack. The 513th should overflow.
    // We can't build a 513-instruction BB easily, so test via handler directly.
    // Build a program that pushes more than 512 times is impractical with
    // the test builder; instead, construct a small program and manually
    // set vm_sp near the limit.

    // Build a minimal program with PUSH
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA4,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}});
    ASSERT_TRUE(engine.has_value());

    // Set vm_sp to 0 (stack full) before execution
    engine->execution().vm_sp = 0;

    // Now try to push — should fail with StackOverflow
    // Skip LOAD_CONST by executing one step
    auto step1 = engine->step();
    ASSERT_TRUE(step1.has_value());

    // The next step is PUSH with vm_sp=0 — should fail
    auto step2 = engine->step();
    ASSERT_FALSE(step2.has_value());
    EXPECT_EQ(step2.error(), DiagnosticCode::StackOverflow);
}

// ============================================================================
// Large value roundtrip (64-bit)
// ============================================================================

TEST(PushPopOram, LargeValueRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);
    uint64_t big = 0xDEADBEEFCAFEBABEull;
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA5,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{big, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, big);
}

// ============================================================================
// RollingKeyOram: LIFO order preserved under re-encryption
// ============================================================================

TEST(PushPopOram, RollingKey_LIFOOrder) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine<DebugPolicy, RollingKeyOram>(seed, 0xA6,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},   // r0 = 20
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},   // r0 = 0
         {VmOpcode::POP, rr(), 0, 0, 0},                 // r0 = 20
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{10, 0, 0}, {20, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u);
}
