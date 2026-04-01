#include <ELFEditor.hpp>
#include <coffi_adapter.hpp>

#include <elfio/elfio.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <vector>

using elfio::PT_LOAD;
using elfio::PT_DYNAMIC;
using elfio::PT_PHDR;
using elfio::SHT_NULL;
using elfio::SHT_PROGBITS;
using elfio::SHT_DYNAMIC;
using elfio::SHF_ALLOC;
using elfio::SHF_EXECINSTR;
using elfio::SHF_WRITE;
using elfio::PF_R;
using elfio::PF_W;
using elfio::PF_X;
using elfio::DT_NULL;
using elfio::DT_NEEDED;

namespace VMPilot::Loader {

using DC = Common::DiagnosticCode;

static tl::unexpected<DC> fail(Common::DiagnosticCollector& diag, DC code,
                               const std::string& msg) {
    diag.error("loader", code, msg);
    return tl::unexpected(code);
}

// ---------------------------------------------------------------------------
// Helper: append a NUL-terminated string to a section_entry's data,
// returning the offset at which the string begins (i.e. the old size).
// Replaces the non-existent section_entry::append_string().
// ---------------------------------------------------------------------------
template <typename Traits>
static size_t append_string_to_section(elfio::section_entry<Traits>& sec,
                                       const std::string& str) {
    const size_t offset = sec.data().size();
    // Build a temporary buffer: existing data + str + NUL
    std::vector<char> buf(sec.data().begin(), sec.data().end());
    buf.insert(buf.end(), str.begin(), str.end());
    buf.push_back('\0');
    sec.set_data(std::move(buf));
    return offset;
}

// ---------------------------------------------------------------------------
// Impl
// ---------------------------------------------------------------------------

struct ELFEditor::Impl {
    ElfEditorVariant editor;
    // Index of the .text section within the editor's section list.
    std::size_t text_sec_idx = 0;
    uint64_t text_va   = 0;
    uint64_t text_size = 0;
    bool is_64 = true;
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
    // Read entire file into memory.
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs.good())
        return fail(diag, DC::PatchBinaryReadFailed, "cannot open ELF: " + path);

    const auto file_size = ifs.tellg();
    ifs.seekg(0);
    std::vector<char> file_buf(static_cast<size_t>(file_size));
    ifs.read(file_buf.data(), file_size);
    ifs.close();

    if (file_buf.size() < 5)
        return fail(diag, DC::PatchBinaryReadFailed, "file too small: " + path);

    // Detect ELF class (32/64) from EI_CLASS byte at offset 4.
    const uint8_t ei_class = static_cast<uint8_t>(file_buf[4]);

    ELFEditor ed;
    const bool is64 = (ei_class != 1);  // ELFCLASS32 = 1
    ed.impl_->is_64 = is64;

