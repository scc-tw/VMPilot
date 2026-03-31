#include <PEEditor.hpp>

#include <coffi/coffi.hpp>
#include <coffi/coffi_import.hpp>

#include <algorithm>
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

TextSectionInfo PEEditor::text_section_impl() const noexcept {
    return {impl_->text_va, impl_->text_size};
}

// ---------------------------------------------------------------------------
// next_segment_va
// ---------------------------------------------------------------------------

uint64_t PEEditor::next_segment_va_impl(uint64_t alignment) const noexcept {
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
PEEditor::add_segment_impl(std::string_view name,
                      const std::vector<uint8_t>& payload,
                      uint64_t alignment,
                      Common::DiagnosticCollector& /*diag*/) noexcept {
    auto& pe = impl_->pe;

    // Compute the next page-aligned VA (as RVA for PE section header)
    uint64_t seg_va = next_segment_va_impl(alignment);
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
// cfi_enforced
// ---------------------------------------------------------------------------

/// Windows CET (Shadow Stack + Indirect Branch Tracking) is flagged in
/// the PE Optional Header's DllCharacteristics.  If a binary is marked
/// CET-compatible, the Windows kernel enforces ENDBR at all indirect-call
/// targets.  Our stubs must carry ENDBR32/64 (they do since af6ca03).
bool PEEditor::cfi_enforced_impl() const noexcept {
    // IMAGE_DLLCHARACTERISTICS_CET_COMPAT = 0x8000 (undocumented but used
    // by MSVC /cetcompat and link.exe /CETCOMPAT).  COFFI exposes this
    // through get_win_header()->get_dll_flags().
    constexpr uint16_t CET_COMPAT = 0x8000;
    const auto* win_hdr = impl_->pe.get_win_header();
    if (!win_hdr) return false;
    return (win_hdr->get_dll_flags() & CET_COMPAT) != 0;
}

// ---------------------------------------------------------------------------
// find_text_gaps
// ---------------------------------------------------------------------------

/// Scan .text for consecutive filler bytes usable as code caves.
/// PE targets are x86/x86_64, so detects:
///   - NOP (0x90), INT3 (0xCC), zero (0x00) — same as ELF x86
/// Returns gaps >= min_size, sorted by size descending.
std::vector<TextGap>
PEEditor::find_text_gaps_impl(std::size_t min_size) const noexcept {
    auto* sec = impl_->text_sec;
    if (!sec || sec->get_data_size() == 0) return {};

    const auto* data = reinterpret_cast<const uint8_t*>(sec->get_data());
    const size_t text_len = sec->get_data_size();
    const uint64_t base_va = impl_->text_va;

    auto is_filler = [](uint8_t b) -> bool {
        return b == 0x90 || b == 0xCC || b == 0x00;
    };

    std::vector<TextGap> gaps;
    size_t i = 0;
    while (i < text_len) {
        if (!is_filler(data[i])) { ++i; continue; }

        const uint8_t filler = data[i];
        const size_t start = i;
        while (i < text_len && data[i] == filler) ++i;
        const size_t run_len = i - start;

        if (run_len >= min_size)
            gaps.push_back({base_va + start, run_len});
    }

    std::sort(gaps.begin(), gaps.end(),
              [](const TextGap& a, const TextGap& b) { return a.size > b.size; });

    return gaps;
}

// ---------------------------------------------------------------------------
// extend_text
// ---------------------------------------------------------------------------

/// Extend .text by appending padding + payload to the section data.
/// Updates virtual_size and data_size so the payload is accessible
/// at the returned VA.  The section keeps its original flags (RX).
tl::expected<NewSegmentInfo, DC>
PEEditor::extend_text(const std::vector<uint8_t>& data,
                      uint64_t alignment,
                      Common::DiagnosticCollector& diag) noexcept {
    auto* sec = impl_->text_sec;
    if (!sec)
        return fail(diag, DC::PatchBinaryReadFailed,
                    "no .text section for extend_text");

    const uint64_t orig_end_va = impl_->text_va + impl_->text_size;
    const uint64_t new_data_va = (orig_end_va + alignment - 1) & ~(alignment - 1);
    const uint64_t padding = new_data_va - orig_end_va;

    // Copy existing data, append padding + payload
    const size_t orig_data_len = sec->get_data_size();
    const size_t new_data_len = orig_data_len + static_cast<size_t>(padding) + data.size();

    std::vector<uint8_t> buf(new_data_len, 0xCC);  // fill padding with INT3
    std::memcpy(buf.data(), sec->get_data(), orig_data_len);
    // Zero the padding region (between old data and new payload)
    std::memset(buf.data() + orig_data_len, 0xCC, static_cast<size_t>(padding));
    std::memcpy(buf.data() + orig_data_len + static_cast<size_t>(padding),
                data.data(), data.size());

    sec->set_data(reinterpret_cast<const char*>(buf.data()),
                  static_cast<uint32_t>(buf.size()));

    // Update virtual size to cover the extended data
    const uint32_t new_vsize = static_cast<uint32_t>(
        impl_->text_size + padding + data.size());
    sec->set_virtual_size(new_vsize);

    // Update cached text_size so overwrite_text() sees the extended range
    impl_->text_size = new_vsize;

    return NewSegmentInfo{new_data_va, data.size()};
}

// ---------------------------------------------------------------------------
// overwrite_text
// ---------------------------------------------------------------------------

tl::expected<void, DC>
PEEditor::overwrite_text_impl(uint64_t va, const uint8_t* data, size_t len,
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

/// Inject an import entry so the Windows loader loads the runtime DLL
/// at process startup.  Without this, the runtime constructor never
/// runs and call_slot stays null — stubs crash on first invocation.
///
/// Uses COFFI's import_section_accessor which builds a new .idata
/// section with the correct IDT/ILT/IAT layout, preserving all
/// existing imports from the original binary.
tl::expected<void, DC>
PEEditor::add_runtime_dep_impl(std::string_view install_name,
                          Common::DiagnosticCollector& diag) noexcept {
    COFFI::import_section_accessor imports(impl_->pe);

    if (!imports.add_import(std::string(install_name), "vm_stub_entry")) {
        return fail(diag, DC::PatchRuntimeDepFailed,
                    "failed to inject PE import for '"
                    + std::string(install_name) + "'");
    }

    return {};
}

// ---------------------------------------------------------------------------
// invalidate_signature
// ---------------------------------------------------------------------------

void PEEditor::invalidate_signature_impl() noexcept {
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
PEEditor::save_impl(const std::string& path,
               Common::DiagnosticCollector& diag) noexcept {
    // COFFI layout() recomputes offsets, alignment, NumberOfSections, etc.
    impl_->pe.layout();
    if (!impl_->pe.save(path))
        return fail(diag, DC::PatchBinaryWriteFailed, "failed to write PE: " + path);
    return {};
}

}  // namespace VMPilot::Loader
