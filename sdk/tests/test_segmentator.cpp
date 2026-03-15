#include <ELFHandler.hpp>
#include <X86Handler.hpp>
#include <file_type_parser.hpp>
#include <utilities.hpp>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace VMPilot::SDK::Segmentator;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s <elf-binary>\n", argv[0]);
        return 1;
    }

    const std::string filename = argv[1];
    std::printf("=== Testing: %s ===\n\n", filename.c_str());

    // Step 1: Parse file metadata
    VMPilot::Common::FileMetadata metadata;
    try {
        metadata = VMPilot::Common::get_file_metadata(filename);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Failed to get metadata: %s\n", e.what());
        return 1;
    }
    std::printf("[1] File format: %s, arch: x86, mode: %s\n",
                metadata.format == VMPilot::Common::FileFormat::ELF ? "ELF"
                                                                    : "other",
                metadata.mode == VMPilot::Common::FileMode::MODE_64 ? "64-bit"
                                                                    : "32-bit");

    // Step 2: Create ELF handler
    ELFFileHandlerStrategy elf_handler(filename);

    // Step 3: Get begin/end addresses (PLT addresses of VMPilot_Begin/End)
    auto [begin_addr, end_addr] = elf_handler.getBeginEndAddr();
    std::printf("[2] VMPilot_Begin PLT addr: 0x%lx\n", begin_addr);
    std::printf("    VMPilot_End   PLT addr: 0x%lx\n", end_addr);

    if (begin_addr == static_cast<uint64_t>(-1) ||
        end_addr == static_cast<uint64_t>(-1)) {
        std::fprintf(stderr, "Failed to find VMPilot signatures\n");
        return 1;
    }

    // Step 4: Get text section
    auto text_section = elf_handler.getTextSection();
    auto text_base = elf_handler.getTextBaseAddr();
    std::printf("[3] .text section: base=0x%lx, size=%zu bytes\n", text_base,
                text_section.size());

    // Step 5: Get symbol table (PLT + GOT entries)
    auto symbol_table = elf_handler.getNativeSymbolTable();
    std::printf("[4] Symbol table: %zu entries\n", symbol_table.size());
    for (const auto& entry : symbol_table) {
        if (entry.name.find("VMPilot") != std::string::npos) {
            std::string entry_type = "DIRECT";
            auto it = entry.additionalAttributes.find("entry_type");
            if (it != entry.additionalAttributes.end()) {
                try {
                    entry_type = std::get<std::string>(it->second);
                } catch (...) {
                }
            }
            std::printf("    [%s] %s @ 0x%lx (size=%lu)\n",
                        entry_type.c_str(), entry.name.c_str(), entry.address,
                        entry.size);
        }
    }

    // Step 6: Create X86Handler with symbol table and load code
    X86Handler x86_handler(metadata.mode, symbol_table);
    if (!x86_handler.Load(text_section, text_base)) {
        std::fprintf(stderr, "Failed to load text section\n");
        return 1;
    }
    std::printf("[5] Disassembly loaded successfully\n");

    // Step 7: Get native functions (protected regions)
    auto native_functions = x86_handler.getNativeFunctions();
    std::printf("[6] Found %zu protected region(s)\n\n",
                native_functions.size());

    for (const auto& nf : native_functions) {
        std::printf("  Region: %s\n", nf->getName().c_str());
        std::printf("    Address: 0x%lx\n", nf->getAddr());
        std::printf("    Size:    %lu bytes\n", nf->getSize());
        std::printf("    Code:    ");
        auto code = nf->getCode();
        for (size_t i = 0; i < code.size() && i < 32; ++i) {
            std::printf("%02x ", code[i]);
        }
        if (code.size() > 32) std::printf("...");
        std::printf("\n\n");
    }

    return native_functions.empty() ? 1 : 0;
}