    // Load the file into an elf_file (zero-copy reader), then copy all
    // sections and segments into an elf_editor (mutable).
    auto open_impl = [&](auto traits_tag) -> tl::expected<ELFEditor, DC> {
        using Traits = decltype(traits_tag);
        elfio::byte_view view{file_buf.data(), file_buf.size()};
        auto parsed = elfio::elf_file<Traits>::from_view(view);
        if (!parsed)
            return fail(diag, DC::PatchBinaryReadFailed,
                        "failed to parse ELF: " + path + " (" +
                        std::string(elfio::to_string(parsed.error())) + ")");

        auto& file = *parsed;

        // Build mutable editor and populate from parsed file.
        elfio::elf_editor<Traits> editor;
        editor.create(file.encoding(), file.type(), file.machine());
        editor.set_os_abi(file.os_abi());
        editor.set_entry(file.entry());
        editor.set_flags(file.flags());

        // Copy sections (skip index 0 — the null section is already in the editor).
        for (auto sec_ref : file.sections()) {
            if (sec_ref.index() == 0) continue;
            auto& sec = editor.add_section(std::string(sec_ref.name()),
                                           sec_ref.type(), sec_ref.flags());
            sec.set_address(sec_ref.address());
            sec.set_link(sec_ref.link());
            sec.set_info(sec_ref.info());
            sec.set_addr_align(sec_ref.addr_align());
            sec.set_entry_size(sec_ref.entry_size());

            auto sec_data = sec_ref.data();
            if (!sec_data.empty()) {
                sec.set_data(reinterpret_cast<const char*>(sec_data.data()),
                             static_cast<std::size_t>(sec_data.size()));
            }
        }

        // Copy segments.
        for (auto seg_ref : file.segments()) {
            auto& seg = editor.add_segment(seg_ref.type(), seg_ref.flags());
            seg.set_vaddr(seg_ref.virtual_address());
            seg.set_paddr(seg_ref.physical_address());
            seg.set_filesz(seg_ref.file_size());
            seg.set_memsz(seg_ref.memory_size());
            seg.set_align(seg_ref.align());
        }

        // Find .text section in the editor.
        bool found = false;
        for (std::size_t i = 0; i < editor.sections().size(); ++i) {
            auto& sec = editor.sections()[i];
            if (sec.name() == ".text") {
                ed.impl_->text_sec_idx = i;
                ed.impl_->text_va     = sec.address();
                ed.impl_->text_size   = sec.size();
                found = true;
                break;
            }
        }
        if (!found)
            return fail(diag, DC::PatchBinaryReadFailed, "no .text section");

        ed.impl_->editor = std::move(editor);
        return {std::move(ed)};
    };

    if (is64)
        return open_impl(elfio::elf64_traits{});
    else
        return open_impl(elfio::elf32_traits{});
}

// ---------------------------------------------------------------------------
// text_section
// ---------------------------------------------------------------------------

TextSectionInfo ELFEditor::text_section_impl() const noexcept {
    return {impl_->text_va, impl_->text_size};
}

// ---------------------------------------------------------------------------
// overwrite_text
// ---------------------------------------------------------------------------

tl::expected<void, DC>
ELFEditor::overwrite_text_impl(uint64_t va, const uint8_t* data, size_t len,
                               Common::DiagnosticCollector& diag) noexcept {
    const uint64_t sec_addr = impl_->text_va;
    const uint64_t sec_size = impl_->text_size;

    if (va < sec_addr || va + len > sec_addr + sec_size)
        return fail(diag, DC::PatchSegmentCreationFailed, "VA outside .text");

    const size_t offset = static_cast<size_t>(va - sec_addr);

    visit_elf(impl_->editor, [&](auto& editor) {
        auto& sec = editor.sections()[impl_->text_sec_idx];
        const auto& sec_data = sec.data();
        std::vector<uint8_t> buf(reinterpret_cast<const uint8_t*>(sec_data.data()),
                                 reinterpret_cast<const uint8_t*>(sec_data.data()) + sec_data.size());
        std::memcpy(buf.data() + offset, data, len);
        sec.set_data(reinterpret_cast<const char*>(buf.data()), buf.size());
    });
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
bool ELFEditor::cfi_enforced_impl() const noexcept {
    // GNU_PROPERTY_X86_FEATURE_1_IBT  = 0x00000001  (Indirect Branch Tracking)
    // GNU_PROPERTY_AARCH64_FEATURE_1_BTI = 0x00000001
    // Both live in a .note.gnu.property section with type NT_GNU_PROPERTY_TYPE_0.
    constexpr uint32_t NT_GNU_PROPERTY_TYPE_0 = 5;
    constexpr uint32_t GNU_PROPERTY_X86_FEATURE_1_AND = 0xC0000002;
    constexpr uint32_t GNU_PROPERTY_AARCH64_FEATURE_1_AND = 0xC0000000;
    constexpr uint32_t FEATURE_1_IBT_OR_BTI = 0x00000001;

    return visit_elf(const_cast<ElfEditorVariant&>(impl_->editor), [&](auto& editor) -> bool {
        for (std::size_t si = 0; si < editor.sections().size(); ++si) {
            auto& sec = editor.sections()[si];
            if (sec.name() != ".note.gnu.property") continue;
            if (sec.data().size() < 16) continue;

            const auto* data = reinterpret_cast<const uint8_t*>(sec.data().data());
            const size_t sec_size = sec.data().size();
            size_t off = 0;
            while (off + 12 <= sec_size) {
                uint32_t namesz, descsz, type;
                std::memcpy(&namesz, data + off, 4);
                std::memcpy(&descsz, data + off + 4, 4);
                std::memcpy(&type,   data + off + 8, 4);

                const size_t name_pad = (namesz + 3) & ~3u;
                const size_t desc_start = off + 12 + name_pad;

                if (type == NT_GNU_PROPERTY_TYPE_0 && namesz == 4 &&
                    desc_start + descsz <= sec_size) {
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
                        const size_t entry_align = impl_->is_64 ? 8 : 4;
                        p += 8 + ((pr_datasz + entry_align - 1) & ~(entry_align - 1));
                    }
                }

                const size_t desc_pad = (descsz + 3) & ~3u;
                off = desc_start + desc_pad;
            }
            break;
        }
        return false;
    });
}

