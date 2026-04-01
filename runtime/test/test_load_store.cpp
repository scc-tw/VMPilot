/// @file test_load_store.cpp
/// @brief Tests for guest memory LOAD/STORE instructions.
///
/// LOAD reads 8 bytes from guest memory at (aux + load_base_delta) and
/// converts from raw memory to register domain via load_convert.
/// STORE converts register to memory domain and writes 8 bytes.

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

// ============================================================================
// Test 1: STORE then LOAD roundtrip
// ============================================================================

TEST(LoadStore, StoreLoadRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0;

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA0);

    // r0 = 42, STORE r0 → guest_mem, LOAD_CONST r0=0, LOAD r0 ← guest_mem, HALT
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 42
        {VmOpcode::STORE, rr(), 0, 0, 0},               // STORE r0 at aux=0
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 0 (clear)
        {VmOpcode::LOAD, rr(), 0, 0, 0},                // LOAD r0 from aux=0
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};
    TestPoolEntry p1{0, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "STORE then LOAD should roundtrip";
}

// ============================================================================
// Test 2: Multiple addresses (store at different offsets)
// ============================================================================

TEST(LoadStore, MultipleAddresses) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem[2] = {0, 0};

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA1);

    // Store 10 at offset 0, store 20 at offset 8, load from offset 0
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::STORE, rr(), 0, 0, 0},               // STORE at offset 0
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::STORE, rr(), 0, 0, 8},               // STORE at offset 8
        {VmOpcode::LOAD, rr(), 0, 0, 0},                // LOAD from offset 0 → r0=10
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem[0])));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "LOAD from offset 0 should read first stored value";
    EXPECT_NE(guest_mem[1], 0u) << "STORE at offset 8 should have written encoded(20)";
}

// ============================================================================
// Test 3: Load from second offset
// ============================================================================

TEST(LoadStore, LoadFromSecondOffset) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem[2] = {0, 0};

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA2);

    // Store 10 at offset 0, store 20 at offset 8, load from offset 8
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::STORE, rr(), 0, 0, 0},               // STORE at offset 0
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::STORE, rr(), 0, 0, 8},               // STORE at offset 8
        {VmOpcode::LOAD, rr(), 0, 0, 8},                // LOAD from offset 8 → r0=20
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem[0])));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "LOAD from offset 8 should read second value";
}

// ============================================================================
// Test 4: Large value roundtrip
// ============================================================================

TEST(LoadStore, LargeValueRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0;
    uint64_t big = 0xDEADBEEFCAFEBABEull;

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA3);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::STORE, rr(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
        {VmOpcode::LOAD, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{big, 0, 0};
    TestPoolEntry p1{0, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, big);
}

// ============================================================================
// Test 5: STORE overwrites previous value
// ============================================================================

TEST(LoadStore, StoreOverwrite) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0;

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA4);

    // Store 10, then overwrite with 20, then load (should get 20)
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::STORE, rr(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::STORE, rr(), 0, 0, 0},               // overwrite
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 0
        {VmOpcode::LOAD, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 0};
    TestPoolEntry p2{0, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1, p2});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "Second STORE should overwrite first";
}

// ============================================================================
// Test 6: Zero value roundtrip
// ============================================================================

TEST(LoadStore, ZeroValueRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0xFFFFFFFFFFFFFFFFull;  // non-zero initial

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA5);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 0
        {VmOpcode::STORE, rr(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 99 (clear)
        {VmOpcode::LOAD, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{0, 0, 0};
    TestPoolEntry p1{99, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "Zero should roundtrip through STORE/LOAD";
}
