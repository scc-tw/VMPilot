#ifndef __LOADER_PE_EDITOR_HPP__
#define __LOADER_PE_EDITOR_HPP__
#pragma once

#include <BinaryEditor.hpp>

#include <memory>

namespace VMPilot::Loader {

/// PE binary editor using COFFI. Pimpl to keep coffi out of the header.
class PEEditor : public BinaryEditor {
public:
    ~PEEditor() override;
    PEEditor(PEEditor&&) noexcept;
    PEEditor& operator=(PEEditor&&) noexcept;

    [[nodiscard]] static tl::expected<PEEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    // --- BinaryEditor interface ---
    [[nodiscard]] TextSectionInfo text_section() const noexcept override;
    [[nodiscard]] uint64_t next_segment_va(uint64_t alignment) const noexcept override;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment(std::string_view name, const std::vector<uint8_t>& data,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] bool cfi_enforced() const noexcept override;

    [[nodiscard]] std::vector<TextGap>
    find_text_gaps(std::size_t min_size) const noexcept override;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    extend_text(const std::vector<uint8_t>& data,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                   Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    add_runtime_dep(std::string_view install_name,
                    Common::DiagnosticCollector& diag) noexcept override;

    void invalidate_signature() noexcept override;

    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    save(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept override;

private:
    PEEditor();
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_PE_EDITOR_HPP__
