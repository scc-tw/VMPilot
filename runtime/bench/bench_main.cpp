/// @file bench_main.cpp
/// @brief CLI entry point for the VMPilot Doc 19 DU timing benchmark.
///
/// Measures per-dispatch-unit timing for all opcodes.  Each opcode is
/// benchmarked as a Doc 19 fixed-width DU (1 real + N-1 NOP).  All
/// programs have identical BB lengths, so timing differences reflect
/// only handler cost — not structural artifacts.
///
/// Usage:
///   vmpilot_bench [--iterations=N] [--dus=K] [--policy=debug|standard|all]
///                 [--warmup=N]

#include "bench_output.hpp"
#include "runner.hpp"

#include "vm_engine.hpp"
#include "vm_policy.hpp"

#include <cstdio>
#include <cstring>
#include <string>

using namespace VMPilot::Bench;
using namespace VMPilot::Runtime;

// ─── CLI parsing ────────────────────────────────────────────────────────

struct Args {
    uint32_t iterations = 11;
    uint32_t dus        = 125;
    uint32_t warmup     = 2;
    std::string policy  = "debug";
};

static bool starts_with(const char* s, const char* prefix) {
    return std::strncmp(s, prefix, std::strlen(prefix)) == 0;
}

static Args parse_args(int argc, char* argv[]) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        if (starts_with(argv[i], "--iterations="))
            a.iterations = static_cast<uint32_t>(std::atoi(argv[i] + 13));
        else if (starts_with(argv[i], "--dus="))
            a.dus = static_cast<uint32_t>(std::atoi(argv[i] + 6));
        else if (starts_with(argv[i], "--warmup="))
            a.warmup = static_cast<uint32_t>(std::atoi(argv[i] + 9));
        else if (starts_with(argv[i], "--policy="))
            a.policy = argv[i] + 9;
        else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            std::printf(
                "Usage: vmpilot_bench [options]\n"
                "  --iterations=N   samples per benchmark (default 11)\n"
                "  --dus=K          dispatch units per benchmark (default 125)\n"
                "  --warmup=N       discarded warm-up runs (default 2)\n"
                "  --policy=P       debug|standard|highsec|all (default debug)\n"
            );
            std::exit(0);
        }
    }
    return a;
}

// ─── Platform string ────────────────────────────────────────────────────

static const char* platform_string() {
#if defined(_WIN32)
    return "windows-x64-msvc";
#elif defined(__APPLE__)
    return "macos-arm64-appleclang";
#else
    return "linux-x64-gcc";
#endif
}

static const char* build_type_string() {
#ifdef NDEBUG
    return "Release";
#else
    return "Debug";
#endif
}

// ─── Main ───────────────────────────────────────────────────────────────

template<typename Policy, typename Oram>
static void run_and_emit(const Args& args, const char* policy_name,
                         const char* oram_name) {
    RunConfig cfg;
    cfg.iterations = args.iterations;
    cfg.du_count   = args.dus;
    cfg.warmup     = args.warmup;

    constexpr uint32_t N = Policy::fusion_granularity;
    std::fprintf(stderr,
        "\n=== %s + %s  (K=%u DUs, N=%u insns/DU, iter=%u, warmup=%u) ===\n",
        policy_name, oram_name, cfg.du_count, N,
        cfg.iterations, cfg.warmup);

    auto results = run_all<Policy, Oram>(cfg);

    double baseline = results.empty() ? 0.0 : results[0].ns_per_insn;

    emit_json("unknown", platform_string(), build_type_string(),
              policy_name, oram_name, baseline, results);
}

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);

    std::fprintf(stderr, "vmpilot_bench: %s build, %u iterations, %u DUs/bench\n",
                 build_type_string(), args.iterations, args.dus);

    if (args.policy == "debug" || args.policy == "all") {
        run_and_emit<DebugPolicy, DirectOram>(args, "DebugPolicy", "DirectOram");
    }
    if (args.policy == "standard" || args.policy == "all") {
        run_and_emit<StandardPolicy, RollingKeyOram>(
            args, "StandardPolicy", "RollingKeyOram");
    }
    if (args.policy == "highsec" || args.policy == "all") {
        run_and_emit<HighSecPolicy, RollingKeyOram>(
            args, "HighSecPolicy", "RollingKeyOram");
    }

    return 0;
}
