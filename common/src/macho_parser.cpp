#include <macho_parser.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>

namespace VMPilot::Common::MachO {

// --- On-disk Mach-O structures (no macOS headers needed) ---

namespace raw {

constexpr uint32_t MH_MAGIC_64 = 0xFEEDFACF;
constexpr uint32_t MH_CIGAM_64 = 0xCFFAEDFE;

constexpr uint32_t LC_SEGMENT_64 = 0x19;
constexpr uint32_t LC_SYMTAB = 0x02;
constexpr uint32_t LC_DYSYMTAB = 0x0B;

constexpr uint32_t INDIRECT_SYMBOL_LOCAL = 0x80000000;
constexpr uint32_t INDIRECT_SYMBOL_ABS = 0x40000000;

struct mach_header_64 {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
};

struct load_command {
    uint32_t cmd;
    uint32_t cmdsize;
};

struct segment_command_64 {
    uint32_t cmd;
    uint32_t cmdsize;
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

struct section_64 {
    char sectname[16];
    char segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
};

struct symtab_command {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t symoff;
    uint32_t nsyms;
    uint32_t stroff;
    uint32_t strsize;
};

struct dysymtab_command {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t ilocalsym;
    uint32_t nlocalsym;
    uint32_t iextdefsym;
    uint32_t nextdefsym;
    uint32_t iundefsym;
    uint32_t nundefsym;
    uint32_t tocoff;
    uint32_t ntoc;
    uint32_t modtaboff;
    uint32_t nmodtab;
    uint32_t extrefsymoff;
    uint32_t nextrefsyms;
    uint32_t indirectsymoff;
    uint32_t nindirectsyms;
    uint32_t extreloff;
    uint32_t nextrel;
    uint32_t locreloff;
    uint32_t nlocrel;
};

struct nlist_64 {
    uint32_t n_strx;
    uint8_t n_type;
    uint8_t n_sect;
    uint16_t n_desc;
    uint64_t n_value;
};

}  // namespace raw

// --- Helpers ---

template <typename T>
static bool readAt(const std::vector<uint8_t>& data, size_t offset, T& out) {
    if (offset + sizeof(T) > data.size()) return false;
    std::memcpy(&out, data.data() + offset, sizeof(T));
    return true;
}

static std::string trimmedString(const char* buf, size_t max_len) {
    auto len = strnlen(buf, max_len);
    return {buf, len};
}

// --- Parser implementation ---

std::optional<Parser> Parser::open(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return std::nullopt;

    auto size = static_cast<size_t>(file.tellg());
    file.seekg(0);

    Parser parser;
    parser.file_data_.resize(size);
    if (!file.read(reinterpret_cast<char*>(parser.file_data_.data()), size)) {
        return std::nullopt;
    }

    // Read header
    raw::mach_header_64 header{};
    if (!readAt(parser.file_data_, 0, header)) return std::nullopt;

    if (header.magic != raw::MH_MAGIC_64) {
        // Only support 64-bit native byte order for now
        return std::nullopt;
    }

    parser.is_64bit_ = true;
    parser.cputype_ = header.cputype;
    parser.filetype_ = header.filetype;

    // Walk load commands
    size_t cmd_offset = sizeof(raw::mach_header_64);
    size_t symtab_offset = 0;
    size_t dysymtab_offset = 0;

    for (uint32_t i = 0; i < header.ncmds; ++i) {
        raw::load_command lc{};
        if (!readAt(parser.file_data_, cmd_offset, lc)) break;

        if (lc.cmd == raw::LC_SEGMENT_64) {
            raw::segment_command_64 seg{};
            if (!readAt(parser.file_data_, cmd_offset, seg)) break;

            size_t sect_offset =
                cmd_offset + sizeof(raw::segment_command_64);
            for (uint32_t j = 0; j < seg.nsects; ++j) {
                raw::section_64 sect{};
                if (!readAt(parser.file_data_, sect_offset, sect)) break;

                Section s;
                s.sectname = trimmedString(sect.sectname, 16);
                s.segname = trimmedString(sect.segname, 16);
                s.addr = sect.addr;
                s.size = sect.size;
                s.offset = sect.offset;
                s.reserved1 = sect.reserved1;
                parser.sections_.push_back(std::move(s));

                sect_offset += sizeof(raw::section_64);
            }
        } else if (lc.cmd == raw::LC_SYMTAB) {
            symtab_offset = cmd_offset;
        } else if (lc.cmd == raw::LC_DYSYMTAB) {
            dysymtab_offset = cmd_offset;
        }

        cmd_offset += lc.cmdsize;
    }

    // Parse symbol table
    if (symtab_offset != 0) {
        raw::symtab_command stcmd{};
        readAt(parser.file_data_, symtab_offset, stcmd);

        for (uint32_t i = 0; i < stcmd.nsyms; ++i) {
            raw::nlist_64 nl{};
            size_t nl_offset = stcmd.symoff + i * sizeof(raw::nlist_64);
            if (!readAt(parser.file_data_, nl_offset, nl)) break;

            Symbol sym;
            if (nl.n_strx < stcmd.strsize) {
                const char* str = reinterpret_cast<const char*>(
                    parser.file_data_.data() + stcmd.stroff + nl.n_strx);
                size_t max_len = stcmd.strsize - nl.n_strx;
                sym.name = trimmedString(str, max_len);
            }
            sym.value = nl.n_value;
            sym.type = nl.n_type;
            sym.sect = nl.n_sect;
            parser.symbols_.push_back(std::move(sym));
        }
    }

    // Parse indirect symbol table
    if (dysymtab_offset != 0) {
        raw::dysymtab_command dscmd{};
        readAt(parser.file_data_, dysymtab_offset, dscmd);

        parser.indirect_symbols_.resize(dscmd.nindirectsyms);
        for (uint32_t i = 0; i < dscmd.nindirectsyms; ++i) {
            uint32_t idx = 0;
            size_t off = dscmd.indirectsymoff + i * sizeof(uint32_t);
            if (!readAt(parser.file_data_, off, idx)) break;
            parser.indirect_symbols_[i] = idx;
        }
    }

    return parser;
}

std::optional<Section> Parser::findSection(std::string_view segname,
                                           std::string_view sectname) const {
    for (const auto& s : sections_) {
        if (s.segname == segname && s.sectname == sectname) {
            return s;
        }
    }
    return std::nullopt;
}

std::vector<uint8_t> Parser::readSectionData(const Section& section) const {
    if (section.offset + section.size > file_data_.size()) return {};
    return {file_data_.begin() + section.offset,
            file_data_.begin() + section.offset + section.size};
}

std::vector<IndirectEntry> Parser::resolveIndirectSymbols(
    const Section& section, uint64_t entry_size) const {
    std::vector<IndirectEntry> entries;

    if (entry_size == 0) return entries;
    uint32_t num_entries = static_cast<uint32_t>(section.size / entry_size);
    uint32_t base_index = section.reserved1;

    for (uint32_t i = 0; i < num_entries; ++i) {
        uint32_t isym_index = base_index + i;
        if (isym_index >= indirect_symbols_.size()) break;

        uint32_t sym_index = indirect_symbols_[isym_index];
        if (sym_index == raw::INDIRECT_SYMBOL_LOCAL ||
            sym_index == raw::INDIRECT_SYMBOL_ABS ||
            sym_index >= symbols_.size()) {
            continue;
        }

        IndirectEntry entry;
        entry.symbol_name = symbols_[sym_index].name;
        entry.address = section.addr + i * entry_size;
        entry.entry_size = entry_size;
        entries.push_back(std::move(entry));
    }

    return entries;
}

std::vector<IndirectEntry> Parser::stubEntries() const {
    auto stubs = findSection("__TEXT", "__stubs");
    if (!stubs) return {};

    // ARM64 stub size: 12 bytes (adrp + ldr + br)
    // x86_64 stub size: 6 bytes (jmp [rip+disp])
    constexpr uint32_t CPU_TYPE_ARM64 = 0x0100000C;
    constexpr uint32_t CPU_TYPE_X86_64 = 0x01000007;

    uint64_t stub_size = 0;
    if (cputype_ == CPU_TYPE_ARM64) {
        stub_size = 12;
    } else if (cputype_ == CPU_TYPE_X86_64) {
        stub_size = 6;
    } else {
        // Try to infer from section alignment
        if (stubs->size > 0) {
            auto num_indirect = indirect_symbols_.size() - stubs->reserved1;
            if (num_indirect > 0) {
                stub_size = stubs->size / num_indirect;
            }
        }
    }

    return resolveIndirectSymbols(*stubs, stub_size);
}

std::vector<IndirectEntry> Parser::gotEntries() const {
    // Try __DATA_CONST,__got first (modern macOS), then __DATA,__got
    auto got = findSection("__DATA_CONST", "__got");
    if (!got) got = findSection("__DATA", "__got");
    if (!got) return {};

    uint64_t ptr_size = is_64bit_ ? 8 : 4;
    return resolveIndirectSymbols(*got, ptr_size);
}

}  // namespace VMPilot::Common::MachO
