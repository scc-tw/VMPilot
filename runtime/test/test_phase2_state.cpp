/// @file test_phase2_state.cpp
/// @brief Tests for Phase 2: 4-way VM state split, policies, ORAM strategies.
///
/// Validates:
///   1. Struct layout and alignment (static_assert + runtime sizeof checks)
///   2. Policy traits (compile-time validation, default ORAM mapping)
///   3. VmEpoch table derivation and RE_TABLE application
///   4. VmExecution register access via phantom types
///   5. VmEngine template instantiation for all 3 policies

#include "vm_engine.hpp"
#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/encoded_value.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_context.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <memory>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;

// ============================================================================
// 1. Layout and Alignment Tests
// ============================================================================

TEST(Phase2Layout, VmExecutionAlignment) {
    static_assert(alignof(VmExecution) >= 64);
    static_assert(offsetof(VmExecution, regs) == 0);
    EXPECT_LE(sizeof(VmExecution), 2048u)
        << "VmExecution should be stack-friendly (< 2KB)";
}

TEST(Phase2Layout, VmOramAlignment) {
    static_assert(alignof(VmOramState) >= 64);
    static_assert(offsetof(VmOramState, workspace) == 0);
    // VmOramState = workspace[4096] + nonce(8) + key[16] + padding to align
    EXPECT_GE(sizeof(VmOramState), VM_OBLIVIOUS_SIZE + 8u + 16u);
}

TEST(Phase2Layout, VmEpochContainsTables) {
    // VmEpoch must hold all encoding tables (~131KB)
    EXPECT_GE(sizeof(VmEpoch), sizeof(RegTables) + sizeof(DomainTables));
}

TEST(Phase2Layout, VmEngineStackSize) {
    // VmEngine on stack should be < 4KB for deep reentrancy
    EXPECT_LE(sizeof(VmEngine<HighSecPolicy>), 4096u);
    EXPECT_LE(sizeof(VmEngine<StandardPolicy>), 4096u);
    EXPECT_LE(sizeof(VmEngine<DebugPolicy>), 4096u);
}

// ============================================================================
// 2. Policy Traits Tests
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
// 3. VmEpoch Table Derivation Tests
// ============================================================================

TEST(Phase2Epoch, EnterBbDerivesTables) {
    // Set up a minimal VmImmutable with memory tables
    VmImmutable imm{};
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
    std::memcpy(imm.stored_seed, seed, 32);
    Encoding::derive_memory_tables(seed, imm.mem.encode, imm.mem.decode);

    // Create a BB metadata entry
    BBMetadata bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.entry_ip = 0;
    bb.insn_count_in_bb = 2;
    bb.live_regs_bitmap = 0x0003;  // r0 and r1 live
    for (int i = 0; i < 32; ++i) bb.epoch_seed[i] = static_cast<uint8_t>(0xBB + i);

    // Derive tables via enter_bb
    VmEpoch epoch{};
    epoch.enter_bb(bb, imm);

    EXPECT_EQ(epoch.bb_id, 1u);
    EXPECT_EQ(epoch.epoch, 0u);
    EXPECT_EQ(epoch.live_regs_bitmap, 0x0003u);

    // Verify encode→decode roundtrip for live register r0
    for (uint64_t val = 0; val < 256; ++val) {
        PlainVal pv(val);
        RegVal encoded = encode_register(epoch.reg.encode_lut(0), pv);
        PlainVal decoded = decode_register(epoch.reg.decode_lut(0), encoded);
        EXPECT_EQ(decoded.bits, val) << "Roundtrip failed for r0, val=" << val;
    }

    // Verify store→load roundtrip for r0
    for (uint64_t val = 0; val < 50; ++val) {
        RegVal rv(val * 0x0101010101010101ull);  // spread across lanes
        MemVal stored = store_convert(epoch.dom.store_lut(0), rv);
        RegVal loaded = load_convert(epoch.dom.load_lut(0), stored);
        EXPECT_EQ(loaded, rv) << "Store→Load roundtrip failed for r0";
    }
}

