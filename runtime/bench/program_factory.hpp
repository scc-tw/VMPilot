#pragma once
/// @file program_factory.hpp
/// @brief Build Doc 19 dispatch-unit benchmark programs.
///
/// Every benchmark program consists of K basic blocks, each containing
/// exactly N instructions (1 real opcode + N-1 chaff NOPs).  This mirrors
/// the Doc 19 fixed-width dispatch unit model exactly.
///
/// All BBs in all programs have the same length N, so max_bb_insn_count = N
/// for all blobs.  This ensures verify_bb_mac's fixed-iteration cost is
/// identical across benchmarks — no BB-length-dependent timing artifacts.
///
/// Setup requirements (register init, ORAM stack fill) are placed in
/// separate setup BBs and stepped through untimed by the runner.

#include "opcode_spec.hpp"

#include <vm/vm_opcode.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Bench {

struct DUBenchProgram {
    std::vector<uint8_t> blob;
    uint8_t  seed[32]{};
    uint32_t du_count         = 0;  ///< K measured dispatch units
    uint32_t setup_du_count   = 0;  ///< setup DUs (stepped untimed)
    uint32_t du_width         = 0;  ///< N (insns per DU = Policy::fusion_granularity)
    VmOpcode opcode           = VmOpcode::NOP;
    bool     needs_guest_memory = false;
    bool     needs_native       = false;
    uint64_t native_fn          = 0;
};

/// Build a Doc 19 DU benchmark for a single opcode.
///
/// @param spec     opcode specification
/// @param K        number of measured dispatch units
/// @param N        dispatch unit width (Policy::fusion_granularity)
DUBenchProgram build_du_program(const OpcodeBenchSpec& spec,
                                uint32_t K, uint32_t N);

/// Build the NOP baseline (K DUs of N NOPs each).
DUBenchProgram build_du_baseline(uint32_t K, uint32_t N);

}  // namespace VMPilot::Bench
