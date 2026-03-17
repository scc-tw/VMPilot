#include <MachOHandler.hpp>
#include <utilities.hpp>

#include <macho_parser.hpp>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

static FileHandlerRegistrar macho_registrar(
    VMPilot::Common::FileFormat::MachO, [](const std::string& filename) {
        return std::make_unique<MachOFileHandlerStrategy>(filename);
    });

/// Mach-O symbols have a leading underscore; strip it to match
/// the cross-platform signature (e.g. "__Z13VMPilot_BeginPKc" -> "_Z13VMPilot_BeginPKc")
static std::string stripLeadingUnderscore(const std::string& name) {
    if (!name.empty() && name[0] == '_') {
        return name.substr(1);
    }
    return name;
}

struct MachOFileHandlerStrategy::Impl {
    VMPilot::Common::MachO::Parser parser;
    uint64_t text_base_addr = static_cast<uint64_t>(-1);

    Impl(VMPilot::Common::MachO::Parser p) : parser(std::move(p)) {}
};

std::unique_ptr<MachOFileHandlerStrategy::Impl>
VMPilot::SDK::Segmentator::make_macho_impl(const std::string& filename) {
    auto parser = VMPilot::Common::MachO::Parser::open(filename);
    if (!parser) {
        throw std::runtime_error("Failed to parse Mach-O file: " + filename);
    }
    return std::make_unique<MachOFileHandlerStrategy::Impl>(std::move(*parser));
}

MachOFileHandlerStrategy::MachOFileHandlerStrategy(const std::string& filename)
    : pImpl(make_macho_impl(filename)) {}

MachOFileHandlerStrategy::~MachOFileHandlerStrategy() = default;

std::vector<uint8_t> MachOFileHandlerStrategy::doGetTextSection() noexcept {
    auto sect = pImpl->parser.findSection("__TEXT", "__text");
    if (!sect) {
        spdlog::error("Could not find __TEXT,__text section");
        return {};
    }
    return pImpl->parser.readSectionData(*sect);
}

uint64_t MachOFileHandlerStrategy::doGetTextBaseAddr() noexcept {
    if (pImpl->text_base_addr == static_cast<uint64_t>(-1)) {
        auto sect = pImpl->parser.findSection("__TEXT", "__text");
        if (!sect) {
            spdlog::error("Could not find __TEXT,__text section");
            return static_cast<uint64_t>(-1);
        }
        pImpl->text_base_addr = sect->addr;
    }
    return pImpl->text_base_addr;
}

std::vector<uint8_t> MachOFileHandlerStrategy::doGetReadOnlyData() noexcept {
    auto sect = pImpl->parser.findSection("__TEXT", "__const");
    if (!sect) {
        return {};
    }
    return pImpl->parser.readSectionData(*sect);
}

uint64_t MachOFileHandlerStrategy::doGetReadOnlyBaseAddr() noexcept {
    auto sect = pImpl->parser.findSection("__TEXT", "__const");
    if (!sect) {
        return static_cast<uint64_t>(-1);
    }
    return sect->addr;
}

NativeSymbolTable MachOFileHandlerStrategy::doGetSymbols() noexcept {
    NativeSymbolTable table;

    for (const auto& sym : pImpl->parser.symbols()) {
        if (sym.name.empty() || sym.value == 0)
            continue;

        NativeSymbolTableEntry entry;
        entry.name = stripLeadingUnderscore(sym.name);
        entry.address = sym.value;
        entry.size = 0;
        // Mach-O n_type: bit 0xe = N_TYPE mask
        // N_SECT (0xe) = defined in a section
        entry.type =
            (sym.type & 0x0e) == 0x0e ? SymbolType::FUNC : SymbolType::NOTYPE;
        entry.isGlobal = (sym.type & 0x01) != 0;  // N_EXT
        table.push_back(std::move(entry));
    }

    return table;
}

std::vector<CallTarget>
MachOFileHandlerStrategy::doGetStubCallTargets() noexcept {
    std::vector<CallTarget> targets;

    for (const auto& entry : pImpl->parser.stubEntries()) {
        targets.push_back({stripLeadingUnderscore(entry.symbol_name),
                           entry.address, entry.entry_size});
    }

    return targets;
}

std::vector<CallTarget>
MachOFileHandlerStrategy::doGetPointerTableTargets() noexcept {
    std::vector<CallTarget> targets;

    for (const auto& entry : pImpl->parser.gotEntries()) {
        targets.push_back({stripLeadingUnderscore(entry.symbol_name),
                           entry.address, entry.entry_size});
    }

    return targets;
}
