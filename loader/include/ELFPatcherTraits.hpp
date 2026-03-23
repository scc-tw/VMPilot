#ifndef __LOADER_ELF_PATCHER_TRAITS_HPP__
#define __LOADER_ELF_PATCHER_TRAITS_HPP__
#pragma once

#include <PatcherTraits.hpp>

#include <memory>

namespace VMPilot::Loader {

/// ELF-specific patcher traits using ELFIO.
struct ELFPatcherTraits {
    [[nodiscard]] static tl::expected<std::unique_ptr<BinaryContext>, Common::DiagnosticCode>
    open(const std::string& path, Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] static tl::expected<TextSectionInfo, Common::DiagnosticCode>
    get_text_section(BinaryContext& ctx, Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    overwrite_text(BinaryContext& ctx, uint64_t va, const uint8_t* data, size_t len,
                   Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] static tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment(BinaryContext& ctx, const std::string& name,
                const std::vector<uint8_t>& payload, uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept;

    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    save(BinaryContext& ctx, const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_ELF_PATCHER_TRAITS_HPP__
