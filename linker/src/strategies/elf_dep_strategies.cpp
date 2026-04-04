/// @file strategies/elf_dep_strategies.cpp
/// @brief FallbackChain strategy implementations for ELF DT_NEEDED injection.
///
/// Extracted from ELFEditor::add_runtime_dep to be independently testable
/// and composable via FallbackChain<StealDtNull, GrowDynamic>.

#include <strategies/elf_dep_strategies.hpp>

#include <elfio/elfio.hpp>

#include <cstring>
#include <string>
#include <vector>

namespace VMPilot::Loader::strategies {

using DC = Common::DiagnosticCode;

// elfio-modern constants
using elfio::SHT_DYNAMIC;
using elfio::DT_NULL;
using elfio::DT_NEEDED;

// ─────────────────────────────────────────────────────────────────────────────
// Shared helpers
// ─────────────────────────────────────────────────────────────────────────────

/// Validated dynamic section info (works generically via visit).
struct DynInfo {
    std::size_t dyn_sec_idx;
    std::size_t str_link;
    size_t entry_size;
    size_t total_slots;
    bool is64;
    const uint8_t* raw;
};

/// Find the index of the first DT_NULL entry.
static size_t find_dt_null(const DynInfo& info) noexcept {
    for (size_t i = 0; i < info.total_slots; ++i) {
        const uint8_t* ent = info.raw + i * info.entry_size;
        int64_t tag = 0;
        if (info.is64) {
            elfio::Elf64_Dyn d;
            std::memcpy(&d, ent, sizeof(d));
            tag = static_cast<int64_t>(d.d_tag);
        } else {
            elfio::Elf32_Dyn d;
            std::memcpy(&d, ent, sizeof(d));
            tag = static_cast<int64_t>(d.d_tag);
        }
        if (tag == static_cast<int64_t>(DT_NULL))
            return i;
    }
    return info.total_slots;  // not found
}

/// Write a DT_NEEDED entry at the given slot.
static void write_dt_needed(uint8_t* target, bool is64,
                             size_t str_offset) noexcept {
    if (is64) {
        elfio::Elf64_Dyn d{};
        d.d_tag = static_cast<elfio::Elf_Sxword>(DT_NEEDED);
        d.d_un.d_val = static_cast<elfio::Elf_Xword>(str_offset);
        std::memcpy(target, &d, sizeof(d));
    } else {
        elfio::Elf32_Dyn d{};
        d.d_tag = static_cast<elfio::Elf_Sword>(DT_NEEDED);
        d.d_un.d_val = static_cast<elfio::Elf_Word>(str_offset);
        std::memcpy(target, &d, sizeof(d));
    }
}

/// Append a NUL-terminated string to a section_entry's data, returning the
/// offset at which the string begins (i.e. the old data size).
template <typename Traits>
static size_t append_string_to_section(elfio::section_entry<Traits>& sec,
                                       const std::string& str) {
    const size_t offset = sec.data().size();
    std::vector<char> buf(sec.data().begin(), sec.data().end());
    buf.insert(buf.end(), str.begin(), str.end());
    buf.push_back('\0');
    sec.set_data(std::move(buf));
    return offset;
}

// ─────────────────────────────────────────────────────────────────────────────
// StealDtNull
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<void, DC>
StealDtNull::try_execute(Common::DiagnosticCollector& diag,
                          ElfEditorVariant& editor_var,
                          std::string_view soname) noexcept {
    tl::expected<void, DC> result = tl::make_unexpected(DC::PatchRuntimeDepFailed);

    std::visit([&](auto& editor) {
        // Determine bitness from traits
        using Traits = typename std::decay_t<decltype(editor)>::traits_type;
        constexpr bool is64 = sizeof(typename Traits::address_type) == 8;

        // Find .dynamic section
        const std::size_t sec_count = editor.sections().size();
        std::size_t dyn_idx = sec_count;
        for (std::size_t i = 0; i < sec_count; ++i) {
            if (editor.sections()[i].type() == SHT_DYNAMIC) {
                dyn_idx = i;
                break;
            }
        }
        if (dyn_idx == sec_count) {
            diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                      std::string("no .dynamic section for '") + std::string(soname) + "'");
            return;
        }

        auto& dyn_sec = editor.sections()[dyn_idx];
        size_t entry_size = dyn_sec.entry_size();
        if (entry_size == 0) {
            diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                      ".dynamic has entry_size 0");
            return;
        }

        auto str_link = dyn_sec.link();
        if (str_link == 0 || str_link >= sec_count) {
            diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                      ".dynamic has no linked .dynstr");
            return;
        }

