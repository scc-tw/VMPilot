/// @file test_execution_independence.cpp
/// @brief Per-execution independence via RDRAND nonce (Doc 16 §12 S4).
///
/// Two VmEngine::create() calls with identical blob+seed must produce
/// different bb_chain_state (seeded by RDRAND).  This ensures each
/// execution derives independent FPE keys.
///
/// Extracted from test_security_properties.cpp.

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include "vm_state.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>

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

static constexpr uint8_t f_none() { return 0; }

// ============================================================================
// #8: Per-execution independence via RDRAND nonce (Doc 16 §12 S4)
//
// Two VmEngine::create() calls with identical blob+seed must produce
// different bb_chain_state (seeded by RDRAND).  This ensures each
// execution derives independent FPE keys.
// ============================================================================

TEST(SecurityProperties, PerExecutionIndependence) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    bb.instructions = {
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {});

    auto e1 = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    auto e2 = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(e1.has_value());
    ASSERT_TRUE(e2.has_value());

    // bb_chain_state is seeded by RDRAND nonce — must differ between runs
    // (probability of collision: 2^-64)
    EXPECT_NE(
        std::memcmp(e1->execution().bb_chain_state,
                    e2->execution().bb_chain_state, 32), 0)
        << "Two create() calls must produce different bb_chain_state "
           "(RDRAND nonce diversification, Doc 16 §12 S4). "
           "Collision probability 2^-64.";

    // Consequently, insn_fpe_key must also differ (derived from chain_state)
    EXPECT_NE(
        std::memcmp(e1->execution().insn_fpe_key,
                    e2->execution().insn_fpe_key, 16), 0)
        << "Different bb_chain_state must produce different insn_fpe_key "
           "(path-dependent key derivation)";
}
