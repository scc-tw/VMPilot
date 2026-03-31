#include <ELFEditor.hpp>

#include <elfio/elfio.hpp>

#include <algorithm>
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
// cfi_enforced
// ---------------------------------------------------------------------------

/// If the original binary was compiled with CET or BTI, the kernel
/// validates ALL indirect-call targets at runtime.  A patched binary
/// that introduces new call targets (our stubs) without landing pads
/// will be killed by the kernel.  This query lets Loader::patch()
/// confirm that stubs carry ENDBR64/BTI c before injecting them.
bool ELFEditor::cfi_enforced() const noexcept {
    // GNU_PROPERTY_X86_FEATURE_1_IBT  = 0x00000001  (Indirect Branch Tracking)
    // GNU_PROPERTY_AARCH64_FEATURE_1_BTI = 0x00000001
    // Both live in a .note.gnu.property section with type NT_GNU_PROPERTY_TYPE_0.
    constexpr uint32_t NT_GNU_PROPERTY_TYPE_0 = 5;
    constexpr uint32_t GNU_PROPERTY_X86_FEATURE_1_AND = 0xC0000002;
    constexpr uint32_t GNU_PROPERTY_AARCH64_FEATURE_1_AND = 0xC0000000;
    constexpr uint32_t FEATURE_1_IBT_OR_BTI = 0x00000001;

    for (const auto& sec : impl_->reader.sections) {
        if (sec->get_name() != ".note.gnu.property") continue;
        if (sec->get_size() < 16) continue;

        const auto* data = reinterpret_cast<const uint8_t*>(sec->get_data());
        size_t off = 0;
        while (off + 12 <= sec->get_size()) {
            uint32_t namesz, descsz, type;
            std::memcpy(&namesz, data + off, 4);
            std::memcpy(&descsz, data + off + 4, 4);
            std::memcpy(&type,   data + off + 8, 4);

            // Align namesz to 4 bytes
            const size_t name_pad = (namesz + 3) & ~3u;
            const size_t desc_start = off + 12 + name_pad;

            if (type == NT_GNU_PROPERTY_TYPE_0 && namesz == 4 &&
                desc_start + descsz <= sec->get_size()) {
                // Parse property entries within the descriptor
                size_t p = desc_start;
                while (p + 8 <= desc_start + descsz) {
                    uint32_t pr_type, pr_datasz;
                    std::memcpy(&pr_type,   data + p, 4);
                    std::memcpy(&pr_datasz, data + p + 4, 4);
                    if (pr_datasz >= 4 &&
                        (pr_type == GNU_PROPERTY_X86_FEATURE_1_AND ||
                         pr_type == GNU_PROPERTY_AARCH64_FEATURE_1_AND)) {
                        uint32_t features;
                        std::memcpy(&features, data + p + 8, 4);
                        if (features & FEATURE_1_IBT_OR_BTI)
                            return true;
                    }
                    // Next property: 8 (header) + pr_datasz aligned to 4/8
                    const size_t entry_align =
                        (impl_->reader.get_class() == ELFIO::ELFCLASS64) ? 8 : 4;
                    p += 8 + ((pr_datasz + entry_align - 1) & ~(entry_align - 1));
                }
            }

            // Next note: 12 + aligned namesz + aligned descsz
            const size_t desc_pad = (descsz + 3) & ~3u;
            off = desc_start + desc_pad;
        }
        break;
    }
    return false;
}

// ---------------------------------------------------------------------------
// find_text_gaps
// ---------------------------------------------------------------------------

