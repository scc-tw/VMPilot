#include <ELFHandler.hpp>
#include <ELFSectionViewer.hpp>
#include <utilities.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

static FileHandlerRegistrar elf_registrar(
    VMPilot::Common::FileFormat::ELF, [](const std::string& filename) {
        return std::make_unique<ELFFileHandlerStrategy>(filename);
    });

struct ELFFileHandlerStrategy::Impl {
    ELFIO::elfio reader;
    std::unordered_map<std::string, ELFSectionViewer> section_table;
    uint64_t vmp_begin_addr = static_cast<uint64_t>(-1);
    uint64_t vmp_end_addr = static_cast<uint64_t>(-1);
    uint64_t text_base_addr = static_cast<uint64_t>(-1);
};

std::unique_ptr<ELFFileHandlerStrategy::Impl>
VMPilot::SDK::Segmentator::make_elf_impl(const std::string& file_name) {
    auto impl = std::make_unique<ELFFileHandlerStrategy::Impl>();
    if (!impl->reader.load(file_name)) {
        throw std::runtime_error("File not found or it is not an ELF file");
    }

    // Cache all section accessors in the section_table
    // This allows us to access the section_accessor by the section name directly
    for (auto&& section : impl->reader.sections) {
        impl->section_table[section->get_name()] =
            ELFSectionViewer(section.get());
    }

    return impl;
}

ELFFileHandlerStrategy::ELFFileHandlerStrategy(const std::string& file_name)
    : pImpl(make_elf_impl(file_name)) {}

ELFFileHandlerStrategy::~ELFFileHandlerStrategy() {
    pImpl->section_table.clear();
}

std::pair<uint64_t, uint64_t>
ELFFileHandlerStrategy::doGetBeginEndAddr() noexcept {
    if (pImpl->vmp_begin_addr == static_cast<uint64_t>(-1) ||
        pImpl->vmp_end_addr == static_cast<uint64_t>(-1)) {
        const auto& [begin_addr, end_addr] = doGetBeginEndAddrIntl();
        pImpl->vmp_begin_addr = begin_addr;
        pImpl->vmp_end_addr = end_addr;
    }

    return {pImpl->vmp_begin_addr, pImpl->vmp_end_addr};
}

std::vector<uint8_t> ELFFileHandlerStrategy::doGetTextSection() noexcept {
    const auto& chunk = this->doGetTextSectionIntl();
    if (chunk.empty()) {
        spdlog::error("Error: Could not find the .text section");
        return {};
    }
    return chunk;
}

uint64_t ELFFileHandlerStrategy::doGetTextBaseAddr() noexcept {
    if (pImpl->text_base_addr == static_cast<uint64_t>(-1)) {
        const auto& text_section = pImpl->section_table.find(".text");
        if (text_section == pImpl->section_table.end()) {
            spdlog::error("Error: Could not find the .text section");
            return static_cast<uint64_t>(-1);
        }

        pImpl->text_base_addr =
            text_section->second.getSection()->get_address();
    }

    return pImpl->text_base_addr;
}

uint64_t ELFFileHandlerStrategy::getEntryIndex(
    const std::string& signature) noexcept {
    const auto& dynsym = pImpl->section_table.find(".dynsym");
    if (dynsym == pImpl->section_table.end()) {
        return static_cast<uint64_t>(-1);
    }

    auto section = dynsym->second.getSection();
    if (!section) {
        spdlog::error("Failed to get the .dynsym section");
        return static_cast<uint64_t>(-1);
    }

    ELFIO::elfio& reader = pImpl->reader;  // NOLINT
    ELFIO::symbol_section_accessor accessor(reader, section);
    auto sym_count = accessor.get_symbols_num();
    for (size_t i = 0; i < sym_count; ++i) {
        std::string name;
        ELFIO::Elf64_Addr value;
        ELFIO::Elf_Xword sym_size;
        unsigned char bind;
        unsigned char type;
        ELFIO::Elf_Half section_index;
        unsigned char other;

        if (!accessor.get_symbol(i, name, value, sym_size, bind, type,
                                 section_index, other)) {
            spdlog::error("Failed to get the symbol at index {}", i);
            continue;
        }

        if (name == signature) {
            return i;
        }
    }

    return static_cast<uint64_t>(-1);
}

