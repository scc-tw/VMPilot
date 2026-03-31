#ifndef __LOADER_MACHO_EDITOR_HPP__
#define __LOADER_MACHO_EDITOR_HPP__
#pragma once

#include <BinaryEditor.hpp>
#include <MachOStructs.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Mach-O 64-bit binary editor. Implements BinaryEditor.
class MachOEditor : public BinaryEditor {
public:
    [[nodiscard]] static tl::expected<MachOEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    // --- BinaryEditor interface ---
    [[nodiscard]] TextSectionInfo text_section() const noexcept override;
    [[nodiscard]] uint64_t next_segment_va(uint64_t alignment) const noexcept override;

    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment(std::string_view name, const std::vector<uint8_t>& data,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept override;

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
