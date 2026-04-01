/// @file test_error_paths.cpp
/// @brief Tests for error handling paths.
///
/// Tests: bad magic, BB MAC tamper, division by zero, stack underflow (RET_VM),
/// and blob integrity.

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_state.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_blob.hpp>

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

// ============================================================================
// Test 1: Bad magic rejected at create()
// ============================================================================

TEST(ErrorPaths, BadMagicRejected) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xAA);
    bb.instructions = {{VmOpcode::HALT, flags_none(), 0, 0, 0}};

    auto blob = build_test_blob(seed, {bb});
    // Corrupt magic bytes
    blob[0] = 0xFF;
    blob[1] = 0xFF;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_FALSE(engine.has_value());
    EXPECT_EQ(engine.error(), DiagnosticCode::BlobMagicMismatch);
}

// ============================================================================
// Test 2: BB MAC tamper detection
// ============================================================================

TEST(ErrorPaths, BBMacTamper) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);
    // Need at least 2 instructions so the MAC check sees the real content
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};
    auto blob = build_test_blob_ex(seed, {bb}, {p0});

    // Tamper with the MAC section
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t mac_off = blob_section_mac(hdr);
    // Flip a byte in the MAC
    blob[mac_off] ^= 0xFF;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() should succeed (MAC is checked at BB exit)";

    auto r = engine->execute();
    ASSERT_FALSE(r.has_value()) << "Execution should fail with MAC verification error";
    EXPECT_EQ(r.error(), DiagnosticCode::BBMacVerificationFailed);
}

// ============================================================================
// Test 3: Division by zero returns 0 (not an error)
// ============================================================================

TEST(ErrorPaths, DivByZeroReturnsZero) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xCC);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 42
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 0
        {VmOpcode::DIV, rr(), 0, 1, 0},                  // r0 = 42 / 0
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};
    TestPoolEntry p1{0, 0, 1};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value()) << "DIV by zero should not error, just return 0";
    EXPECT_EQ(r->return_value, 0u);
}

// ============================================================================
// Test 4: RET_VM without CALL_VM → StackUnderflow
// ============================================================================

TEST(ErrorPaths, RetWithoutCall) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xDD);
    bb.instructions = {
        {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), DiagnosticCode::StackUnderflow);
}

// ============================================================================
// Test 5: Truncated blob rejected at create()
// ============================================================================

TEST(ErrorPaths, TruncatedBlobRejected) {
    uint8_t seed[32]; fill_seed(seed);

    // Create a valid blob then truncate it
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xEE);
    bb.instructions = {{VmOpcode::HALT, flags_none(), 0, 0, 0}};

    auto blob = build_test_blob(seed, {bb});
    // Truncate to half size
    size_t half = blob.size() / 2;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), half, seed);
    ASSERT_FALSE(engine.has_value()) << "Truncated blob should be rejected";
}
