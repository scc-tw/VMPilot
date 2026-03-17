#include <segmentator.hpp>

#include <CompilationContext.hpp>
#include <file_type_parser.hpp>

#include <new>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

// Forward-declare explicit registration functions from handler .o files.
// Calling these creates real symbol dependencies that force the linker
// to include handler .o files from the static library.
namespace VMPilot::SDK::Segmentator {
void registerELFHandler();
void registerPEHandler();
void registerMachOHandler();
void registerX86Handler();
void registerARM64Handler();
}  // namespace VMPilot::SDK::Segmentator

static void registerBuiltinHandlers() {
    static bool done = false;
    if (done) return;
    done = true;
    registerELFHandler();
    registerPEHandler();
    registerMachOHandler();
    registerX86Handler();
    registerARM64Handler();
}

std::optional<SegmentationResult> VMPilot::SDK::Segmentator::segment(
    const std::string& filename) noexcept {
    registerBuiltinHandlers();

    // 1. Detect format & arch
    VMPilot::Common::FileMetadata metadata;
    try {
        metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        spdlog::error("segment: {}", e.what());
        return std::nullopt;
    }

    const auto& registry = HandlerRegistry::instance();

    // 2. Create file handler
    auto fh = registry.createFileHandler(metadata.format, filename);
    if (!fh) {
        spdlog::error("segment: unsupported format {}",
                      static_cast<uint8_t>(metadata.format));
        return std::nullopt;
    }

    auto text = fh->getTextSection();
    auto text_base = fh->getTextBaseAddr();
    if (text.empty() || text_base == static_cast<uint64_t>(-1)) {
        spdlog::error("segment: failed to read .text section");
        return std::nullopt;
    }

    // 3. Build symbol table once
    auto symbols = fh->getNativeSymbolTable();

    // 4. Create arch handler
    auto ah = registry.createArchHandler(metadata.arch, metadata.mode, symbols);
    if (!ah) {
        spdlog::error("segment: unsupported arch {}",
                      static_cast<uint8_t>(metadata.arch));
        return std::nullopt;
    }

    // 5. Build compilation context
    CompilationContext ctx;
    ctx.symbols = std::move(symbols);
    ctx.arch = metadata.arch;
    ctx.mode = metadata.mode;
    ctx.rodata_sections = fh->getReadOnlySections();

    ah->setCompilationContext(ctx);  // copy — arch handler owns one

    // 6. Load & extract
    if (!ah->Load(text, text_base)) {
        spdlog::error("segment: disassembly failed");
        return std::nullopt;
    }

    auto regions = ah->getNativeFunctions();
    if (regions.empty()) {
        spdlog::info("segment: no protected regions found");
        return std::nullopt;
    }

    // 7. Refine & group
    auto refined = RegionRefiner::refine(std::move(regions));
    auto groups = RegionRefiner::group(refined);

    SegmentationResult result;
    result.groups = std::move(groups);
    result.context = std::move(ctx);
    return result;
}

std::unique_ptr<Segmentator> VMPilot::SDK::Segmentator::create_segmentator(
    const std::string& filename) noexcept {
    registerBuiltinHandlers();

    auto segmentator =
        std::unique_ptr<Segmentator>(new (std::nothrow) Segmentator());
    if (!segmentator) {
        spdlog::error("Failed to allocate Segmentator");
        return nullptr;
    }

    try {
        segmentator->m_metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        spdlog::error("Error creating segmentator: {}", e.what());
        return nullptr;
    }

    const auto& registry = HandlerRegistry::instance();

    segmentator->m_file_handler =
        registry.createFileHandler(segmentator->m_metadata.format, filename);
    if (!segmentator->m_file_handler) {
        spdlog::error("Unsupported file format: {}",
                      static_cast<uint8_t>(segmentator->m_metadata.format));
        return nullptr;
    }

    // Build symbol table from file handler, then create arch handler
    NativeSymbolTable symbols =
        segmentator->m_file_handler->getNativeSymbolTable();

    segmentator->m_arch_handler = registry.createArchHandler(
        segmentator->m_metadata.arch, segmentator->m_metadata.mode, symbols);
    if (!segmentator->m_arch_handler) {
        spdlog::error("Unsupported architecture: {}",
                      static_cast<uint8_t>(segmentator->m_metadata.arch));
        return nullptr;
    }

    // Build compilation context from file handler data
    CompilationContext ctx;
    ctx.symbols = segmentator->m_file_handler->getNativeSymbolTable();
    ctx.arch = segmentator->m_metadata.arch;
    ctx.mode = segmentator->m_metadata.mode;

    ctx.rodata_sections = segmentator->m_file_handler->getReadOnlySections();

    segmentator->m_arch_handler->setCompilationContext(std::move(ctx));

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

    auto native_functions = m_arch_handler->getNativeFunctions();
    if (native_functions.empty()) {
        spdlog::error("Segmentation failed: native_functions empty");
        return;
    }

    spdlog::info("Segmentation succeeded: found {} protected regions",
                 native_functions.size());
}