// ---------------------------------------------------------------------------
// ensure_cfi_note
// ---------------------------------------------------------------------------

/// Preserve .note.gnu.property after injecting executable code.
///
/// If the original binary declares CET/BTI, the kernel enforces that
/// ALL indirect-call targets carry landing pads.  Our stubs have
/// ENDBR64/BTI c, so we're compliant — but only if the note survives
/// the patching process.
///
/// We don't CREATE the note for binaries without it — adding CET/BTI
/// enforcement to code not compiled with -fcf-protection would crash
/// all non-stub indirect calls that lack ENDBR.
///
/// What we DO: verify the note section still exists after mutations.
/// If it was somehow lost (section reorder), rebuild it.
void ELFEditor::ensure_cfi_note() noexcept {
    // Check if the binary originally had CFI enforcement.
    // cfi_enforced() reads the live section data — if it returns true,
    // the note survived all mutations and will be written by save().
    if (!cfi_enforced()) return;

    // If we get here, the note exists and is intact.  elfio-modern preserves
    // all sections during save(), so no reconstruction needed.
    //
    // Defense-in-depth: verify the section is still in the section list.
    bool found = false;
    visit_elf(impl_->editor, [&](auto& editor) {
        for (std::size_t i = 0; i < editor.sections().size(); ++i) {
            if (editor.sections()[i].name() == ".note.gnu.property") {
                found = true;
                break;
            }
        }
    });

    if (!found) {
        // Section was lost — should never happen, but if it does,
        // we rebuild the note from scratch.
        constexpr uint32_t NT_GNU_PROPERTY_TYPE_0 = 5;

        std::vector<uint8_t> note;
        auto push32 = [&](uint32_t v) {
            for (int i = 0; i < 4; ++i)
                note.push_back(static_cast<uint8_t>(v >> (i * 8)));
        };

        push32(4);   // namesz ("GNU\0")
        push32(16);  // descsz (pr_type + pr_datasz + data + padding)
        push32(NT_GNU_PROPERTY_TYPE_0);
        note.push_back('G'); note.push_back('N');
        note.push_back('U'); note.push_back('\0');

        visit_elf(impl_->editor, [&](auto& editor) {
            const bool is_aarch64 = (editor.machine() == 0xB7);  // EM_AARCH64
            // Property entry
            push32(is_aarch64 ? 0xC0000000u : 0xC0000002u);
            push32(4);           // pr_datasz
            push32(0x00000001);  // IBT or BTI
            push32(0);           // alignment padding

            auto& new_sec = editor.add_section(".note.gnu.property",
                                                elfio::SHT_NOTE, elfio::SHF_ALLOC);
            new_sec.set_addr_align(8);
            new_sec.set_data(reinterpret_cast<const char*>(note.data()),
                              note.size());
        });
    }
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
ELFEditor::find_text_gaps_impl(std::size_t min_size) const noexcept {
    if (impl_->text_size == 0) return {};

    const uint8_t* data = nullptr;
    size_t text_len = 0;
    visit_elf(const_cast<ElfEditorVariant&>(impl_->editor), [&](auto& editor) {
        auto& sec = editor.sections()[impl_->text_sec_idx];
        data = reinterpret_cast<const uint8_t*>(sec.data().data());
        text_len = sec.data().size();
    });
    if (!data || text_len == 0) return {};
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
    if (impl_->text_size == 0)
        return fail(diag, DC::PatchBinaryReadFailed, "no .text section for extend_text");

    // 2. Compute the new data's VA = .text VA + current size, aligned
    const uint64_t orig_text_end_va = impl_->text_va + impl_->text_size;
    const uint64_t new_data_va = (orig_text_end_va + alignment - 1) & ~(alignment - 1);
    const uint64_t padding = new_data_va - orig_text_end_va;
    const uint64_t growth = static_cast<uint64_t>(padding) + data.size();

    bool found_seg = false;
    visit_elf(impl_->editor, [&](auto& editor) {
        auto& text_sec = editor.sections()[impl_->text_sec_idx];

        // 3. Grow the .text section data: append padding + payload
        const size_t orig_sec_size = text_sec.data().size();
        const size_t new_sec_size = orig_sec_size + static_cast<size_t>(padding) + data.size();

        std::vector<uint8_t> buf(new_sec_size, 0x00);
        std::memcpy(buf.data(), text_sec.data().data(), orig_sec_size);
        std::memcpy(buf.data() + orig_sec_size + static_cast<size_t>(padding),
                    data.data(), data.size());

        text_sec.set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

        // 1+4. Find the PT_LOAD segment that contains .text and update sizes
        for (std::size_t si = 0; si < editor.segments().size(); ++si) {
            auto& seg = editor.segments()[si];
            if (seg.type() != PT_LOAD)
                continue;
            const uint64_t seg_start = seg.vaddr();
            const uint64_t seg_end   = seg_start + seg.memsz();
            if (impl_->text_va >= seg_start && impl_->text_va < seg_end) {
                seg.set_memsz(seg.memsz() + growth);
                seg.set_filesz(seg.filesz() + growth);
                found_seg = true;
                break;
            }
        }

        // 5. Update cached .text size
        impl_->text_size = new_sec_size;
    });

    if (!found_seg)
        return fail(diag, DC::PatchSegmentCreationFailed,
                    "no PT_LOAD segment contains .text; cannot extend");

    return NewSegmentInfo{new_data_va, data.size()};
}

// ---------------------------------------------------------------------------
// next_segment_va
// ---------------------------------------------------------------------------

uint64_t ELFEditor::next_segment_va_impl(uint64_t alignment) const noexcept {
    uint64_t highest = 0;
    visit_elf(const_cast<ElfEditorVariant&>(impl_->editor), [&](auto& editor) {
        for (std::size_t i = 0; i < editor.segments().size(); ++i) {
            auto& seg = editor.segments()[i];
            if (seg.type() == PT_LOAD) {
                uint64_t end = seg.vaddr() + seg.memsz();
                if (end > highest) highest = end;
            }
        }
    });
    return (highest + alignment - 1) & ~(alignment - 1);
}

// ---------------------------------------------------------------------------
// add_segment
// ---------------------------------------------------------------------------

tl::expected<NewSegmentInfo, DC>
ELFEditor::add_segment_impl(std::string_view name,
                       const std::vector<uint8_t>& payload,
                       uint64_t alignment,
                       Common::DiagnosticCollector& /*diag*/) noexcept {
    // Next page-aligned VA after all existing PT_LOAD segments
    uint64_t highest = 0;
    visit_elf(impl_->editor, [&](auto& editor) {
        for (std::size_t i = 0; i < editor.segments().size(); ++i) {
            auto& seg = editor.segments()[i];
            if (seg.type() == PT_LOAD) {
                uint64_t end = seg.vaddr() + seg.memsz();
                if (end > highest) highest = end;
            }
        }
    });
    const uint64_t seg_va = (highest + alignment - 1) & ~(alignment - 1);

    visit_elf(impl_->editor, [&](auto& editor) {
        auto& new_sec = editor.add_section(std::string{name}, SHT_PROGBITS,
                                            SHF_ALLOC | SHF_WRITE | SHF_EXECINSTR);
        new_sec.set_addr_align(16);
        new_sec.set_address(seg_va);
        new_sec.set_data(reinterpret_cast<const char*>(payload.data()), payload.size());

        auto& new_seg = editor.add_segment(PT_LOAD, PF_R | PF_W);
        new_seg.set_align(alignment);
        new_seg.set_vaddr(seg_va);
        new_seg.set_paddr(seg_va);
        // In elfio-modern, add_section_index takes just the index
        auto sec_idx = static_cast<uint16_t>(editor.sections().size() - 1);  // just added
        new_seg.add_section_index(sec_idx);
    });

    // If the binary has CET/BTI enforcement, verify the .note.gnu.property
    // survives our mutations.  Our stubs have ENDBR/BTI c, so the note
    // declaring "this binary is IBT/BTI clean" remains truthful.
    ensure_cfi_note();

    return NewSegmentInfo{seg_va, payload.size()};
}

// ---------------------------------------------------------------------------
// add_runtime_dep
// ---------------------------------------------------------------------------

/// Inject DT_NEEDED for `soname` into the ELF .dynamic section.
///
/// Multi-layer fallback:
///   Layer 1: Steal a spare DT_NULL padding slot (no section growth).
///   Layer 2: Grow .dynamic by appending a new DT_NULL after the
///            overwritten entry (elfio-modern relayouts on save).
///   Layer 3: Return PatchRuntimeDepFailed — the patched binary will NOT
///            auto-load the runtime.  Stubs will dereference null call_slot.
tl::expected<void, DC>
ELFEditor::add_runtime_dep_impl(std::string_view soname,
                           Common::DiagnosticCollector& diag) noexcept {
    const std::string soname_str(soname);
    const bool is64 = impl_->is_64;

    // We need to operate on raw dynamic section bytes.  Use a lambda
    // dispatched through visit_elf to handle both 32/64-bit editors.
    tl::expected<void, DC> result = {};

    visit_elf(impl_->editor, [&](auto& editor) {
        // -- Find .dynamic section --
        const std::size_t sec_count = editor.sections().size();
        std::size_t dyn_idx = sec_count;
        for (std::size_t i = 0; i < sec_count; ++i) {
            if (editor.sections()[i].type() == SHT_DYNAMIC) {
                dyn_idx = i;
                break;
            }
        }
        if (dyn_idx == sec_count) {
            result = fail(diag, DC::PatchRuntimeDepFailed,
                        "no .dynamic section — cannot inject DT_NEEDED for '"
                        + soname_str + "' (static binary?)");
            return;
        }

        auto& dyn_sec = editor.sections()[dyn_idx];
        const size_t entry_size = dyn_sec.entry_size();
        if (entry_size == 0) {
            result = fail(diag, DC::PatchRuntimeDepFailed,
                        ".dynamic has entry_size 0 — malformed ELF");
            return;
        }

        auto str_link = dyn_sec.link();
        if (str_link == 0 || str_link >= sec_count) {
            result = fail(diag, DC::PatchRuntimeDepFailed,
                        ".dynamic has no linked .dynstr — cannot inject DT_NEEDED");
            return;
        }

        const size_t total_slots = dyn_sec.data().size() / entry_size;
        const auto* raw = reinterpret_cast<const uint8_t*>(dyn_sec.data().data());

        // Helper to read tag from a dynamic entry
        auto read_tag = [&](const uint8_t* ent) -> int64_t {
            if (is64) {
                elfio::Elf64_Dyn d;
                std::memcpy(&d, ent, sizeof(d));
                return static_cast<int64_t>(d.d_tag);
            } else {
                elfio::Elf32_Dyn d;
                std::memcpy(&d, ent, sizeof(d));
                return static_cast<int64_t>(d.d_tag);
            }
        };

        // Helper to write DT_NEEDED at a slot
        auto write_needed = [&](uint8_t* target, size_t str_offset) {
            if (is64) {
                elfio::Elf64_Dyn d{};
                d.d_tag      = static_cast<elfio::Elf_Sxword>(DT_NEEDED);
                d.d_un.d_val = static_cast<elfio::Elf_Xword>(str_offset);
                std::memcpy(target, &d, sizeof(d));
            } else {
                elfio::Elf32_Dyn d{};
                d.d_tag      = static_cast<elfio::Elf_Sword>(DT_NEEDED);
                d.d_un.d_val = static_cast<elfio::Elf_Word>(str_offset);
                std::memcpy(target, &d, sizeof(d));
            }
        };

        // ================================================================
        // Layer 1: Steal a spare DT_NULL slot (preferred — no section growth)
        // ================================================================
        if (raw && total_slots >= 2) {
            size_t null_idx = total_slots;
            for (size_t i = 0; i < total_slots; ++i) {
                if (read_tag(raw + i * entry_size) == static_cast<int64_t>(DT_NULL)) {
                    null_idx = i;
                    break;
                }
            }

            if (null_idx < total_slots && null_idx + 1 < total_slots) {
                // Add soname to .dynstr
                auto& strsec = editor.sections()[str_link];
                auto str_offset = append_string_to_section(strsec, soname_str);

                std::vector<uint8_t> buf(raw, raw + dyn_sec.data().size());
                write_needed(buf.data() + null_idx * entry_size, str_offset);
                dyn_sec.set_data(reinterpret_cast<const char*>(buf.data()), buf.size());
                return;  // Layer 1 success
            }

            diag.warn("loader", DC::None,
                      "no spare DT_NULL in .dynamic — falling back to section growth");
        }

        // ================================================================
        // Layer 2: Grow .dynamic
        // ================================================================
        size_t null_idx = total_slots;
        for (size_t i = 0; i < total_slots; ++i) {
            if (read_tag(raw + i * entry_size) == static_cast<int64_t>(DT_NULL)) {
                null_idx = i;
                break;
            }
        }

        if (null_idx >= total_slots) {
            result = fail(diag, DC::PatchRuntimeDepFailed,
                        "no DT_NULL terminator in .dynamic — cannot grow");
            return;
        }

        auto& strsec = editor.sections()[str_link];
        auto str_offset = append_string_to_section(strsec, soname_str);

        std::vector<uint8_t> buf(raw, raw + dyn_sec.data().size());
        buf.resize(buf.size() + entry_size, 0);

        write_needed(buf.data() + null_idx * entry_size, str_offset);
        dyn_sec.set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

        diag.warn("loader", DC::None,
                  "DT_NEEDED injected via .dynamic growth for '" + soname_str
                  + "' — verify output with readelf -d");
    });

    return result;
}

// ---------------------------------------------------------------------------
// invalidate_signature
// ---------------------------------------------------------------------------

void ELFEditor::invalidate_signature_impl() noexcept {
    // ELF has no binary-level code signature to invalidate.
}

// ---------------------------------------------------------------------------
// save
// ---------------------------------------------------------------------------

tl::expected<void, DC>
ELFEditor::save_impl(const std::string& path,
                Common::DiagnosticCollector& diag) noexcept {
    tl::expected<void, DC> result = {};
    visit_elf(impl_->editor, [&](auto& editor) {
        auto save_result = editor.save();
        if (!save_result) {
            result = fail(diag, DC::PatchBinaryWriteFailed,
                          "failed to serialize ELF: " + path + " (" +
                          std::string(elfio::to_string(save_result.error())) + ")");
            return;
        }
        std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
        if (!ofs.good()) {
            result = fail(diag, DC::PatchBinaryWriteFailed,
                          "failed to open file for writing: " + path);
            return;
        }
        ofs.write(save_result->data(),
                  static_cast<std::streamsize>(save_result->size()));
        if (!ofs.good()) {
            result = fail(diag, DC::PatchBinaryWriteFailed,
                          "failed to write ELF: " + path);
        }
    });
    return result;
}

}  // namespace VMPilot::Loader
