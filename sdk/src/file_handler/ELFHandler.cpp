#include <ELFHandler.hpp>
#include <ELFSectionViewer.hpp>
#include <HandlerRegistry.hpp>
#include <utilities.hpp>

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <elfio/elfio.hpp>
#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

namespace VMPilot::SDK::Segmentator {
void registerELFHandler() {
    HandlerRegistry::instance().registerFileHandler(
        VMPilot::Common::FileFormat::ELF, [](const std::string& filename) {
            return std::make_unique<ELFFileHandlerStrategy>(filename);
        });
}
}  // namespace VMPilot::SDK::Segmentator

using ElfFileVar = std::variant<
    std::monostate,
    elfio::elf_file<elfio::elf32_traits>,
    elfio::elf_file<elfio::elf64_traits>
>;

struct ELFFileHandlerStrategy::Impl {
    std::vector<char> file_buf;          // owns the memory backing byte_view
    ElfFileVar        file;
    bool is_64 = true;
    std::unordered_map<std::string, ELFSectionViewer> section_table;
    uint64_t text_base_addr = static_cast<uint64_t>(-1);
};

std::unique_ptr<ELFFileHandlerStrategy::Impl>
VMPilot::SDK::Segmentator::make_elf_impl(const std::string& file_name) {
    auto impl = std::make_unique<ELFFileHandlerStrategy::Impl>();

    // Read the entire file into memory
    {
        std::ifstream ifs(file_name, std::ios::binary | std::ios::ate);
        if (!ifs.good())
            throw std::runtime_error("File not found: " + file_name);
        auto sz = static_cast<std::size_t>(ifs.tellg());
        impl->file_buf.resize(sz);
        ifs.seekg(0);
        ifs.read(impl->file_buf.data(), static_cast<std::streamsize>(sz));
    }

    elfio::byte_view view{impl->file_buf.data(), impl->file_buf.size()};

    // Detect ELF class from EI_CLASS byte
    if (impl->file_buf.size() > 4) {
        auto ei_class = static_cast<uint8_t>(impl->file_buf[4]);
        impl->is_64 = (ei_class != 1);  // ELFCLASS32 = 1
    }

    auto load_and_cache = [&](auto traits_tag) {
        using Traits = decltype(traits_tag);
        auto loaded = elfio::elf_file<Traits>::from_view(view);
        if (!loaded) {
            throw std::runtime_error("File not found or it is not an ELF file: " +
                                     std::string(elfio::to_string(loaded.error())));
        }
        auto& elf = *loaded;
        // Cache section name -> index
        uint16_t sec_count = elf.section_count();
        for (uint16_t i = 0; i < sec_count; ++i) {
            auto sec = elf.sections()[i];
            impl->section_table[std::string(sec.name())] =
                ELFSectionViewer(i);
        }
        impl->file = std::move(elf);
    };

    if (impl->is_64)
        load_and_cache(elfio::elf64_traits{});
    else
        load_and_cache(elfio::elf32_traits{});

    return impl;
}

ELFFileHandlerStrategy::ELFFileHandlerStrategy(const std::string& file_name)
    : pImpl(make_elf_impl(file_name)) {}

ELFFileHandlerStrategy::~ELFFileHandlerStrategy() {
    pImpl->section_table.clear();
}

// Helper to visit the variant file (skips monostate)
namespace {
template <typename F>
void visit_file(ElfFileVar& v, F&& f) {
    std::visit([&](auto& alt) {
        if constexpr (!std::is_same_v<std::decay_t<decltype(alt)>, std::monostate>) {
            f(alt);
        }
    }, v);
}
template <typename F>
void visit_file(const ElfFileVar& v, F&& f) {
    std::visit([&](const auto& alt) {
        if constexpr (!std::is_same_v<std::decay_t<decltype(alt)>, std::monostate>) {
            f(alt);
        }
    }, v);
}
}  // namespace

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

        auto idx = text_section->second.getSectionIndex();
        visit_file(pImpl->file, [&](auto& f) {
            pImpl->text_base_addr = f.sections()[static_cast<uint16_t>(idx)].address();
        });
    }

    return pImpl->text_base_addr;
}

