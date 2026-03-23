#include <ELFPatcherTraits.hpp>
#include <PatcherTraits.hpp>

#include <elfio/elfio.hpp>

#include <cstring>
#include <vector>

using ELFIO::PT_LOAD;
using ELFIO::SHT_PROGBITS;
using ELFIO::SHF_ALLOC;
using ELFIO::SHF_EXECINSTR;
using ELFIO::PF_R;
using ELFIO::PF_X;

namespace VMPilot::Loader {

// --- Opaque context holding the ELFIO reader ---

struct ELFBinaryContext : BinaryContext {
    ELFIO::elfio reader;
    ELFIO::section* text_sec = nullptr;
    uint64_t new_seg_va = 0;
};

// --- ELFPatcherTraits implementation ---

tl::expected<std::unique_ptr<BinaryContext>, Common::DiagnosticCode>
ELFPatcherTraits::open(const std::string& path,
                       Common::DiagnosticCollector& diag) noexcept {
    auto ctx = std::make_unique<ELFBinaryContext>();
    if (!ctx->reader.load(path)) {
        diag.error("loader", Common::DiagnosticCode::PatchBinaryReadFailed,
                   "failed to load ELF: " + path);
        return tl::unexpected(Common::DiagnosticCode::PatchBinaryReadFailed);
    }
    // Cache .text section
    for (auto& sec : ctx->reader.sections) {
        if (sec->get_name() == ".text") {
            ctx->text_sec = sec.get();
            break;
        }
    }
    if (!ctx->text_sec) {
        diag.error("loader", Common::DiagnosticCode::PatchBinaryReadFailed,
                   "no .text section found");
        return tl::unexpected(Common::DiagnosticCode::PatchBinaryReadFailed);
    }
    return ctx;
}

tl::expected<TextSectionInfo, Common::DiagnosticCode>
ELFPatcherTraits::get_text_section(BinaryContext& ctx_base,
                                   Common::DiagnosticCollector& /*diag*/) noexcept {
    auto& ctx = static_cast<ELFBinaryContext&>(ctx_base);
    return TextSectionInfo{ctx.text_sec->get_address(), ctx.text_sec->get_size()};
}

tl::expected<void, Common::DiagnosticCode>
ELFPatcherTraits::overwrite_text(BinaryContext& ctx_base, uint64_t va,
                                 const uint8_t* data, size_t len,
                                 Common::DiagnosticCollector& diag) noexcept {
    auto& ctx = static_cast<ELFBinaryContext&>(ctx_base);
    auto* sec = ctx.text_sec;

    const uint64_t sec_addr = sec->get_address();
    const uint64_t sec_size = sec->get_size();
    if (va < sec_addr || va + len > sec_addr + sec_size) {
        diag.error("loader", Common::DiagnosticCode::PatchSegmentCreationFailed,
                   "VA 0x" + std::to_string(va) + " outside .text");
        return tl::unexpected(Common::DiagnosticCode::PatchSegmentCreationFailed);
    }

    const size_t offset = static_cast<size_t>(va - sec_addr);
    std::vector<uint8_t> sec_data(sec->get_data(),
                                   sec->get_data() + sec->get_size());
    std::memcpy(sec_data.data() + offset, data, len);
    sec->set_data(reinterpret_cast<const char*>(sec_data.data()), sec_data.size());
    return {};
}

tl::expected<NewSegmentInfo, Common::DiagnosticCode>
ELFPatcherTraits::add_segment(BinaryContext& ctx_base, const std::string& name,
                              const std::vector<uint8_t>& payload,
                              uint64_t alignment,
                              Common::DiagnosticCollector& /*diag*/) noexcept {
    auto& ctx = static_cast<ELFBinaryContext&>(ctx_base);

    // Compute next available VA
    uint64_t highest = 0;
    for (const auto& seg : ctx.reader.segments) {
        if (seg->get_type() == PT_LOAD) {
            const uint64_t end = seg->get_virtual_address() + seg->get_memory_size();
            if (end > highest) highest = end;
        }
    }
    const uint64_t seg_va = (highest + alignment - 1) & ~(alignment - 1);
    ctx.new_seg_va = seg_va;

    auto* new_sec = ctx.reader.sections.add(name);
    new_sec->set_type(SHT_PROGBITS);
    new_sec->set_flags(SHF_ALLOC | SHF_EXECINSTR);
    new_sec->set_addr_align(16);
    new_sec->set_address(seg_va);
    new_sec->set_data(reinterpret_cast<const char*>(payload.data()), payload.size());

    auto* new_seg = ctx.reader.segments.add();
    new_seg->set_type(PT_LOAD);
    new_seg->set_flags(PF_R | PF_X);
    new_seg->set_align(alignment);
    new_seg->set_virtual_address(seg_va);
    new_seg->set_physical_address(seg_va);
    new_seg->add_section_index(new_sec->get_index(), new_sec->get_addr_align());

    return NewSegmentInfo{seg_va, payload.size()};
}

tl::expected<void, Common::DiagnosticCode>
ELFPatcherTraits::save(BinaryContext& ctx_base, const std::string& path,
                       Common::DiagnosticCollector& diag) noexcept {
    auto& ctx = static_cast<ELFBinaryContext&>(ctx_base);
    if (!ctx.reader.save(path)) {
        diag.error("loader", Common::DiagnosticCode::PatchBinaryWriteFailed,
                   "failed to write ELF: " + path);
        return tl::unexpected(Common::DiagnosticCode::PatchBinaryWriteFailed);
    }
    return {};
}

}  // namespace VMPilot::Loader
