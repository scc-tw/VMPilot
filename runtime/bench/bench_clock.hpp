#pragma once
#include <chrono>
#include <cstdint>

namespace VMPilot::Bench {

struct Clock {
    using clock      = std::chrono::high_resolution_clock;
    using time_point = clock::time_point;

    static time_point now() noexcept { return clock::now(); }

    static uint64_t elapsed_ns(time_point start, time_point end) noexcept {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                .count());
    }
};

}  // namespace VMPilot::Bench
