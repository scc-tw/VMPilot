/// @file test_verify_bb_mac.cpp
/// @brief Isolated tests for verify_bb_mac enc_state evolution, REKEY handling,
///        and constant-time padding behavior.
///
/// Coverage gaps addressed:
///
///   1. No existing test directly calls pipeline::verify_bb_mac() in isolation.
///      All prior tests exercise it indirectly via engine execution.
///
///   2. No test verifies REKEY in the middle of a BB with padding iterations.
///      The existing RekeyAdvancesEncState test only has REKEY at position 0
///      with HALT at position 1 — no padding stress.
///
///   3. No test verifies the "1 real instruction + max padding" boundary.
///      verify_bb_mac iterates max_bb_insn_count times; the short-BB case
///      with many dummy iterations is untested.
///
///   4. No test verifies enc_state evolution inside verify_bb_mac matches
///      the builder's enc_state evolution — a divergence would silently
///      produce wrong MAC input, detectable only by MAC mismatch.
///
/// Specification references:
///   Doc 19 §4.2 Fix #2: fixed-iteration MAC verification
///   Doc 17 §2.2: full 8-byte enc_state ratchet (SipHash chain)
///   Doc 17 §3.1: avalanche-driven implicit denial (cascade failure)

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include "vm_state.hpp"
#include "pipeline.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_blob.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Test;

// ============================================================================
// Helpers
// ============================================================================

static constexpr uint8_t f_pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static constexpr uint8_t f_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static constexpr uint8_t f_none() { return 0; }

// ============================================================================
// #1: Direct pipeline-level verify_bb_mac — success case
//
// Doc 19 §4.2 Fix #2: verify_bb_mac must correctly re-derive the MAC
// by replaying the SipHash decryption chain from bb_enc_seed.
// ============================================================================

TEST(VerifyBbMac, DirectCallSucceeds) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb = make_test_bb(1, 0x50, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb}, {42});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // Call verify_bb_mac directly on the initial BB state.
    // The engine was just created — exec is at BB0, insn 0.
    auto mac_result = pipeline::verify_bb_mac(
        *engine->shared_immutable(),
        engine->execution(),
        engine->epoch());
    EXPECT_TRUE(mac_result.has_value())
        << "verify_bb_mac must succeed on an untampered blob";
}

// ============================================================================
// #2: REKEY mid-BB — verify_bb_mac handles enc_state mutation correctly
//
// Doc 17 §2.2: REKEY mutates enc_state via BLAKE3+SipHash.  verify_bb_mac
// must replay this mutation during its re-decryption loop.  If REKEY handling
// is missing or incorrect, the keystream for subsequent instructions will be
// wrong, producing incorrect plaintext and a MAC mismatch.
//
// This test places REKEY at position 1 of 4 instructions — subsequent
// instructions depend on the mutated enc_state.
// ============================================================================

TEST(VerifyBbMac, RekeyMidBbProducesCorrectMac) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb = make_test_bb(1, 0x60, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::REKEY, f_none(), 0, 0, 99},
        {VmOpcode::NOP, f_rr(), 2, 3, 0xAA},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb}, {100});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // Full execution must succeed — verify_bb_mac is called at BB boundary
    // in Phase L.  If REKEY at position 1 is handled incorrectly inside
    // verify_bb_mac, the MAC of the 4-instruction BB will not match.
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "REKEY mid-BB must not break verify_bb_mac (enc_state mutation "
           "must be replayed correctly during MAC verification)";
    EXPECT_EQ(result->return_value, 100u);
}

// ============================================================================
// #3: Multiple REKEY instructions in one BB
//
// Stress test: two REKEY instructions with different counters at positions
// 1 and 3.  Each REKEY mutates enc_state independently.  verify_bb_mac must
// handle both mutations in sequence.
// ============================================================================

