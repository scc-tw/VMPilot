#ifndef __LOADER_PE_EDITOR_HPP__
#define __LOADER_PE_EDITOR_HPP__
#pragma once

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace VMPilot::Loader {

/// PE binary editor — not yet implemented.
/// All methods return PatchFormatUnsupported.
class PEEditor {
public:
    [[nodiscard]] static tl::expected<PEEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] TextSectionInfo text_section() const noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                   Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment(std::string_view name, const std::vector<uint8_t>& payload,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_segment(uint64_t va, const uint8_t* data, size_t len,
                      Common::DiagnosticCollector& diag) noexcept;

    /// Add a dynamic library dependency — not yet implemented for PE.
    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    add_dylib(std::string_view install_name,
              Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    save(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_PE_EDITOR_HPP__