uint64_t ELFFileHandlerStrategy::getRelapltIdx(uint64_t dynsym_idx) noexcept {
    // 32-bit is .rel.plt, 64-bit is .rela.plt
    static const char* relaplt_name[] = {".rel.plt", ".rela.plt"};
    const int& is_64_bit = pImpl->reader.get_class() == ELFIO::ELFCLASS64;

    const auto& relaplt = pImpl->section_table.find(relaplt_name[is_64_bit]);
    if (relaplt == pImpl->section_table.end()) {
        return static_cast<uint64_t>(-1);
    }

    auto section = relaplt->second.getSection();
    if (!section) {
        spdlog::error("Failed to get the .rela.plt section");
        return static_cast<uint64_t>(-1);
    }

    const auto& reader = pImpl->reader;  // NOLINT
    ELFIO::relocation_section_accessor accessor(reader, section);
    auto size = accessor.get_entries_num();
    for (size_t i = 0; i < size; ++i) {
        ELFIO::Elf64_Addr offset;
        ELFIO::Elf_Word symbol;
        ELFIO::Elf_Sxword addend;
        unsigned int type;

        if (!accessor.get_entry(i, offset, symbol, type, addend)) {
            spdlog::error("Failed to get the entry at index {}", i);
            continue;
        }

        if (symbol == dynsym_idx) {
            return i;
        }
    }

    return static_cast<uint64_t>(-1);
}

uint64_t ELFFileHandlerStrategy::getPltAddr(uint64_t relaplt_idx) noexcept {
    const auto& plt = pImpl->section_table.find(".plt");
    if (plt == pImpl->section_table.end()) {
        return static_cast<uint64_t>(-1);
    }

    const auto& section = plt->second.getSection();
    if (!section) {
        spdlog::error("Failed to get the .plt section");
        return static_cast<uint64_t>(-1);
    }

    uint64_t alignment = section->get_addr_align();
    uint64_t plt_base_addr = section->get_address();

    return plt_base_addr + alignment * (relaplt_idx + 1);
}

std::pair<uint64_t, uint64_t>
ELFFileHandlerStrategy::doGetBeginEndAddrIntl() noexcept {
    // Step 1: get the index of begin and end with getEntryIndex
    // Try all known mangling variants (Itanium, MSVC x64, MSVC x86)
    uint64_t begin_dynsym_idx = static_cast<uint64_t>(-1);
    for (const auto& sig : VMPilot::Common::BEGIN_VMPILOT_SIGNATURES) {
        begin_dynsym_idx = getEntryIndex(sig);
        if (begin_dynsym_idx != static_cast<uint64_t>(-1)) break;
    }
    uint64_t end_dynsym_idx = static_cast<uint64_t>(-1);
    for (const auto& sig : VMPilot::Common::END_VMPILOT_SIGNATURES) {
        end_dynsym_idx = getEntryIndex(sig);
        if (end_dynsym_idx != static_cast<uint64_t>(-1)) break;
    }
    if (begin_dynsym_idx == static_cast<uint64_t>(-1) ||
        end_dynsym_idx == static_cast<uint64_t>(-1)) {
        spdlog::error(
            "Error: Could not find the VMPilot signatures in the "
            ".dynsym section");
        return {static_cast<uint64_t>(-1), static_cast<uint64_t>(-1)};
    }

    // Step 2: get the index of the .rela.plt section
    uint64_t begin_relaplt_idx = getRelapltIdx(begin_dynsym_idx);
    uint64_t end_relaplt_idx = getRelapltIdx(end_dynsym_idx);
    if (begin_relaplt_idx == static_cast<uint64_t>(-1) ||
        end_relaplt_idx == static_cast<uint64_t>(-1)) {
        spdlog::error(
            "Error: Could not find the VMPilot signatures in the "
            ".rela.plt or .rel.plt section");
        return {static_cast<uint64_t>(-1), static_cast<uint64_t>(-1)};
    }

    // Step 3: get the base address and alignment of the .plt section
    uint64_t begin_addr = getPltAddr(begin_relaplt_idx);
    uint64_t end_addr = getPltAddr(end_relaplt_idx);
    if (begin_addr == static_cast<uint64_t>(-1) ||
        end_addr == static_cast<uint64_t>(-1)) {
        spdlog::error("Error: Could not find the .plt section");
        return {static_cast<uint64_t>(-1), static_cast<uint64_t>(-1)};
    }

    return {begin_addr, end_addr};
}

