#ifndef __LOADER_BINARY_EDITOR_HPP__
#define __LOADER_BINARY_EDITOR_HPP__
#pragma once

/// @file BinaryEditor.hpp
/// @brief Abstract interface for format-specific binary mutation.
///
/// Each platform (ELF, PE, Mach-O) implements this interface.
/// The Loader orchestrator calls these methods without knowing the format.
///
/// Source: D13§D1-D3 (binary patching / loader).

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace VMPilot::Loader {

class BinaryEditor {
public:
    virtual ~BinaryEditor() = default;

    // --- Query (read-only, no mutation) ---

    /// Get .text section VA and size.
    [[nodiscard]] virtual TextSectionInfo text_section() const noexcept = 0;

    /// Compute the VA where the next segment would be placed, without
    /// actually creating it. Enables build_payload() to resolve all
    /// displacements before injection.
    [[nodiscard]] virtual uint64_t
    next_segment_va(uint64_t alignment) const noexcept = 0;

    // --- Mutate ---

    /// Create a new loadable segment/section with the given data.
    [[nodiscard]] virtual tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment(std::string_view name, const std::vector<uint8_t>& data,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept = 0;

    /// Extend the .text section/segment to accommodate additional data.
    /// Returns the VA where data was placed (at the end of .text, aligned).
    /// The payload becomes indistinguishable from original code in static analysis.
    [[nodiscard]] virtual tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    extend_text(const std::vector<uint8_t>& data,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept = 0;

    /// Overwrite bytes in .text at the given VA.
    [[nodiscard]] virtual tl::expected<void, Common::DiagnosticCode>
    overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                   Common::DiagnosticCollector& diag) noexcept = 0;

    /// Add a runtime library dependency (LC_LOAD_DYLIB / DT_NEEDED / import).
    [[nodiscard]] virtual tl::expected<void, Common::DiagnosticCode>
    add_runtime_dep(std::string_view install_name,
                    Common::DiagnosticCollector& diag) noexcept = 0;

    /// Invalidate old code signature. Called before save().
    /// Mach-O: zero LC_CODE_SIGNATURE. PE: zero Certificate Table. ELF: no-op.
    virtual void invalidate_signature() noexcept = 0;

    // --- Persist ---

    [[nodiscard]] virtual tl::expected<void, Common::DiagnosticCode>
    save(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept = 0;
};

/// Factory. Opens a binary file and returns the appropriate editor.
[[nodiscard]] tl::expected<std::unique_ptr<BinaryEditor>, Common::DiagnosticCode>
open_binary(const std::string& path, Common::FileFormat format,
            Common::DiagnosticCollector& diag) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_BINARY_EDITOR_HPP__
