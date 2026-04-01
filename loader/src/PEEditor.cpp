#include <PEEditor.hpp>

#include <coffi/coffi.hpp>

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
    coffi::coff_editor<coffi::pe32_traits> pe;
    coffi::section_entry<coffi::pe32_traits>* text_sec = nullptr;
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
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(path);
    if (!loaded)
        return fail(diag, DC::PatchBinaryReadFailed,
                    "failed to load PE: " + path + " (" +
                    std::string(coffi::to_string(loaded.error())) + ")");
    ed.impl_->pe = std::move(*loaded);

    // Retrieve image base from the Windows NT header
    ed.impl_->image_base = ed.impl_->pe.win_header()->image_base;

    // Find .text section
    ed.impl_->text_sec = ed.impl_->pe.find_section(".text");
    if (ed.impl_->text_sec) {
        ed.impl_->text_va   = ed.impl_->image_base +
                              ed.impl_->text_sec->virtual_address();
        ed.impl_->text_size = ed.impl_->text_sec->virtual_size();
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
    for (std::size_t i = 0; i < impl_->pe.section_count(); ++i) {
        auto& sec = impl_->pe.sections()[i];
        uint64_t va = impl_->image_base + sec.virtual_address();
        // Use virtual_size if nonzero (preferred), else data_size
        uint64_t sz = sec.virtual_size();
        if (sz == 0) sz = sec.data_length();
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

    // Add new section via coffi-modern
    auto& new_sec = pe.add_section(std::string{name},
        coffi::SCN_MEM_READ | coffi::SCN_MEM_WRITE |
        coffi::SCN_CNT_INITIALIZED_DATA);
    new_sec.set_data(reinterpret_cast<const char*>(payload.data()),
                     static_cast<uint32_t>(payload.size()));
    new_sec.set_virtual_address(rva);
    new_sec.set_virtual_size(static_cast<uint32_t>(payload.size()));

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
    // by MSVC /cetcompat and link.exe /CETCOMPAT).  coffi-modern exposes
    // this through win_header()->dll_flags.
    constexpr uint16_t CET_COMPAT = 0x8000;
    auto* win_hdr = impl_->pe.win_header();
    if (!win_hdr) return false;
    return (win_hdr->dll_flags & CET_COMPAT) != 0;
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
    if (!sec || sec->data_length() == 0) return {};

    const auto* data = reinterpret_cast<const uint8_t*>(sec->data_ptr());
    const std::size_t text_len = sec->data_length();
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
    const size_t orig_data_len = sec->data_length();
    const size_t new_data_len = orig_data_len + static_cast<size_t>(padding) + data.size();

    std::vector<uint8_t> buf(new_data_len, 0xCC);  // fill padding with INT3
    std::memcpy(buf.data(), sec->data_ptr(), orig_data_len);
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
    const uint64_t sec_data_len = sec->data_length();

    if (va < sec_va || va + len > sec_va + sec_data_len)
        return fail(diag, DC::PatchSegmentCreationFailed, "VA outside .text");

    const auto offset = static_cast<uint32_t>(va - sec_va);

    // coffi-modern: data_ptr() is const; copy, patch, set_data.
    std::vector<uint8_t> buf(sec->data_ptr(),
                             sec->data_ptr() + sec->data_length());
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
/// Uses coffi-modern's coff_editor::imports() builder which constructs
/// the correct IDT/ILT/IAT layout during save(), preserving all
/// existing imports from the original binary.
tl::expected<void, DC>
PEEditor::add_runtime_dep_impl(std::string_view install_name,
                          Common::DiagnosticCollector& /*diag*/) noexcept {
    impl_->pe.imports().add_symbol(
        std::string(install_name), "vm_stub_entry", 0);

    return {};
}

// ---------------------------------------------------------------------------
// invalidate_signature
// ---------------------------------------------------------------------------

void PEEditor::invalidate_signature_impl() noexcept {
    // Zero the Certificate Table data directory entry (index 4) to
    // invalidate any Authenticode signature.
    auto* cert_dir = impl_->pe.directory(coffi::DIR_SECURITY);
    if (cert_dir) {
        impl_->pe.set_directory(coffi::DIR_SECURITY, {0, 0});
    }
}

// ---------------------------------------------------------------------------
// save
// ---------------------------------------------------------------------------

tl::expected<void, DC>
PEEditor::save_impl(const std::string& path,
               Common::DiagnosticCollector& diag) noexcept {
    auto r = impl_->pe.save(path);
    if (!r)
        return fail(diag, DC::PatchBinaryWriteFailed,
                    "failed to write PE: " + path + " (" +
                    std::string(coffi::to_string(r.error())) + ")");
    return {};
}

}  // namespace VMPilot::Loader
