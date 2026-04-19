/// @file test_tpm_backed_state_provider.cpp
/// @brief Unit tests for the PR-B1 TpmBackedStateProvider unavailable
///        skeleton — doc 17a Stage B plan §13 / Acceptance.
///
/// These tests assert the single invariant PR-B1 must hold before any
/// TPM library integration lands: the skeleton must never lie about
/// availability. Every operation must fail closed; capabilities()
/// must report Unavailable; the free satisfies_highsec() predicate
/// must reject the provider on every platform.

#include "state/tpm_backed_state_provider.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

namespace {

using VMPilot::Runtime::State::EpochState;
using VMPilot::Runtime::State::PersistenceCapability;
using VMPilot::Runtime::State::ProviderOperationalState;
using VMPilot::Runtime::State::satisfies_highsec;
using VMPilot::Runtime::State::StoreError;
using VMPilot::Runtime::State::TpmBackedStateProvider;
using VMPilot::Runtime::State::TpmProviderConfig;

TEST(TpmBackedSkeleton, CapabilitiesReportUnavailable) {
    TpmBackedStateProvider p{TpmProviderConfig{}};
    const PersistenceCapability c = p.capabilities();

    EXPECT_EQ   (c.operational_state, ProviderOperationalState::Unavailable);
    EXPECT_FALSE(c.cross_process_atomic);
    EXPECT_FALSE(c.crash_consistent);
    EXPECT_FALSE(c.rollback_resistant);
    EXPECT_FALSE(c.tamper_evident);
    EXPECT_FALSE(c.hardware_bound);
    EXPECT_FALSE(c.policy_bound);
    // signed_recovery_required is the single static design claim the
    // TPM path always makes — no local override, even while the
    // provider itself is Unavailable. Every other claim becomes true
    // only once a real backend is provisioned (PR-B7).
    EXPECT_TRUE (c.signed_recovery_required);
}

TEST(TpmBackedSkeleton, DoesNotSatisfyHighsec) {
    // This is the whole point of PR-B0 + PR-B1 together: an
    // unavailable TPM provider must be rejected for highsec no matter
    // how many structural claims it nominally carries. Without the
    // operational_state gate, a TPM-less host could pass the rule
    // just because the class compiles.
    TpmBackedStateProvider p{TpmProviderConfig{}};
    EXPECT_FALSE(satisfies_highsec(p.capabilities()));
}

TEST(TpmBackedSkeleton, ReserveNonceReturnsProviderUnavailable) {
    TpmBackedStateProvider p{TpmProviderConfig{}};
    std::array<std::uint8_t, 32> nonce{};
    nonce.fill(0xA5);
    auto r = p.reserve_nonce(nonce);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::ProviderUnavailable);
}

TEST(TpmBackedSkeleton, CurrentEpochStateReturnsProviderUnavailable) {
    TpmBackedStateProvider p{TpmProviderConfig{}};
    auto r = p.current_epoch_state();
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::ProviderUnavailable);
}

TEST(TpmBackedSkeleton, AdvanceEpochStateReturnsProviderUnavailable) {
    TpmBackedStateProvider p{TpmProviderConfig{}};
    EpochState s{};
    s.runtime_epoch          = 1;
    s.minimum_accepted_epoch = 1;
    auto r = p.advance_epoch_state(s);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::ProviderUnavailable);
}

TEST(TpmBackedSkeleton, ReprovisionReturnsProviderUnavailable) {
    TpmBackedStateProvider p{TpmProviderConfig{}};
    TpmBackedStateProvider::ReprovisionTokenPlaceholder token{};
    auto r = p.reprovision(token);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::ProviderUnavailable);
}

// Smoke: holding the unavailable provider by base-class reference
// works and the dispatch route through the vtable reports the same
// Unavailable state. Downstream code that works against the base
// class (doc 14 §6.1 acceptance rule site) will see exactly this.
TEST(TpmBackedSkeleton, PolymorphicAccessStillUnavailable) {
    TpmBackedStateProvider owned{TpmProviderConfig{}};
    VMPilot::Runtime::State::PersistentStateProvider& base = owned;

    EXPECT_EQ(base.capabilities().operational_state,
              ProviderOperationalState::Unavailable);
    EXPECT_FALSE(satisfies_highsec(base.capabilities()));

    std::array<std::uint8_t, 32> nonce{};
    nonce.fill(0x11);
    auto r = base.reserve_nonce(nonce);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::ProviderUnavailable);
}

}  // namespace
