#ifndef __SDK_REFERENCE_ANALYZER_PATTERN_ANALYZER_HPP__
#define __SDK_REFERENCE_ANALYZER_PATTERN_ANALYZER_HPP__
#pragma once

#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>
#include <capstone.hpp>

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

class SectionLookup;

/// Layer 3: Detect TLS patterns, jump table patterns, and other special access
/// patterns. Returns new refs or upgrades to merge with existing refs.
std::vector<Core::DataReference> analyzePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const Segmentator::NativeSymbolTable& symbols,
    const SectionLookup& sections,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept;

}  // namespace VMPilot::SDK::ReferenceAnalyzer

#endif  // __SDK_REFERENCE_ANALYZER_PATTERN_ANALYZER_HPP__