std::vector<VMPilot::SDK::Core::Section>
ELFFileHandlerStrategy::doGetSections() noexcept {
    namespace Core = VMPilot::SDK::Core;
    std::vector<Core::Section> result;

    for (auto& kv : pImpl->section_table) {
        const auto& sec_name = kv.first;
        auto idx = kv.second.getSectionIndex();
        visit_file(pImpl->file, [&](auto& f) {
            auto sec = f.sections()[static_cast<uint16_t>(idx)];
            if (sec.size() == 0)
                return;

            Core::Section s;
            s.base_addr = sec.address();
            s.size = sec.size();
            s.name = sec_name;

            // Classify by name
            if (sec_name == ".text")
                s.kind = Core::SectionKind::Text;
            else if (sec_name == ".rodata" || sec_name == ".rodata.str1.1" ||
                     sec_name == ".rodata.str1.8" || sec_name == ".rodata.cst4" ||
                     sec_name == ".rodata.cst8" || sec_name == ".rodata.cst16")
                s.kind = Core::SectionKind::Rodata;
            else if (sec_name == ".data" || sec_name == ".data.rel.ro")
                s.kind = Core::SectionKind::Data;
            else if (sec_name == ".bss")
                s.kind = Core::SectionKind::Bss;
            else if (sec_name == ".tdata" || sec_name == ".tbss")
                s.kind = Core::SectionKind::Tls;
            else if (sec_name.substr(0, 4) == ".got")
                s.kind = Core::SectionKind::Got;
            else if (sec_name.substr(0, 4) == ".plt")
                s.kind = Core::SectionKind::Plt;
            else
                s.kind = Core::SectionKind::Unknown;

            const bool skip_data =
                s.kind == Core::SectionKind::Bss ||
                s.kind == Core::SectionKind::Text;
            auto sec_data = sec.data();
            if (!skip_data && !sec_data.empty()) {
                auto sz = static_cast<std::size_t>(sec.size());
                s.data.resize(sz);
                std::memcpy(s.data.data(), sec_data.data(), sz);
            }

            result.push_back(std::move(s));
        });
    }

    return result;
}

uint64_t ELFFileHandlerStrategy::doGetImageBase() noexcept {
    uint64_t lowest = UINT64_MAX;
    visit_file(pImpl->file, [&](auto& f) {
        for (auto seg : f.segments()) {
            if (seg.type() == elfio::PT_LOAD) {
                uint64_t vaddr = seg.virtual_address();
                if (vaddr < lowest)
                    lowest = vaddr;
            }
        }
    });
    return (lowest == UINT64_MAX) ? 0 : lowest;
}

std::unordered_map<uint64_t, ELFFileHandlerStrategy::RelaInfo>
ELFFileHandlerStrategy::buildRelocationMap() noexcept {
    std::unordered_map<uint64_t, RelaInfo> result;

    const char* rela_name = pImpl->is_64 ? ".rela.plt" : ".rel.plt";
    const auto& rela_it = pImpl->section_table.find(rela_name);
    if (rela_it == pImpl->section_table.end()) {
        return result;
    }

    auto rela_idx = rela_it->second.getSectionIndex();

    visit_file(pImpl->file, [&](auto& f) {
        auto rela_sec = f.sections()[static_cast<uint16_t>(rela_idx)];

        if (pImpl->is_64) {
            // SHT_RELA sections
            std::size_t idx = 0;
            for (auto rel : f.relas(rela_sec)) {
                uint64_t symbol = rel.symbol();
                uint64_t offset = rel.r_offset();
                result[symbol] = {idx++, offset};
            }
        } else {
            // SHT_REL sections
            std::size_t idx = 0;
            for (auto rel : f.rels(rela_sec)) {
                uint64_t symbol = rel.symbol();
                uint64_t offset = rel.r_offset();
                result[symbol] = {idx++, offset};
            }
        }
    });

    return result;
}

NativeSymbolTable ELFFileHandlerStrategy::doGetSymbols() noexcept {
    NativeSymbolTable table;

    auto readSymbolSection = [&](const char* section_name) {
        const auto& it = pImpl->section_table.find(section_name);
        if (it == pImpl->section_table.end())
            return;

        auto sec_idx = it->second.getSectionIndex();

        visit_file(pImpl->file, [&](auto& f) {
            auto sec = f.sections()[static_cast<uint16_t>(sec_idx)];

            // Get linked string table section
            auto strtab_link = sec.link();
            if (strtab_link >= f.section_count()) return;
            auto strtab_sec = f.sections()[static_cast<uint16_t>(strtab_link)];
            elfio::string_table_view strtab{strtab_sec.data()};

            for (auto sym : f.symbols_with_strtab(sec, strtab)) {
                auto name_sv = sym.name();
                if (name_sv.empty() || sym.value() == 0)
                    continue;

                SymbolType sym_type = SymbolType::NOTYPE;
                if (sym.type() == elfio::STT_FUNC)
                    sym_type = SymbolType::FUNC;
                else if (sym.type() == elfio::STT_OBJECT)
                    sym_type = SymbolType::OBJECT;
                else if (sym.type() == elfio::STT_SECTION)
                    sym_type = SymbolType::SECTION;
                else if (sym.type() == elfio::STT_FILE)
                    sym_type = SymbolType::FILE;

                NativeSymbolTableEntry entry;
                entry.name = std::string(name_sv);
                entry.address = sym.value();
                entry.size = sym.size();
                entry.type = sym_type;
                entry.isGlobal =
                    (sym.bind() == elfio::STB_GLOBAL || sym.bind() == elfio::STB_WEAK);
                table.push_back(std::move(entry));
            }
        });
    };

    readSymbolSection(".dynsym");
    readSymbolSection(".symtab");

    if (table.empty()) {
        spdlog::error("Could not find symbols in .dynsym or .symtab");
    }

    return table;
}

