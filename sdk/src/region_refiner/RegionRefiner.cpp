#include <RegionRefiner.hpp>

#include <algorithm>

#include <spdlog/spdlog.h>

using NativeFunc = VMPilot::SDK::Segmentator::NativeFunctionBase;

std::vector<std::unique_ptr<NativeFunc>> VMPilot::SDK::RegionRefiner::refine(
    std::vector<std::unique_ptr<NativeFunc>> regions) noexcept {
    if (regions.size() <= 1) {
        return regions;
    }

    // Sort by start address, then by size descending (larger regions first)
    std::sort(regions.begin(), regions.end(), [](const auto& a, const auto& b) {
        if (a->getAddr() == b->getAddr()) {
            return a->getSize() > b->getSize();
        }
        return a->getAddr() < b->getAddr();
    });

    std::vector<std::unique_ptr<NativeFunc>> result;

    // Sweep: maintain the "current" outermost region's end address.
    // Any region that starts within and ends within the current region
    // is contained and gets dropped. Partial overlaps get merged.
    uint64_t current_end = 0;

    for (auto& region : regions) {
        uint64_t start = region->getAddr();
        uint64_t end = start + region->getSize();

        if (start >= current_end) {
            // No overlap with previous — start a new region
            current_end = end;
            result.push_back(std::move(region));
        } else if (end <= current_end) {
            // Fully contained within the current region — drop it
            spdlog::warn(
                "RegionRefiner: dropping contained region '{}' "
                "[0x{:x}, 0x{:x}) inside '{}' [0x{:x}, 0x{:x})",
                region->getName(), start, end, result.back()->getName(),
                result.back()->getAddr(), current_end);
            continue;
        } else {
            // Partial overlap — extend the current region to cover both
            auto& last = result.back();
            spdlog::warn(
                "RegionRefiner: merging partially overlapping regions "
                "'{}' [0x{:x}, 0x{:x}) and '{}' [0x{:x}, 0x{:x})",
                last->getName(), last->getAddr(), current_end,
                region->getName(), start, end);
            uint64_t merged_start = last->getAddr();
            uint64_t merged_size = end - merged_start;

            // Rebuild the code bytes for the merged region
            auto last_code = last->getCode();
            auto this_code = region->getCode();

            // The overlap portion: region starts at 'start', last ends at
            // 'current_end'. Overlap = current_end - start bytes.
            uint64_t overlap = current_end - start;
            // Append only the non-overlapping tail of the new region
            if (overlap < this_code.size()) {
                last_code.insert(last_code.end(), this_code.begin() + overlap,
                                 this_code.end());
            }

            auto merged = std::make_unique<NativeFunc>(
                merged_start, merged_size, last->getName(),
                std::move(last_code));

            if (!merged->isValid()) {
                spdlog::error(
                    "RegionRefiner: merged region '{}' at 0x{:x} is invalid: "
                    "size={} but code.size()={}, dropping",
                    merged->getName(), merged_start, merged_size,
                    merged->getCode().size());
                result.pop_back();
                // Don't update current_end — treat as if both were dropped
                continue;
            }

            result.back() = std::move(merged);
            current_end = end;
        }
    }

    return result;
}