        DynInfo info{};
        info.dyn_sec_idx = dyn_idx;
        info.str_link = str_link;
        info.entry_size = entry_size;
        info.total_slots = dyn_sec.data().size() / entry_size;
        info.is64 = is64;
        info.raw = reinterpret_cast<const uint8_t*>(dyn_sec.data().data());

        if (!info.raw || info.total_slots < 2) {
            diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                      "too few .dynamic slots");
            return;
        }

        size_t null_idx = find_dt_null(info);

        // Need at least 2 DT_NULL slots
        if (null_idx >= info.total_slots || null_idx + 1 >= info.total_slots) {
            diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                      "no spare DT_NULL slot to steal");
            return;
        }

        // Check that the NEXT slot is also DT_NULL
        const uint8_t* next_ent = info.raw + (null_idx + 1) * info.entry_size;
        int64_t next_tag = 0;
        if (is64) {
            elfio::Elf64_Dyn d;
            std::memcpy(&d, next_ent, sizeof(d));
            next_tag = static_cast<int64_t>(d.d_tag);
        } else {
            elfio::Elf32_Dyn d;
            std::memcpy(&d, next_ent, sizeof(d));
            next_tag = static_cast<int64_t>(d.d_tag);
        }
        if (next_tag != static_cast<int64_t>(DT_NULL)) {
            diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                      "no consecutive DT_NULL pair — only one terminator");
            return;
        }

        // Add soname to .dynstr
        auto& strsec = editor.sections()[str_link];
        auto str_offset = append_string_to_section(strsec, std::string(soname));

        // Overwrite the first DT_NULL with DT_NEEDED
        std::vector<uint8_t> buf(info.raw, info.raw + dyn_sec.data().size());
        write_dt_needed(buf.data() + null_idx * info.entry_size,
                        info.is64, str_offset);
        dyn_sec.set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

        result = {};
    }, editor_var);

    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// GrowDynamic
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<void, DC>
GrowDynamic::try_execute(Common::DiagnosticCollector& diag,
                          ElfEditorVariant& editor_var,
                          std::string_view soname) noexcept {
    tl::expected<void, DC> result = tl::make_unexpected(DC::PatchRuntimeDepFailed);

    std::visit([&](auto& editor) {
        using Traits = typename std::decay_t<decltype(editor)>::traits_type;
        constexpr bool is64 = sizeof(typename Traits::address_type) == 8;

        // Find .dynamic section
        const std::size_t sec_count = editor.sections().size();
        std::size_t dyn_idx = sec_count;
        for (std::size_t i = 0; i < sec_count; ++i) {
            if (editor.sections()[i].type() == SHT_DYNAMIC) {
                dyn_idx = i;
                break;
            }
        }
        if (dyn_idx == sec_count) {
            diag.note("GrowDynamic", DC::PatchRuntimeDepFailed,
                      std::string("no .dynamic section for '") + std::string(soname) + "'");
            return;
        }

        auto& dyn_sec = editor.sections()[dyn_idx];
        size_t entry_size = dyn_sec.entry_size();
        if (entry_size == 0) {
            diag.note("GrowDynamic", DC::PatchRuntimeDepFailed,
                      ".dynamic has entry_size 0");
            return;
        }

        auto str_link = dyn_sec.link();
        if (str_link == 0 || str_link >= sec_count) {
            diag.note("GrowDynamic", DC::PatchRuntimeDepFailed,
                      ".dynamic has no linked .dynstr");
            return;
        }

        DynInfo info{};
        info.dyn_sec_idx = dyn_idx;
        info.str_link = str_link;
        info.entry_size = entry_size;
        info.total_slots = dyn_sec.data().size() / entry_size;
        info.is64 = is64;
        info.raw = reinterpret_cast<const uint8_t*>(dyn_sec.data().data());

        size_t null_idx = find_dt_null(info);
        if (null_idx >= info.total_slots) {
            diag.note("GrowDynamic", DC::PatchRuntimeDepFailed,
                      "no DT_NULL terminator in .dynamic — cannot grow");
            return;
        }

        // Add soname to .dynstr
        auto& strsec = editor.sections()[str_link];
        auto str_offset = append_string_to_section(strsec, std::string(soname));

        // Overwrite DT_NULL with DT_NEEDED, append new DT_NULL
        std::vector<uint8_t> buf(info.raw, info.raw + dyn_sec.data().size());
        buf.resize(buf.size() + info.entry_size, 0);  // grow by one entry

        write_dt_needed(buf.data() + null_idx * info.entry_size,
                        info.is64, str_offset);

        // The appended zero-filled bytes are already DT_NULL (tag=0)
        dyn_sec.set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

        diag.note("GrowDynamic", DC::None,
                  std::string("grew .dynamic for '") + std::string(soname) + "'");
        result = {};
    }, editor_var);

    return result;
}

}  // namespace VMPilot::Loader::strategies
