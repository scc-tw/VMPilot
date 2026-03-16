#ifndef __COMMON_MACHO_PARSER_HPP__
#define __COMMON_MACHO_PARSER_HPP__
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace VMPilot::Common::MachO {

struct Section {
    std::string sectname;
    std::string segname;
    uint64_t addr = 0;
    uint64_t size = 0;
    uint32_t offset = 0;     // file offset to section data
    uint32_t reserved1 = 0;  // indirect symbol table index (for stubs/got)
};

struct Symbol {
    std::string name;
    uint64_t value = 0;
    uint8_t type = 0;
    uint8_t sect = 0;
};

struct IndirectEntry {
    std::string symbol_name;
    uint64_t address = 0;
    uint64_t entry_size = 0;
};

class Parser {
   public:
    /// Parse a Mach-O file. Returns std::nullopt on failure.
    static std::optional<Parser> open(const std::string& path);

    bool is64Bit() const noexcept { return is_64bit_; }
    uint32_t cpuType() const noexcept { return cputype_; }
    uint32_t fileType() const noexcept { return filetype_; }

    /// Find a section by segment and section name.
    std::optional<Section> findSection(std::string_view segname,
                                       std::string_view sectname) const;

    /// Read raw bytes of a section from the file data.
    std::vector<uint8_t> readSectionData(const Section& section) const;

    /// All parsed symbols.
    const std::vector<Symbol>& symbols() const noexcept { return symbols_; }

    /// Stub entries (__TEXT,__stubs) with resolved symbol names.
    std::vector<IndirectEntry> stubEntries() const;

    /// GOT entries (__DATA_CONST,__got or __DATA,__got) with resolved symbol names.
    std::vector<IndirectEntry> gotEntries() const;

   private:
    Parser() = default;

    /// Resolve indirect symbol entries for a given section.
    std::vector<IndirectEntry> resolveIndirectSymbols(
        const Section& section, uint64_t entry_size) const;

    std::vector<uint8_t> file_data_;
    std::vector<Section> sections_;
    std::vector<Symbol> symbols_;
    std::vector<uint32_t> indirect_symbols_;
    uint32_t cputype_ = 0;
    uint32_t filetype_ = 0;
    bool is_64bit_ = false;
};

}  // namespace VMPilot::Common::MachO

#endif  // __COMMON_MACHO_PARSER_HPP__
