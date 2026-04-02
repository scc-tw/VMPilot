#pragma once
/// @file program_factory.hpp
/// @brief Auto-generate benchmark programs from OpcodeBenchSpec.

#include "opcode_spec.hpp"

#include <vm/vm_opcode.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Bench {

struct BenchProgram {
    std::vector<uint8_t> blob;
    uint8_t  seed[32]{};
    uint32_t measured_insn_count = 0;  ///< N (target opcode repetitions)
    uint32_t total_insn_count    = 0;  ///< setup + N + HALT
    VmOpcode opcode              = VmOpcode::NOP;
};

/// Build a micro-benchmark for a single opcode from its spec.
/// @param spec   opcode specification from SPECS[]
/// @param N      number of repetitions (0 → use spec.max_n or default 500)
BenchProgram build_opcode_program(const OpcodeBenchSpec& spec, uint32_t N = 0);

/// Build the NOP baseline program (N NOPs in a straight-line BB).
BenchProgram build_nop_baseline(uint32_t N);

}  // namespace VMPilot::Bench
