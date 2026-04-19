/// @file test_file_backed_state_provider.cpp
/// @brief Single-process unit tests for FileBackedStateProvider — doc 17 §10 (Stage A).
///
/// Covers:
///   - fresh install (missing file) reads zeroed state
///   - first reserve_nonce materialises the file and persists
///   - replay detection across a new provider instance
///   - advance_epoch_state monotonicity per-field
///   - byte-corruption → Corrupt error
///   - capabilities() report the honest doc 17 §3 claims
///
/// Cross-process behaviour is covered separately by the helper binary
/// introduced in Stage A PR-4.

#include "state/file_backed_state_provider.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

using VMPilot::Runtime::State::EpochState;
using VMPilot::Runtime::State::FileBackedStateProvider;
using VMPilot::Runtime::State::PersistenceCapability;
using VMPilot::Runtime::State::ProviderOperationalState;
using VMPilot::Runtime::State::satisfies_highsec;
using VMPilot::Runtime::State::StoreError;

// Build a deterministic 32-byte nonce from a single seed byte so tests
// can produce any number of distinct nonces without a PRNG.
std::array<std::uint8_t, 32> make_nonce(std::uint8_t seed) {
    std::array<std::uint8_t, 32> n{};
    n.fill(seed);
    return n;
}

class FileBackedStateProviderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Unique temp dir per test — shared process-wide temp_directory
        // with a pid+name suffix is robust enough for ASAN CI and keeps
        // the test hermetic against leftover state.
        const auto base = std::filesystem::temp_directory_path();
        const auto test = ::testing::UnitTest::GetInstance()->current_test_info();
        const std::string name = std::string(test->test_suite_name()) + "_" +
                                 test->name() + "_" +
                                 std::to_string(static_cast<std::uint64_t>(
                                     reinterpret_cast<std::uintptr_t>(this)));
        dir_ = base / ("vmpilot_fbsp_" + name);
        std::filesystem::create_directories(dir_);
        path_ = (dir_ / "state.bin").string();
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(dir_, ec);
    }

    std::filesystem::path dir_;
    std::string           path_;
};

TEST_F(FileBackedStateProviderTest, FreshInstallReadsZeroedState) {
    FileBackedStateProvider p(path_);
    auto epoch_or = p.current_epoch_state();
    ASSERT_TRUE(epoch_or.has_value());
    EXPECT_EQ(epoch_or->runtime_epoch,          0u);
    EXPECT_EQ(epoch_or->minimum_accepted_epoch, 0u);
    EXPECT_FALSE(std::filesystem::exists(path_))
        << "read-only call must not materialise the state file";
}

TEST_F(FileBackedStateProviderTest, FirstReserveNoncePersists) {
    const auto nonce = make_nonce(0x11);

    {
        FileBackedStateProvider p(path_);
        ASSERT_TRUE(p.reserve_nonce(nonce).has_value());
    }
    ASSERT_TRUE(std::filesystem::exists(path_));

    // Fresh instance sees the previously-reserved nonce as consumed.
    FileBackedStateProvider p2(path_);
    auto again = p2.reserve_nonce(nonce);
    ASSERT_FALSE(again.has_value());
    EXPECT_EQ(again.error(), StoreError::NonceAlreadyPresent);
}

TEST_F(FileBackedStateProviderTest, DistinctNoncesBothSucceed) {
    FileBackedStateProvider p(path_);
    ASSERT_TRUE(p.reserve_nonce(make_nonce(0x01)).has_value());
    ASSERT_TRUE(p.reserve_nonce(make_nonce(0x02)).has_value());
    ASSERT_TRUE(p.reserve_nonce(make_nonce(0x03)).has_value());

    // All three persist across a new instance.
    FileBackedStateProvider p2(path_);
    EXPECT_EQ(p2.reserve_nonce(make_nonce(0x01)).error(),
              StoreError::NonceAlreadyPresent);
    EXPECT_EQ(p2.reserve_nonce(make_nonce(0x02)).error(),
              StoreError::NonceAlreadyPresent);
    EXPECT_EQ(p2.reserve_nonce(make_nonce(0x03)).error(),
              StoreError::NonceAlreadyPresent);
}

TEST_F(FileBackedStateProviderTest, AdvanceEpochForwardsAreAccepted) {
    FileBackedStateProvider p(path_);

    EpochState s1{};
    s1.runtime_epoch          = 5;
    s1.minimum_accepted_epoch = 3;
    ASSERT_TRUE(p.advance_epoch_state(s1).has_value());

    EpochState s2{};
    s2.runtime_epoch          = 7;
    s2.minimum_accepted_epoch = 3;  // same is fine (monotonic, not strict)
    ASSERT_TRUE(p.advance_epoch_state(s2).has_value());

    auto got = p.current_epoch_state();
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->runtime_epoch,          7u);
    EXPECT_EQ(got->minimum_accepted_epoch, 3u);
}