std::unordered_map<uint64_t, ELFFileHandlerStrategy::RelaInfo>
ELFFileHandlerStrategy::buildRelocationMap() noexcept {
    std::unordered_map<uint64_t, RelaInfo> result;

    const bool is_64_bit = pImpl->reader.get_class() == ELFIO::ELFCLASS64;
    const char* rela_name = is_64_bit ? ".rela.plt" : ".rel.plt";
    const auto& rela_it = pImpl->section_table.find(rela_name);
    if (rela_it == pImpl->section_table.end()) {
        return result;
    }

    auto rela_sec = rela_it->second.getSection();
    if (!rela_sec) {
        return result;
    }

    const auto& reader = pImpl->reader;  // NOLINT
    ELFIO::relocation_section_accessor accessor(reader, rela_sec);
    const auto count = accessor.get_entries_num();
    for (size_t i = 0; i < count; ++i) {
        ELFIO::Elf64_Addr offset;
        ELFIO::Elf_Word symbol;
        ELFIO::Elf_Sxword addend;
        unsigned int type;

        if (!accessor.get_entry(i, offset, symbol, type, addend)) {
            continue;
        }
        result[symbol] = {i, offset};
    }

    return result;
}

NativeSymbolTable ELFFileHandlerStrategy::doGetSymbols() noexcept {
    NativeSymbolTable table;

    const auto& dynsym_it = pImpl->section_table.find(".dynsym");
    if (dynsym_it == pImpl->section_table.end()) {
        spdlog::error("Could not find .dynsym section");
        return table;
    }

    auto dynsym_sec = dynsym_it->second.getSection();
    if (!dynsym_sec) {
        return table;
    }

    ELFIO::elfio& reader = pImpl->reader;  // NOLINT
    ELFIO::symbol_section_accessor accessor(reader, dynsym_sec);
    const auto count = accessor.get_symbols_num();

    for (size_t i = 0; i < count; ++i) {
        std::string name;
        ELFIO::Elf64_Addr value;
        ELFIO::Elf_Xword sym_size;
        unsigned char bind;
        unsigned char type;
        ELFIO::Elf_Half section_index;
        unsigned char other;

        if (!accessor.get_symbol(i, name, value, sym_size, bind, type,
                                 section_index, other)) {
            continue;
        }

        if (name.empty() || value == 0) {
            continue;
        }

        SymbolType sym_type = SymbolType::NOTYPE;
        if (type == ELFIO::STT_FUNC)
            sym_type = SymbolType::FUNC;
        else if (type == ELFIO::STT_OBJECT)
            sym_type = SymbolType::OBJECT;
        else if (type == ELFIO::STT_SECTION)
            sym_type = SymbolType::SECTION;
        else if (type == ELFIO::STT_FILE)
            sym_type = SymbolType::FILE;

        NativeSymbolTableEntry entry;
        entry.name = name;
        entry.address = value;
        entry.size = sym_size;
        entry.type = sym_type;
        entry.isGlobal = (bind == ELFIO::STB_GLOBAL || bind == ELFIO::STB_WEAK);
        table.push_back(std::move(entry));
    }

    return table;
}

