/// @file test_phase2_state.cpp
/// @brief Tests for Phase 2: 4-way VM state split, policies, ORAM strategies.
///
/// Validates:
///   1. Struct layout and alignment (static_assert + runtime sizeof checks)
///   2. Policy traits (compile-time validation, default ORAM mapping)
///   3. VmEpoch opcode permutation derivation (doc 16 — no RegTables/DomainTables)
///   4. VmExecution register access via phantom types
///   5. Speck-FPE encode/decode roundtrip and per-register independence
///   6. VmEngine template instantiation for all 3 policies

#include "vm_engine.hpp"
#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/encoded_value.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_context.hpp>
#include <vm/xex_speck64.hpp>

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
    // Doc 16 added insn_fpe_key[16] + bb_chain_state[32], growing to ~2112 bytes.
    EXPECT_LE(sizeof(VmExecution), 2176u)
        << "VmExecution should be stack-friendly (< ~2KB)";
}

TEST(Phase2Layout, VmOramAlignment) {
    static_assert(alignof(VmOramState) >= 64);
    static_assert(offsetof(VmOramState, workspace) == 0);
    // VmOramState = workspace[4096] + nonce(8) + key[16] + padding to align
    EXPECT_GE(sizeof(VmOramState), VM_OBLIVIOUS_SIZE + 8u + 16u);
}

TEST(Phase2Layout, VmEpochContainsTables) {
    // Doc 16: VmEpoch holds only opcode_perm[256] + opcode_perm_inv[256]
    // + 3 scalar fields (bb_id, epoch, live_regs_bitmap) ≈ 524 bytes.
    // RegTables and DomainTables are gone — register encoding is Speck-FPE.
    EXPECT_GE(sizeof(VmEpoch), 512u);
    EXPECT_LT(sizeof(VmEpoch), 1024u);  // must be small, not ~131KB
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
    // Doc 16: enter_bb(bb) takes a single arg — no VmImmutable needed.
    // It sets identity fields and derives the D4 opcode permutation.

    BBMetadata bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.entry_ip = 0;
    bb.insn_count_in_bb = 2;
    bb.live_regs_bitmap = 0x0003;  // r0 and r1 live
    for (int i = 0; i < 32; ++i) bb.epoch_seed[i] = static_cast<uint8_t>(0xBB + i);

    VmEpoch epoch{};
    epoch.enter_bb(bb);

    // Verify identity fields are set correctly
    EXPECT_EQ(epoch.bb_id, 1u);
    EXPECT_EQ(epoch.epoch, 0u);
    EXPECT_EQ(epoch.live_regs_bitmap, 0x0003u);

    // Verify opcode_perm is derived (not all zeros)
    bool all_zero = true;
    for (int i = 0; i < 256; ++i) {
        if (epoch.opcode_perm[i] != 0) { all_zero = false; break; }
    }
    EXPECT_FALSE(all_zero) << "opcode_perm should not be all zeros after enter_bb";

    // Verify opcode_perm is not the identity permutation
    bool is_identity = true;
    for (int i = 0; i < 256; ++i) {
        if (epoch.opcode_perm[i] != static_cast<uint8_t>(i)) {
            is_identity = false; break;
        }
    }
    EXPECT_FALSE(is_identity) << "opcode_perm should not be the identity permutation";

    // Verify opcode_perm_inv is the inverse of opcode_perm
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(epoch.opcode_perm_inv[epoch.opcode_perm[i]],
                  static_cast<uint8_t>(i))
            << "opcode_perm_inv[opcode_perm[" << i << "]] != " << i;
    }
}

TEST(Phase2Epoch, FpeEncodeDecodeRoundtrip) {
    // Doc 16: register encoding uses Speck-FPE (XEX mode over Speck64/128)
    // instead of per-BB RegTables LUTs.  Verify roundtrip and per-register
    // independence using the xex_speck64.hpp API.

    using namespace VMPilot::Common::VM::Crypto;

    // Set up a test key (128 bits)
    uint8_t key[16];
    for (int i = 0; i < 16; ++i) key[i] = static_cast<uint8_t>(0xA0 + i);

    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key, rk);

    XEX_Tweaks tweaks;
    XEX_ComputeTweaks(rk, tweaks);

    // Verify encode→decode roundtrip for several values across registers
    const uint64_t test_values[] = {0, 1, 0x42, 0xDEADBEEF, 0xCAFEBABE12345678ull};
    for (uint8_t reg = 0; reg < 4; ++reg) {
        for (uint64_t plain : test_values) {
            uint64_t encoded = FPE_Encode(rk, tweaks, reg, plain);
            uint64_t decoded = FPE_Decode(rk, tweaks, reg, encoded);
            EXPECT_EQ(decoded, plain)
                << "FPE roundtrip failed for reg=" << int(reg)
                << ", plain=0x" << std::hex << plain;
        }
    }

    // Verify per-register independence: same plaintext in different registers
    // must produce different ciphertext (XEX tweak guarantees this).
    uint64_t plain = 0x42;
    uint64_t enc_r0 = FPE_Encode(rk, tweaks, 0, plain);
    uint64_t enc_r1 = FPE_Encode(rk, tweaks, 1, plain);
    EXPECT_NE(enc_r0, enc_r1)
        << "Same plaintext in different registers must produce different ciphertext";

    // Encoding is not identity (ciphertext != plaintext)
    EXPECT_NE(enc_r0, plain) << "FPE_Encode should not return plaintext";
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

    // Doc 16 forward-secrecy fields default to zero
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(exec.insn_fpe_key[i], 0u) << "insn_fpe_key[" << i << "] not zero";
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(exec.bb_chain_state[i], 0u) << "bb_chain_state[" << i << "] not zero";
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
