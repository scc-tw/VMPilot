#ifndef __SDK_SEGMENTATOR_HPP__
#define __SDK_SEGMENTATOR_HPP__

#include <CompilationContext.hpp>
#include <NativeFunctionBase.hpp>
#include <RegionRefiner.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Result of the full segmentation pipeline.
struct SegmentationResult {
    std::vector<RegionRefiner::ProtectedRegion> groups;
    std::vector<NativeFunctionBase> refined_regions;
    CompilationContext context;
    std::string binary_path;
    std::string compiler_info;
};

/// Run the complete segmentation pipeline on a binary file.
///
/// Error codes returned via DiagnosticCode (1000–1099 range):
///   FileNotFound, UnsupportedFormat, UnsupportedArch,
///   TextSectionMissing, DisassemblyFailed.
///
/// NoRegionsFound is emitted as a Warning and returns an empty
/// SegmentationResult (not an error).
tl::expected<SegmentationResult, Common::DiagnosticCode> segment(
    const std::string& filename,
    Common::DiagnosticCollector& diag =
        Common::DiagnosticCollector::noop()) noexcept;

}  // namespace VMPilot::SDK::Segmentator

#endif
