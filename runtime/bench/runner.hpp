#pragma once
/// @file runner.hpp
/// @brief Doc 19 DU benchmark runner — measures ns_per_dispatch_unit.
///
/// Each opcode is benchmarked as a Doc 19 dispatch unit (1 real + N-1 NOP).
/// All programs have BBs of identical length N, so verify_bb_mac cost is
/// constant across benchmarks.  The runner:
///   1. Creates VmEngine from the benchmark blob
///   2. Steps through setup DUs (untimed)
///   3. Times execute() for K measured DUs
///   4. Reports ns_per_du and delta from NOP baseline

#include "bench_clock.hpp"
#include "bench_stats.hpp"
#include "opcode_spec.hpp"
#include "program_factory.hpp"

#include "vm_engine.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

namespace VMPilot::Bench {

using Runtime::VmEngine;
using Runtime::VmResult;

struct RunConfig {
    uint32_t iterations = 11;   ///< samples per benchmark (odd → clean median)
    uint32_t du_count   = 125;  ///< dispatch units per benchmark
    uint32_t warmup     = 2;    ///< discarded warm-up runs
};

/// Guest memory buffer for Memory/Atomic benchmarks.
alignas(64) static uint64_t g_guest_mem[64] = {};

template<typename Policy, typename Oram>
std::vector<BenchResult> run_all(const RunConfig& cfg) {
    constexpr uint32_t N = Policy::fusion_granularity;
    const uint32_t K = cfg.du_count;
    const uint32_t total_samples = cfg.warmup + cfg.iterations;

    auto time_program = [&](const DUBenchProgram& prog) -> uint64_t {
        int64_t delta = prog.needs_guest_memory
            ? static_cast<int64_t>(reinterpret_cast<uintptr_t>(g_guest_mem))
            : 0;
        auto engine = VmEngine<Policy, Oram>::create(
            prog.blob.data(), prog.blob.size(), prog.seed, delta);
        if (!engine) return 0;

        // Untimed: run setup DUs via dispatch_unit (NOT step()).
        //
        // WHY dispatch_unit: step() does not have the pipeline-level
        // ORAM scan (Phase D.oram).  PUSH setup via step() would fail
        // to write to ORAM, causing POP to read garbage.
        for (uint32_t i = 0; i < prog.setup_du_count; ++i) {
            auto sr = engine->dispatch_unit();
            if (!sr || *sr == VmResult::Halted) return 0;
        }

        // Timed: execute remaining (K measured DUs)
        auto t0 = Clock::now();
        auto r  = engine->execute();
        auto t1 = Clock::now();
        return r ? Clock::elapsed_ns(t0, t1) : 0;
    };

    // Pre-build all programs
    std::vector<DUBenchProgram> progs;
    std::vector<const char*> names;
    std::vector<VmOpcode> opcodes;
    
    // Add NOP baseline as the first entry
    progs.push_back(build_du_baseline(K, N));
    names.push_back("NOP_BASELINE");
    opcodes.push_back(VmOpcode::NOP);

    for (size_t si = 0; si < SPEC_COUNT; ++si) {
        const auto& spec = SPECS[si];
        auto prog = build_du_program(spec, K, N);
        if (prog.blob.empty()) continue;
        progs.push_back(std::move(prog));
        names.push_back(spec.name ? spec.name : Common::VM::to_string(spec.opcode));
        opcodes.push_back(spec.opcode);
    }

    const size_t num_progs = progs.size();
    std::vector<std::vector<uint64_t>> all_samples(num_progs);
    for (auto& s : all_samples) s.reserve(cfg.iterations);

    std::mt19937 rng(1337); // Fixed seed for reproducibility across CI runs

    for (uint32_t s = 0; s < total_samples; ++s) {
        std::vector<size_t> indices(num_progs);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), rng);

        for (size_t idx : indices) {
            std::memset(g_guest_mem, 0, sizeof(g_guest_mem));
            uint64_t ns = time_program(progs[idx]);
            if (s >= cfg.warmup && ns > 0) {
                all_samples[idx].push_back(ns);
            }
        }
        
        // Print progress (every 10 iterations to reduce spam)
        if ((s + 1) % 10 == 0 || s + 1 == total_samples) {
            std::fprintf(stderr, "\r  Progress: %u / %u iterations ...", 
                         s + 1, total_samples);
        }
    }
    std::fprintf(stderr, "\n");

    // ── Compute stats ──
    std::vector<BenchResult> results;
    results.reserve(num_progs);
    
    // Compute NOP baseline first (it's at index 0)
    auto nop_result = compute_stats(names[0], opcodes[0], K, all_samples[0]);
    double baseline = nop_result.ns_per_insn;
    results.push_back(nop_result);

    for (size_t idx = 1; idx < num_progs; ++idx) {
        auto result = compute_stats(names[idx], opcodes[idx], K, all_samples[idx]);
        result.handler_ns = result.ns_per_insn - baseline;
        results.push_back(result);
        
        std::fprintf(stderr, "  bench %-20s  %8.1f ns/DU  (Δ: %+.1f ns)\n",
                     names[idx], result.ns_per_insn, result.handler_ns);
    }

    return results;
}

}  // namespace VMPilot::Bench
