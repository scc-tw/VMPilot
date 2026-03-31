#include <PEEditor.hpp>

#include <coffi/coffi.hpp>

#include <cstring>
#include <vector>

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

struct PEEditor::Impl {
    COFFI::coffi pe;
    COFFI::section* text_sec = nullptr;
    uint64_t text_va   = 0;
    uint64_t text_size = 0;
    uint64_t image_base = 0;
};

PEEditor::PEEditor() : impl_(std::make_unique<Impl>()) {}
PEEditor::~PEEditor() = default;
PEEditor::PEEditor(PEEditor&&) noexcept = default;
PEEditor& PEEditor::operator=(PEEditor&&) noexcept = default;

// ---------------------------------------------------------------------------
// open
// ---------------------------------------------------------------------------

tl::expected<PEEditor, DC>
PEEditor::open(const std::string& path,
               Common::DiagnosticCollector& diag) noexcept {
    PEEditor ed;
    if (!ed.impl_->pe.load(path))
        return fail(diag, DC::PatchBinaryReadFailed, "failed to load PE: " + path);

    // Retrieve image base from the Windows NT header
    const auto* win_hdr = ed.impl_->pe.get_win_header();
    if (win_hdr) {
        ed.impl_->image_base = win_hdr->get_image_base();
    }

    // Find .text section
    for (auto& sec : ed.impl_->pe.get_sections()) {
        if (sec.get_name() == ".text") {
            ed.impl_->text_sec  = &sec;
            // In PE, section virtual_address is an RVA; absolute VA = image_base + RVA
            ed.impl_->text_va   = ed.impl_->image_base + sec.get_virtual_address();
            ed.impl_->text_size = sec.get_virtual_size();
            break;
        }
    }
    if (!ed.impl_->text_sec)
        return fail(diag, DC::PatchBinaryReadFailed, "no .text section in PE");

    return ed;
}

// ---------------------------------------------------------------------------
// text_section
// ---------------------------------------------------------------------------

TextSectionInfo PEEditor::text_section() const noexcept {
    return {impl_->text_va, impl_->text_size};
}

// ---------------------------------------------------------------------------
// next_segment_va
// ---------------------------------------------------------------------------

uint64_t PEEditor::next_segment_va(uint64_t alignment) const noexcept {
    // Find the highest VA end across all sections.
    // PE section virtual_address is an RVA; we work in absolute VA space.
    uint64_t highest = 0;
    for (const auto& sec : impl_->pe.get_sections()) {
        uint64_t va = impl_->image_base + sec.get_virtual_address();
        // Use virtual_size if nonzero (preferred), else data_size
        uint64_t sz = sec.get_virtual_size();
        if (sz == 0) sz = sec.get_data_size();
        uint64_t end = va + sz;
        if (end > highest) highest = end;
    }
    // Align up
    return (highest + alignment - 1) & ~(alignment - 1);
}

// ---------------------------------------------------------------------------
// add_segment
// ---------------------------------------------------------------------------

tl::expected<NewSegmentInfo, DC>
PEEditor::add_segment(std::string_view name,
                      const std::vector<uint8_t>& payload,
                      uint64_t alignment,
                      Common::DiagnosticCollector& /*diag*/) noexcept {
    auto& pe = impl_->pe;

    // Compute the next page-aligned VA (as RVA for PE section header)
    uint64_t seg_va = next_segment_va(alignment);
    uint32_t rva = static_cast<uint32_t>(seg_va - impl_->image_base);

    // Add new section via COFFI
    auto* new_sec = pe.add_section(std::string{name});
    new_sec->set_data(reinterpret_cast<const char*>(payload.data()),
                      static_cast<uint32_t>(payload.size()));
    new_sec->set_virtual_address(rva);
    new_sec->set_virtual_size(static_cast<uint32_t>(payload.size()));

    // W^X compliance: RW (not RWX). Runtime constructor does VirtualProtect
    // to RX after writing call_slot.
    new_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
                       IMAGE_SCN_CNT_INITIALIZED_DATA);

    return NewSegmentInfo{seg_va, payload.size()};
}

// ---------------------------------------------------------------------------
// extend_text
// ---------------------------------------------------------------------------

tl::expected<NewSegmentInfo, DC>
PEEditor::extend_text(const std::vector<uint8_t>& /*data*/,
                      uint64_t /*alignment*/,
                      Common::DiagnosticCollector& diag) noexcept {
    return fail(diag, DC::PatchFormatUnsupported,
                "PE .text extension not yet implemented");
}

// ---------------------------------------------------------------------------
// overwrite_text
// ---------------------------------------------------------------------------

tl::expected<void, DC>
PEEditor::overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                         Common::DiagnosticCollector& diag) noexcept {
    auto* sec = impl_->text_sec;
    const uint64_t sec_va = impl_->text_va;
    // Use data_size for bounds check on raw data buffer
    const uint64_t sec_data_len = sec->get_data_size();

    if (va < sec_va || va + len > sec_va + sec_data_len)
        return fail(diag, DC::PatchSegmentCreationFailed, "VA outside .text");

    const auto offset = static_cast<uint32_t>(va - sec_va);

    // COFFI: get_data() is const; copy, patch, set_data.
    std::vector<uint8_t> buf(sec->get_data(),
                             sec->get_data() + sec->get_data_size());
    std::memcpy(buf.data() + offset, data, len);
    sec->set_data(reinterpret_cast<const char*>(buf.data()),
                  static_cast<uint32_t>(buf.size()));
    return {};
}

// ---------------------------------------------------------------------------
// add_runtime_dep
// ---------------------------------------------------------------------------

tl::expected<void, DC>
PEEditor::add_runtime_dep(std::string_view install_name,
                          Common::DiagnosticCollector& diag) noexcept {
    // PE import injection is complex (IDT/ILT/IAT manipulation).
    // For v1, log a note about the required DLL. The call_slot + runtime
    // constructor pattern means we need vmpilot_runtime.dll loaded, but
    // the actual import can be handled by placing the DLL in PATH or via
    // LoadLibrary in the runtime constructor.
    diag.note("loader", DC::None,
              "PE import injection deferred; ensure '" +
              std::string(install_name) +
              "' is in PATH or loaded via LoadLibrary");
    return {};
}

// ---------------------------------------------------------------------------
// invalidate_signature
// ---------------------------------------------------------------------------

void PEEditor::invalidate_signature() noexcept {
    // Zero the Certificate Table data directory entry (index 4) to
    // invalidate any Authenticode signature.
    auto& dirs = impl_->pe.get_directories();
    if (dirs.get_count() > DIRECTORY_CERTIFICATE_TABLE) {
        auto* cert_dir = dirs[DIRECTORY_CERTIFICATE_TABLE];
        cert_dir->set_virtual_address(0);
        cert_dir->set_size(0);
    }
}

// ---------------------------------------------------------------------------
// save
// ---------------------------------------------------------------------------

tl::expected<void, DC>
PEEditor::save(const std::string& path,
               Common::DiagnosticCollector& diag) noexcept {
    // COFFI layout() recomputes offsets, alignment, NumberOfSections, etc.
    impl_->pe.layout();
    if (!impl_->pe.save(path))
        return fail(diag, DC::PatchBinaryWriteFailed, "failed to write PE: " + path);
    return {};
}

}  // namespace VMPilot::Loader
