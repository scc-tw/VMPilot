/// @file test_hardware_rng.cpp
/// @brief Tests for hardware_random_u64() — platform-independent black-box tests.
///
/// The function under test has a single public signature:
///
///   uint64_t hardware_random_u64() noexcept;
///
/// We cannot unit-test internal tier selection (anonymous-namespace helpers),
/// but we can verify the observable contract:
///   1. Returns non-zero with overwhelming probability
///   2. Successive calls produce distinct values
///   3. Output has reasonable bit entropy (not stuck-at)
///   4. Thread-safe — concurrent calls do not crash or corrupt
///   5. Deterministic-free — no repeated patterns across runs

#include <vm/hardware_rng.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <set>
#include <thread>
#include <vector>

using VMPilot::Common::VM::hardware_random_u64;

// ============================================================================
// Basic contract tests
// ============================================================================

/// The function must not consistently return zero.
/// P(100 consecutive zeros from a real RNG) ≈ 2^{-6400} — effectively impossible.
TEST(HardwareRng, NonZero) {
    int non_zero = 0;
    constexpr int N = 100;
    for (int i = 0; i < N; ++i) {
        if (hardware_random_u64() != 0) ++non_zero;
    }
    EXPECT_GE(non_zero, N - 1)
        << "Expected almost all values to be non-zero";
}

/// Every call should produce a distinct 64-bit value.
/// Birthday bound for 64-bit: collision expected around 2^{32} samples.
/// With 1000 samples, P(collision) ≈ 2.7×10^{-14}.
TEST(HardwareRng, Uniqueness) {
    constexpr int N = 1000;
    std::set<uint64_t> seen;
    for (int i = 0; i < N; ++i)
        seen.insert(hardware_random_u64());
    EXPECT_EQ(seen.size(), static_cast<size_t>(N))
        << "Duplicate values detected in " << N << " samples";
}

/// Two consecutive calls should not return the same value.
TEST(HardwareRng, ConsecutiveCallsDiffer) {
    uint64_t a = hardware_random_u64();
    uint64_t b = hardware_random_u64();
    EXPECT_NE(a, b);
}

// ============================================================================
// Bit-level entropy tests
// ============================================================================

/// OR of many samples should set all (or nearly all) 64 bits.
/// AND of many samples should clear all (or nearly all) 64 bits.
/// This catches stuck-at-0 and stuck-at-1 bit faults.
TEST(HardwareRng, NoBitsStuck) {
    constexpr int N = 256;
    uint64_t or_acc = 0;
    uint64_t and_acc = ~uint64_t{0};
    for (int i = 0; i < N; ++i) {
        uint64_t v = hardware_random_u64();
        or_acc |= v;
        and_acc &= v;
    }
    EXPECT_EQ(or_acc, ~uint64_t{0})
        << "Some bits were never set in " << N << " samples (stuck-at-0)";
    EXPECT_EQ(and_acc, uint64_t{0})
        << "Some bits were never cleared in " << N << " samples (stuck-at-1)";
}

/// Each bit position should be set roughly 50% of the time.
/// We use a ±6σ window (p=0.5, n=10000 → σ=50, window=[4700, 5300]).
TEST(HardwareRng, BitFrequencyDistribution) {
    constexpr int N = 10000;
    std::array<int, 64> counts{};

    for (int i = 0; i < N; ++i) {
        uint64_t v = hardware_random_u64();
        for (int b = 0; b < 64; ++b) {
            if (v & (uint64_t{1} << b)) ++counts[b];
        }
    }

    // 6σ window: for p=0.5, n=10000 → σ = sqrt(n*p*(1-p)) = 50
    constexpr int lo = N / 2 - 300;  // 4700
    constexpr int hi = N / 2 + 300;  // 5300

    for (int b = 0; b < 64; ++b) {
        EXPECT_GE(counts[b], lo)
            << "Bit " << b << " set only " << counts[b]
            << " times (expected ~" << N / 2 << ")";
        EXPECT_LE(counts[b], hi)
            << "Bit " << b << " set " << counts[b]
            << " times (expected ~" << N / 2 << ")";
    }
}

/// Split each value into high/low 32-bit halves; they should differ.
/// Catches implementations that only randomise one half (e.g., broken x86-32 combine).
TEST(HardwareRng, HighLowHalvesIndependent) {
    constexpr int N = 100;
    int same_halves = 0;
    for (int i = 0; i < N; ++i) {
        uint64_t v = hardware_random_u64();
        uint32_t lo = static_cast<uint32_t>(v);
        uint32_t hi = static_cast<uint32_t>(v >> 32);
        if (lo == hi) ++same_halves;
    }
    // P(lo == hi) = 2^{-32} per sample; expecting 0 matches in 100 samples.
    EXPECT_LE(same_halves, 1)
        << "Too many samples have identical high/low halves";
}

