/// @file test_tls_intrinsic.cpp
/// @brief Integration test: TLS intrinsic resolution via NATIVE_CALL.
///
/// Verifies that a blob containing an intrinsic sentinel target_offset
/// (e.g. TLS_READ64) is correctly resolved at engine creation, and the
/// NATIVE_CALL handler dispatches to vmpilot_tls_read64.

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_intrinsics.hpp"

#include <tls_helpers.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_blob.hpp>

#include <gtest/gtest.h>

#include <cstdint>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

static uint8_t flags_none() { return 0; }

// ============================================================================
// Test: TLS_READ64 intrinsic through NATIVE_CALL
// ============================================================================

TEST(TlsIntrinsic, Read64ViaIntrinsic) {
    uint8_t seed[32];
    fill_seed(seed);

    // Build a single-BB program: NATIVE_CALL(aux=0) → HALT
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA0);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},  // aux=0 → first transition entry
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    // Transition entry: intrinsic sentinel for TLS_READ64, 1 arg
    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = te_pack_arg_count(1, 0, false, false, false);
    tc.target_addr  = intrinsic_target(IntrinsicId::TLS_READ64);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    // r0 = TLS offset.  On most platforms, offset 0 of the thread pointer
    // contains a self-pointer or well-known value.  We read it directly
    // for the expected result.
    uint64_t tls_offset = 0;
    uint64_t expected = vmpilot_tls_read64(tls_offset);

    uint64_t initial_regs[16] = {};
    initial_regs[0] = tls_offset;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, 0, initial_regs, 1);
    ASSERT_TRUE(engine.has_value()) << "Engine creation should succeed";

    auto r = engine->execute();
    ASSERT_TRUE(r.has_value()) << "Execution should succeed";
    EXPECT_EQ(r->return_value, expected)
        << "TLS_READ64 intrinsic should return the same value as vmpilot_tls_read64(0)";
}

// ============================================================================
// Test: TLS_READ32 intrinsic through NATIVE_CALL
// ============================================================================

TEST(TlsIntrinsic, Read32ViaIntrinsic) {
    uint8_t seed[32];
    fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA1);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = te_pack_arg_count(1, 0, false, false, false);
    tc.target_addr  = intrinsic_target(IntrinsicId::TLS_READ32);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    uint64_t tls_offset = 0;
    uint64_t expected = vmpilot_tls_read32(tls_offset);

    uint64_t initial_regs[16] = {};
    initial_regs[0] = tls_offset;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, 0, initial_regs, 1);
    ASSERT_TRUE(engine.has_value());

    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, expected)
        << "TLS_READ32 intrinsic should return the same value as vmpilot_tls_read32(0)";
}

// ============================================================================
// Test: intrinsic resolution with non-zero load_base_delta
// ============================================================================

TEST(TlsIntrinsic, ResolutionWithNonZeroDelta) {
    uint8_t seed[32];
    fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA2);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = te_pack_arg_count(1, 0, false, false, false);
    tc.target_addr  = intrinsic_target(IntrinsicId::TLS_READ64);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    uint64_t tls_offset = 0;
    uint64_t expected = vmpilot_tls_read64(tls_offset);

    uint64_t initial_regs[16] = {};
    initial_regs[0] = tls_offset;

    // Non-zero delta: intrinsic resolution must compensate correctly
    int64_t delta = 0x1000;
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, delta, initial_regs, 1);
    ASSERT_TRUE(engine.has_value())
        << "Engine creation should succeed with non-zero load_base_delta";

    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, expected)
        << "Intrinsic should produce correct result regardless of load_base_delta";
}

// ============================================================================
// Test: sentinel outside intrinsic range is NOT resolved as intrinsic
// ============================================================================

TEST(TlsIntrinsic, OutOfRangeSentinelNotResolvedAsIntrinsic) {
    // INTRINSIC_BASE + COUNT falls outside is_intrinsic_target() range,
    // so it's treated as a regular (non-intrinsic) native call target.
    // Creation succeeds, but execution fails because the address is garbage.
    uint8_t seed[32];
    fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA3);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = te_pack_arg_count(1, 0, false, false, false);
    tc.target_addr  = INTRINSIC_BASE + static_cast<uint8_t>(IntrinsicId::COUNT);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    // Creation succeeds — the out-of-range sentinel is not recognized
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    EXPECT_TRUE(engine.has_value())
        << "Out-of-range sentinel should not be treated as intrinsic; "
           "creation succeeds (it looks like a regular native call target)";
}
