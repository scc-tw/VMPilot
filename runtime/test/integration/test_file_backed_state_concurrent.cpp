/// @file test_file_backed_state_concurrent.cpp
/// @brief Cross-process concurrency invariants for FileBackedStateProvider.
///
/// Doc 17 §10 Stage A tests #1-#3: spawn N copies of
/// `vmpilot_state_concurrent_helper` against the same state file and
/// tally their exit codes to confirm the provider's lock + atomic
/// rename protocol holds across process boundaries.
///
/// The helper path is injected by the build via the
/// VMPILOT_STATE_HELPER_PATH compile define.

#include "helpers/subprocess_run.hpp"
#include "state/file_backed_state_provider.hpp"
#include "state_helper_path.hpp"   // generated at build time — see runtime/CMakeLists.txt

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace {

using VMPilot::Runtime::State::EpochState;
using VMPilot::Runtime::State::FileBackedStateProvider;
using VMPilot::Runtime::State::StoreError;
using VMPilot::RuntimeTest::run_subprocess;

// Helper exit codes — must match runtime/test/helpers/state_concurrent_helper.cpp.
constexpr int kExitOk             = 0;
constexpr int kExitNonceReplay    = 10;
constexpr int kExitEpochRollback  = 11;

// 32 hex chars → 16 bytes; we want 64 hex chars → 32 bytes. Build a
// deterministic nonce hex string from a seed byte.
std::string make_nonce_hex(std::uint8_t seed) {
    static constexpr char hex[] = "0123456789abcdef";
    std::string out(64, '0');
    for (std::size_t i = 0; i < 32; ++i) {
        out[i * 2]     = hex[(seed >> 4) & 0xF];
        out[i * 2 + 1] = hex[seed & 0xF];
    }
    return out;
}

std::array<std::uint8_t, 32> bytes_from_seed(std::uint8_t seed) {
    std::array<std::uint8_t, 32> n{};
    n.fill(seed);
    return n;
}

class FileBackedStateConcurrentTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto base = std::filesystem::temp_directory_path();
        const auto test = ::testing::UnitTest::GetInstance()->current_test_info();
        const std::string name = std::string(test->test_suite_name()) + "_" +
                                 test->name() + "_" +
                                 std::to_string(static_cast<std::uint64_t>(
                                     reinterpret_cast<std::uintptr_t>(this)));
        dir_         = base / ("vmpilot_fbsp_cc_" + name);
        std::filesystem::create_directories(dir_);
        state_path_  = (dir_ / "state.bin").string();
        helper_path_ = VMPILOT_STATE_HELPER_PATH;
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(dir_, ec);
    }

    // Launch N children in parallel, each running `op` with its own
    // arg list. Returns the collected exit codes (same order as
    // `per_child_args`). Parallelism comes from std::thread — the
    // helper's file lock is what serialises their critical sections.
    std::vector<int> run_parallel(
        const std::string& op,
        const std::vector<std::vector<std::string>>& per_child_args) {
        const std::size_t n = per_child_args.size();
        std::vector<int>         codes(n, -1);
        std::vector<std::thread> threads;
        threads.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            threads.emplace_back([&, i]() {
                std::vector<std::string> args;
                args.reserve(per_child_args[i].size() + 4);
                args.push_back("--state-path");
                args.push_back(state_path_);
                args.push_back("--op");
                args.push_back(op);
                for (const auto& a : per_child_args[i]) args.push_back(a);
                codes[i] = run_subprocess(helper_path_, args);
            });
        }
        for (auto& t : threads) t.join();
        return codes;
    }

    std::filesystem::path dir_;
    std::string           state_path_;
    std::string           helper_path_;
};

