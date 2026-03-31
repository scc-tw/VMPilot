/// @file strategies/macho_dep_strategies.cpp
/// @brief MachO LC_LOAD_DYLIB injection strategies for FallbackChain.
///
/// Layer 1: UseHeaderPadding — inject LC_LOAD_DYLIB into existing header gap.
/// Layer 2: ShiftSectionData — expand header by shifting section data forward.

#include <strategies/macho_dep_strategies.hpp>
#include <MachOStructs.hpp>

#include <cstring>
#include <string>

namespace VMPilot::Loader::strategies {

using DC = Common::DiagnosticCode;
namespace MO = MachO;

/// Align `value` up to the nearest multiple of `align`.
static size_t align_up(size_t value, size_t align) noexcept {
    return (value + align - 1) & ~(align - 1);
}

/// Compute the size of an LC_LOAD_DYLIB command for a given install name.
static size_t dylib_cmd_size(std::string_view name) noexcept {
    size_t raw = sizeof(MO::dylib_command) + name.size() + 1;
    return align_up(raw, 8);
}

/// Write an LC_LOAD_DYLIB command at the given offset in the buffer.
static void write_dylib_cmd(std::vector<uint8_t>& buf, size_t offset,
                             size_t cmdsize, std::string_view name) noexcept {
    MO::dylib_command cmd{};
    cmd.cmd             = MO::LC_LOAD_DYLIB;
    cmd.cmdsize         = static_cast<uint32_t>(cmdsize);
    cmd.name_offset     = sizeof(MO::dylib_command);
    cmd.timestamp       = 0;
    cmd.current_version = 0x00010000;
    cmd.compat_version  = 0x00010000;

    std::memcpy(buf.data() + offset, &cmd, sizeof(cmd));

    size_t str_off = offset + sizeof(MO::dylib_command);
    std::memset(buf.data() + str_off, 0, cmdsize - sizeof(MO::dylib_command));
    std::memcpy(buf.data() + str_off, name.data(), name.size());
}

// ─────────────────────────────────────────────────────────────────────────────
// UseHeaderPadding
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<void, DC>
UseHeaderPadding::try_execute(
    Common::DiagnosticCollector& diag,
    std::vector<uint8_t>& buf,
    uint32_t& ncmds,
    uint32_t& sizeofcmds,
    size_t first_sect_off,
    std::string_view install_name) noexcept
{
    const size_t cmdsize = dylib_cmd_size(install_name);
    const size_t header_size = sizeof(MO::mach_header_64);
    const size_t lcmds_end = header_size + sizeofcmds;

    // Check available padding
    if (first_sect_off <= lcmds_end || (first_sect_off - lcmds_end) < cmdsize) {
        diag.note("UseHeaderPadding", DC::PatchRuntimeDepFailed,
                  "insufficient header padding for LC_LOAD_DYLIB ("
                  + std::to_string(first_sect_off > lcmds_end ? first_sect_off - lcmds_end : 0)
                  + " < " + std::to_string(cmdsize) + ")");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    write_dylib_cmd(buf, lcmds_end, cmdsize, install_name);

    ncmds += 1;
    sizeofcmds += static_cast<uint32_t>(cmdsize);

    // Update the in-memory mach_header_64
    MO::mach_header_64 hdr{};
    std::memcpy(&hdr, buf.data(), sizeof(hdr));
    hdr.ncmds = ncmds;
    hdr.sizeofcmds = sizeofcmds;
    std::memcpy(buf.data(), &hdr, sizeof(hdr));

    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
// ShiftSectionData
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<void, DC>
ShiftSectionData::try_execute(
    Common::DiagnosticCollector& diag,
    std::vector<uint8_t>& buf,
    uint32_t& ncmds,
    uint32_t& sizeofcmds,
    size_t first_sect_off,
    std::string_view install_name) noexcept
{
    const size_t cmdsize = dylib_cmd_size(install_name);
    const size_t header_size = sizeof(MO::mach_header_64);
    const size_t lcmds_end = header_size + sizeofcmds;
    const size_t padding = (first_sect_off > lcmds_end) ? (first_sect_off - lcmds_end) : 0;

    if (padding >= cmdsize) {
        // Shouldn't reach here if UseHeaderPadding already succeeded,
        // but handle gracefully
        return UseHeaderPadding::try_execute(diag, buf, ncmds, sizeofcmds,
                                              first_sect_off, install_name);
    }

    const size_t needed = cmdsize - padding;
    const size_t shift = align_up(needed, 16);

    diag.note("ShiftSectionData", DC::None,
              "expanding header by " + std::to_string(shift) + " bytes");

    // Insert zero bytes at first_sect_off
    buf.insert(buf.begin() + static_cast<ptrdiff_t>(first_sect_off),
               shift, 0x00);

    // Fix up all file offsets in load commands
    size_t lc_off = header_size;
    MO::mach_header_64 hdr{};
    std::memcpy(&hdr, buf.data(), sizeof(hdr));

    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        if (lc_off + sizeof(MO::load_command) > buf.size()) break;
        MO::load_command lc{};
        std::memcpy(&lc, buf.data() + lc_off, sizeof(lc));

        if (lc.cmd == MO::LC_SEGMENT_64 &&
            lc_off + sizeof(MO::segment_command_64) <= buf.size()) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, buf.data() + lc_off, sizeof(seg));
            if (seg.fileoff >= first_sect_off) {
                seg.fileoff += shift;
                std::memcpy(buf.data() + lc_off, &seg, sizeof(seg));
            }
            // Fix up sections within segment
            size_t sec_off = lc_off + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                if (sec_off + sizeof(MO::section_64) > buf.size()) break;
                MO::section_64 sec{};
                std::memcpy(&sec, buf.data() + sec_off, sizeof(sec));
                if (sec.offset >= first_sect_off) {
                    sec.offset += static_cast<uint32_t>(shift);
                    std::memcpy(buf.data() + sec_off, &sec, sizeof(sec));
                }
                sec_off += sizeof(MO::section_64);
            }
        }
        lc_off += lc.cmdsize;
    }

    // Now retry with the expanded padding
    size_t new_first_sect_off = first_sect_off + shift;
    return UseHeaderPadding::try_execute(diag, buf, ncmds, sizeofcmds,
                                          new_first_sect_off, install_name);
}

}  // namespace VMPilot::Loader::strategies
