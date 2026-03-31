/// @file test_rekey.cpp
/// @brief Tests for REKEY opcode (D14§5.4).
///
/// Validates that REKEY mixes BLAKE3-derived entropy into enc_state
/// mid-BB, and that verify_bb_mac() correctly replays the transform.

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <random>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(base + i);
}

static uint8_t flags_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}

static VmSecurityConfig test_config() {
    VmSecurityConfig cfg{};
    cfg.debug_mode = true;
    return cfg;
}

// ---------------------------------------------------------------------------
// Test 1: Instructions after REKEY still execute correctly.
//
// Why this matters: REKEY modifies enc_state mid-BB.  If verify_bb_mac()
// and the encryption pipeline don't replay REKEY, the keystream for
// subsequent instructions will be wrong → decryption fails → MAC fails
// → execute() returns an error.
//
// This is the most critical REKEY test: it proves the encryption chain
// stays in sync across REKEY boundaries.
// ---------------------------------------------------------------------------

TEST(Rekey, ChainContinuesAfterRekey) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xDD);

    TestInstruction rekey{};
    rekey.opcode = VmOpcode::REKEY;
    rekey.flags  = 0;
    rekey.aux    = 42;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = flags_rr();
    add.reg_a  = 0; add.reg_b = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {rekey, add, halt};

    auto blob = build_test_blob(seed, {bb});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 100);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 200);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());

    uint64_t actual = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(actual, 300u);
}

// ---------------------------------------------------------------------------
// Test 2: Multiple REKEYs with different counters in one BB.
// ---------------------------------------------------------------------------

TEST(Rekey, MultipleRekeysInOneBB) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xEE);

    TestInstruction rk1{};
    rk1.opcode = VmOpcode::REKEY; rk1.aux = 1;

    TestInstruction rk2{};
    rk2.opcode = VmOpcode::REKEY; rk2.aux = 2;

    TestInstruction rk3{};
    rk3.opcode = VmOpcode::REKEY; rk3.aux = 3;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = flags_rr();
    add.reg_a  = 0; add.reg_b = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {rk1, rk2, rk3, add, halt};

    auto blob = build_test_blob(seed, {bb});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 7);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 8);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value())
        << "execute with 3 REKEYs failed: "
        << static_cast<uint32_t>(result.error());

    EXPECT_EQ(decode_register(ctx, 0, ctx.encoded_regs[0]), 15u);
}

// ---------------------------------------------------------------------------
// Test 3: REKEY is deterministic — same counter → same enc_state change.
// ---------------------------------------------------------------------------

TEST(Rekey, Deterministic) {
    uint8_t seed[32]; fill_seed(seed);

    // Two identical blobs, both with REKEY(42) + HALT
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xFF);

    TestInstruction rk{};
    rk.opcode = VmOpcode::REKEY; rk.aux = 42;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {rk, halt};

    auto blob = build_test_blob(seed, {bb});

    auto loaded1 = load_blob(blob.data(), blob.size(), seed, test_config());
    auto loaded2 = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded1.has_value() && loaded2.has_value());

    auto r1 = execute(loaded1->ctx, test_config());
    auto r2 = execute(loaded2->ctx, test_config());
    ASSERT_TRUE(r1.has_value() && r2.has_value());

    // Both should reach the same final enc_state
    EXPECT_EQ(loaded1->ctx.enc_state, loaded2->ctx.enc_state);
}

// ---------------------------------------------------------------------------
// Test 4: Different counters → different enc_state.
// ---------------------------------------------------------------------------

TEST(Rekey, DifferentCountersDiverge) {
    uint8_t seed[32]; fill_seed(seed);

    auto make_blob = [&](uint32_t counter) {
        TestBB bb{};
        bb.bb_id = 1; bb.epoch = 0;
        bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
        fill_epoch(bb.epoch_seed, 0xAA);

        TestInstruction rk{};
        rk.opcode = VmOpcode::REKEY; rk.aux = counter;

        TestInstruction halt{};
        halt.opcode = VmOpcode::HALT;

        bb.instructions = {rk, halt};
        return build_test_blob(seed, {bb});
    };

    auto blob1 = make_blob(1);
    auto blob2 = make_blob(2);

    auto loaded1 = load_blob(blob1.data(), blob1.size(), seed, test_config());
    auto loaded2 = load_blob(blob2.data(), blob2.size(), seed, test_config());
    ASSERT_TRUE(loaded1.has_value() && loaded2.has_value());

    auto r1 = execute(loaded1->ctx, test_config());
    auto r2 = execute(loaded2->ctx, test_config());
    ASSERT_TRUE(r1.has_value() && r2.has_value());

    EXPECT_NE(loaded1->ctx.enc_state, loaded2->ctx.enc_state)
        << "Different rekey counters should produce different enc_state";
}

// ---------------------------------------------------------------------------
// Test 5: REKEY + MBA ADD correctness (stress test).
//
// REKEY before an MBA ADD — verifies the full pipeline works under
// modified enc_state with the most complex encoded operation.
// ---------------------------------------------------------------------------

TEST(Rekey, WithMbaAdd500Random) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);

    TestInstruction rk{};
    rk.opcode = VmOpcode::REKEY; rk.aux = 99;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = flags_rr();
    add.reg_a  = 0; add.reg_b = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {rk, add, halt};

    auto blob = build_test_blob(seed, {bb});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& vm = *loaded;
    std::mt19937_64 rng(123);

    for (int i = 0; i < 500; ++i) {
        uint64_t a = rng();
        uint64_t b = rng();

        // Reset for re-execution
        auto re = enter_basic_block(vm.ctx, vm.ctx.bb_metadata[0].bb_id);
        (void)re;
        vm.ctx.halted = false;
        vm.ctx.branch_taken = false;
        vm.ctx.vm_flags = 0;

        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, a);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, b);

        auto result = execute(vm.ctx, test_config());
        ASSERT_TRUE(result.has_value())
            << "Failed at iteration " << i;

        uint64_t actual = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);
        EXPECT_EQ(actual, a + b)
            << "REKEY+ADD mismatch at i=" << i;
    }
}