TEST(VerifyBbMac, MultipleRekeyInOneBb) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb = make_test_bb(1, 0x70, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::REKEY, f_none(), 0, 0, 7},
        {VmOpcode::NOP, f_rr(), 2, 3, 0xBB},
        {VmOpcode::REKEY, f_none(), 0, 0, 13},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb}, {200});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "Two REKEY instructions in one BB must both be handled correctly "
           "by verify_bb_mac's enc_state replay";
    EXPECT_EQ(result->return_value, 200u);
}

// ============================================================================
// #4: Short BB padded to match a long BB — constant-time iteration
//
// Doc 19 §4.2 Fix #2: verify_bb_mac iterates max_bb_insn_count times.
// BB0 has 1 instruction, BB1 has 6 instructions.  verify_bb_mac for BB0
// must iterate 6 times (1 real + 5 dummy), and the MAC must still match.
//
// This is the "1 real instruction + max padding" boundary case.
// ============================================================================

TEST(VerifyBbMac, SingleInsnBbWithMaxPadding) {
    uint8_t seed[32]; fill_seed(seed);

    // BB0: single JMP to BB1
    TestBB bb0 = make_test_bb(1, 0x50, {
        {VmOpcode::JMP, f_none(), 0, 0, 2},
    });

    // BB1: 6 instructions (forces max_bb_insn_count = 6)
    TestBB bb1 = make_test_bb(2, 0x70, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x22},
        {VmOpcode::NOP, f_rr(), 6, 7, 0x33},
        {VmOpcode::NOP, f_rr(), 8, 9, 0x44},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb0, bb1}, {55});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    EXPECT_EQ(engine->shared_immutable()->max_bb_insn_count, 6u)
        << "max_bb_insn_count must be 6 (max of 1 and 6)";

    // verify_bb_mac for BB0 must iterate 6 times (1 real + 5 dummy).
    // If dummy iterations don't evolve enc_state correctly, the real
    // instruction's plaintext bytes would be correct but the MAC would
    // still pass (MAC only covers real instructions).  The key invariant
    // is that the fixed iteration count doesn't break MAC correctness.
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "1-instruction BB padded to 6 iterations must pass MAC verification";
    EXPECT_EQ(result->return_value, 55u);
}

// ============================================================================
// #5: REKEY in short BB with large padding
//
// Combination of #2 and #4: REKEY in a 2-instruction BB, padded to match
// a much longer BB.  The REKEY enc_state mutation happens at position 0,
// HALT at position 1, then 8 dummy iterations.  verify_bb_mac must:
//   (a) handle REKEY at position 0
//   (b) correctly advance enc_state through 8 dummy iterations
//   (c) hash only the 2 real instructions for the MAC
// ============================================================================

TEST(VerifyBbMac, RekeyInShortBbWithLargePadding) {
    uint8_t seed[32]; fill_seed(seed);

    // BB0: REKEY + HALT (2 instructions)
    TestBB bb0 = make_test_bb(1, 0x50, {
        {VmOpcode::REKEY, f_none(), 0, 0, 42},
        {VmOpcode::JMP, f_none(), 0, 0, 2},
    });

    // BB1: 10 instructions (forces max_bb_insn_count = 10)
    TestBB bb1 = make_test_bb(2, 0x70, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x22},
        {VmOpcode::NOP, f_rr(), 6, 7, 0x33},
        {VmOpcode::NOP, f_rr(), 8, 9, 0x44},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x55},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x66},
        {VmOpcode::NOP, f_rr(), 6, 7, 0x77},
        {VmOpcode::NOP, f_rr(), 8, 9, 0x88},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb0, bb1}, {77});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    EXPECT_EQ(engine->shared_immutable()->max_bb_insn_count, 10u)
        << "max_bb_insn_count must be 10";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "REKEY in 2-instruction BB padded to 10 iterations must pass "
           "MAC verification (enc_state mutation + dummy iteration handling)";
    EXPECT_EQ(result->return_value, 77u);
}

// ============================================================================
// #6: Direct verify_bb_mac with REKEY mid-BB
//
// Calls pipeline::verify_bb_mac() directly on an engine state where the BB
// contains REKEY at a non-zero position.  This isolates the MAC verification
// logic from the full dispatch_unit Phase L machinery.
// ============================================================================

