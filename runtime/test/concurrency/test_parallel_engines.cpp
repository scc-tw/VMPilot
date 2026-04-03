/// @file test_parallel_engines.cpp
/// @brief Concurrency tests for parallel VmEngine execution.
///
/// Tests: multiple engines from same blob in parallel,
///        shared VmImmutable across threads.

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include <vm/vm_insn.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

TEST(Concurrency, ParallelEnginesSameBlob) {
    uint8_t seed[32]; fill_seed(seed);
    auto bb = make_test_bb(1, 0xA0, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    });
    auto blob = build_test_blob(seed, {bb}, {42});

    constexpr int N = 8;
    std::vector<std::thread> threads;
    std::atomic<int> pass_count{0};
    std::atomic<int> fail_count{0};

    for (int i = 0; i < N; ++i) {
        threads.emplace_back([&]() {
            auto engine = VmEngine<DebugPolicy, DirectOram>::create(
                blob.data(), blob.size(), seed);
            if (!engine.has_value()) { fail_count++; return; }
            auto r = engine->execute();
            if (r.has_value() && r->return_value == 42)
                pass_count++;
            else
                fail_count++;
        });
    }
    for (auto& t : threads) t.join();
    EXPECT_EQ(pass_count.load(), N);
    EXPECT_EQ(fail_count.load(), 0);
}

TEST(Concurrency, SharedImmutableAcrossThreads) {
    uint8_t seed[32]; fill_seed(seed);
    auto bb = make_test_bb(1, 0xA0, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    });
    auto blob = build_test_blob(seed, {bb}, {99});

    // Create parent to get shared VmImmutable
    auto parent = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    constexpr int N = 8;
    std::vector<std::thread> threads;
    std::atomic<int> pass_count{0};

    for (int i = 0; i < N; ++i) {
        threads.emplace_back([&]() {
            auto engine = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm);
            if (!engine.has_value()) return;
            auto r = engine->execute();
            if (r.has_value() && r->return_value == 99)
                pass_count++;
        });
    }
    for (auto& t : threads) t.join();
    EXPECT_EQ(pass_count.load(), N)
        << "All threads sharing VmImmutable must produce correct results";
}
