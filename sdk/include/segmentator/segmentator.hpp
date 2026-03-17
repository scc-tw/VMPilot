#ifndef __SDK_SEGMENTATOR_HPP__
#define __SDK_SEGMENTATOR_HPP__

#include <CompilationContext.hpp>
#include <RegionRefiner.hpp>
#include <Strategy.hpp>
#include <file_type_parser.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Result of the full segmentation pipeline.
struct SegmentationResult {
    std::vector<RegionRefiner::ProtectedRegion> groups;
    CompilationContext context;
};

/// Run the complete segmentation pipeline on a binary file.
/// Returns nullopt on any failure (bad format, no regions, etc.).
std::optional<SegmentationResult> segment(
    const std::string& filename) noexcept;

/// Legacy facade — kept for backward compatibility.
class Segmentator {
   protected:
    VMPilot::Common::FileMetadata m_metadata;
    std::unique_ptr<FileHandlerStrategy> m_file_handler;
    std::unique_ptr<ArchHandlerStrategy> m_arch_handler;

    friend std::unique_ptr<Segmentator> create_segmentator(
        const std::string& filename) noexcept;

   protected:
    virtual void segmentation() noexcept;

   private:
    Segmentator() = default;

   public:
    virtual ~Segmentator() = default;
};

std::unique_ptr<Segmentator> create_segmentator(
    const std::string& filename) noexcept;

}  // namespace VMPilot::SDK::Segmentator

#endif