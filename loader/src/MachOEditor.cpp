#include <MachOEditor.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <functional>
#include <numeric>
#include <optional>

namespace VMPilot::Loader {

namespace MO = MachO;
using DC = Common::DiagnosticCode;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static tl::unexpected<DC> fail(Common::DiagnosticCollector& diag, DC code,
                               const std::string& msg) {
    diag.error("loader", code, msg);
    return tl::unexpected(code);
}

static void fill_name(char (&dst)[16], std::string_view src) {
    std::memset(dst, 0, sizeof(dst));
    std::memcpy(dst, src.data(), std::min(src.size(), size_t{15}));
}

static uint64_t align_up(uint64_t val, uint64_t align) {
    return (val + align - 1) & ~(align - 1);
}

/// Invoke visitor(offset, segment_command_64) for each LC_SEGMENT_64.
using SegVisitor = std::function<void(size_t, const MO::segment_command_64&)>;

static void for_each_segment(const std::vector<uint8_t>& buf,
                             const MO::mach_header_64& hdr,
                             const SegVisitor& visitor) {
    size_t off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        if (off + sizeof(MO::load_command) > buf.size()) break;
        MO::load_command lc{};
        std::memcpy(&lc, buf.data() + off, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64 &&
            off + sizeof(MO::segment_command_64) <= buf.size()) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, buf.data() + off, sizeof(seg));
            visitor(off, seg);
        }
        off += lc.cmdsize;
    }
}

/// Find a section_64 within a segment by (segname, sectname).
static std::optional<MO::section_64>
find_section(const std::vector<uint8_t>& buf, size_t seg_off,
             const MO::segment_command_64& seg,
             std::string_view segname, std::string_view sectname) {
    size_t off = seg_off + sizeof(MO::segment_command_64);
    for (uint32_t s = 0; s < seg.nsects; ++s) {
        if (off + sizeof(MO::section_64) > buf.size()) break;
        MO::section_64 sec{};
        std::memcpy(&sec, buf.data() + off, sizeof(sec));
        if (std::string_view{sec.segname, strnlen(sec.segname, 16)} == segname &&
            std::string_view{sec.sectname, strnlen(sec.sectname, 16)} == sectname)
            return sec;
        off += sizeof(MO::section_64);
    }
    return std::nullopt;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

uint64_t MachOEditor::highest_va() const noexcept {
    return std::accumulate(
        segments_.begin(), segments_.end(), uint64_t{0},
        [](uint64_t hi, const SegEntry& s) {
            return std::max(hi, s.vmaddr + s.vmsize);
        });
}

size_t MachOEditor::header_padding() const noexcept {
    return (first_sect_off_ > lcmds_end_)
        ? (first_sect_off_ - lcmds_end_) : 0;
}

uint64_t MachOEditor::next_segment_va(uint64_t alignment) const noexcept {
    return (highest_va() + alignment - 1) & ~(alignment - 1);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

tl::expected<MachOEditor, DC>
MachOEditor::open(const std::string& path,
                  Common::DiagnosticCollector& diag) noexcept {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs) return fail(diag, DC::PatchBinaryReadFailed, "cannot open: " + path);

    MachOEditor ed;
    ed.buf_.resize(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(ed.buf_.data()),
             static_cast<std::streamsize>(ed.buf_.size()));

    if (ed.buf_.size() < sizeof(MO::mach_header_64))
        return fail(diag, DC::PatchBinaryReadFailed, "file too small for mach_header_64");

    ed.header_ = ed.read_at<MO::mach_header_64>(0);
    if (ed.header_.magic != MO::MH_MAGIC_64)
        return fail(diag, DC::PatchBinaryReadFailed, "not a 64-bit Mach-O");

    ed.lcmds_end_ = sizeof(MO::mach_header_64) + ed.header_.sizeofcmds;
    ed.first_sect_off_ = static_cast<uint32_t>(ed.buf_.size());
    bool found_text = false;

    for_each_segment(ed.buf_, ed.header_,
        [&](size_t seg_off, const MO::segment_command_64& seg) {
            ed.segments_.push_back({seg.vmaddr, seg.vmsize});

            // Track earliest section file offset for padding calculation
            size_t sec_off = seg_off + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                if (sec_off + sizeof(MO::section_64) > ed.buf_.size()) break;
                auto sec = ed.read_at<MO::section_64>(sec_off);
                if (sec.offset > 0 && sec.offset < ed.first_sect_off_)
                    ed.first_sect_off_ = sec.offset;
                sec_off += sizeof(MO::section_64);
            }

            if (auto sec = find_section(ed.buf_, seg_off, seg, "__TEXT", "__text")) {
                ed.text_va_ = sec->addr;
                ed.text_size_ = sec->size;
                ed.text_file_off_ = sec->offset;
                found_text = true;
            }
        });

    if (!found_text)
        return fail(diag, DC::PatchBinaryReadFailed, "no __TEXT,__text section");

    return ed;
}

