#include <segmentator.hpp>

#include <ELFHandler.hpp>
#include <MachOHandler.hpp>
#include <PEHandler.hpp>
#include <X86Handler.hpp>
#include <file_type_parser.hpp>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

namespace {
namespace detail {
static const std::unordered_map<
    VMPilot::Common::FileFormat,
    std::function<std::unique_ptr<FileHandlerStrategy>(const std::string&)>>
    file_strategy_table = {
        {VMPilot::Common::FileFormat::ELF,
         [](const std::string& filename) {
             return std::make_unique<ELFFileHandlerStrategy>(filename);
         }},
        {VMPilot::Common::FileFormat::PE,
         [](const std::string& filename) {
             return std::make_unique<PEFileHandlerStrategy>(filename);
         }},
        {VMPilot::Common::FileFormat::MachO,
         [](const std::string& filename) {
             return std::make_unique<MachOFileHandlerStrategy>(filename);
         }},
};

// Architecture strategy table now takes mode + symbol table
static const std::unordered_map<
    VMPilot::Common::FileArch,
    std::function<std::unique_ptr<ArchHandlerStrategy>(
        VMPilot::Common::FileMode, const NativeSymbolTable&)>>
    arch_strategy_table = {
        {VMPilot::Common::FileArch::X86,
         [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
             return std::make_unique<X86Handler>(mode, symbols);
         }},
};

}  // namespace detail
}  // namespace

std::unique_ptr<Segmentator> VMPilot::SDK::Segmentator::create_segmentator(
    const std::string& filename) noexcept {
    auto segmentator = std::unique_ptr<Segmentator>(nullptr);

    try {
        segmentator->m_metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        spdlog::error("Error creating segmentator: {}", e.what());
    }

    const auto& format = segmentator->m_metadata.format;
    auto it = detail::file_strategy_table.find(format);
    if (it != detail::file_strategy_table.end()) {
        segmentator->m_file_handler = it->second(filename);
    } else {
        spdlog::error("Unsupported file format: {}",
                      static_cast<uint8_t>(format));
    }

    // Build symbol table from file handler, then create arch handler
    NativeSymbolTable symbols;
    if (segmentator->m_file_handler) {
        symbols = segmentator->m_file_handler->getNativeSymbolTable();
    }

    const auto& arch = segmentator->m_metadata.arch;
    auto it2 = detail::arch_strategy_table.find(arch);
    if (it2 != detail::arch_strategy_table.end()) {
        segmentator->m_arch_handler =
            it2->second(segmentator->m_metadata.mode, symbols);
    } else {
        spdlog::error("Unsupported architecture: {}",
                      static_cast<uint8_t>(arch));
    }

    return segmentator;
}

void VMPilot::SDK::Segmentator::Segmentator::segmentation() noexcept {
    if (m_file_handler == nullptr || m_arch_handler == nullptr) {
        spdlog::error("Segmentation failed: file_handler: {}, arch_handler: {}",
                      m_file_handler == nullptr, m_arch_handler == nullptr);
        return;
    }

    const auto text_section = m_file_handler->getTextSection();
    const auto text_base_addr = m_file_handler->getTextBaseAddr();

    if (text_base_addr == static_cast<uint64_t>(-1) || text_section.empty()) {
        spdlog::error(
            "Segmentation failed: text_base_addr: {}, text_section size: {}",
            text_base_addr, text_section.size());
        return;
    }

    if (!m_arch_handler->Load(text_section, text_base_addr)) {
        spdlog::error("Segmentation failed: load failed");
        return;
    }

    const auto native_functions = m_arch_handler->getNativeFunctions();
    if (native_functions.empty()) {
        spdlog::error("Segmentation failed: native_functions empty");
        return;
    }

    spdlog::info("Segmentation succeeded: found {} protected regions",
                 native_functions.size());
}
