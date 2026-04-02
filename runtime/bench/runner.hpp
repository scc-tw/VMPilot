#pragma once
/// @file runner.hpp
/// @brief Benchmark runner — creates engines, times execute(), collects samples.

#include "bench_clock.hpp"
#include "bench_stats.hpp"
#include "opcode_spec.hpp"
#include "program_factory.hpp"

#include "vm_engine.hpp"

#include <cstdint>
#include <cstdio>
#include <vector>

namespace VMPilot::Bench {

using Runtime::VmEngine;
using Runtime::VmResult;

struct RunConfig {
    uint32_t iterations = 11;   ///< samples per benchmark (odd → clean median)
    uint32_t insns      = 500;  ///< repetitions per opcode
    uint32_t warmup     = 2;    ///< discarded warm-up runs
};

/// Run all registered specs and return results with baseline subtraction.
template<typename Policy, typename Oram>
std::vector<BenchResult> run_all(const RunConfig& cfg) {
    const uint32_t N = cfg.insns;
    const uint32_t total_samples = cfg.warmup + cfg.iterations;

    // ── NOP baseline ────────────────────────────────────────────────
    auto nop_prog = build_nop_baseline(N);
    std::vector<uint64_t> nop_samples;
    nop_samples.reserve(cfg.iterations);

    for (uint32_t s = 0; s < total_samples; ++s) {
        auto engine = VmEngine<Policy, Oram>::create(
            nop_prog.blob.data(), nop_prog.blob.size(), nop_prog.seed);
        if (!engine) continue;

        auto t0 = Clock::now();
        auto r  = engine->execute();
        auto t1 = Clock::now();

        if (s >= cfg.warmup && r)
            nop_samples.push_back(Clock::elapsed_ns(t0, t1));
    }
    auto nop_result = compute_stats("NOP_BASELINE", VmOpcode::NOP,
                                     nop_prog.measured_insn_count, nop_samples);
    double baseline = nop_result.ns_per_insn;

    // ── Per-opcode benchmarks ───────────────────────────────────────
    std::vector<BenchResult> results;
    results.push_back(nop_result);

    for (size_t si = 0; si < SPEC_COUNT; ++si) {
        const auto& spec = SPECS[si];

        // Skip shapes not yet implemented
        if (spec.shape != Shape::RegReg &&
            spec.shape != Shape::RegOnly &&
            spec.shape != Shape::NoOperand)
            continue;

        auto prog = build_opcode_program(spec, N);
        if (prog.blob.empty()) continue;

        const char* name = spec.name ? spec.name
                                     : Common::VM::to_string(spec.opcode);
        std::fprintf(stderr, "  bench %-20s  ", name);

        std::vector<uint64_t> samples;
        samples.reserve(cfg.iterations);

        for (uint32_t s = 0; s < total_samples; ++s) {
            auto engine = VmEngine<Policy, Oram>::create(
                prog.blob.data(), prog.blob.size(), prog.seed);
            if (!engine) continue;

            auto t0 = Clock::now();
            auto r  = engine->execute();
            auto t1 = Clock::now();

            if (s >= cfg.warmup && r)
                samples.push_back(Clock::elapsed_ns(t0, t1));
        }

        auto result = compute_stats(name, spec.opcode,
                                     prog.measured_insn_count, samples);
        result.handler_ns = result.ns_per_insn - baseline;
        results.push_back(result);

        std::fprintf(stderr, "%8.1f ns/insn  (handler: %+.1f ns)\n",
                     result.ns_per_insn, result.handler_ns);
    }

    return results;
}

}  // namespace VMPilot::Bench