TextSectionInfo MachOEditor::text_section() const noexcept {
    return {text_va_, text_size_};
}

tl::expected<void, DC>
MachOEditor::overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                            Common::DiagnosticCollector& diag) noexcept {
    if (va < text_va_ || va + len > text_va_ + text_size_)
        return fail(diag, DC::PatchSegmentCreationFailed, "VA outside __text");

    const size_t off = text_file_off_ + static_cast<size_t>(va - text_va_);
    if (off + len > buf_.size())
        return fail(diag, DC::PatchSegmentCreationFailed, "file offset out of bounds");

    std::memcpy(buf_.data() + off, data, len);
    return {};
}

tl::expected<NewSegmentInfo, DC>
MachOEditor::add_segment(std::string_view name,
                         const std::vector<uint8_t>& payload,
                         uint64_t alignment,
                         Common::DiagnosticCollector& diag) noexcept {
    constexpr size_t LC_SIZE = sizeof(MO::segment_command_64) + sizeof(MO::section_64);

    if (header_padding() < LC_SIZE)
        return fail(diag, DC::PatchSegmentCreationFailed,
                    "not enough header padding (" + std::to_string(header_padding())
                    + " < " + std::to_string(LC_SIZE) + ")");

    const uint64_t seg_va     = align_up(highest_va(), alignment);
    const uint64_t seg_vmsize = align_up(payload.size(), alignment);
    const uint64_t file_off   = buf_.size();

    // Build segment command + section
    MO::segment_command_64 seg_cmd{};
    seg_cmd.cmd      = MO::LC_SEGMENT_64;
    seg_cmd.cmdsize  = static_cast<uint32_t>(LC_SIZE);
    fill_name(seg_cmd.segname, "__VMPILOT");
    seg_cmd.vmaddr   = seg_va;
    seg_cmd.vmsize   = seg_vmsize;
    seg_cmd.fileoff  = file_off;
    seg_cmd.filesize = payload.size();
    // maxprot includes X so the runtime constructor can mprotect to RX
    // after writing call_slot.  initprot is RW — the section starts
    // writable (for call_slot init) and NOT executable, maintaining W^X.
    seg_cmd.maxprot  = MO::VM_PROT_READ | MO::VM_PROT_WRITE | MO::VM_PROT_EXECUTE;
    seg_cmd.initprot = MO::VM_PROT_READ | MO::VM_PROT_WRITE;
    seg_cmd.nsects   = 1;

    MO::section_64 sec{};
    fill_name(sec.sectname, name);
    fill_name(sec.segname, "__VMPILOT");
    sec.addr   = seg_va;
    sec.size   = payload.size();
    sec.offset = static_cast<uint32_t>(file_off);
    sec.align  = 4;  // 2^4 = 16

    // Insert load command into header padding
    write_at(lcmds_end_, seg_cmd);
    write_at(lcmds_end_ + sizeof(seg_cmd), sec);

    // Update header
    header_.ncmds     += 1;
    header_.sizeofcmds += static_cast<uint32_t>(LC_SIZE);
    write_at(0, header_);

    lcmds_end_ += LC_SIZE;
    segments_.push_back({seg_va, seg_vmsize});

    // Append payload + alignment padding
    buf_.insert(buf_.end(), payload.begin(), payload.end());
    buf_.resize(align_up(buf_.size(), alignment), 0);

    return NewSegmentInfo{seg_va, payload.size()};
}

// ---------------------------------------------------------------------------
// find_text_gaps
// ---------------------------------------------------------------------------

