#ifndef __LOADER_ELF_EDITOR_HPP__
#define __LOADER_ELF_EDITOR_HPP__
#pragma once

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace VMPilot::Loader {

/// ELF binary editor using ELFIO.  Pimpl to keep elfio out of the header.
class ELFEditor {
public:
    ~ELFEditor();
    ELFEditor(ELFEditor&&) noexcept;
    ELFEditor& operator=(ELFEditor&&) noexcept;

    [[nodiscard]] static tl::expected<ELFEditor, Common::DiagnosticCode>
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

    /// Overwrite bytes at any VA in any loaded section (including .text.vm).
    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_segment(uint64_t va, const uint8_t* data, size_t len,
                      Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    save(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

private:
    ELFEditor();
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_ELF_EDITOR_HPP__
