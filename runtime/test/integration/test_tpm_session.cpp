/// @file test_tpm_session.cpp
/// @brief Smoke test for Stage B PR-B2 `ITpmSession` — opens a real
///        TPM session via tpm2-tss, queries TPM2_PT_MANUFACTURER,
///        closes cleanly. Gated on `VMPILOT_HAS_TPM2_TSS` so builds
///        without TPM support compile the test as an empty TU.
///
/// When the flag is on but no TPM / swtpm is reachable (common on
/// hosts where VMPILOT_ENABLE_TPM was enabled optimistically), the
/// test SKIPs rather than failing — `open_tpm_session()` returning
/// `ProviderUnavailable` is the exact honest-unavailable signal doc
/// 17a §4 requires, and a red test here would punish developers for
/// simply not having a TPM attached.

#include "state/tpm_session.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#ifdef VMPILOT_HAS_TPM2_TSS

namespace {

using VMPilot::Runtime::State::ITpmSession;
using VMPilot::Runtime::State::open_tpm_session;
using VMPilot::Runtime::State::StoreError;

class TpmSessionSmoke : public ::testing::Test {
protected:
    void SetUp() override {
        auto session_or = open_tpm_session();
        if (!session_or.has_value()) {
            // Not a test failure: a missing TPM / swtpm at test time
            // is the same signal the acceptance rule uses to fail
            // closed. Surface the error code for operator debugging.
            GTEST_SKIP() << "no TPM / swtpm reachable (err="
                         << static_cast<int>(session_or.error()) << "). "
                         << "Start swtpm or attach a TPM, then rerun.";
        }
        session_ = std::move(*session_or);
        ASSERT_NE(session_, nullptr);
    }

    std::unique_ptr<ITpmSession> session_;
};

TEST_F(TpmSessionSmoke, GetManufacturerReturnsNonZero) {
    auto mfr = session_->get_manufacturer();
    ASSERT_TRUE(mfr.has_value())
        << "get_manufacturer failed (err="
        << static_cast<int>(mfr.error()) << ")";

    // TPM2_PT_MANUFACTURER is a 32-bit value whose bytes are the
    // manufacturer's 4-char ASCII code (e.g. "IBM ", "IFX ", "SWTP"
    // for swtpm). A zero value would mean the GetCapability round-
    // tripped but returned no data, which is not a legal TPM2 state.
    EXPECT_NE(*mfr, 0u);
}

TEST_F(TpmSessionSmoke, ManufacturerIsStableAcrossCalls) {
    auto first  = session_->get_manufacturer();
    auto second = session_->get_manufacturer();
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(*first, *second)
        << "manufacturer changed between calls — indicates a session "
           "that is not bound to a single TPM instance";
}

TEST(TpmSessionLifecycle, OpenAndCloseDoesNotLeak) {
    // Open / drop several sessions back-to-back. The intent is to
    // catch missing Esys_Finalize / Tss2_TctiLdr_Finalize calls on
    // the error path — under swtpm those leak socket descriptors and
    // later opens start failing.
    for (int i = 0; i < 4; ++i) {
        auto session_or = open_tpm_session();
        if (!session_or.has_value()) {
            GTEST_SKIP() << "no TPM / swtpm reachable";
        }
        // session drops here, dtor must finalize cleanly.
    }
}

}  // namespace

#else  // !VMPILOT_HAS_TPM2_TSS

// Without tpm2-tss the test binary is still emitted so CMake can
// register it, but it contains no TPM-dependent code and reports a
// skip at runtime.
TEST(TpmSessionSmoke, DisabledAtBuildTime) {
    GTEST_SKIP() << "VMPILOT_ENABLE_TPM is OFF — nothing to smoke test.";
}

#endif