TEST(VerifyBbMac, DirectCallWithRekeyMidBb) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb = make_test_bb(1, 0x60, {
        {VmOpcode::NOP, f_rr(), 2, 3, 0xAA},
        {VmOpcode::REKEY, f_none(), 0, 0, 55},
        {VmOpcode::NOP, f_rr(), 4, 5, 0xBB},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // Direct call: verify_bb_mac on the initial BB state.
    auto mac_result = pipeline::verify_bb_mac(
        *engine->shared_immutable(),
        engine->execution(),
        engine->epoch());
    EXPECT_TRUE(mac_result.has_value())
        << "Direct verify_bb_mac with REKEY at position 1 must succeed "
           "(enc_state mutation replayed correctly in verification loop)";
}

// ============================================================================
// #7: Tampered instruction after REKEY causes MAC failure
//
// Doc 17 §3.1: any bit flip cascades through the enc_state chain.
// This test verifies that tampering with an instruction AFTER a REKEY
// in the same BB is detected by verify_bb_mac.  The REKEY mutates
// enc_state, so verify_bb_mac must replay that mutation to correctly
// derive the keystream for subsequent instructions.
// ============================================================================

TEST(VerifyBbMac, TamperAfterRekeyDetected) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb = make_test_bb(1, 0x60, {
        {VmOpcode::NOP, f_rr(), 2, 3, 0xAA},
        {VmOpcode::REKEY, f_none(), 0, 0, 55},
        {VmOpcode::NOP, f_rr(), 4, 5, 0xBB},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb});

    // Tamper: flip a bit in instruction 2 (NOP after REKEY).
    // This sits after the REKEY enc_state mutation point.
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t insn_off = blob_section_insn(hdr);
    blob[insn_off + 2 * 8] ^= 0x01;  // flip bit in insn[2]

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_FALSE(result.has_value())
        << "Tampered instruction after REKEY must cause MAC verification failure";
}

// ============================================================================
// #8: HighSec dispatch unit — REKEY mid-BB with N=4 fusion
//
// HighSecPolicy processes 4 instructions per dispatch unit.  With REKEY
// at position 1 of a 4-instruction BB, the dispatch unit executes all 4
// instructions, then Phase L calls verify_bb_mac.  This tests the
// interaction between fusion granularity and REKEY enc_state mutation.
// ============================================================================

TEST(VerifyBbMac, HighSecFusionWithRekeyMidBb) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb = make_test_bb(1, 0x60, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::REKEY, f_none(), 0, 0, 33},
        {VmOpcode::NOP, f_rr(), 2, 3, 0xCC},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb}, {300});
    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "HighSec N=4 fusion with REKEY at position 1 must pass "
           "verify_bb_mac (same enc_state evolution as DebugPolicy)";
    EXPECT_EQ(result->return_value, 300u);
}

// ============================================================================
// #9: Two BBs with REKEY — cross-BB MAC independence
//
// Each BB has its own REKEY and different instruction counts.
// verify_bb_mac for each BB must independently replay its enc_state chain
// from its own bb_enc_seed.  The REKEY in BB0 must not affect BB1's MAC.
// ============================================================================

TEST(VerifyBbMac, CrossBbRekeyIndependence) {
    uint8_t seed[32]; fill_seed(seed);

    // BB0: 3 instructions with REKEY at position 1
    TestBB bb0 = make_test_bb(1, 0x50, {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::REKEY, f_none(), 0, 0, 11},
        {VmOpcode::JMP, f_none(), 0, 0, 2},
    });

    // BB1: 4 instructions with REKEY at position 2
    TestBB bb1 = make_test_bb(2, 0x70, {
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x22},
        {VmOpcode::REKEY, f_none(), 0, 0, 22},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    });

    auto blob = build_test_blob(seed, {bb0, bb1}, {500});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "Two BBs with independent REKEY instructions must both pass "
           "verify_bb_mac (each BB's enc_state starts from its own seed)";
    EXPECT_EQ(result->return_value, 500u);
}
