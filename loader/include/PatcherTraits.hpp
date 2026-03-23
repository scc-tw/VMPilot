#ifndef __LOADER_PATCHER_TRAITS_HPP__
#define __LOADER_PATCHER_TRAITS_HPP__
#pragma once

#include <diagnostic.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Platform-independent binary context produced by PatcherTraits::open().
/// Holds whatever opaque state the format-specific implementation needs.
struct BinaryContext {
    virtual ~BinaryContext() = default;
};

/// Result of locating the .text section.
struct TextSectionInfo {
    uint64_t base_addr = 0;     // VA of .text start
    uint64_t size = 0;          // .text size in bytes
};

/// Result of adding a new loadable segment/section.
struct NewSegmentInfo {
    uint64_t va = 0;            // VA where the segment was placed
    size_t size = 0;            // size of the injected data
};

/// Traits interface for format-specific binary mutation.
///
/// Each format (ELF, PE, Mach-O) provides a concrete traits struct
/// with static methods.  The generic BinaryPatcher algorithm calls
/// these through the FormatPatcher<Traits> template.
///
/// All methods follow the project convention: return tl::expected,
/// accept DiagnosticCollector, noexcept.
struct PatcherTraitsBase {
    /// Open a binary file and return an opaque context.
    // static tl::expected<std::unique_ptr<BinaryContext>, Common::DiagnosticCode>
    // open(const std::string& path, Common::DiagnosticCollector& diag) noexcept;

    /// Get .text section info from the opened binary.
    // static tl::expected<TextSectionInfo, Common::DiagnosticCode>
    // get_text_section(BinaryContext& ctx, Common::DiagnosticCollector& diag) noexcept;

    /// Overwrite bytes in .text at the given VA.
    // static tl::expected<void, Common::DiagnosticCode>
    // overwrite_text(BinaryContext& ctx, uint64_t va, const uint8_t* data, size_t len,
    //                Common::DiagnosticCollector& diag) noexcept;

    /// Add a new loadable segment/section with the given payload.
    /// Returns the VA where the segment was placed.
    // static tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    // add_segment(BinaryContext& ctx, const std::string& name,
    //             const std::vector<uint8_t>& payload, uint64_t alignment,
    //             Common::DiagnosticCollector& diag) noexcept;

    /// Compute the next available VA for a new segment (page-aligned).
    // static uint64_t
    // next_segment_va(const BinaryContext& ctx, uint64_t page_size) noexcept;

    /// Write the modified binary to disk.
    // static tl::expected<void, Common::DiagnosticCode>
    // save(BinaryContext& ctx, const std::string& path,
    //      Common::DiagnosticCollector& diag) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_PATCHER_TRAITS_HPP__
