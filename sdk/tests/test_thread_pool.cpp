#include <thread_pool.hpp>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <numeric>
#include <vector>

using VMPilot::Common::ThreadPool;

TEST(ThreadPool, ConstructDefault) {
    ThreadPool pool;
    EXPECT_GT(pool.size(), 0u);
}

TEST(ThreadPool, ConstructWithSize) {
    ThreadPool pool(4);
    EXPECT_EQ(pool.size(), 4u);
}

TEST(ThreadPool, SingleTask) {
    ThreadPool pool(2);
    auto future = pool.submit([]() { return 42; });
    EXPECT_EQ(future.get(), 42);
}

TEST(ThreadPool, MultipleTasks) {
    ThreadPool pool(4);
    constexpr int N = 100;
    std::vector<std::future<int>> futures;
    futures.reserve(N);

    for (int i = 0; i < N; ++i) {
        futures.push_back(pool.submit([i]() { return i * i; }));
    }

    for (int i = 0; i < N; ++i) {
        EXPECT_EQ(futures[i].get(), i * i);
    }
}

TEST(ThreadPool, WaitAll) {
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    constexpr int N = 200;

    for (int i = 0; i < N; ++i) {
        pool.submit([&counter]() {
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    pool.wait_all();
    EXPECT_EQ(counter.load(), N);
}

TEST(ThreadPool, ConcurrentExecution) {
    ThreadPool pool(4);
    std::atomic<int> max_concurrent{0};
    std::atomic<int> current{0};
    constexpr int N = 50;

    std::vector<std::future<void>> futures;
    for (int i = 0; i < N; ++i) {
        futures.push_back(pool.submit([&]() {
            int c = current.fetch_add(1, std::memory_order_relaxed) + 1;
            // Record maximum observed concurrency
            int prev_max = max_concurrent.load(std::memory_order_relaxed);
            while (c > prev_max &&
                   !max_concurrent.compare_exchange_weak(
                       prev_max, c, std::memory_order_relaxed)) {
            }
            // Small work to allow overlap
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            current.fetch_sub(1, std::memory_order_relaxed);
        }));
    }

    for (auto& f : futures) f.get();
    // With 4 threads and 50 tasks, we should have observed > 1 concurrent
    EXPECT_GT(max_concurrent.load(), 1);
}

TEST(ThreadPool, TaskWithException) {
    ThreadPool pool(2);
    auto future = pool.submit([]() -> int {
        throw std::runtime_error("test error");
    });
    EXPECT_THROW(future.get(), std::runtime_error);
}

TEST(ThreadPool, ReturnTypes) {
    ThreadPool pool(2);

    auto f_int = pool.submit([]() { return 1; });
    auto f_str = pool.submit([]() { return std::string("hello"); });
    auto f_void = pool.submit([]() {});

    EXPECT_EQ(f_int.get(), 1);
    EXPECT_EQ(f_str.get(), "hello");
    f_void.get();  // should not throw
}
