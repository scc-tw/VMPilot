#include <ELFEditor.hpp>

#include <elfio/elfio.hpp>

#include <cstring>
#include <vector>

using ELFIO::PT_LOAD;
using ELFIO::SHT_PROGBITS;
using ELFIO::SHT_DYNAMIC;
using ELFIO::SHF_ALLOC;
using ELFIO::SHF_EXECINSTR;
using ELFIO::SHF_WRITE;
using ELFIO::PF_R;
using ELFIO::PF_W;
using ELFIO::PF_X;
using ELFIO::DT_NULL;
using ELFIO::DT_NEEDED;

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

    return ed;
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
    new_sec->set_flags(SHF_ALLOC | SHF_WRITE | SHF_EXECINSTR);
    new_sec->set_addr_align(16);
    new_sec->set_address(seg_va);
    new_sec->set_data(reinterpret_cast<const char*>(payload.data()), payload.size());

    auto* new_seg = reader.segments.add();
    new_seg->set_type(PT_LOAD);
    // Initially RW (writable, NOT executable).  The runtime constructor
    // writes &vm_stub_entry to call_slot (offset 0), then mprotects the
    // entire section to RX.  This maintains W^X at all times.
    new_seg->set_flags(PF_R | PF_W);
    new_seg->set_align(alignment);
    new_seg->set_virtual_address(seg_va);
    new_seg->set_physical_address(seg_va);
    new_seg->add_section_index(new_sec->get_index(), new_sec->get_addr_align());

    return NewSegmentInfo{seg_va, payload.size()};
}

// ---------------------------------------------------------------------------
// add_runtime_dep
// ---------------------------------------------------------------------------

/// Inject DT_NEEDED for `soname` by stealing a DT_NULL padding slot.
///
/// Most linkers emit 1-3 DT_NULL entries at the end of .dynamic as
/// padding.  We replace the first DT_NULL with {DT_NEEDED, offset}
/// — the next slot is zero (implicitly DT_NULL), preserving the
/// terminator.  This avoids growing .dynamic (which would shift all
/// subsequent sections and break program headers).
///
/// If .dynamic has no padding slots, falls back to an LD_PRELOAD note.
tl::expected<void, DC>
ELFEditor::add_runtime_dep(std::string_view soname,
                           Common::DiagnosticCollector& diag) noexcept {
    auto& reader = impl_->reader;

    // 1. Find .dynamic section
    ELFIO::section* dyn_sec = nullptr;
    for (auto& sec : reader.sections) {
        if (sec->get_type() == SHT_DYNAMIC) {
            dyn_sec = sec.get();
            break;
        }
    }
    if (!dyn_sec) {
        diag.note("loader", DC::None,
                  "No .dynamic section (static binary?); use LD_PRELOAD for '"
                  + std::string(soname) + "'");
        return {};
    }

    // 2. Scan raw entries to count DT_NULL padding slots.
    //    We need ≥2 consecutive DT_NULL at the end: steal one, keep one.
    const size_t entry_size = dyn_sec->get_entry_size();
    if (entry_size == 0) {
        diag.note("loader", DC::None,
                  ".dynamic has entry_size 0; use LD_PRELOAD for '"
                  + std::string(soname) + "'");
        return {};
    }
    const size_t total_slots = dyn_sec->get_size() / entry_size;
    const auto* raw = reinterpret_cast<const uint8_t*>(dyn_sec->get_data());
    if (!raw || total_slots < 2)
        return {};

    // Find the first DT_NULL.  Walk raw bytes for 32/64-bit portability.
    const bool is64 = (reader.get_class() == ELFIO::ELFCLASS64);
    size_t null_idx = total_slots;  // sentinel: "not found"

    for (size_t i = 0; i < total_slots; ++i) {
        const uint8_t* ent = raw + i * entry_size;
        int64_t tag = 0;
        if (is64) {
            ELFIO::Elf64_Dyn d;
            std::memcpy(&d, ent, sizeof(d));
            tag = static_cast<int64_t>(d.d_tag);
        } else {
            ELFIO::Elf32_Dyn d;
            std::memcpy(&d, ent, sizeof(d));
            tag = static_cast<int64_t>(d.d_tag);
        }
        if (tag == static_cast<int64_t>(DT_NULL)) {
            null_idx = i;
            break;
        }
    }

    if (null_idx >= total_slots || null_idx + 1 >= total_slots) {
        // No DT_NULL found, or DT_NULL is the very last slot with no room
        // for a replacement terminator.
        diag.note("loader", DC::None,
                  "No spare DT_NULL in .dynamic; use LD_PRELOAD for '"
                  + std::string(soname) + "'");
        return {};
    }

    // 3. Add soname string to .dynstr (via ELFIO string accessor).
    auto str_link = dyn_sec->get_link();
    if (str_link == 0 || str_link >= reader.sections.size()) {
        diag.note("loader", DC::None,
                  ".dynamic has no linked .dynstr; use LD_PRELOAD for '"
                  + std::string(soname) + "'");
        return {};
    }
    ELFIO::string_section_accessor strsec(reader.sections[str_link]);
    auto str_offset = strsec.add_string(std::string(soname));

    // 4. Overwrite the DT_NULL slot with {DT_NEEDED, str_offset}.
    //    The slot at null_idx+1 is already zero (DT_NULL) — it becomes
    //    the new terminator.
    std::vector<uint8_t> buf(raw, raw + dyn_sec->get_size());
    uint8_t* target = buf.data() + null_idx * entry_size;

    if (is64) {
        ELFIO::Elf64_Dyn d{};
        d.d_tag      = static_cast<ELFIO::Elf_Sxword>(DT_NEEDED);
        d.d_un.d_val = static_cast<ELFIO::Elf_Xword>(str_offset);
        std::memcpy(target, &d, sizeof(d));
    } else {
        ELFIO::Elf32_Dyn d{};
        d.d_tag      = static_cast<ELFIO::Elf_Sword>(DT_NEEDED);
        d.d_un.d_val = static_cast<ELFIO::Elf_Word>(str_offset);
        std::memcpy(target, &d, sizeof(d));
    }

    dyn_sec->set_data(reinterpret_cast<const char*>(buf.data()), buf.size());
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
