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

// ELFIO constants (avoid using namespace ELFIO to keep scope clean)
using ELFIO::SHT_DYNAMIC;
using ELFIO::DT_NULL;
using ELFIO::DT_NEEDED;

// ─────────────────────────────────────────────────────────────────────────────
// Shared helpers
// ─────────────────────────────────────────────────────────────────────────────

/// Find the .dynamic section.  Returns nullptr if not found.
static ELFIO::section* find_dynamic(ELFIO::elfio& reader) noexcept {
    for (auto& sec : reader.sections) {
        if (sec->get_type() == SHT_DYNAMIC)
            return sec.get();
    }
    return nullptr;
}

/// Validate .dynamic and locate .dynstr.  Returns (dyn_sec, str_link) or error.
struct DynInfo {
    ELFIO::section* dyn_sec;
    ELFIO::Elf_Half str_link;
    size_t entry_size;
    size_t total_slots;
    bool is64;
    const uint8_t* raw;
};

static tl::expected<DynInfo, DC> validate_dynamic(
    ELFIO::elfio& reader,
    Common::DiagnosticCollector& diag,
    std::string_view soname) noexcept
{
    auto* dyn_sec = find_dynamic(reader);
    if (!dyn_sec) {
        diag.note("ElfDep", DC::PatchRuntimeDepFailed,
                  std::string("no .dynamic section for '") + std::string(soname) + "'");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    size_t entry_size = dyn_sec->get_entry_size();
    if (entry_size == 0) {
        diag.note("ElfDep", DC::PatchRuntimeDepFailed,
                  ".dynamic has entry_size 0");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    auto str_link = dyn_sec->get_link();
    if (str_link == 0 || str_link >= reader.sections.size()) {
        diag.note("ElfDep", DC::PatchRuntimeDepFailed,
                  ".dynamic has no linked .dynstr");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    DynInfo info{};
    info.dyn_sec = dyn_sec;
    info.str_link = str_link;
    info.entry_size = entry_size;
    info.total_slots = dyn_sec->get_size() / entry_size;
    info.is64 = (reader.get_class() == ELFIO::ELFCLASS64);
    info.raw = reinterpret_cast<const uint8_t*>(dyn_sec->get_data());
    return info;
}

/// Find the index of the first DT_NULL entry.
static size_t find_dt_null(const DynInfo& info) noexcept {
    for (size_t i = 0; i < info.total_slots; ++i) {
        const uint8_t* ent = info.raw + i * info.entry_size;
        int64_t tag = 0;
        if (info.is64) {
            ELFIO::Elf64_Dyn d;
            std::memcpy(&d, ent, sizeof(d));
            tag = static_cast<int64_t>(d.d_tag);
        } else {
            ELFIO::Elf32_Dyn d;
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
        ELFIO::Elf64_Dyn d{};
        d.d_tag = static_cast<ELFIO::Elf_Sxword>(DT_NEEDED);
        d.d_un.d_val = static_cast<ELFIO::Elf_Xword>(str_offset);
        std::memcpy(target, &d, sizeof(d));
    } else {
        ELFIO::Elf32_Dyn d{};
        d.d_tag = static_cast<ELFIO::Elf_Sword>(DT_NEEDED);
        d.d_un.d_val = static_cast<ELFIO::Elf_Word>(str_offset);
        std::memcpy(target, &d, sizeof(d));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// StealDtNull
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<void, DC>
StealDtNull::try_execute(Common::DiagnosticCollector& diag,
                          ELFIO::elfio& reader,
                          std::string_view soname) noexcept {
    auto info_or = validate_dynamic(reader, diag, soname);
    if (!info_or) return tl::make_unexpected(info_or.error());
    auto& info = *info_or;

    if (!info.raw || info.total_slots < 2) {
        diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                  "too few .dynamic slots");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    size_t null_idx = find_dt_null(info);

    // Need at least 2 DT_NULL slots: one to steal, one to remain as terminator
    if (null_idx >= info.total_slots || null_idx + 1 >= info.total_slots) {
        diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                  "no spare DT_NULL slot to steal");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    // Check that the NEXT slot is also DT_NULL (the one that becomes terminator)
    const uint8_t* next_ent = info.raw + (null_idx + 1) * info.entry_size;
    int64_t next_tag = 0;
    if (info.is64) {
        ELFIO::Elf64_Dyn d;
        std::memcpy(&d, next_ent, sizeof(d));
        next_tag = static_cast<int64_t>(d.d_tag);
    } else {
        ELFIO::Elf32_Dyn d;
        std::memcpy(&d, next_ent, sizeof(d));
        next_tag = static_cast<int64_t>(d.d_tag);
    }
    if (next_tag != static_cast<int64_t>(DT_NULL)) {
        diag.note("StealDtNull", DC::PatchRuntimeDepFailed,
                  "no consecutive DT_NULL pair — only one terminator");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    // Add soname to .dynstr
    ELFIO::string_section_accessor strsec(reader.sections[info.str_link]);
    auto str_offset = strsec.add_string(std::string(soname));

    // Overwrite the first DT_NULL with DT_NEEDED
    std::vector<uint8_t> buf(info.raw, info.raw + info.dyn_sec->get_size());
    write_dt_needed(buf.data() + null_idx * info.entry_size,
                    info.is64, str_offset);
    info.dyn_sec->set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
// GrowDynamic
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<void, DC>
GrowDynamic::try_execute(Common::DiagnosticCollector& diag,
                          ELFIO::elfio& reader,
                          std::string_view soname) noexcept {
    auto info_or = validate_dynamic(reader, diag, soname);
    if (!info_or) return tl::make_unexpected(info_or.error());
    auto& info = *info_or;

    size_t null_idx = find_dt_null(info);
    if (null_idx >= info.total_slots) {
        diag.note("GrowDynamic", DC::PatchRuntimeDepFailed,
                  "no DT_NULL terminator in .dynamic — cannot grow");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    // Add soname to .dynstr
    ELFIO::string_section_accessor strsec(reader.sections[info.str_link]);
    auto str_offset = strsec.add_string(std::string(soname));

    // Overwrite DT_NULL with DT_NEEDED, append new DT_NULL
    std::vector<uint8_t> buf(info.raw, info.raw + info.dyn_sec->get_size());
    buf.resize(buf.size() + info.entry_size, 0);  // grow by one entry

    write_dt_needed(buf.data() + null_idx * info.entry_size,
                    info.is64, str_offset);

    // The appended zero-filled bytes are already DT_NULL (tag=0)
    info.dyn_sec->set_data(reinterpret_cast<const char*>(buf.data()), buf.size());

    diag.note("GrowDynamic", DC::None,
              std::string("grew .dynamic for '") + std::string(soname) + "'");
    return {};
}

}  // namespace VMPilot::Loader::strategies
