#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <string>
#include <vector>

#include <vm/vm_opcode.hpp>

namespace VMPilot::Bench {

using Common::VM::VmOpcode;

struct BenchResult {
    const char* name       = "";
    VmOpcode    opcode     = VmOpcode::NOP;
    uint8_t     category   = 0;
    uint32_t    insn_count = 0;
    uint32_t    iterations = 0;
    uint64_t    min_ns     = 0;
    uint64_t    median_ns  = 0;
    uint64_t    p95_ns     = 0;
    double      mean_ns    = 0.0;
    double      stddev_ns  = 0.0;
    double      ns_per_insn = 0.0;   // median / insn_count
    double      handler_ns  = 0.0;   // ns_per_insn - baseline
    double      ips         = 0.0;   // insn_count * 1e9 / median_ns
};

inline BenchResult compute_stats(const char* name, VmOpcode opcode,
                                 uint32_t insn_count,
                                 std::vector<uint64_t> samples) {
    BenchResult r;
    r.name       = name;
    r.opcode     = opcode;
    r.category   = Common::VM::vm_opcode_category(opcode);
    r.insn_count = insn_count;
    r.iterations = static_cast<uint32_t>(samples.size());

    if (samples.empty()) return r;

    std::sort(samples.begin(), samples.end());

    r.min_ns    = samples.front();
    r.median_ns = samples[samples.size() / 2];
    r.p95_ns    = samples[static_cast<size_t>(samples.size() * 95 / 100)];

    double sum = 0;
    for (auto s : samples) sum += static_cast<double>(s);
    r.mean_ns = sum / static_cast<double>(samples.size());

    double var = 0;
    for (auto s : samples) {
        double d = static_cast<double>(s) - r.mean_ns;
        var += d * d;
    }
    r.stddev_ns = std::sqrt(var / static_cast<double>(samples.size()));

    if (insn_count > 0) {
        r.ns_per_insn = static_cast<double>(r.median_ns)
                      / static_cast<double>(insn_count);
        r.ips = static_cast<double>(insn_count) * 1.0e9
              / static_cast<double>(r.median_ns);
    }
    return r;
}

}  // namespace VMPilot::Bench