// Doc 17 §10 Stage A #1 — two processes racing on the same nonce must
// produce exactly one success and the rest must fail with replay.
TEST_F(FileBackedStateConcurrentTest, SameNonceExactlyOneWinner) {
    constexpr std::size_t N = 4;
    const std::string nonce = make_nonce_hex(0xA5);

    std::vector<std::vector<std::string>> per_child_args(
        N, std::vector<std::string>{"--nonce", nonce});

    const auto codes = run_parallel("reserve-nonce", per_child_args);

    int ok = 0, replay = 0, other = 0;
    for (int c : codes) {
        if      (c == kExitOk)          ++ok;
        else if (c == kExitNonceReplay) ++replay;
        else                            ++other;
    }
    EXPECT_EQ(ok,     1) << "exactly one process must win the race";
    EXPECT_EQ(replay, static_cast<int>(N) - 1)
        << "every loser must report NonceAlreadyPresent";
    EXPECT_EQ(other,  0) << "no other failure modes expected";

    // Disk state reflects exactly one consumed nonce: a fresh provider
    // re-tries the same nonce and sees replay.
    FileBackedStateProvider after(state_path_);
    EXPECT_EQ(after.reserve_nonce(bytes_from_seed(0xA5)).error(),
              StoreError::NonceAlreadyPresent);
}

// Doc 17 §10 Stage A #2 — distinct nonces serialise via the lock, all
// succeed, all persist.
TEST_F(FileBackedStateConcurrentTest, DistinctNoncesAllPersist) {
    constexpr std::size_t N = 4;

    std::vector<std::vector<std::string>> per_child_args;
    per_child_args.reserve(N);
    for (std::size_t i = 0; i < N; ++i) {
        per_child_args.push_back({"--nonce",
            make_nonce_hex(static_cast<std::uint8_t>(0x20 + i))});
    }

    const auto codes = run_parallel("reserve-nonce", per_child_args);
    for (std::size_t i = 0; i < N; ++i) {
        EXPECT_EQ(codes[i], kExitOk) << "child #" << i << " expected Ok";
    }

    // Every committed nonce must now be visible as consumed.
    FileBackedStateProvider after(state_path_);
    for (std::size_t i = 0; i < N; ++i) {
        const auto seed = static_cast<std::uint8_t>(0x20 + i);
        EXPECT_EQ(after.reserve_nonce(bytes_from_seed(seed)).error(),
                  StoreError::NonceAlreadyPresent)
            << "nonce seed 0x" << std::hex << static_cast<int>(seed)
            << " should have persisted from child #" << std::dec << i;
    }
}

// Doc 17 §10 Stage A #3 — advance_epoch_state race. Order is
// non-deterministic, but the monotonic invariant forces the final
// on-disk value to equal max(proposals) regardless of interleaving.
TEST_F(FileBackedStateConcurrentTest, AdvanceEpochRaceFinalIsMax) {
    const std::vector<std::uint64_t> proposals{3, 9, 5, 7};
    constexpr std::uint64_t kMinEpoch = 0;

    std::vector<std::vector<std::string>> per_child_args;
    per_child_args.reserve(proposals.size());
    for (std::uint64_t p : proposals) {
        per_child_args.push_back({
            "--runtime-epoch", std::to_string(p),
            "--minimum-epoch", std::to_string(kMinEpoch),
        });
    }

    const auto codes = run_parallel("advance-epoch", per_child_args);

    int ok = 0, rollback = 0, other = 0;
    for (int c : codes) {
        if      (c == kExitOk)            ++ok;
        else if (c == kExitEpochRollback) ++rollback;
        else                              ++other;
    }
    EXPECT_GE(ok, 1) << "at least one advance must succeed";
    EXPECT_EQ(ok + rollback, static_cast<int>(proposals.size()));
    EXPECT_EQ(other, 0);

    // Disk truth: final runtime_epoch == max(proposals), by the
    // monotonic guarantee of advance_epoch_state.
    FileBackedStateProvider after(state_path_);
    auto final_state = after.current_epoch_state();
    ASSERT_TRUE(final_state.has_value());
    std::uint64_t expected_max = 0;
    for (std::uint64_t p : proposals) {
        if (p > expected_max) expected_max = p;
    }
    EXPECT_EQ(final_state->runtime_epoch, expected_max);
}

}  // namespace
