/// @file test_policy_matrix.cpp
/// @brief Cross-policy consistency and policy trait tests.
///
/// Combined from:
///   - test_policy_consistency.cpp: Debug vs Standard, Debug vs HighSec
///   - test_phase4_engine.cpp: CrossPolicyConsistency, ReentrantSharesImmutable
///   - test_engine_comprehensive.cpp: AllOpcodesCovered (handler table coverage)
///   - test_phase2_state.cpp: Policy traits, DefaultOramMapping, OramOrthogonality,
///                            VmEngine template instantiation

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ============================================================================
// Helpers
// ============================================================================

static uint8_t pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static uint8_t flags_pool_none(uint8_t cond = 0) {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4) | cond);
}
static uint8_t flags_none() { return 0; }

template<typename Policy>
tl::expected<VmExecResult, DiagnosticCode>
run_add_program(const uint8_t seed[32], uint64_t a, uint64_t b) {
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF5);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
        {VmOpcode::ADD, rr(), 0, 1, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{a, 0, 0};
    TestPoolEntry p1{b, 0, 1};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});
    auto engine = VmEngine<Policy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ============================================================================
// From test_policy_consistency.cpp: Debug vs Standard
// ============================================================================

TEST(PolicyConsistency, DebugVsStandard) {
    uint8_t seed[32]; fill_seed(seed);

    auto r_debug = run_add_program<DebugPolicy>(seed, 100, 200);
    auto r_standard = run_add_program<StandardPolicy>(seed, 100, 200);

    ASSERT_TRUE(r_debug.has_value()) << "Debug policy ADD should succeed";
    ASSERT_TRUE(r_standard.has_value()) << "Standard policy ADD should succeed";

    EXPECT_EQ(r_debug->return_value, 300u);
    EXPECT_EQ(r_standard->return_value, 300u);
    EXPECT_EQ(r_debug->return_value, r_standard->return_value)
        << "Debug and Standard policies should produce identical plaintext results";
}

// ============================================================================
// From test_policy_consistency.cpp: Debug vs HighSec
// ============================================================================

TEST(PolicyConsistency, DebugVsHighSec) {
    uint8_t seed[32]; fill_seed(seed);

    auto r_debug = run_add_program<DebugPolicy>(seed, 0xDEAD, 0xBEEF);
    auto r_highsec = run_add_program<HighSecPolicy>(seed, 0xDEAD, 0xBEEF);

    ASSERT_TRUE(r_debug.has_value()) << "Debug policy ADD should succeed";
    ASSERT_TRUE(r_highsec.has_value()) << "HighSec policy ADD should succeed";

    uint64_t expected = 0xDEAD + 0xBEEF;
    EXPECT_EQ(r_debug->return_value, expected);
    EXPECT_EQ(r_highsec->return_value, expected);
    EXPECT_EQ(r_debug->return_value, r_highsec->return_value)
        << "Debug and HighSec policies should produce identical plaintext results";
}

// ============================================================================
// From test_phase4_engine.cpp: CrossPolicyConsistency
// ============================================================================

TEST(Phase4Engine, CrossPolicyConsistency) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xEE);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags = flags_pool_none();
    lc.reg_a = 0; lc.reg_b = 0;
    lc.aux = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {lc, halt};

    TestPoolEntry pe{12345u, 0, 0};
    auto blob = build_test_blob_ex(seed, {bb}, {pe});

    // Execute with all 3 policies
    auto r1 = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed)->execute();
    auto r2 = VmEngine<StandardPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed)->execute();
    auto r3 = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed)->execute();

    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    ASSERT_TRUE(r3.has_value());

    // All must produce the same return value
    EXPECT_EQ(r1->return_value, 12345u);
    EXPECT_EQ(r2->return_value, 12345u);
    EXPECT_EQ(r3->return_value, 12345u);
}

// ============================================================================
// From test_phase4_engine.cpp: ReentrantSharesImmutable
// ============================================================================