std::vector<CallTarget>
ELFFileHandlerStrategy::doGetStubCallTargets() noexcept {
    std::vector<CallTarget> targets;

    const auto& plt_it = pImpl->section_table.find(".plt");
    if (plt_it == pImpl->section_table.end()) {
        return targets;
    }

    auto plt_idx = plt_it->second.getSectionIndex();
    uint64_t plt_base = 0;
    uint64_t plt_align = 0;

    visit_file(pImpl->file, [&](auto& f) {
        auto plt_sec = f.sections()[static_cast<uint16_t>(plt_idx)];
        plt_base = plt_sec.address();
        plt_align = plt_sec.addr_align();
    });

    if (plt_base == 0 || plt_align == 0) {
        return targets;
    }

    const auto& dynsym_it = pImpl->section_table.find(".dynsym");
    if (dynsym_it == pImpl->section_table.end()) {
        return targets;
    }

    auto dynsym_idx = dynsym_it->second.getSectionIndex();
    auto rela_map = buildRelocationMap();

    visit_file(pImpl->file, [&](auto& f) {
        auto dynsym_sec = f.sections()[static_cast<uint16_t>(dynsym_idx)];

        auto strtab_link = dynsym_sec.link();
        if (strtab_link >= f.section_count()) return;
        auto strtab_sec = f.sections()[static_cast<uint16_t>(strtab_link)];
        elfio::string_table_view strtab{strtab_sec.data()};

        // Build a vector of symbols for indexed access
        struct SymInfo {
            std::string name;
            unsigned char type;
        };
        std::vector<SymInfo> syms;
        for (auto sym : f.symbols_with_strtab(dynsym_sec, strtab)) {
            syms.push_back(SymInfo{std::string(sym.name()), sym.type()});
        }

        for (auto& [dynsym_index, info] : rela_map) {
            if (dynsym_index >= syms.size()) continue;
            auto& sym = syms[dynsym_index];

            if (sym.name.empty() || sym.type != elfio::STT_FUNC) {
                continue;
            }

            uint64_t plt_addr = plt_base + plt_align * (info.rela_idx + 1);
            targets.push_back({std::move(sym.name), plt_addr, plt_align});
        }
    });

    return targets;
}

std::vector<CallTarget>
ELFFileHandlerStrategy::doGetPointerTableTargets() noexcept {
    std::vector<CallTarget> targets;

    const uint64_t ptr_size = pImpl->is_64 ? 8 : 4;

    const auto& dynsym_it = pImpl->section_table.find(".dynsym");
    if (dynsym_it == pImpl->section_table.end()) {
        return targets;
    }

    auto dynsym_idx = dynsym_it->second.getSectionIndex();
    auto rela_map = buildRelocationMap();

    visit_file(pImpl->file, [&](auto& f) {
        auto dynsym_sec = f.sections()[static_cast<uint16_t>(dynsym_idx)];

        auto strtab_link = dynsym_sec.link();
        if (strtab_link >= f.section_count()) return;
        auto strtab_sec = f.sections()[static_cast<uint16_t>(strtab_link)];
        elfio::string_table_view strtab{strtab_sec.data()};

        // Build a vector of symbols for indexed access
        struct SymInfo {
            std::string name;
            unsigned char type;
        };
        std::vector<SymInfo> syms;
        for (auto sym : f.symbols_with_strtab(dynsym_sec, strtab)) {
            syms.push_back(SymInfo{std::string(sym.name()), sym.type()});
        }

        for (auto& [dynsym_index, info] : rela_map) {
            if (dynsym_index >= syms.size()) continue;
            auto& sym = syms[dynsym_index];

            if (sym.name.empty() || sym.type != elfio::STT_FUNC) {
                continue;
            }

            targets.push_back({std::move(sym.name), info.got_addr, ptr_size});
        }
    });

    return targets;
}

