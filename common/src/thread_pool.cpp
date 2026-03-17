#include <thread_pool.hpp>

#include <algorithm>

namespace VMPilot::Common {

ThreadPool::ThreadPool(size_t num_threads) {
    if (num_threads == 0)
        num_threads = std::max(1u, std::thread::hardware_concurrency());

    queues_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i)
        queues_.push_back(std::make_unique<WorkerQueue>());

    workers_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i)
        workers_.emplace_back(&ThreadPool::worker_loop, this, i);
}

ThreadPool::~ThreadPool() {
    stop_.store(true, std::memory_order_release);
    wake_cv_.notify_all();
    for (auto& w : workers_) {
        if (w.joinable())
            w.join();
    }
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(wait_mtx_);
    wait_cv_.wait(lock, [this] {
        return pending_tasks_.load(std::memory_order_acquire) == 0;
    });
}

void ThreadPool::worker_loop(size_t id) {
    // Thread-local RNG for steal victim selection
    std::mt19937 rng(static_cast<unsigned>(id));

    while (true) {
        std::function<void()> task;

        // 1. Try own queue (front — LIFO for locality)
        {
            std::lock_guard<std::mutex> lock(queues_[id]->mtx);
            if (!queues_[id]->tasks.empty()) {
                task = std::move(queues_[id]->tasks.front());
                queues_[id]->tasks.pop_front();
            }
        }

        // 2. If own queue empty, try stealing from a random other queue (back — FIFO)
        if (!task) {
            const size_t n = queues_.size();
            if (n > 1) {
                std::uniform_int_distribution<size_t> dist(0, n - 2);
                size_t victim = dist(rng);
                if (victim >= id)
                    ++victim;  // skip self

                std::lock_guard<std::mutex> lock(queues_[victim]->mtx);
                if (!queues_[victim]->tasks.empty()) {
                    task = std::move(queues_[victim]->tasks.back());
                    queues_[victim]->tasks.pop_back();
                }
            }
        }

        if (task) {
            task();
            if (pending_tasks_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                wait_cv_.notify_all();
            }
            continue;
        }

        // 3. No work found — wait
        if (stop_.load(std::memory_order_acquire))
            return;

        std::unique_lock<std::mutex> lock(wake_mtx_);
        wake_cv_.wait_for(lock, std::chrono::milliseconds(1), [this] {
            return stop_.load(std::memory_order_acquire) ||
                   pending_tasks_.load(std::memory_order_acquire) > 0;
        });

        if (stop_.load(std::memory_order_acquire))
            return;
    }
}

}  // namespace VMPilot::Common