std::vector<CallTarget>
ELFFileHandlerStrategy::doGetDirectCallTargets() noexcept {
    std::vector<CallTarget> targets;

    const auto& plt_it = pImpl->section_table.find(".plt");
    if (plt_it == pImpl->section_table.end()) {
        return targets;
    }

    auto plt_sec = plt_it->second.getSection();
    if (!plt_sec) {
        return targets;
    }

    const uint64_t plt_base = plt_sec->get_address();
    const uint64_t plt_align = plt_sec->get_addr_align();
    if (plt_base == 0 || plt_align == 0) {
        return targets;
    }

    const auto& dynsym_it = pImpl->section_table.find(".dynsym");
    if (dynsym_it == pImpl->section_table.end()) {
        return targets;
    }

    auto dynsym_sec = dynsym_it->second.getSection();
    if (!dynsym_sec) {
        return targets;
    }

    ELFIO::elfio& reader = pImpl->reader;  // NOLINT
    ELFIO::symbol_section_accessor sym_accessor(reader, dynsym_sec);
    auto rela_map = buildRelocationMap();

    for (auto& [dynsym_idx, info] : rela_map) {
        std::string name;
        ELFIO::Elf64_Addr value;
        ELFIO::Elf_Xword sym_size;
        unsigned char bind, type;
        ELFIO::Elf_Half section_index;
        unsigned char other;

        if (!sym_accessor.get_symbol(dynsym_idx, name, value, sym_size, bind,
                                     type, section_index, other)) {
            continue;
        }

        if (name.empty() || type != ELFIO::STT_FUNC) {
            continue;
        }

        uint64_t plt_addr = plt_base + plt_align * (info.rela_idx + 1);
        targets.push_back({std::move(name), plt_addr, plt_align});
    }

    return targets;
}

std::vector<CallTarget>
ELFFileHandlerStrategy::doGetIndirectCallTargets() noexcept {
    std::vector<CallTarget> targets;

    const bool is_64_bit = pImpl->reader.get_class() == ELFIO::ELFCLASS64;
    const uint64_t ptr_size = is_64_bit ? 8 : 4;

    const auto& dynsym_it = pImpl->section_table.find(".dynsym");
    if (dynsym_it == pImpl->section_table.end()) {
        return targets;
    }

    auto dynsym_sec = dynsym_it->second.getSection();
    if (!dynsym_sec) {
        return targets;
    }

    ELFIO::elfio& reader = pImpl->reader;  // NOLINT
    ELFIO::symbol_section_accessor sym_accessor(reader, dynsym_sec);
    auto rela_map = buildRelocationMap();

    for (auto& [dynsym_idx, info] : rela_map) {
        std::string name;
        ELFIO::Elf64_Addr value;
        ELFIO::Elf_Xword sym_size;
        unsigned char bind, type;
        ELFIO::Elf_Half section_index;
        unsigned char other;

        if (!sym_accessor.get_symbol(dynsym_idx, name, value, sym_size, bind,
                                     type, section_index, other)) {
            continue;
        }

        if (name.empty() || type != ELFIO::STT_FUNC) {
            continue;
        }

        targets.push_back({std::move(name), info.got_addr, ptr_size});
    }

    return targets;
}

std::vector<uint8_t> ELFFileHandlerStrategy::doGetTextSectionIntl() noexcept {
    const auto& text_section_iter = pImpl->section_table.find(".text");
    if (text_section_iter == pImpl->section_table.end()) {
        return {};
    }

    auto section = text_section_iter->second.getSection();
    std::vector<uint8_t> text_section(section->get_size());
    std::memcpy(text_section.data(), section->get_data(), section->get_size());

    return text_section;
}
