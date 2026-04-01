/// @file test_native_call_variants.cpp
/// @brief Tests for NATIVE_CALL edge cases.
///
/// Tests: 0-arg call, 8-arg call, variadic-style (printf pattern),
/// and negative return value.

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

static uint8_t flags_none() { return 0; }

// ============================================================================
// Native functions used by tests
// ============================================================================

/// 0-arg: always returns 42
static uint64_t native_zero_args(uint64_t, uint64_t, uint64_t, uint64_t,
                                  uint64_t, uint64_t, uint64_t, uint64_t) {
    return 42;
}

/// 8-arg: sum of all 8 arguments
static uint64_t native_sum_all(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                                uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7) {
    return a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7;
}

/// Returns -1 (0xFFFFFFFFFFFFFFFF) to test negative return encoding
static uint64_t native_negative_return(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t) {
    return static_cast<uint64_t>(-1LL);
}

/// Returns first arg * 2 (like a simple transform)
static uint64_t native_double(uint64_t a, uint64_t, uint64_t, uint64_t,
                               uint64_t, uint64_t, uint64_t, uint64_t) {
    return a * 2;
}

// ============================================================================
// Test 1: 0-arg native call
// ============================================================================

TEST(NativeCallVariants, ZeroArgCall) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF0);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count = 0;
    tc.target_addr = reinterpret_cast<uint64_t>(&native_zero_args);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "0-arg native call should return 42";
}

// ============================================================================
// Test 2: 8-arg native call (sum of all registers)
// ============================================================================

TEST(NativeCallVariants, EightArgCall) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF1);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count = 8;
    tc.target_addr = reinterpret_cast<uint64_t>(&native_sum_all);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    // Set initial regs r0..r7 = 1,2,3,4,5,6,7,8
    uint64_t regs[16] = {};
    for (int i = 0; i < 8; ++i)
        regs[i] = static_cast<uint64_t>(i + 1);

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, 0, regs, 8);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // 1+2+3+4+5+6+7+8 = 36
    EXPECT_EQ(r->return_value, 36u) << "8-arg native call should sum all args";
}

// ============================================================================
// Test 3: Variadic-style call (only first arg meaningful)
// ============================================================================

TEST(NativeCallVariants, SingleArgTransform) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF2);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count = 1;
    tc.target_addr = reinterpret_cast<uint64_t>(&native_double);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    uint64_t regs[16] = {};
    regs[0] = 25;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, 0, regs, 1);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 50u) << "native_double(25) should return 50";
}

// ============================================================================
// Test 4: Negative return value
// ============================================================================

TEST(NativeCallVariants, NegativeReturnValue) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF3);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count = 0;
    tc.target_addr = reinterpret_cast<uint64_t>(&native_negative_return);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull)
        << "Negative return should be preserved as uint64_t";
}