TEST_F(FileBackedStateProviderTest, AdvanceEpochRollbackDenied) {
    FileBackedStateProvider p(path_);

    EpochState forward{};
    forward.runtime_epoch          = 10;
    forward.minimum_accepted_epoch = 5;
    ASSERT_TRUE(p.advance_epoch_state(forward).has_value());

    EpochState rt_rollback{};
    rt_rollback.runtime_epoch          = 9;    // rewind
    rt_rollback.minimum_accepted_epoch = 5;
    auto r1 = p.advance_epoch_state(rt_rollback);
    ASSERT_FALSE(r1.has_value());
    EXPECT_EQ(r1.error(), StoreError::EpochRollbackDenied);

    EpochState min_rollback{};
    min_rollback.runtime_epoch          = 10;
    min_rollback.minimum_accepted_epoch = 4;   // rewind
    auto r2 = p.advance_epoch_state(min_rollback);
    ASSERT_FALSE(r2.has_value());
    EXPECT_EQ(r2.error(), StoreError::EpochRollbackDenied);

    // Denials must not mutate disk state.
    auto got = p.current_epoch_state();
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->runtime_epoch,          10u);
    EXPECT_EQ(got->minimum_accepted_epoch, 5u);
}

TEST_F(FileBackedStateProviderTest, CorruptFileIsRejected) {
    {
        FileBackedStateProvider p(path_);
        ASSERT_TRUE(p.reserve_nonce(make_nonce(0x42)).has_value());
    }
    ASSERT_TRUE(std::filesystem::exists(path_));

    // Flip one byte of the payload — targeting the schema_version area
    // where corruption must surface as a CRC mismatch.
    {
        std::fstream fs(path_, std::ios::in | std::ios::out | std::ios::binary);
        ASSERT_TRUE(fs.is_open());
        fs.seekp(15);  // inside the body, past the 10-byte magic + u32 ver
        char c = 0;
        fs.read(&c, 1);
        fs.seekp(15);
        c = static_cast<char>(static_cast<std::uint8_t>(c) ^ 0x01);
        fs.write(&c, 1);
    }

    FileBackedStateProvider p(path_);
    auto r = p.reserve_nonce(make_nonce(0x43));
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::Corrupt);
}

TEST_F(FileBackedStateProviderTest, CapabilitiesAreHonest) {
    FileBackedStateProvider p(path_);
    const PersistenceCapability c = p.capabilities();
    EXPECT_TRUE (c.cross_process_atomic);
    EXPECT_TRUE (c.crash_consistent);
    EXPECT_FALSE(c.rollback_resistant);
    EXPECT_FALSE(c.tamper_evident);
    EXPECT_FALSE(c.hardware_bound);
    EXPECT_FALSE(c.policy_bound);
    EXPECT_FALSE(c.signed_recovery_required);
    EXPECT_EQ   (c.operational_state, ProviderOperationalState::Available);
}

TEST_F(FileBackedStateProviderTest, DoesNotSatisfyHighsec) {
    // Doc 17 §3.1 / stage B plan §4 acceptance rule: the debug/standard
    // provider MUST be rejected for highsec no matter how Available it
    // is — its rollback / tamper claims are honestly false.
    FileBackedStateProvider p(path_);
    EXPECT_FALSE(satisfies_highsec(p.capabilities()));
}

// ─── Free-function acceptance rule ──────────────────────────────────────

namespace {
PersistenceCapability fully_capable_highsec() {
    PersistenceCapability c{};
    c.cross_process_atomic     = true;
    c.crash_consistent         = true;
    c.rollback_resistant       = true;
    c.tamper_evident           = true;
    c.hardware_bound           = true;
    c.policy_bound             = true;
    c.signed_recovery_required = true;
    c.operational_state        = ProviderOperationalState::Available;
    return c;
}
}  // namespace

TEST(SatisfiesHighsec, FullyCapableAvailableAccepts) {
    EXPECT_TRUE(satisfies_highsec(fully_capable_highsec()));
}

TEST(SatisfiesHighsec, EveryBoolIsRequired) {
    // Flip each bool to false independently; the rule must reject each
    // one. This prevents a future refactor from silently accepting a
    // near-miss capability (e.g. forgetting one of the new claims in
    // the highsec predicate after adding it to the struct).
    {
        auto c = fully_capable_highsec();
        c.cross_process_atomic = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
    {
        auto c = fully_capable_highsec();
        c.crash_consistent = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
    {
        auto c = fully_capable_highsec();
        c.rollback_resistant = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
    {
        auto c = fully_capable_highsec();
        c.tamper_evident = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
    {
        auto c = fully_capable_highsec();
        c.hardware_bound = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
    {
        auto c = fully_capable_highsec();
        c.policy_bound = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
    {
        auto c = fully_capable_highsec();
        c.signed_recovery_required = false;
        EXPECT_FALSE(satisfies_highsec(c));
    }
}

TEST(SatisfiesHighsec, NonAvailableOperationalStateRejects) {
    // This is the core PR-B0 invariant: a provider that *claims* every
    // structural property but whose runtime is not Available (e.g. a
    // TpmBackedStateProvider compiled without a TPM attached) must be
    // rejected. Otherwise capability honesty collapses.
    for (auto bad : {ProviderOperationalState::Unavailable,
                     ProviderOperationalState::ProvisioningRequired,
                     ProviderOperationalState::RecoveryRequired,
                     ProviderOperationalState::PolicyUnavailable}) {
        auto c = fully_capable_highsec();
        c.operational_state = bad;
        EXPECT_FALSE(satisfies_highsec(c))
            << "operational_state = " << static_cast<int>(bad)
            << " must reject highsec";
    }
}

}  // namespace
