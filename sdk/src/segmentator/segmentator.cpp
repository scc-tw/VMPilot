#include <segmentator.hpp>

#include <file_type_parser.hpp>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

std::unique_ptr<Segmentator> VMPilot::SDK::Segmentator::create_segmentator(
    const std::string& filename) noexcept {
    auto segmentator = std::unique_ptr<Segmentator>(nullptr);

    try {
        segmentator->m_metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        spdlog::error("Error creating segmentator: {}", e.what());
    }

    const auto& registry = HandlerRegistry::instance();

    segmentator->m_file_handler =
        registry.createFileHandler(segmentator->m_metadata.format, filename);
    if (!segmentator->m_file_handler) {
        spdlog::error("Unsupported file format: {}",
                      static_cast<uint8_t>(segmentator->m_metadata.format));
    }

    // Build symbol table from file handler, then create arch handler
    NativeSymbolTable symbols;
    if (segmentator->m_file_handler) {
        symbols = segmentator->m_file_handler->getNativeSymbolTable();
    }

    segmentator->m_arch_handler = registry.createArchHandler(
        segmentator->m_metadata.arch, segmentator->m_metadata.mode, symbols);
    if (!segmentator->m_arch_handler) {
        spdlog::error("Unsupported architecture: {}",
                      static_cast<uint8_t>(segmentator->m_metadata.arch));
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
