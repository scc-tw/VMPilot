#ifndef __SDK_SEGMENTATOR_HPP__
#define __SDK_SEGMENTATOR_HPP__

#include <CompilationContext.hpp>
#include <NativeFunctionBase.hpp>
#include <RegionRefiner.hpp>

#include <tl/expected.hpp>

#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Error codes returned by segment().
enum class SegmentError {
    FileNotFound,
    UnsupportedFormat,
    UnsupportedArch,
    TextSectionMissing,
    DisassemblyFailed,
    NoRegionsFound,
};

/// Human-readable description of a SegmentError.
const char* to_string(SegmentError e) noexcept;

/// Result of the full segmentation pipeline.
struct SegmentationResult {
    std::vector<RegionRefiner::ProtectedRegion> groups;
    std::vector<NativeFunctionBase> refined_regions;
    CompilationContext context;
    std::string binary_path;
    std::string compiler_info;
};

/// Run the complete segmentation pipeline on a binary file.
tl::expected<SegmentationResult, SegmentError> segment(
    const std::string& filename) noexcept;

}  // namespace VMPilot::SDK::Segmentator

#endif
