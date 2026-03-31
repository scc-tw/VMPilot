#ifndef __LOADER_ELF_EDITOR_HPP__
#define __LOADER_ELF_EDITOR_HPP__
#pragma once

#include <editor_base.hpp>

#include <memory>

namespace VMPilot::Loader {

/// ELF binary editor using ELFIO. Pimpl to keep elfio out of the header.
class ELFEditor : public EditorBase<ELFEditor> {
    friend class EditorBase<ELFEditor>;

public:
    ~ELFEditor();
    ELFEditor(ELFEditor&&) noexcept;
    ELFEditor& operator=(ELFEditor&&) noexcept;

    [[nodiscard]] static tl::expected<ELFEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    // --- EditorBase _impl() interface ---

    [[nodiscard]] TextSectionInfo text_section_impl() const noexcept;
    [[nodiscard]] uint64_t next_segment_va_impl(uint64_t alignment) const noexcept;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment_impl(std::string_view name, const std::vector<uint8_t>& data,
                     uint64_t alignment,
                     Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] bool cfi_enforced_impl() const noexcept;

    [[nodiscard]] std::vector<TextGap>
    find_text_gaps_impl(std::size_t min_size) const noexcept;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    extend_text(const std::vector<uint8_t>& data,
                     uint64_t alignment,
                     Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_text_impl(uint64_t va, const uint8_t* data, size_t len,
                        Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    add_runtime_dep_impl(std::string_view install_name,
                         Common::DiagnosticCollector& diag) noexcept;

    void invalidate_signature_impl() noexcept;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    save_impl(const std::string& path,
              Common::DiagnosticCollector& diag) noexcept;

private:
    ELFEditor();

    /// If the binary has .note.gnu.property with CET/BTI enforcement,
    /// our injected code must be declared compatible.  If the binary
    /// doesn't have the note, we don't add one (preserving original
    /// security posture).  If it does, we ensure our flag is set.
    void ensure_cfi_note() noexcept;

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_ELF_EDITOR_HPP__
