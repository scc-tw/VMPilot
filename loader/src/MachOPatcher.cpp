#include <MachOPatcherTraits.hpp>
#include <MachOStructs.hpp>
#include <PatcherTraits.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <functional>
#include <numeric>
#include <string_view>

namespace VMPilot::Loader {

namespace MO = MachO;
using DC = Common::DiagnosticCode;

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

/// Type-safe read of a POD struct from a byte buffer at a given offset.
template <typename T>
[[nodiscard]] static T read_at(const std::vector<uint8_t>& buf, size_t offset) {
    T val{};
    std::memcpy(&val, buf.data() + offset, sizeof(T));
    return val;
}

/// Type-safe write of a POD struct into a byte buffer at a given offset.
template <typename T>
static void write_at(std::vector<uint8_t>& buf, size_t offset, const T& val) {
    std::memcpy(buf.data() + offset, &val, sizeof(T));
}

/// Zero-padded copy into a fixed-size char[16] field.
static void fill_name(char (&dst)[16], std::string_view src) {
    std::memset(dst, 0, sizeof(dst));
    const auto n = std::min(src.size(), size_t{15});
    std::memcpy(dst, src.data(), n);
}

/// Page-align upward.
[[nodiscard]] static uint64_t align_up(uint64_t val, uint64_t align) {
    return (val + align - 1) & ~(align - 1);
}

/// Emit a diagnostic error and return unexpected.
[[nodiscard]] static tl::unexpected<DC>
fail(Common::DiagnosticCollector& diag, DC code, std::string_view msg) {
    diag.error("loader", code, std::string{msg});
    return tl::unexpected(code);
}

// ---------------------------------------------------------------------------
// Mach-O context
// ---------------------------------------------------------------------------

struct MachOBinaryContext : BinaryContext {
    std::vector<uint8_t> buf;

    MO::mach_header_64 header{};
    uint64_t text_va           = 0;
    uint64_t text_size         = 0;
    uint32_t text_file_off     = 0;
    size_t   lcmds_end         = 0;   // byte offset past last load command
    uint32_t first_sect_offset = 0;   // file offset of earliest section data

    struct Segment { uint64_t vmaddr, vmsize; };
    std::vector<Segment> segments;

    [[nodiscard]] uint64_t highest_va() const {
        return std::accumulate(
            segments.begin(), segments.end(), uint64_t{0},
            [](uint64_t hi, const Segment& s) {
                return std::max(hi, s.vmaddr + s.vmsize);
            });
    }

    /// Available bytes between end of load commands and first section data.
    [[nodiscard]] size_t header_padding() const {
        return (first_sect_offset > lcmds_end)
            ? (first_sect_offset - lcmds_end) : 0;
    }
};

/// Downcast helper — avoids repeating static_cast everywhere.
[[nodiscard]] static MachOBinaryContext& as(BinaryContext& base) {
    return static_cast<MachOBinaryContext&>(base);
}

// ---------------------------------------------------------------------------
// Load command walker
// ---------------------------------------------------------------------------

using LcVisitor = std::function<void(size_t offset, const MO::segment_command_64& seg)>;

/// Walk LC_SEGMENT_64 commands, invoking visitor for each.
static bool for_each_segment(const std::vector<uint8_t>& buf,
                             const MO::mach_header_64& hdr,
                             const LcVisitor& visitor) {
    size_t offset = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        if (offset + sizeof(MO::load_command) > buf.size()) return false;
        auto lc = read_at<MO::load_command>(buf, offset);
        if (lc.cmdsize < sizeof(MO::load_command)) return false;

        if (lc.cmd == MO::LC_SEGMENT_64 &&
            offset + sizeof(MO::segment_command_64) <= buf.size()) {
            visitor(offset, read_at<MO::segment_command_64>(buf, offset));
        }
        offset += lc.cmdsize;
    }
    return true;
}

/// Find a section within a segment command by name pair.
static std::optional<MO::section_64>
find_section_in_segment(const std::vector<uint8_t>& buf, size_t seg_offset,
                        const MO::segment_command_64& seg,
                        std::string_view segname, std::string_view sectname) {
    size_t sec_off = seg_offset + sizeof(MO::segment_command_64);
    for (uint32_t s = 0; s < seg.nsects; ++s) {
        if (sec_off + sizeof(MO::section_64) > buf.size()) break;
        auto sec = read_at<MO::section_64>(buf, sec_off);
        if (std::string_view{sec.segname, strnlen(sec.segname, 16)} == segname &&
            std::string_view{sec.sectname, strnlen(sec.sectname, 16)} == sectname) {
            return sec;
        }
        sec_off += sizeof(MO::section_64);
    }
    return std::nullopt;
}

// ---------------------------------------------------------------------------
// Traits implementation
// ---------------------------------------------------------------------------