/// Scan __text for consecutive filler instructions that can be
/// repurposed as code caves.  Detects:
///   - ARM64 NOP (0xD503201F) — alignment padding
///   - ARM64 BRK #0 (0xD4200000) — unreachable code marker
///   - Zero words (0x00000000) — linker fill
/// Returns gaps >= min_size, sorted by size descending.
std::vector<TextGap>
MachOEditor::find_text_gaps(std::size_t min_size) const noexcept {
    if (text_size_ == 0 || text_file_off_ + text_size_ > buf_.size())
        return {};

    const uint8_t* data = buf_.data() + text_file_off_;
    const size_t text_len = static_cast<size_t>(text_size_);

    // ARM64 filler detection (4-byte aligned)
    constexpr uint32_t ARM64_NOP = 0xD503201F;
    constexpr uint32_t ARM64_BRK = 0xD4200000;

    auto is_filler_word = [](uint32_t w) -> bool {
        return w == ARM64_NOP || w == ARM64_BRK || w == 0;
    };

    std::vector<TextGap> gaps;
    size_t i = 0;
    while (i + 3 < text_len) {
        uint32_t word;
        std::memcpy(&word, data + i, 4);
        if (!is_filler_word(word)) { i += 4; continue; }

        // Found start of a filler run — measure length
        const uint32_t filler = word;
        const size_t start = i;
        while (i + 3 < text_len) {
            std::memcpy(&word, data + i, 4);
            if (word != filler) break;
            i += 4;
        }
        const size_t run_len = i - start;

        if (run_len >= min_size)
            gaps.push_back({text_va_ + start, run_len});
    }

    std::sort(gaps.begin(), gaps.end(),
              [](const TextGap& a, const TextGap& b) { return a.size > b.size; });

    return gaps;
}

// ---------------------------------------------------------------------------
// extend_text
// ---------------------------------------------------------------------------

tl::expected<NewSegmentInfo, DC>
MachOEditor::extend_text(const std::vector<uint8_t>& data,
                         uint64_t alignment,
                         Common::DiagnosticCollector& diag) noexcept {
    // Find __TEXT segment and __text section in the load commands.
    size_t text_seg_off = 0;
    bool found_seg = false;
    size_t text_sec_off = 0;
    bool found_sec = false;

    size_t off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < header_.ncmds; ++i) {
        if (off + sizeof(MO::load_command) > buf_.size()) break;
        MO::load_command lc{};
        std::memcpy(&lc, buf_.data() + off, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64 &&
            off + sizeof(MO::segment_command_64) <= buf_.size()) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, buf_.data() + off, sizeof(seg));
            if (std::string_view{seg.segname, strnlen(seg.segname, 16)} == "__TEXT") {
                text_seg_off = off;
                found_seg = true;

                size_t sec_off = off + sizeof(MO::segment_command_64);
                for (uint32_t s = 0; s < seg.nsects; ++s) {
                    if (sec_off + sizeof(MO::section_64) > buf_.size()) break;
                    MO::section_64 sec{};
                    std::memcpy(&sec, buf_.data() + sec_off, sizeof(sec));
                    if (std::string_view{sec.sectname, strnlen(sec.sectname, 16)} == "__text") {
                        text_sec_off = sec_off;
                        found_sec = true;
                        break;
                    }
                    sec_off += sizeof(MO::section_64);
                }
                break;
            }
        }
        off += lc.cmdsize;
    }

    if (!found_seg || !found_sec)
        return fail(diag, DC::PatchSegmentCreationFailed,
                    "cannot find __TEXT segment or __text section for extend_text");

    auto seg_cmd = read_at<MO::segment_command_64>(text_seg_off);
    auto sec_cmd = read_at<MO::section_64>(text_sec_off);

    const uint64_t orig_text_end_va = text_va_ + text_size_;
    const uint64_t new_data_va = align_up(orig_text_end_va, alignment);
    const uint64_t padding = new_data_va - orig_text_end_va;
    const uint64_t growth = padding + data.size();

    // Insert padding + data into the buffer at the end of __text's file data.
    const size_t orig_text_file_end = text_file_off_ + static_cast<size_t>(text_size_);

    std::vector<uint8_t> insert_data(static_cast<size_t>(growth), 0x00);
    std::memcpy(insert_data.data() + static_cast<size_t>(padding),
                data.data(), data.size());
    buf_.insert(buf_.begin() + static_cast<ptrdiff_t>(orig_text_file_end),
                insert_data.begin(), insert_data.end());

    // Update __text section and __TEXT segment headers
    sec_cmd.size += growth;
    write_at(text_sec_off, sec_cmd);

    seg_cmd.vmsize   = align_up(seg_cmd.vmsize + growth, alignment);
    seg_cmd.filesize += growth;
    write_at(text_seg_off, seg_cmd);

    text_size_ += growth;

    for (auto& s : segments_) {
        if (s.vmaddr == seg_cmd.vmaddr) {
            s.vmsize = seg_cmd.vmsize;
            break;
        }
    }

    // Fix up file offsets for sections that come after __text in the file.
    size_t lc_off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < header_.ncmds; ++i) {
        if (lc_off + sizeof(MO::load_command) > buf_.size()) break;
        auto lc = read_at<MO::load_command>(lc_off);
        if (lc.cmd == MO::LC_SEGMENT_64 &&
            lc_off + sizeof(MO::segment_command_64) <= buf_.size()) {
            auto seg = read_at<MO::segment_command_64>(lc_off);
            if (seg.fileoff > orig_text_file_end) {
                seg.fileoff += growth;
                write_at(lc_off, seg);
            }
            size_t sec_off = lc_off + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                if (sec_off + sizeof(MO::section_64) > buf_.size()) break;
                auto sec = read_at<MO::section_64>(sec_off);
                if (sec.offset > orig_text_file_end && sec_off != text_sec_off) {
                    sec.offset += static_cast<uint32_t>(growth);
                    write_at(sec_off, sec);
                }
                sec_off += sizeof(MO::section_64);
            }
        }
        lc_off += lc.cmdsize;
    }

    return NewSegmentInfo{new_data_va, data.size()};
}