/// Scan .text for consecutive filler bytes that can be repurposed as
/// code caves.  Detects:
///   - x86 NOP sleds (0x90)
///   - INT3 padding (0xCC) — compiler/linker alignment fill
///   - Zero padding (0x00) — linker alignment fill
/// Returns gaps >= min_size, sorted by size descending.
std::vector<TextGap>
ELFEditor::find_text_gaps(std::size_t min_size) const noexcept {
    auto* sec = impl_->text_sec;
    if (!sec || sec->get_size() == 0) return {};

    const auto* data = reinterpret_cast<const uint8_t*>(sec->get_data());
    const size_t text_len = sec->get_size();
    const uint64_t base_va = impl_->text_va;

    // Helper: is this byte a filler?
    // x86: 0x90 (NOP), 0xCC (INT3), 0x00 (zero padding)
    auto is_filler = [](uint8_t b) -> bool {
        return b == 0x90 || b == 0xCC || b == 0x00;
    };

    std::vector<TextGap> gaps;
    size_t i = 0;
    while (i < text_len) {
        if (!is_filler(data[i])) { ++i; continue; }

        // Found start of a filler run — measure its length.
        // All bytes in the run must be the SAME filler type
        // (don't merge a NOP sled with an INT3 pad).
        const uint8_t filler = data[i];
        const size_t start = i;
        while (i < text_len && data[i] == filler) ++i;
        const size_t run_len = i - start;

        if (run_len >= min_size)
            gaps.push_back({base_va + start, run_len});
    }

    // Sort by size descending (largest caves first)
    std::sort(gaps.begin(), gaps.end(),
              [](const TextGap& a, const TextGap& b) { return a.size > b.size; });

    return gaps;
}

// ---------------------------------------------------------------------------
// extend_text
// ---------------------------------------------------------------------------

