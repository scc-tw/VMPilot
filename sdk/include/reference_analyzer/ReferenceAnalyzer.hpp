#ifndef __SDK_REFERENCE_ANALYZER_HPP__
#define __SDK_REFERENCE_ANALYZER_HPP__
#pragma once

#include <AnalysisContext.hpp>
#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <Section.hpp>
#include <capstone.hpp>

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Analyze data references in a code region.
///
/// Three-layer analysis:
///   Layer 1 (Relocation): parse .rela.text entries — most authoritative
///   Layer 2 (InsnAnalysis): walk instructions, resolve memory operands
///   Layer 3 (PatternMatch): detect TLS/jump table patterns, upgrade refs
///
/// Merge: L1 wins -> L2 fills gaps -> L3 upgrades.

/// Primary API — accepts a pre-built AnalysisContext.
/// Use this when analyzing multiple regions from the same binary
/// to share pre-computed lookups.
std::vector<Core::DataReference> analyze(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const AnalysisContext& ctx) noexcept;

/// Convenience overload — builds AnalysisContext internally.
std::vector<Core::DataReference> analyze(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const std::vector<Core::Section>& sections,
    const std::vector<Core::RelocationEntry>& text_relocations,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept;

}  // namespace VMPilot::SDK::ReferenceAnalyzer

#endif  // __SDK_REFERENCE_ANALYZER_HPP__