tl::expected<void, DC>
MachOEditor::add_runtime_dep(std::string_view install_name,
                       Common::DiagnosticCollector& diag) noexcept {
    // LC_LOAD_DYLIB: dylib_command header (24 bytes) + null-terminated string,
    // total size aligned to 8 bytes.
    const size_t name_len = install_name.size() + 1;  // include null terminator
    const size_t raw_size = sizeof(MO::dylib_command) + name_len;
    const size_t cmdsize = align_up(raw_size, 8);

    if (header_padding() < cmdsize)
        return fail(diag, DC::PatchSegmentCreationFailed,
                    "not enough header padding for LC_LOAD_DYLIB ("
                    + std::to_string(header_padding()) + " < "
                    + std::to_string(cmdsize) + ")");

    // Build the dylib_command
    MO::dylib_command cmd{};
    cmd.cmd             = MO::LC_LOAD_DYLIB;
    cmd.cmdsize         = static_cast<uint32_t>(cmdsize);
    cmd.name_offset     = sizeof(MO::dylib_command);  // string starts right after header
    cmd.timestamp       = 0;
    cmd.current_version = 0x00010000;   // 1.0.0
    cmd.compat_version  = 0x00010000;   // 1.0.0

    // Write the command header into header padding
    write_at(lcmds_end_, cmd);

    // Write the install name string (null-terminated) after the header
    const size_t str_off = lcmds_end_ + sizeof(MO::dylib_command);
    // Zero the entire region first (for alignment padding)
    std::memset(buf_.data() + lcmds_end_ + sizeof(MO::dylib_command), 0,
                cmdsize - sizeof(MO::dylib_command));
    std::memcpy(buf_.data() + str_off, install_name.data(), install_name.size());
    // null terminator is already set by memset

    // Update header
    header_.ncmds     += 1;
    header_.sizeofcmds += static_cast<uint32_t>(cmdsize);
    write_at(0, header_);

    lcmds_end_ += cmdsize;

    diag.note("loader", DC::None,
              "added LC_LOAD_DYLIB: " + std::string(install_name));

    return {};
}

void MachOEditor::invalidate_signature() noexcept {
    size_t off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < header_.ncmds; ++i) {
        if (off + sizeof(MO::load_command) > buf_.size()) break;
        auto lc = read_at<MO::load_command>(off);
        if (lc.cmd == MO::LC_CODE_SIGNATURE) {
            auto cs = read_at<MO::linkedit_data_command>(off);
            // Truncate signature data if at EOF
            if (cs.dataoff + cs.datasize == buf_.size())
                buf_.resize(cs.dataoff);
            // Zero dataoff/datasize but keep load command in chain
            cs.dataoff = 0;
            cs.datasize = 0;
            write_at(off, cs);
            return;
        }
        off += lc.cmdsize;
    }
}

tl::expected<void, DC>
MachOEditor::save(const std::string& path,
                  Common::DiagnosticCollector& diag) noexcept {
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    if (!ofs) return fail(diag, DC::PatchBinaryWriteFailed, "cannot write: " + path);

    ofs.write(reinterpret_cast<const char*>(buf_.data()),
              static_cast<std::streamsize>(buf_.size()));
    return {};
}

}  // namespace VMPilot::Loader