TEST(Phase4Engine, ReentrantSharesImmutable) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xDD);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    auto engine = VmEngine<StandardPolicy>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto shared_imm = engine->shared_immutable();
    EXPECT_EQ(shared_imm.use_count(), 2);  // engine + our copy

    auto inner = VmEngine<StandardPolicy>::create_reentrant(shared_imm);
    ASSERT_TRUE(inner.has_value());

    EXPECT_EQ(shared_imm.use_count(), 3);  // engine + our copy + inner

    // Both should execute successfully
    auto r1 = engine->execute();
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(r1->status, VmResult::Halted);

    auto r2 = inner->execute();
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r2->status, VmResult::Halted);
}

// ============================================================================
// From test_engine_comprehensive.cpp: AllOpcodesCovered
// ============================================================================

TEST(EngineTable, AllOpcodesCovered) {
    auto t1 = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto t2 = build_handler_table<StandardPolicy, RollingKeyOram>();
    auto t3 = build_handler_table<DebugPolicy, DirectOram>();
    for (size_t i = 0; i < VM_OPCODE_COUNT; ++i) {
        EXPECT_NE(t1[i], nullptr) << "null at opcode " << i;
        EXPECT_NE(t2[i], nullptr) << "null at opcode " << i;
        EXPECT_NE(t3[i], nullptr) << "null at opcode " << i;
    }
}

// ============================================================================
// From test_phase2_state.cpp: Policy traits
// ============================================================================

TEST(Phase2Policy, HighSecTraits) {
    static_assert(HighSecPolicy::use_mba == true);
    static_assert(HighSecPolicy::constant_time == true);
    static_assert(HighSecPolicy::fusion_granularity == 4);
    static_assert(HighSecPolicy::validate());
    SUCCEED();
}

TEST(Phase2Policy, StandardTraits) {
    static_assert(StandardPolicy::use_mba == true);
    static_assert(StandardPolicy::constant_time == true);
    static_assert(StandardPolicy::fusion_granularity == 2);
    static_assert(StandardPolicy::validate());
    SUCCEED();
}

TEST(Phase2Policy, DebugTraits) {
    static_assert(DebugPolicy::use_mba == false);
    static_assert(DebugPolicy::constant_time == false);
    static_assert(DebugPolicy::fusion_granularity == 1);
    static_assert(DebugPolicy::validate());
    SUCCEED();
}

TEST(Phase2Policy, DefaultOramMapping) {
    // HighSec and Standard default to RollingKeyOram
    static_assert(std::is_same_v<DefaultOramFor<HighSecPolicy>::type, RollingKeyOram>);
    static_assert(std::is_same_v<DefaultOramFor<StandardPolicy>::type, RollingKeyOram>);

    // Debug defaults to DirectOram
    static_assert(std::is_same_v<DefaultOramFor<DebugPolicy>::type, DirectOram>);
    SUCCEED();
}

TEST(Phase2Policy, OramOrthogonality) {
    // Can combine any policy with any ORAM strategy
    // (these should compile — the static_assert in VmEngine validates the policy)
    using TestVM  = VmEngine<HighSecPolicy, DirectOram>;
    using FastVM  = VmEngine<DebugPolicy, RollingKeyOram>;

    // Just verify they're distinct types (no ODR issues)
    static_assert(!std::is_same_v<TestVM, FastVM>);
    SUCCEED();
}

// ============================================================================
// From test_phase2_state.cpp: VmEngine template instantiation
// ============================================================================

TEST(Phase2Engine, InstantiatesAllPolicies) {
    // Verify VmEngine compiles with all policy + ORAM combinations
    // (type-level test — if this compiles, the templates are valid)
    using E1 = VmEngine<HighSecPolicy>;
    using E2 = VmEngine<StandardPolicy>;
    using E3 = VmEngine<DebugPolicy>;
    using E4 = VmEngine<HighSecPolicy, DirectOram>;
    using E5 = VmEngine<DebugPolicy, RollingKeyOram>;

    // Different types (no accidental merging)
    static_assert(!std::is_same_v<E1, E2>);
    static_assert(!std::is_same_v<E2, E3>);
    static_assert(!std::is_same_v<E1, E4>);
    static_assert(!std::is_same_v<E3, E5>);

    SUCCEED();
}

// ============================================================================
// EngineArithmetic: policy-specific arithmetic (from test_engine_comprehensive.cpp)
// ============================================================================

#include "handler_impls.hpp"

