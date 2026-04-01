#ifndef __LOADER_PE_EDITOR_HPP__
#define __LOADER_PE_EDITOR_HPP__
#pragma once

#include <editor_base.hpp>

#include <memory>

namespace VMPilot::Loader {

/// PE binary editor using coffi-modern. Pimpl to keep coffi out of the header.
class PEEditor : public EditorBase<PEEditor> {
    friend class EditorBase<PEEditor>;

public:
    ~PEEditor();
    PEEditor(PEEditor&&) noexcept;
    PEEditor& operator=(PEEditor&&) noexcept;

    [[nodiscard]] static tl::expected<PEEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    // --- PE-specific (not part of EditorBase) ---

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    extend_text(const std::vector<uint8_t>& data,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept;

private:
    PEEditor();
    struct Impl;
    std::unique_ptr<Impl> impl_;

    // --- EditorBase CRTP impl methods ---
    [[nodiscard]] TextSectionInfo text_section_impl() const noexcept;
    [[nodiscard]] uint64_t next_segment_va_impl(uint64_t alignment) const noexcept;
    [[nodiscard]] bool cfi_enforced_impl() const noexcept;

    [[nodiscard]] std::vector<TextGap>
    find_text_gaps_impl(std::size_t min_size) const noexcept;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment_impl(std::string_view name, const std::vector<uint8_t>& data,
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
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_PE_EDITOR_HPP__