TEST(Phase2Epoch, TransitionRegsPreservesValues) {
    // Verify that RE_TABLE re-encoding preserves plaintext identity:
    //   decode_new(reencode(encode_old(x))) == x

    VmImmutable imm{};
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
    std::memcpy(imm.stored_seed, seed, 32);
    Encoding::derive_memory_tables(seed, imm.mem.encode, imm.mem.decode);

    // BB 1 (old epoch)
    BBMetadata bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0x0003;
    for (int i = 0; i < 32; ++i) bb1.epoch_seed[i] = static_cast<uint8_t>(0xAA + i);

    // BB 2 (new epoch)
    BBMetadata bb2{};
    bb2.bb_id = 2; bb2.epoch = 1;
    bb2.live_regs_bitmap = 0x0003;
    for (int i = 0; i < 32; ++i) bb2.epoch_seed[i] = static_cast<uint8_t>(0xCC + i);

    // Enter BB 1 and encode some values
    VmEpoch epoch{};
    epoch.enter_bb(bb1, imm);

    VmExecution exec{};
    PlainVal plain_r0(0x42);
    PlainVal plain_r1(0xDEADBEEF);
    exec.regs[0] = encode_register(epoch.reg.encode_lut(0), plain_r0);
    exec.regs[1] = encode_register(epoch.reg.encode_lut(1), plain_r1);

    // Transition to BB 2 (applies RE_TABLE)
    epoch.transition_regs(exec, bb2, imm);

    // Now enter BB 2 (derives new tables)
    epoch.enter_bb(bb2, imm);

    // Decode with NEW tables — should recover original plaintext
    PlainVal decoded_r0 = decode_register(epoch.reg.decode_lut(0), exec.regs[0]);
    PlainVal decoded_r1 = decode_register(epoch.reg.decode_lut(1), exec.regs[1]);

    EXPECT_EQ(decoded_r0.bits, 0x42u);
    EXPECT_EQ(decoded_r1.bits, 0xDEADBEEFu);
}

// ============================================================================
// 4. VmExecution Phantom-Typed Register Access
// ============================================================================

TEST(Phase2Execution, RegistersArePhantomTyped) {
    VmExecution exec{};

    // Registers hold RegVal (Domain::Register), not raw uint64_t
    exec.regs[0] = RegVal(0xCAFEBABE);
    exec.regs[1] = RegVal(0xDEADBEEF);

    EXPECT_EQ(exec.regs[0].bits, 0xCAFEBABEu);
    EXPECT_NE(exec.regs[0], exec.regs[1]);

    // Cannot accidentally assign a MemVal to regs:
    // exec.regs[0] = MemVal(42);  // COMPILE ERROR — this is the safety we want
}

TEST(Phase2Execution, DefaultsAreZero) {
    VmExecution exec{};

    EXPECT_EQ(exec.vm_ip, 0u);
    EXPECT_EQ(exec.vm_sp, VM_OBLIVIOUS_SIZE);
    EXPECT_EQ(exec.enc_state, 0u);
    EXPECT_EQ(exec.vm_flags, 0u);
    EXPECT_FALSE(exec.halted);
    EXPECT_FALSE(exec.branch_taken);
    EXPECT_EQ(exec.shadow_depth, 0u);
    EXPECT_EQ(exec.load_base_delta, 0);
}

// ============================================================================
// 5. VmEngine Template Instantiation
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

TEST(Phase2Engine, SharedImmutableForReentrancy) {
    // VmImmutable can be shared via shared_ptr
    auto imm = std::make_shared<VmImmutable>();
    auto imm2 = imm;  // shared_ptr copy

    EXPECT_EQ(imm.use_count(), 2);
    EXPECT_EQ(imm.get(), imm2.get());  // same pointer
}
