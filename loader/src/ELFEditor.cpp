#include <ELFEditor.hpp>

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

using DC = Common::DiagnosticCode;

static tl::unexpected<DC> fail(Common::DiagnosticCollector& diag, DC code,
                               const std::string& msg) {
    diag.error("loader", code, msg);
    return tl::unexpected(code);
}

// ---------------------------------------------------------------------------
// Impl
// ---------------------------------------------------------------------------

struct ELFEditor::Impl {
    ELFIO::elfio reader;
    ELFIO::section* text_sec = nullptr;
    uint64_t text_va   = 0;
    uint64_t text_size = 0;
};

ELFEditor::ELFEditor() : impl_(std::make_unique<Impl>()) {}
ELFEditor::~ELFEditor() = default;
ELFEditor::ELFEditor(ELFEditor&&) noexcept = default;
ELFEditor& ELFEditor::operator=(ELFEditor&&) noexcept = default;

// ---------------------------------------------------------------------------
// open
// ---------------------------------------------------------------------------

tl::expected<ELFEditor, DC>
ELFEditor::open(const std::string& path,
                Common::DiagnosticCollector& diag) noexcept {
    ELFEditor ed;
    if (!ed.impl_->reader.load(path))
        return fail(diag, DC::PatchBinaryReadFailed, "failed to load ELF: " + path);

    for (auto& sec : ed.impl_->reader.sections) {
        if (sec->get_name() == ".text") {
            ed.impl_->text_sec  = sec.get();
            ed.impl_->text_va   = sec->get_address();
            ed.impl_->text_size = sec->get_size();
            break;
        }
    }
    if (!ed.impl_->text_sec)
        return fail(diag, DC::PatchBinaryReadFailed, "no .text section");

    return std::move(ed);
}

// ---------------------------------------------------------------------------
// text_section
// ---------------------------------------------------------------------------

TextSectionInfo ELFEditor::text_section() const noexcept {
    return {impl_->text_va, impl_->text_size};
}

// ---------------------------------------------------------------------------
// overwrite_text
// ---------------------------------------------------------------------------

tl::expected<void, DC>
ELFEditor::overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                          Common::DiagnosticCollector& diag) noexcept {
    auto* sec = impl_->text_sec;
    const uint64_t sec_addr = sec->get_address();
    const uint64_t sec_size = sec->get_size();

    if (va < sec_addr || va + len > sec_addr + sec_size)
        return fail(diag, DC::PatchSegmentCreationFailed, "VA outside .text");

    const size_t offset = static_cast<size_t>(va - sec_addr);

    // ELFIO: get_data() is const; copy, patch, set_data.
    std::vector<uint8_t> buf(sec->get_data(), sec->get_data() + sec->get_size());
    std::memcpy(buf.data() + offset, data, len);
    sec->set_data(reinterpret_cast<const char*>(buf.data()), buf.size());
    return {};
}

// ---------------------------------------------------------------------------
// next_segment_va
// ---------------------------------------------------------------------------

uint64_t ELFEditor::next_segment_va(uint64_t alignment) const noexcept {
    uint64_t highest = 0;
    for (const auto& seg : impl_->reader.segments) {
        if (seg->get_type() == PT_LOAD) {
            uint64_t end = seg->get_virtual_address() + seg->get_memory_size();
            if (end > highest) highest = end;
        }
    }
    return (highest + alignment - 1) & ~(alignment - 1);
}

// ---------------------------------------------------------------------------
// add_segment
// ---------------------------------------------------------------------------

tl::expected<NewSegmentInfo, DC>
ELFEditor::add_segment(std::string_view name,
                       const std::vector<uint8_t>& payload,
                       uint64_t alignment,
                       Common::DiagnosticCollector& /*diag*/) noexcept {
    auto& reader = impl_->reader;

    // Next page-aligned VA after all existing PT_LOAD segments
    uint64_t highest = 0;
    for (const auto& seg : reader.segments) {
        if (seg->get_type() == PT_LOAD) {
            uint64_t end = seg->get_virtual_address() + seg->get_memory_size();
            if (end > highest) highest = end;
        }
    }
    const uint64_t seg_va = (highest + alignment - 1) & ~(alignment - 1);

    auto* new_sec = reader.sections.add(std::string{name});
    new_sec->set_type(SHT_PROGBITS);
    new_sec->set_flags(SHF_ALLOC | SHF_EXECINSTR);
    new_sec->set_addr_align(16);
    new_sec->set_address(seg_va);
    new_sec->set_data(reinterpret_cast<const char*>(payload.data()), payload.size());

    auto* new_seg = reader.segments.add();
    new_seg->set_type(PT_LOAD);
    new_seg->set_flags(PF_R | PF_X);
    new_seg->set_align(alignment);
    new_seg->set_virtual_address(seg_va);
    new_seg->set_physical_address(seg_va);
    new_seg->add_section_index(new_sec->get_index(), new_sec->get_addr_align());

    return NewSegmentInfo{seg_va, payload.size()};
}

// ---------------------------------------------------------------------------
// add_runtime_dep
// ---------------------------------------------------------------------------

tl::expected<void, DC>
ELFEditor::add_runtime_dep(std::string_view soname,
                           Common::DiagnosticCollector& diag) noexcept {
    // ELF DT_NEEDED injection requires modifying .dynamic section
    // (fixed size set by linker). Deferred.
    diag.note("loader", DC::None,
              "ELF DT_NEEDED deferred for '" + std::string(soname)
              + "'; use LD_PRELOAD");
    return {};
}

// ---------------------------------------------------------------------------
// invalidate_signature
// ---------------------------------------------------------------------------

void ELFEditor::invalidate_signature() noexcept {
    // ELF has no binary-level code signature to invalidate.
}

// ---------------------------------------------------------------------------
// save
// ---------------------------------------------------------------------------

tl::expected<void, DC>
ELFEditor::save(const std::string& path,
                Common::DiagnosticCollector& diag) noexcept {
    if (!impl_->reader.save(path))
        return fail(diag, DC::PatchBinaryWriteFailed, "failed to write ELF: " + path);
    return {};
}

}  // namespace VMPilot::Loader
