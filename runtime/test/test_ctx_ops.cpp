/// @file test_ctx_ops.cpp
/// @brief Tests for LOAD_CTX and STORE_CTX opcodes.
///
/// LOAD_CTX reads VM context fields (vm_ip, vm_sp, bb_id, epoch) into a register.
/// STORE_CTX writes a register value to a VM context field (currently only vm_sp).

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
static uint8_t reg_none() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_NONE << 4));
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
// Test 1: LOAD_CTX vm_sp (aux=1)
// vm_sp starts at VM_OBLIVIOUS_SIZE (4096). After LOAD_CTX, r0 should be 4096.
// ============================================================================

TEST(CtxOps, LoadCtxSp) {
    uint8_t seed[32]; fill_seed(seed);

    // LOAD_CTX r0, vm_sp (aux=1), HALT
    auto engine = single_bb_engine(seed, 0xA0,
        {{VmOpcode::LOAD_CTX, flags_none(), 0, 0, 1},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, VM_OBLIVIOUS_SIZE)
        << "LOAD_CTX vm_sp should return initial stack pointer (4096)";
}

// ============================================================================
// Test 2: LOAD_CTX vm_ip (aux=0)
// vm_ip at the LOAD_CTX instruction depends on where it is in the program.
// It's the first instruction (ip=0).
// ============================================================================

TEST(CtxOps, LoadCtxIp) {
    uint8_t seed[32]; fill_seed(seed);

    // LOAD_CTX r0, vm_ip (aux=0), HALT
    auto engine = single_bb_engine(seed, 0xA1,
        {{VmOpcode::LOAD_CTX, flags_none(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // vm_ip at the first instruction should be 0 (entry_ip of first BB)
    EXPECT_EQ(r->return_value, 0u)
        << "LOAD_CTX vm_ip at first instruction should be 0";
}

// ============================================================================
// Test 3: STORE_CTX + LOAD_CTX roundtrip on vm_sp
// Set vm_sp to 2048 via STORE_CTX, then read it back via LOAD_CTX.
// ============================================================================

TEST(CtxOps, StoreCtxLoadCtxRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);

    // LOAD_CONST r0=2048, STORE_CTX r0 → vm_sp, LOAD_CTX r0 → vm_sp, HALT
    auto engine = single_bb_engine(seed, 0xA2,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 2048
         {VmOpcode::STORE_CTX, reg_none(), 0, 0, 1},     // vm_sp = r0 (operand A = REG)
         {VmOpcode::LOAD_CTX, flags_none(), 0, 0, 1},    // r0 = vm_sp
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{2048, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 2048u)
        << "STORE_CTX then LOAD_CTX should roundtrip vm_sp = 2048";
}