std::string ELFFileHandlerStrategy::doGetCompilerInfo() noexcept {
    const auto it = pImpl->section_table.find(".comment");
    if (it == pImpl->section_table.end())
        return {};

    auto idx = it->second.getSectionIndex();
    std::string info;
    visit_file(pImpl->file, [&](auto& f) {
        auto section = f.sections()[static_cast<uint16_t>(idx)];
        auto sec_data = section.data();
        if (sec_data.empty())
            return;

        auto sz = static_cast<std::size_t>(section.size());
        info.assign(sec_data.as_chars(), sz);
    });

    if (info.empty()) return {};

    for (char& c : info) {
        if (c == '\0') c = ' ';
    }
    auto start = info.find_first_not_of(' ');
    if (start == std::string::npos) return {};
    auto end = info.find_last_not_of(' ');
    return info.substr(start, end - start + 1);
}

std::vector<VMPilot::SDK::Core::RelocationEntry>
ELFFileHandlerStrategy::doGetTextRelocations() noexcept {
    namespace Core = VMPilot::SDK::Core;
    std::vector<Core::RelocationEntry> result;

    const char* rela_name = pImpl->is_64 ? ".rela.text" : ".rel.text";
    const auto& rela_it = pImpl->section_table.find(rela_name);
    if (rela_it == pImpl->section_table.end())
        return result;

    auto rela_idx = rela_it->second.getSectionIndex();

    // Find symbol table for name resolution
    std::size_t sym_idx = 0;
    bool has_symtab = false;
    auto symtab_it = pImpl->section_table.find(".symtab");
    if (symtab_it != pImpl->section_table.end()) {
        sym_idx = symtab_it->second.getSectionIndex();
        has_symtab = true;
    }
    if (!has_symtab) {
        auto dynsym_it = pImpl->section_table.find(".dynsym");
        if (dynsym_it != pImpl->section_table.end()) {
            sym_idx = dynsym_it->second.getSectionIndex();
            has_symtab = true;
        }
    }

    visit_file(pImpl->file, [&](auto& f) {
        auto rela_sec = f.sections()[static_cast<uint16_t>(rela_idx)];

        // Build symbol name lookup if we have a symbol table
        std::vector<std::string> sym_names;
        if (has_symtab) {
            auto sym_sec = f.sections()[static_cast<uint16_t>(sym_idx)];
            auto strtab_link = sym_sec.link();
            if (strtab_link < f.section_count()) {
                auto strtab_sec = f.sections()[static_cast<uint16_t>(strtab_link)];
                elfio::string_table_view strtab{strtab_sec.data()};
                for (auto sym : f.symbols_with_strtab(sym_sec, strtab)) {
                    sym_names.push_back(std::string(sym.name()));
                }
            }
        }

        if (pImpl->is_64) {
            for (auto rel : f.relas(rela_sec)) {
                Core::RelocationEntry entry;
                entry.offset = rel.r_offset();
                entry.type = static_cast<unsigned int>(rel.type());
                entry.symbol_index = static_cast<uint32_t>(rel.symbol());
                entry.addend = rel.r_addend();

                if (entry.symbol_index > 0 &&
                    entry.symbol_index < sym_names.size()) {
                    entry.symbol_name = sym_names[entry.symbol_index];
                }

                result.push_back(std::move(entry));
            }
        } else {
            for (auto rel : f.rels(rela_sec)) {
                Core::RelocationEntry entry;
                entry.offset = rel.r_offset();
                entry.type = static_cast<unsigned int>(rel.type());
                entry.symbol_index = static_cast<uint32_t>(rel.symbol());
                entry.addend = 0;

                if (entry.symbol_index > 0 &&
                    entry.symbol_index < sym_names.size()) {
                    entry.symbol_name = sym_names[entry.symbol_index];
                }

                result.push_back(std::move(entry));
            }
        }
    });

    return result;
}

std::vector<uint8_t> ELFFileHandlerStrategy::doGetTextSectionIntl() noexcept {
    const auto& text_section_iter = pImpl->section_table.find(".text");
    if (text_section_iter == pImpl->section_table.end()) {
        return {};
    }

    auto idx = text_section_iter->second.getSectionIndex();
    std::vector<uint8_t> text_section;
    visit_file(pImpl->file, [&](auto& f) {
        auto section = f.sections()[static_cast<uint16_t>(idx)];
        auto sec_data = section.data();
        auto sz = static_cast<std::size_t>(section.size());
        text_section.resize(sz);
        std::memcpy(text_section.data(), sec_data.data(), sz);
    });

    return text_section;
}