tl::expected<NewSegmentInfo, DC>
ELFEditor::extend_text(const std::vector<uint8_t>& data,
                       uint64_t alignment,
                       Common::DiagnosticCollector& diag) noexcept {
    auto& reader = impl_->reader;
    auto* text_sec = impl_->text_sec;

    if (!text_sec)
        return fail(diag, DC::PatchBinaryReadFailed, "no .text section for extend_text");

    // 1. Find the PT_LOAD segment that contains .text
    ELFIO::segment* text_seg = nullptr;
    for (auto& seg : reader.segments) {
        if (seg->get_type() != PT_LOAD)
            continue;
        const uint64_t seg_start = seg->get_virtual_address();
        const uint64_t seg_end   = seg_start + seg->get_memory_size();
        if (impl_->text_va >= seg_start && impl_->text_va < seg_end) {
            text_seg = seg.get();
            break;
        }
    }

    if (!text_seg)
        return fail(diag, DC::PatchSegmentCreationFailed,
                    "no PT_LOAD segment contains .text; cannot extend");

    // 2. Compute the new data's VA = .text VA + current size, aligned
    const uint64_t orig_text_end_va = impl_->text_va + impl_->text_size;
    const uint64_t new_data_va = (orig_text_end_va + alignment - 1) & ~(alignment - 1);
    const uint64_t padding = new_data_va - orig_text_end_va;

    // 3. Grow the .text section data: append padding + payload
    const size_t orig_sec_size = text_sec->get_size();
    const size_t new_sec_size = orig_sec_size + static_cast<size_t>(padding) + data.size();

    std::vector<uint8_t> buf(new_sec_size, 0x00);
    std::memcpy(buf.data(), text_sec->get_data(), orig_sec_size);
    std::memcpy(buf.data() + orig_sec_size + static_cast<size_t>(padding),
                data.data(), data.size());

    text_sec->set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

    // 4. Update the PT_LOAD segment sizes to cover the growth
    const uint64_t growth = static_cast<uint64_t>(padding) + data.size();
    text_seg->set_memory_size(text_seg->get_memory_size() + growth);
    text_seg->set_file_size(text_seg->get_file_size() + growth);

    // 5. Update cached .text size so overwrite_text() sees the extended range
    impl_->text_size = new_sec_size;

    return NewSegmentInfo{new_data_va, data.size()};
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

/// Inject DT_NEEDED for `soname` into the ELF .dynamic section.
///
/// Multi-layer fallback:
///   Layer 1: Steal a spare DT_NULL padding slot (no section growth).
///   Layer 2: Append via ELFIO dynamic_section_accessor (grows .dynamic;
///            ELFIO relayouts the file on save).
///   Layer 3: Return PatchRuntimeDepFailed — the patched binary will NOT
///            auto-load the runtime.  Stubs will dereference null call_slot.
tl::expected<void, DC>
ELFEditor::add_runtime_dep(std::string_view soname,
                           Common::DiagnosticCollector& diag) noexcept {
    auto& reader = impl_->reader;
    const std::string soname_str(soname);

    // -- Find .dynamic section --
    // Without .dynamic, this is a statically linked binary.  The dynamic
    // linker won't process it, so DT_NEEDED has no effect.
    ELFIO::section* dyn_sec = nullptr;
    for (auto& sec : reader.sections) {
        if (sec->get_type() == SHT_DYNAMIC) {
            dyn_sec = sec.get();
            break;
        }
    }
    if (!dyn_sec) {
        return fail(diag, DC::PatchRuntimeDepFailed,
                    "no .dynamic section — cannot inject DT_NEEDED for '"
                    + soname_str + "' (static binary?)");
    }

    const size_t entry_size = dyn_sec->get_entry_size();
    if (entry_size == 0) {
        return fail(diag, DC::PatchRuntimeDepFailed,
                    ".dynamic has entry_size 0 — malformed ELF");
    }

    // -- Locate .dynstr --
    // DT_NEEDED stores an offset into .dynstr.  Without it, we cannot
    // represent the soname string.
    auto str_link = dyn_sec->get_link();
    if (str_link == 0 || str_link >= reader.sections.size()) {
        return fail(diag, DC::PatchRuntimeDepFailed,
                    ".dynamic has no linked .dynstr — cannot inject DT_NEEDED");
    }

    // ================================================================
    // Layer 1: Steal a spare DT_NULL slot (preferred — no section growth)
    // ================================================================
    const size_t total_slots = dyn_sec->get_size() / entry_size;
    const auto* raw = reinterpret_cast<const uint8_t*>(dyn_sec->get_data());
    const bool is64 = (reader.get_class() == ELFIO::ELFCLASS64);

    if (raw && total_slots >= 2) {
        size_t null_idx = total_slots;
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

        if (null_idx < total_slots && null_idx + 1 < total_slots) {
            // Spare slot found — steal it.
            ELFIO::string_section_accessor strsec(reader.sections[str_link]);
            auto str_offset = strsec.add_string(soname_str);

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
            return {};  // Layer 1 success
        }

        diag.warn("loader", DC::None,
                  "no spare DT_NULL in .dynamic — falling back to section growth");
    }

    // ================================================================
    // Layer 2: Grow .dynamic by overwriting the DT_NULL terminator with
    //          our DT_NEEDED entry, then appending a new DT_NULL.
    //          ELFIO recalculates file layout on save().
    // ================================================================
    {
        ELFIO::string_section_accessor strsec(reader.sections[str_link]);
        auto str_offset = strsec.add_string(soname_str);

        // Find the DT_NULL terminator (must exist — dynamic section always has one)
        size_t null_idx = total_slots;
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

        if (null_idx >= total_slots) {
            return fail(diag, DC::PatchRuntimeDepFailed,
                        "no DT_NULL terminator in .dynamic — cannot grow");
        }

        // Copy existing data, overwrite DT_NULL with DT_NEEDED, append new DT_NULL
        std::vector<uint8_t> buf(raw, raw + dyn_sec->get_size());
        buf.resize(buf.size() + entry_size, 0);  // grow by one entry (new DT_NULL)

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
        // The appended bytes are already zero = DT_NULL terminator

        dyn_sec->set_data(reinterpret_cast<const char*>(buf.data()), buf.size());
    }

    diag.warn("loader", DC::None,
              "DT_NEEDED injected via .dynamic growth for '" + soname_str
              + "' — verify output with readelf -d");
    return {};  // Layer 2 success (degraded)
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
