#ifndef __COMMON_THREAD_POOL_HPP__
#define __COMMON_THREAD_POOL_HPP__

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

namespace VMPilot::Common {

/// A work-stealing thread pool.
///
/// Each worker has a local task queue.  When a worker's queue is empty it
/// steals from another worker's queue (back end) before going to sleep.
class ThreadPool {
public:
    /// @param num_threads  Number of worker threads. 0 = hardware_concurrency.
    explicit ThreadPool(size_t num_threads = 0);

    /// Signals stop and joins all workers.
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /// Submit a callable and return a future for its result.
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    /// Number of worker threads.
    size_t size() const noexcept { return workers_.size(); }

    /// Block until every submitted task has finished.
    void wait_all();

private:
    struct WorkerQueue {
        std::deque<std::function<void()>> tasks;
        std::mutex mtx;
    };

    void worker_loop(size_t id);

    std::vector<std::unique_ptr<WorkerQueue>> queues_;
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_{false};
    std::atomic<size_t> pending_tasks_{0};
    std::atomic<size_t> next_queue_{0};  // round-robin counter

    std::mutex wait_mtx_;
    std::condition_variable wait_cv_;

    std::mutex wake_mtx_;
    std::condition_variable wake_cv_;
};

// ---------- template implementation ----------

template <typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {
    using R = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<R()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    auto future = task->get_future();

    pending_tasks_.fetch_add(1, std::memory_order_acq_rel);

    // Round-robin to a worker queue
    size_t idx = next_queue_.fetch_add(1, std::memory_order_relaxed) %
                 queues_.size();
    {
        std::lock_guard<std::mutex> lock(queues_[idx]->mtx);
        queues_[idx]->tasks.emplace_back([task]() { (*task)(); });
    }
    wake_cv_.notify_one();

    return future;
}

}  // namespace VMPilot::Common

#endif  // __COMMON_THREAD_POOL_HPP__
