#pragma once
#include "bench_stats.hpp"

#include <vm/vm_opcode.hpp>

#include <cstdio>
#include <string>
#include <vector>

namespace VMPilot::Bench {

using Common::VM::to_string;

/// Emit benchmark results as JSON to stdout.  No third-party deps.
inline void emit_json(const char* commit,
                      const char* platform,
                      const char* build_type,
                      const char* policy,
                      const char* oram,
                      double baseline_ns_per_insn,
                      const std::vector<BenchResult>& results) {
    std::printf("{\n");
    std::printf("  \"version\": 1,\n");
    std::printf("  \"metadata\": {\n");
    std::printf("    \"commit\": \"%s\",\n", commit);
    std::printf("    \"platform\": \"%s\",\n", platform);
    std::printf("    \"build_type\": \"%s\",\n", build_type);
    std::printf("    \"policy\": \"%s\",\n", policy);
    std::printf("    \"oram\": \"%s\"\n", oram);
    std::printf("  },\n");
    std::printf("  \"baseline_ns_per_du\": %.2f,\n", baseline_ns_per_insn);
    std::printf("  \"results\": [\n");

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        std::printf("    {\n");
        std::printf("      \"opcode\": \"%s\",\n", to_string(r.opcode));
        std::printf("      \"category\": %u,\n", r.category);
        std::printf("      \"du_count\": %u,\n", r.insn_count);
        std::printf("      \"iterations\": %u,\n", r.iterations);
        std::printf("      \"min_ns\": %llu,\n",
                    static_cast<unsigned long long>(r.min_ns));
        std::printf("      \"median_ns\": %llu,\n",
                    static_cast<unsigned long long>(r.median_ns));
        std::printf("      \"p95_ns\": %llu,\n",
                    static_cast<unsigned long long>(r.p95_ns));
        std::printf("      \"mean_ns\": %.2f,\n", r.mean_ns);
        std::printf("      \"stddev_ns\": %.2f,\n", r.stddev_ns);
        std::printf("      \"ns_per_du\": %.2f,\n", r.ns_per_insn);
        std::printf("      \"delta_ns\": %.2f,\n", r.handler_ns);
        std::printf("      \"du_per_sec\": %.0f\n", r.ips);
        std::printf("    }%s\n", (i + 1 < results.size()) ? "," : "");
    }

    std::printf("  ]\n");
    std::printf("}\n");
}

}  // namespace VMPilot::Bench
