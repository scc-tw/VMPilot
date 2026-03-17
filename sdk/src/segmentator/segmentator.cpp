#include <segmentator.hpp>

#include <CompilationContext.hpp>
#include <Strategy.hpp>
#include <file_type_parser.hpp>

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

const char* VMPilot::SDK::Segmentator::to_string(SegmentError e) noexcept {
    switch (e) {
        case SegmentError::FileNotFound:       return "file not found or corrupt";
        case SegmentError::UnsupportedFormat:   return "unsupported binary format";
        case SegmentError::UnsupportedArch:     return "unsupported architecture";
        case SegmentError::TextSectionMissing:  return "failed to read .text section";
        case SegmentError::DisassemblyFailed:   return "disassembly failed";
        case SegmentError::NoRegionsFound:      return "no protected regions found";
    }
    return "unknown error";
}

tl::expected<SegmentationResult, SegmentError>
VMPilot::SDK::Segmentator::segment(const std::string& filename) noexcept {
    registerBuiltinHandlers();

    // 1. Detect format & arch
    VMPilot::Common::FileMetadata metadata;
    try {
        metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        spdlog::error("segment: {}", e.what());
        return tl::make_unexpected(SegmentError::FileNotFound);
    }

    const auto& registry = HandlerRegistry::instance();

    // 2. Create file handler
    auto fh = registry.createFileHandler(metadata.format, filename);
    if (!fh) {
        spdlog::error("segment: unsupported format {}",
                      static_cast<uint8_t>(metadata.format));
        return tl::make_unexpected(SegmentError::UnsupportedFormat);
    }

    auto text = fh->getTextSection();
    auto text_base = fh->getTextBaseAddr();
    if (text.empty() || text_base == static_cast<uint64_t>(-1)) {
        spdlog::error("segment: failed to read .text section");
        return tl::make_unexpected(SegmentError::TextSectionMissing);
    }

    // 3. Build symbol table once
    auto symbols = fh->getNativeSymbolTable();

    // 4. Create arch handler
    auto ah = registry.createArchHandler(metadata.arch, metadata.mode, symbols);
    if (!ah) {
        spdlog::error("segment: unsupported arch {}",
                      static_cast<uint8_t>(metadata.arch));
        return tl::make_unexpected(SegmentError::UnsupportedArch);
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
        return tl::make_unexpected(SegmentError::DisassemblyFailed);
    }

    auto regions = ah->getNativeFunctions();
    if (regions.empty()) {
        spdlog::info("segment: no protected regions found");
        return tl::make_unexpected(SegmentError::NoRegionsFound);
    }

    // 7. Refine & group
    auto refined = RegionRefiner::refine(std::move(regions));
    auto groups = RegionRefiner::group(refined);

    SegmentationResult result;
    result.groups = std::move(groups);
    result.context = std::move(ctx);
    return result;
}
