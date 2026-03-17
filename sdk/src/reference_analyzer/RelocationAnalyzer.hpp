#ifndef __SDK_REFERENCE_ANALYZER_RELOCATION_ANALYZER_HPP__
#define __SDK_REFERENCE_ANALYZER_RELOCATION_ANALYZER_HPP__
#pragma once

#include <DataReference.hpp>
#include <SectionInfo.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

class SectionLookup;

/// Layer 1: Analyze relocations in .rela.text to produce DataReferences.
std::vector<Core::DataReference> analyzeRelocations(
    const std::vector<Core::RelocationEntry>& text_relocations,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections) noexcept;

}  // namespace VMPilot::SDK::ReferenceAnalyzer

#endif  // __SDK_REFERENCE_ANALYZER_RELOCATION_ANALYZER_HPP__