tl::expected<std::unique_ptr<BinaryContext>, DC>
MachOPatcherTraits::open(const std::string& path,
                         Common::DiagnosticCollector& diag) noexcept {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs) return fail(diag, DC::PatchBinaryReadFailed, "cannot open: " + path);

    auto ctx = std::make_unique<MachOBinaryContext>();
    ctx->buf.resize(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(ctx->buf.data()),
             static_cast<std::streamsize>(ctx->buf.size()));

    if (ctx->buf.size() < sizeof(MO::mach_header_64))
        return fail(diag, DC::PatchBinaryReadFailed, "file too small for mach_header_64");

    ctx->header = read_at<MO::mach_header_64>(ctx->buf, 0);
    if (ctx->header.magic != MO::MH_MAGIC_64)
        return fail(diag, DC::PatchBinaryReadFailed, "not a 64-bit Mach-O");

    ctx->lcmds_end = sizeof(MO::mach_header_64) + ctx->header.sizeofcmds;
    ctx->first_sect_offset = static_cast<uint32_t>(ctx->buf.size());  // sentinel
    bool found = false;

    for_each_segment(ctx->buf, ctx->header,
        [&](size_t offset, const MO::segment_command_64& seg) {
            ctx->segments.push_back({seg.vmaddr, seg.vmsize});

            // Walk all sections to find earliest file offset
            size_t sec_off = offset + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                if (sec_off + sizeof(MO::section_64) > ctx->buf.size()) break;
                auto sec = read_at<MO::section_64>(ctx->buf, sec_off);
                if (sec.offset > 0 && sec.offset < ctx->first_sect_offset)
                    ctx->first_sect_offset = sec.offset;
                sec_off += sizeof(MO::section_64);
            }

            if (auto sec = find_section_in_segment(
                    ctx->buf, offset, seg, "__TEXT", "__text")) {
                ctx->text_va       = sec->addr;
                ctx->text_size     = sec->size;
                ctx->text_file_off = sec->offset;
                found = true;
            }
        });

    if (!found)
        return fail(diag, DC::PatchBinaryReadFailed, "no __TEXT,__text section");

    return ctx;
}

tl::expected<TextSectionInfo, DC>
MachOPatcherTraits::get_text_section(BinaryContext& ctx_base,
                                     Common::DiagnosticCollector&) noexcept {
    auto& ctx = as(ctx_base);
    return TextSectionInfo{ctx.text_va, ctx.text_size};
}

tl::expected<void, DC>
MachOPatcherTraits::overwrite_text(BinaryContext& ctx_base, uint64_t va,
                                   const uint8_t* data, size_t len,
                                   Common::DiagnosticCollector& diag) noexcept {
    auto& ctx = as(ctx_base);

    if (va < ctx.text_va || va + len > ctx.text_va + ctx.text_size)
        return fail(diag, DC::PatchSegmentCreationFailed, "VA outside __text");

    const size_t off = ctx.text_file_off + static_cast<size_t>(va - ctx.text_va);
    if (off + len > ctx.buf.size())
        return fail(diag, DC::PatchSegmentCreationFailed, "file offset out of bounds");

    std::memcpy(ctx.buf.data() + off, data, len);
    return {};
}

tl::expected<NewSegmentInfo, DC>
MachOPatcherTraits::add_segment(BinaryContext& ctx_base, const std::string& name,
                                const std::vector<uint8_t>& payload,
                                uint64_t alignment,
                                Common::DiagnosticCollector& diag) noexcept {
    auto& ctx = as(ctx_base);

    constexpr size_t LC_SIZE = sizeof(MO::segment_command_64) + sizeof(MO::section_64);

    if (ctx.header_padding() < LC_SIZE)
        return fail(diag, DC::PatchSegmentCreationFailed,
                    "not enough header padding (" + std::to_string(ctx.header_padding())
                    + " < " + std::to_string(LC_SIZE) + ")");

    const uint64_t seg_va     = align_up(ctx.highest_va(), alignment);
    const uint64_t seg_vmsize = align_up(payload.size(), alignment);
    const uint64_t file_off   = ctx.buf.size();

    // Build segment + section
    MO::segment_command_64 seg_cmd{};
    seg_cmd.cmd      = MO::LC_SEGMENT_64;
    seg_cmd.cmdsize  = static_cast<uint32_t>(LC_SIZE);
    fill_name(seg_cmd.segname, "__VMPILOT");
    seg_cmd.vmaddr   = seg_va;
    seg_cmd.vmsize   = seg_vmsize;
    seg_cmd.fileoff  = file_off;
    seg_cmd.filesize = payload.size();
    seg_cmd.maxprot  = MO::VM_PROT_READ | MO::VM_PROT_EXECUTE;
    seg_cmd.initprot = MO::VM_PROT_READ | MO::VM_PROT_EXECUTE;
    seg_cmd.nsects   = 1;

    MO::section_64 sec{};
    fill_name(sec.sectname, name);
    fill_name(sec.segname, "__VMPILOT");
    sec.addr   = seg_va;
    sec.size   = payload.size();
    sec.offset = static_cast<uint32_t>(file_off);
    sec.align  = 4;  // 2^4 = 16

    // Write load command into header padding
    write_at(ctx.buf, ctx.lcmds_end, seg_cmd);
    write_at(ctx.buf, ctx.lcmds_end + sizeof(seg_cmd), sec);

    // Update header
    ctx.header.ncmds     += 1;
    ctx.header.sizeofcmds += static_cast<uint32_t>(LC_SIZE);
    write_at(ctx.buf, 0, ctx.header);

    // Update tracked state
    ctx.lcmds_end += LC_SIZE;
    ctx.segments.push_back({seg_va, seg_vmsize});

    // Append payload + padding
    ctx.buf.insert(ctx.buf.end(), payload.begin(), payload.end());
    ctx.buf.resize(align_up(ctx.buf.size(), alignment), 0);

    return NewSegmentInfo{seg_va, payload.size()};
}

tl::expected<void, DC>
MachOPatcherTraits::save(BinaryContext& ctx_base, const std::string& path,
                         Common::DiagnosticCollector& diag) noexcept {
    auto& ctx = as(ctx_base);

    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    if (!ofs) return fail(diag, DC::PatchBinaryWriteFailed, "cannot write: " + path);

    ofs.write(reinterpret_cast<const char*>(ctx.buf.data()),
              static_cast<std::streamsize>(ctx.buf.size()));
    return {};
}

}  // namespace VMPilot::Loader
