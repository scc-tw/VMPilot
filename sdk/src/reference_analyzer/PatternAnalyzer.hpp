#ifndef __SDK_REFERENCE_ANALYZER_PATTERN_ANALYZER_HPP__
#define __SDK_REFERENCE_ANALYZER_PATTERN_ANALYZER_HPP__
#pragma once

#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <capstone.hpp>

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Layer 3: Detect TLS patterns and other special access patterns.
/// Returns new refs or upgrades to merge with existing refs.
std::vector<Core::DataReference> analyzePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept;

}  // namespace VMPilot::SDK::ReferenceAnalyzer

#endif  // __SDK_REFERENCE_ANALYZER_PATTERN_ANALYZER_HPP__
