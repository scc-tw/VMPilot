#include <HandlerRegistry.hpp>

#include <string>
#include <utility>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

// --- FileHandlerStrategy ---

std::vector<uint8_t> FileHandlerStrategy::doGetTextSection() noexcept {
    spdlog::error("FileHandlerStrategy::doGetTextSection not implemented");
    return {};
}

uint64_t FileHandlerStrategy::doGetTextBaseAddr() noexcept {
    spdlog::error("FileHandlerStrategy::doGetTextBaseAddr not implemented");
    return static_cast<uint64_t>(-1);
}

std::vector<VMPilot::SDK::Core::Section>
FileHandlerStrategy::doGetSections() noexcept {
    return {};
}

NativeSymbolTable FileHandlerStrategy::doGetSymbols() noexcept {
    spdlog::error("FileHandlerStrategy::doGetSymbols not implemented");
    return {};
}

std::vector<CallTarget> FileHandlerStrategy::doGetStubCallTargets() noexcept {
    spdlog::error("FileHandlerStrategy::doGetStubCallTargets not implemented");
    return {};
}

std::vector<CallTarget>
FileHandlerStrategy::doGetPointerTableTargets() noexcept {
    spdlog::error(
        "FileHandlerStrategy::doGetPointerTableTargets not implemented");
    return {};
}

std::string FileHandlerStrategy::doGetCompilerInfo() noexcept {
    return {};
}

std::vector<VMPilot::SDK::Core::RelocationEntry>
FileHandlerStrategy::doGetTextRelocations() noexcept {
    return {};
}

std::vector<uint8_t> FileHandlerStrategy::getTextSection() {
    return doGetTextSection();
}

uint64_t FileHandlerStrategy::getTextBaseAddr() {
    return doGetTextBaseAddr();
}

std::vector<VMPilot::SDK::Core::Section> FileHandlerStrategy::getSections() {
    return doGetSections();
}

uint64_t FileHandlerStrategy::getImageBase() {
    return doGetImageBase();
}

std::string FileHandlerStrategy::getCompilerInfo() {
    return doGetCompilerInfo();
}

std::vector<VMPilot::SDK::Core::RelocationEntry>
FileHandlerStrategy::getTextRelocations() {
    return doGetTextRelocations();
}

NativeSymbolTable FileHandlerStrategy::getNativeSymbolTable() {
    NativeSymbolTable table;

    for (auto& sym : doGetSymbols()) {
        table.push_back(std::move(sym));
    }

    for (auto& target : doGetStubCallTargets()) {
        NativeSymbolTableEntry entry;
        entry.name = std::move(target.name);
        entry.address = target.address;
        entry.size = target.size;
        entry.type = SymbolType::FUNC;
        entry.isGlobal = true;
        entry.setAttribute("entry_type", std::string("stub"));
        table.push_back(std::move(entry));
    }

    for (auto& target : doGetPointerTableTargets()) {
        NativeSymbolTableEntry entry;
        entry.name = std::move(target.name);
        entry.address = target.address;
        entry.size = target.size;
        entry.type = SymbolType::OBJECT;
        entry.isGlobal = true;
        entry.setAttribute("entry_type", std::string("pointer_table"));
        table.push_back(std::move(entry));
    }

    return table;
}

// --- ArchHandlerStrategy ---

ArchHandlerStrategy::ArchHandlerStrategy()
    : ArchHandlerStrategy(Arch::X86, Mode::MODE_64) {}

ArchHandlerStrategy::ArchHandlerStrategy(Arch arch, Mode mode)
    : m_arch(arch), m_mode(mode) {}

void ArchHandlerStrategy::setCompilationContext(CompilationContext ctx) {
    m_compilation_ctx = std::move(ctx);
}

const CompilationContext& ArchHandlerStrategy::getCompilationContext() const {
    return m_compilation_ctx;
}

bool ArchHandlerStrategy::doLoad(const std::vector<uint8_t>& code,
                                 const uint64_t base_addr) {
    spdlog::error(
        "ArchHandlerStrategy::doLoad not implemented: code size: {}, "
        "base_addr: {}",
        code.size(), base_addr);
    return false;
}

std::vector<NativeFunctionBase>
ArchHandlerStrategy::doGetNativeFunctions() {
    spdlog::error("ArchHandlerStrategy::doGetNativeFunctions not implemented");
    return {};
}

bool ArchHandlerStrategy::Load(const std::vector<uint8_t>& code,
                               const uint64_t base_addr) {
    return doLoad(code, base_addr);
}

std::vector<NativeFunctionBase>
ArchHandlerStrategy::getNativeFunctions() {
    return doGetNativeFunctions();
}

// --- HandlerRegistry ---

HandlerRegistry& HandlerRegistry::instance() {
    static HandlerRegistry reg;
    return reg;
}

void HandlerRegistry::registerFileHandler(VMPilot::Common::FileFormat format,
                                          FileHandlerFactory factory) {
    file_handlers_[format] = std::move(factory);
}

void HandlerRegistry::registerArchHandler(VMPilot::Common::FileArch arch,
                                          ArchHandlerFactory factory) {
    arch_handlers_[arch] = std::move(factory);
}

std::unique_ptr<FileHandlerStrategy> HandlerRegistry::createFileHandler(
    VMPilot::Common::FileFormat format, const std::string& filename) const {
    auto it = file_handlers_.find(format);
    if (it != file_handlers_.end())
        return it->second(filename);
    return nullptr;
}

std::unique_ptr<ArchHandlerStrategy> HandlerRegistry::createArchHandler(
    VMPilot::Common::FileArch arch, VMPilot::Common::FileMode mode,
    const NativeSymbolTable& symbols) const {
    auto it = arch_handlers_.find(arch);
    if (it != arch_handlers_.end())
        return it->second(mode, symbols);
    return nullptr;
}

// --- Registrar helpers ---

FileHandlerRegistrar::FileHandlerRegistrar(VMPilot::Common::FileFormat format,
                                           FileHandlerFactory factory) {
    HandlerRegistry::instance().registerFileHandler(format, std::move(factory));
}

ArchHandlerRegistrar::ArchHandlerRegistrar(VMPilot::Common::FileArch arch,
                                           ArchHandlerFactory factory) {
    HandlerRegistry::instance().registerArchHandler(arch, std::move(factory));
}
