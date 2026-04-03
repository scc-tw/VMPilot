/// @file test_oram_strategies.cpp
/// @brief Tests for ORAM strategy implementations: DirectOram, RollingKeyOram.
///
/// Extracted from test_engine_comprehensive.cpp into unit/ directory.

#include "test_blob_builder.hpp"

#include "oram_strategy.hpp"
#include "vm_state.hpp"

#include <vm/vm_context.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <vector>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ============================================================================
// ORAM: RollingKeyOram workspace changes on every write
// ============================================================================

TEST(EngineOramOblivious, WorkspaceChangesOnEveryAccess) {
    // Directly test RollingKeyOram: write same value twice, workspace
    // should differ due to rolling nonce
    VmOramState oram{};
    uint8_t key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    std::memcpy(oram.key, key, 16);

    // Snapshot workspace after first write
    (void)RollingKeyOram::access(oram, 0, 42, true);
    std::vector<uint8_t> snap1(oram.workspace, oram.workspace + VM_OBLIVIOUS_SIZE);

    // Write same value at same address — workspace must differ (rolling nonce)
    (void)RollingKeyOram::access(oram, 0, 42, true);
    std::vector<uint8_t> snap2(oram.workspace, oram.workspace + VM_OBLIVIOUS_SIZE);

    EXPECT_NE(snap1, snap2) << "Rolling keystream ORAM must produce different "
                               "workspace bytes on each access (IND-CPA)";

    // But read back should still give 42
    auto val = RollingKeyOram::access(oram, 0, 0, false);
    EXPECT_EQ(val, 42u);
}

// ============================================================================
// ORAM: nonce monotonicity
// ============================================================================

TEST(EngineOramOblivious, NonceMonotonicallyIncreases) {
    VmOramState oram{};
    uint8_t key[16] = {};
    std::memcpy(oram.key, key, 16);

    EXPECT_EQ(oram.nonce, 0u);
    (void)RollingKeyOram::access(oram, 0, 1, true);
    EXPECT_EQ(oram.nonce, 1u);
    (void)RollingKeyOram::access(oram, 0, 0, false);
    EXPECT_EQ(oram.nonce, 2u);
    (void)RollingKeyOram::access(oram, 8, 2, true);
    EXPECT_EQ(oram.nonce, 3u);
}

// ============================================================================
// ORAM: DirectOram basic read/write
// ============================================================================

TEST(EngineOramDirect, BasicAccessRoundtrip) {
    VmOramState oram{};

    // Write then read — DirectOram is a simple array, no encryption
    (void)DirectOram::access(oram, 0, 42, true);
    auto val = DirectOram::access(oram, 0, 0, false);
    EXPECT_EQ(val, 42u);

    // Multiple addresses
    (void)DirectOram::access(oram, 8, 100, true);
    (void)DirectOram::access(oram, 16, 200, true);
    EXPECT_EQ(DirectOram::access(oram, 8, 0, false), 100u);
    EXPECT_EQ(DirectOram::access(oram, 16, 0, false), 200u);
    // Original address unchanged
    EXPECT_EQ(DirectOram::access(oram, 0, 0, false), 42u);
}

// ############################################################################
// EngineOramEquivalence (from test_engine_comprehensive.cpp)
// ############################################################################

#include "vm_engine.hpp"
#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

static uint8_t pool_none_os() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t rr_os() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static uint8_t flags_none_os() { return 0; }

template<typename Policy = DebugPolicy, typename Oram = DirectOram>
auto single_bb_engine_os(const uint8_t seed[32], uint8_t epoch_base,
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

TEST(EngineOramEquivalence, DirectVsRollingProduceSameResult) {
    uint8_t seed[32]; fill_seed(seed);

    auto make = [&](auto oram_tag) {
        using O = std::decay_t<decltype(oram_tag)>;
        return single_bb_engine_os<DebugPolicy, O>(seed, 0xC0,
            {{VmOpcode::LOAD_CONST, pool_none_os(), 0, 0, 0},
             {VmOpcode::PUSH, rr_os(), 0, 0, 0},
             {VmOpcode::LOAD_CONST, pool_none_os(), 0, 0, 1},
             {VmOpcode::POP, rr_os(), 0, 0, 0},
             {VmOpcode::HALT, flags_none_os(), 0, 0, 0}},
            {{999, 0, 0}, {0, 0, 0}});
    };

    auto r1 = make(DirectOram{})->execute();
    auto r2 = make(RollingKeyOram{})->execute();
    ASSERT_TRUE(r1.has_value() && r2.has_value());
    EXPECT_EQ(r1->return_value, 999u);
    EXPECT_EQ(r2->return_value, 999u);
}
