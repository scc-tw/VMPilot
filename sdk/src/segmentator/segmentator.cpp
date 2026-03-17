#include <segmentator.hpp>

#include <CompilationContext.hpp>
#include <HandlerRegistry.hpp>
#include <file_type_parser.hpp>

using namespace VMPilot::SDK::Segmentator;
using VMPilot::Common::DiagnosticCode;

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

tl::expected<SegmentationResult, DiagnosticCode>
VMPilot::SDK::Segmentator::segment(
    const std::string& filename,
    Common::DiagnosticCollector& diag) noexcept {
    registerBuiltinHandlers();

    // 1. Detect format & arch
    VMPilot::Common::FileMetadata metadata;
    try {
        metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        diag.error("segmentator", DiagnosticCode::FileNotFound,
                   std::string("file not found or corrupt: ") + e.what(),
                   {}, 0);
        return tl::make_unexpected(DiagnosticCode::FileNotFound);
    }

    const auto& registry = HandlerRegistry::instance();

    // 2. Create file handler
    auto fh = registry.createFileHandler(metadata.format, filename);
    if (!fh) {
        diag.error("segmentator", DiagnosticCode::UnsupportedFormat,
                   "unsupported binary format: " +
                       std::to_string(static_cast<uint8_t>(metadata.format)));
        return tl::make_unexpected(DiagnosticCode::UnsupportedFormat);
    }

    auto text = fh->getTextSection();
    auto text_base = fh->getTextBaseAddr();
    if (text.empty() || text_base == static_cast<uint64_t>(-1)) {
        diag.error("segmentator", DiagnosticCode::TextSectionMissing,
                   "failed to read .text section");
        return tl::make_unexpected(DiagnosticCode::TextSectionMissing);
    }

    // 3. Build symbol table once
    auto symbols = fh->getNativeSymbolTable();

    // 4. Create arch handler
    auto ah = registry.createArchHandler(metadata.arch, metadata.mode, symbols);
    if (!ah) {
        diag.error("segmentator", DiagnosticCode::UnsupportedArch,
                   "unsupported architecture: " +
                       std::to_string(static_cast<uint8_t>(metadata.arch)));
        return tl::make_unexpected(DiagnosticCode::UnsupportedArch);
    }

    // 5. Build compilation context
    CompilationContext ctx;
    ctx.symbols = std::move(symbols);
    ctx.arch = metadata.arch;
    ctx.mode = metadata.mode;
    ctx.rodata_sections = fh->getReadOnlySections();

    ah->setCompilationContext(ctx);

    // 6. Load & extract
    if (!ah->Load(text, text_base)) {
        diag.error("segmentator", DiagnosticCode::DisassemblyFailed,
                   "disassembly failed");
        return tl::make_unexpected(DiagnosticCode::DisassemblyFailed);
    }

    auto regions = ah->getNativeFunctions();
    if (regions.empty()) {
        // NoRegionsFound is a Warning — return empty result, not error
        diag.warn("segmentator", DiagnosticCode::NoRegionsFound,
                  "no protected regions found in " + filename);
        SegmentationResult empty;
        empty.context = std::move(ctx);
        empty.binary_path = filename;
        empty.compiler_info = fh->getCompilerInfo();
        return empty;
    }

    // 7. Refine & group
    auto refined = RegionRefiner::refine(std::move(regions), diag);
    auto groups = RegionRefiner::group(refined, diag);

    SegmentationResult result;
    result.groups = std::move(groups);
    result.refined_regions = std::move(refined);
    result.context = std::move(ctx);
    result.binary_path = filename;
    result.compiler_info = fh->getCompilerInfo();
    return result;
}