// ============================================================================
// Byte-level uniformity (lightweight chi-squared)
// ============================================================================

/// Extract the lowest byte from many samples and verify rough uniformity
/// across 256 buckets via chi-squared test.
TEST(HardwareRng, ByteUniformity) {
    constexpr int N = 25600;  // 100 expected per bucket
    std::array<int, 256> buckets{};

    for (int i = 0; i < N; ++i) {
        uint8_t byte = static_cast<uint8_t>(hardware_random_u64());
        ++buckets[byte];
    }

    double expected = static_cast<double>(N) / 256.0;
    double chi2 = 0.0;
    for (int b = 0; b < 256; ++b) {
        double diff = buckets[b] - expected;
        chi2 += (diff * diff) / expected;
    }

    // chi-squared critical value for df=255 at p=0.001 ≈ 329
    // Generous threshold to avoid flaky tests.
    EXPECT_LT(chi2, 350.0)
        << "Byte distribution failed chi-squared test (chi2=" << chi2 << ")";
}

// ============================================================================
// Thread safety
// ============================================================================

/// Concurrent calls from multiple threads must not crash, deadlock,
/// or produce correlated values.
TEST(HardwareRng, ThreadSafety) {
    constexpr int NUM_THREADS = 8;
    constexpr int CALLS_PER_THREAD = 500;

    std::vector<std::vector<uint64_t>> results(NUM_THREADS);

    auto worker = [&](int tid) {
        results[tid].reserve(CALLS_PER_THREAD);
        for (int i = 0; i < CALLS_PER_THREAD; ++i)
            results[tid].push_back(hardware_random_u64());
    };

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);
    for (int t = 0; t < NUM_THREADS; ++t)
        threads.emplace_back(worker, t);
    for (auto& t : threads)
        t.join();

    // Merge all values and check uniqueness.
    std::set<uint64_t> all;
    for (auto& v : results)
        all.insert(v.begin(), v.end());

    size_t total = NUM_THREADS * CALLS_PER_THREAD;
    EXPECT_EQ(all.size(), total)
        << "Duplicate values across threads: got " << all.size()
        << " unique out of " << total;
}

/// Each thread should produce values independent from other threads.
/// Check that no two threads share the same first value (simple correlation check).
TEST(HardwareRng, CrossThreadIndependence) {
    constexpr int NUM_THREADS = 16;
    std::vector<uint64_t> first_values(NUM_THREADS);

    auto worker = [&](int tid) {
        first_values[tid] = hardware_random_u64();
    };

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);
    for (int t = 0; t < NUM_THREADS; ++t)
        threads.emplace_back(worker, t);
    for (auto& t : threads)
        t.join();

    std::set<uint64_t> unique(first_values.begin(), first_values.end());
    EXPECT_EQ(unique.size(), static_cast<size_t>(NUM_THREADS))
        << "Some threads produced the same first value";
}

// ============================================================================
// Sequential correlation
// ============================================================================

/// Consecutive outputs should not exhibit obvious linear correlation.
/// Compute Pearson correlation coefficient between consecutive pairs;
/// for true random, |r| should be near zero.
TEST(HardwareRng, NoSequentialCorrelation) {
    constexpr int N = 5000;
    std::vector<uint64_t> values(N);
    for (int i = 0; i < N; ++i)
        values[i] = hardware_random_u64();

    // Compute Pearson r between values[i] and values[i+1].
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0, sum_y2 = 0;
    int n = N - 1;
    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(values[i]);
        double y = static_cast<double>(values[i + 1]);
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
        sum_y2 += y * y;
    }

    double numerator = n * sum_xy - sum_x * sum_y;
    double denom_x = n * sum_x2 - sum_x * sum_x;
    double denom_y = n * sum_y2 - sum_y * sum_y;
    double denom = std::sqrt(denom_x * denom_y);

    double r = (denom > 0) ? (numerator / denom) : 0.0;

    // |r| < 0.05 is generous; true random gives |r| ≈ 1/sqrt(N) ≈ 0.014
    EXPECT_LT(std::abs(r), 0.05)
        << "Sequential correlation too high: r=" << r;
}
