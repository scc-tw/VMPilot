#include <ARM64Handler.hpp>
#include <CompilationContext.hpp>
#include <ELFHandler.hpp>
#include <MachOHandler.hpp>
#include <PEHandler.hpp>
#include <RegionRefiner.hpp>
#include <X86Handler.hpp>
#include <file_type_parser.hpp>

#include <cinttypes>
#include <cstdio>

using namespace VMPilot::SDK::Segmentator;
using VMPilot::Common::FileArch;
using VMPilot::Common::FileFormat;
using VMPilot::Common::FileMode;

static const char* formatName(FileFormat fmt) {
    switch (fmt) {
        case FileFormat::ELF:
            return "ELF";
        case FileFormat::PE:
            return "PE";
        case FileFormat::MachO:
            return "Mach-O";
        default:
            return "unknown";
    }
}

static const char* archName(FileArch arch) {
    switch (arch) {
        case FileArch::X86:
            return "x86";
        case FileArch::ARM64:
            return "ARM64";
        default:
            return "unknown";
    }
}

static const char* modeName(FileMode mode) {
    switch (mode) {
        case FileMode::MODE_32:
            return "32-bit";
        case FileMode::MODE_64:
            return "64-bit";
        default:
            return "unknown";
    }
}

static std::unique_ptr<FileHandlerStrategy> makeFileHandler(FileFormat fmt,
                                                            const std::string& path) {
    switch (fmt) {
        case FileFormat::ELF:
            return std::make_unique<ELFFileHandlerStrategy>(path);
        case FileFormat::PE:
            return std::make_unique<PEFileHandlerStrategy>(path);
        case FileFormat::MachO:
            return std::make_unique<MachOFileHandlerStrategy>(path);
        default:
            return nullptr;
    }
}

static std::unique_ptr<ArchHandlerStrategy> makeArchHandler(
    FileArch arch, FileMode mode, const NativeSymbolTable& symbols) {
    switch (arch) {
        case FileArch::X86:
            return std::make_unique<X86Handler>(mode, symbols);
        case FileArch::ARM64:
            return std::make_unique<ARM64Handler>(mode, symbols);
        default:
            return nullptr;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    const std::string path = argv[1];

    // 1. Detect format & arch
    VMPilot::Common::FileMetadata metadata;
    try {
        metadata = VMPilot::Common::get_file_metadata(path);
    } catch (const std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }

    fprintf(stderr, "%s  %s %s\n", formatName(metadata.format),
            archName(metadata.arch), modeName(metadata.mode));

    // 2. Create file handler
    std::unique_ptr<FileHandlerStrategy> fh;
    try {
        fh = makeFileHandler(metadata.format, path);
    } catch (const std::exception& e) {
        fprintf(stderr, "Error opening binary: %s\n", e.what());
        return 1;
    }
    if (!fh) {
        fprintf(stderr, "Unsupported format: %s\n", formatName(metadata.format));
        return 1;
    }

    auto text = fh->getTextSection();
    auto text_base = fh->getTextBaseAddr();
    auto symbols = fh->getNativeSymbolTable();

    if (text.empty() || text_base == static_cast<uint64_t>(-1)) {
        fprintf(stderr, "Failed to read .text section\n");
        return 1;
    }

    fprintf(stderr, ".text: 0x%" PRIx64 "  %zu bytes  %zu symbols\n",
            text_base, text.size(), symbols.size());

    // 3. Create arch handler
    auto ah = makeArchHandler(metadata.arch, metadata.mode, symbols);
    if (!ah) {
        fprintf(stderr, "Unsupported arch: %s\n", archName(metadata.arch));
        return 1;
    }

    // 4. Build compilation context with rodata
    CompilationContext ctx;
    ctx.symbols = fh->getNativeSymbolTable();
    ctx.arch = metadata.arch;
    ctx.mode = metadata.mode;

    ctx.rodata_sections = fh->getReadOnlySections();
    for (const auto& sec : ctx.rodata_sections) {
        fprintf(stderr, ".rodata: 0x%" PRIx64 "  %zu bytes\n", sec.base_addr,
                sec.data.size());
    }

    ah->setCompilationContext(std::move(ctx));

    // 5. Load & extract
    if (!ah->Load(text, text_base)) {
        fprintf(stderr, "Failed to disassemble .text\n");
        return 1;
    }

    auto regions = ah->getNativeFunctions();
    auto refined = VMPilot::SDK::RegionRefiner::refine(std::move(regions));

    if (refined.empty()) {
        printf("No protected regions found.\n");
        return 0;
    }

    // 6. Group and print
    auto groups =
        VMPilot::SDK::RegionRefiner::group(refined);

    size_t total_regions = 0;
    for (const auto& g : groups)
        total_regions += g.sites.size();

    printf("Found %zu protected region(s) in %zu group(s):\n\n",
           total_regions, groups.size());

    for (size_t gi = 0; gi < groups.size(); ++gi) {
        const auto& g = groups[gi];
        size_t inlined = g.sites.size() - 1;
        printf("  Group [%zu] \"%s\"  (%zu cop%s%s)\n", gi,
               g.source_name.c_str(), g.sites.size(),
               g.sites.size() == 1 ? "y" : "ies",
               inlined > 0
                   ? (", " + std::to_string(inlined) + " inlined").c_str()
                   : "");

        for (size_t si = 0; si < g.sites.size(); ++si) {
            const auto& s = g.sites[si];
            uint64_t end = s.addr + s.size;
            printf("    [%s] 0x%" PRIx64 " - 0x%" PRIx64
                   "  (%" PRIu64 " bytes)",
                   s.is_canonical ? "canonical" : "inline  ",
                   s.addr, end, s.size);
            if (s.enclosing_symbol)
                printf("  in %s", s.enclosing_symbol->c_str());
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}
