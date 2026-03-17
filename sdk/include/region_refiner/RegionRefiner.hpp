#ifndef __SDK_REGION_REFINER_HPP__
#define __SDK_REGION_REFINER_HPP__
#pragma once

#include <NativeFunctionBase.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::RegionRefiner {

/// One physical copy of a protected region in the binary.
struct RegionSite {
    std::string source_name;                     // __FUNCTION__ name
    std::optional<std::string> enclosing_symbol;  // mangled
    bool is_canonical = false;
    uint64_t addr = 0;
    uint64_t size = 0;
};

/// A logical protected region grouping all physical copies.
struct ProtectedRegion {
    std::string source_name;          // __FUNCTION__ name
    std::vector<RegionSite> sites;    // all physical copies
    size_t canonical_index = 0;       // index of original body
};

/**
 * @brief Group refined regions by source name into ProtectedRegions.
 *
 * Identifies canonical vs inlined copies using enclosing symbol
 * demangling.  Takes a const ref (doesn't consume ownership).
 */
std::vector<ProtectedRegion> group(
    const std::vector<std::unique_ptr<Segmentator::NativeFunctionBase>>&
        regions) noexcept;

/**
 * @brief Refine a set of protected regions by removing overlaps.
 *
 * Given raw regions from the segmentator (which may contain nested or
 * partially overlapping regions due to inlining), produce a clean set
 * where no region is contained within or overlaps another.
 *
 * Rules:
 *   1. Containment: if region A fully contains region B, B is removed.
 *   2. Partial overlap: if A and B partially overlap, they are merged.
 *   3. Duplicate: regions at the same address are deduplicated.
 *
 * @param regions The raw regions from segmentator.
 * @return Refined regions with no overlaps.
 */
std::vector<std::unique_ptr<Segmentator::NativeFunctionBase>> refine(
    std::vector<std::unique_ptr<Segmentator::NativeFunctionBase>>
        regions) noexcept;

}  // namespace VMPilot::SDK::RegionRefiner

#endif  // __SDK_REGION_REFINER_HPP__
