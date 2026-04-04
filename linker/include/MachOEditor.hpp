#ifndef __LOADER_MACHO_EDITOR_HPP__
#define __LOADER_MACHO_EDITOR_HPP__
#pragma once

#include <editor_base.hpp>
#include <MachOStructs.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Mach-O 64-bit binary editor. Implements EditorBase via CRTP.
class MachOEditor : public EditorBase<MachOEditor> {
    friend class EditorBase<MachOEditor>;
public:
    [[nodiscard]] static tl::expected<MachOEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    // --- EditorBase CRTP interface ---
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
    MachOEditor() = default;

    template <typename T>
    [[nodiscard]] T read_at(size_t offset) const {
        T val{};
        std::memcpy(&val, buf_.data() + offset, sizeof(T));
        return val;
    }
    template <typename T>
    void write_at(size_t offset, const T& val) {
        std::memcpy(buf_.data() + offset, &val, sizeof(T));
    }

    std::vector<uint8_t> buf_;
    MachO::mach_header_64 header_{};
    uint64_t text_va_       = 0;
    uint64_t text_size_     = 0;
    uint32_t text_file_off_ = 0;
    size_t   lcmds_end_     = 0;
    uint32_t first_sect_off_= 0;

    struct SegEntry { uint64_t vmaddr, vmsize; };
    std::vector<SegEntry> segments_;

    [[nodiscard]] uint64_t highest_va() const noexcept;
    [[nodiscard]] size_t header_padding() const noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_MACHO_EDITOR_HPP__
