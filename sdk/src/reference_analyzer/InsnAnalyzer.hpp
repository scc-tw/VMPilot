#ifndef __SDK_REFERENCE_ANALYZER_INSN_ANALYZER_HPP__
#define __SDK_REFERENCE_ANALYZER_INSN_ANALYZER_HPP__
#pragma once

#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <capstone.hpp>

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

class SectionLookup;

/// Layer 2: Walk instructions and resolve memory operands to data references.
std::vector<Core::DataReference> analyzeInstructions(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept;

}  // namespace VMPilot::SDK::ReferenceAnalyzer

#endif  // __SDK_REFERENCE_ANALYZER_INSN_ANALYZER_HPP__
