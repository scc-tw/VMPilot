#ifndef __SDK_REGION_REFINER_HPP__
#define __SDK_REGION_REFINER_HPP__
#pragma once

#include <NativeFunctionBase.hpp>

#include <memory>
#include <vector>

namespace VMPilot::SDK::RegionRefiner {

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
