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

    auto rodata = fh->getReadOnlyData();
    auto rodata_base = fh->getReadOnlyBaseAddr();
    if (!rodata.empty() && rodata_base != static_cast<uint64_t>(-1)) {
        ctx.rodata_sections.push_back({std::move(rodata), rodata_base});
        fprintf(stderr, ".rodata: 0x%" PRIx64 "  %zu bytes\n", rodata_base,
                ctx.rodata_sections.back().data.size());
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

    // 6. Print
    printf("Found %zu protected region(s):\n\n", refined.size());
    for (size_t i = 0; i < refined.size(); ++i) {
        const auto& r = refined[i];
        uint64_t end = r->getAddr() + r->getSize();
        printf("  [%zu] 0x%" PRIx64 " - 0x%" PRIx64 "  (%" PRIu64
               " bytes)  \"%s\"\n",
               i, r->getAddr(), end, r->getSize(), r->getName().c_str());
    }

    return 0;
}