template<typename Policy = DebugPolicy, typename Oram = DirectOram>
auto single_bb_engine_pm(const uint8_t seed[32], uint8_t epoch_base,
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

TEST(EngineArithmetic, AddHighSecPolicy) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine_pm<HighSecPolicy>(seed, 0xF0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::ADD, rr(), 0, 1, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{30, 0, 0}, {12, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(EngineArithmetic, AddStandardPolicy) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine_pm<StandardPolicy>(seed, 0xF1,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::ADD, rr(), 0, 1, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{30, 0, 0}, {12, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(EngineArithmetic, SubAllPoliciesConsistent) {
    uint8_t seed[32]; fill_seed(seed);

    auto make_sub = [&](auto tag) {
        using P = std::decay_t<decltype(tag)>;
        return single_bb_engine_pm<P, DirectOram>(seed, 0xF2,
            {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
             {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
             {VmOpcode::SUB, rr(), 0, 1, 0},
             {VmOpcode::HALT, flags_none(), 0, 0, 0}},
            {{100, 0, 0}, {58, 0, 1}});
    };

    auto r1 = make_sub(HighSecPolicy{})->execute();
    auto r2 = make_sub(StandardPolicy{})->execute();
    auto r3 = make_sub(DebugPolicy{})->execute();

    ASSERT_TRUE(r1.has_value() && r2.has_value() && r3.has_value());
    EXPECT_EQ(r1->return_value, 42u);
    EXPECT_EQ(r2->return_value, 42u);
    EXPECT_EQ(r3->return_value, 42u);
}

// ============================================================================
// EngineCrossPolicy (from test_engine_comprehensive.cpp)
// ============================================================================

TEST(EngineCrossPolicy, SameResultAllPolicies) {
    uint8_t seed[32]; fill_seed(seed);

    auto make = [&](auto tag) {
        using P = std::decay_t<decltype(tag)>;
        return single_bb_engine_pm<P, DirectOram>(seed, 0xF5,
            {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
             {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
             {VmOpcode::XOR, rr(), 0, 1, 0},
             {VmOpcode::HALT, flags_none(), 0, 0, 0}},
            {{0xAAAAAAAAAAAAAAAAull, 0, 0}, {0x5555555555555555ull, 0, 1}});
    };

    auto r1 = make(HighSecPolicy{})->execute();
    auto r2 = make(StandardPolicy{})->execute();
    auto r3 = make(DebugPolicy{})->execute();

    ASSERT_TRUE(r1.has_value() && r2.has_value() && r3.has_value());
    EXPECT_EQ(r1->return_value, 0xFFFFFFFFFFFFFFFFull);
    EXPECT_EQ(r2->return_value, r1->return_value);
    EXPECT_EQ(r3->return_value, r1->return_value);
}

// ============================================================================
// Phase4Engine tests (from test_phase4_engine.cpp)
// ============================================================================

static uint8_t flags_rr(uint8_t cond = 0) {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4) | cond);
}

TEST(Phase4Engine, HaltOnly) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() failed";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value()) << "execute() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
}

TEST(Phase4Engine, LoadConstAndHalt) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags = flags_pool_none();
    lc.reg_a = 0; lc.reg_b = 0;
    lc.aux = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {lc, halt};

    TestPoolEntry pool_entry{};
    pool_entry.plaintext = 42;
    pool_entry.target_bb_index = 0;
    pool_entry.target_reg = 0;

    auto blob = build_test_blob_ex(seed, {bb}, {pool_entry});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() failed";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value()) << "execute() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u);
}

TEST(Phase4Engine, XorComputation) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xCC);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags = flags_pool_none();
    lc0.reg_a = 0; lc0.reg_b = 0;
    lc0.aux = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags = flags_pool_none();
    lc1.reg_a = 1; lc1.reg_b = 0;
    lc1.aux = 1;

    TestInstruction xor_insn{};
    xor_insn.opcode = VmOpcode::XOR;
    xor_insn.flags = flags_rr();
    xor_insn.reg_a = 0; xor_insn.reg_b = 1;
    xor_insn.aux = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();

    bb.instructions = {lc0, lc1, xor_insn, halt};

    TestPoolEntry p0{0xFF00FF00FF00FF00ull, 0, 0};
    TestPoolEntry p1{0x00FF00FF00FF00FFull, 0, 1};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() failed";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value()) << "execute() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 0xFFFFFFFFFFFFFFFFull);
}
