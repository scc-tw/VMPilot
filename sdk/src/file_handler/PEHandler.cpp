#include <PEHandler.hpp>
#include <HandlerRegistry.hpp>
#include <Section.hpp>
#include <utilities.hpp>

#include <cstring>
#include <unordered_map>

#include <spdlog/spdlog.h>
#include <coffi/coffi.hpp>

using namespace VMPilot::SDK::Segmentator;

namespace VMPilot::SDK::Segmentator {
void registerPEHandler() {
    HandlerRegistry::instance().registerFileHandler(
        VMPilot::Common::FileFormat::PE, [](const std::string& filename) {
            return std::make_unique<PEFileHandlerStrategy>(filename);
        });
}
}  // namespace VMPilot::SDK::Segmentator

namespace {

#pragma pack(push, 1)
struct PE_IMPORT_DESCRIPTOR {
    uint32_t OriginalFirstThunk;  // RVA to ILT
    uint32_t TimeDateStamp;
    uint32_t ForwarderChain;
    uint32_t Name;        // RVA to DLL name string
    uint32_t FirstThunk;  // RVA to IAT
};
#pragma pack(pop)
}  // namespace

struct PEFileHandlerStrategy::Impl {
    coffi::coff_editor<coffi::pe32_traits> reader;
    uint64_t image_base = 0;
    bool is_pe32_plus = false;

    struct ImportEntry {
        std::string func_name;
        uint64_t iat_va;    // Virtual address of this IAT slot
        uint64_t iat_size;  // Size of IAT entry (4 or 8)
    };
    std::vector<ImportEntry> import_entries;
    bool imports_parsed = false;

    uint64_t text_base_addr = static_cast<uint64_t>(-1);
};

const char* PEFileHandlerStrategy::rvaToPtr(uint32_t rva) const noexcept {
    for (std::size_t i = 0; i < pImpl->reader.section_count(); ++i) {
        auto& sec = pImpl->reader.sections()[i];
        uint32_t sec_va = sec.virtual_address();
        auto sec_size = static_cast<uint32_t>(sec.data_length());
        if (sec_size == 0 || !sec.data_ptr())
            continue;
        if (rva >= sec_va && rva < sec_va + sec_size) {
            return sec.data_ptr() + (rva - sec_va);
        }
    }
    return nullptr;
}

void PEFileHandlerStrategy::parseImports() noexcept {
    if (pImpl->imports_parsed)
        return;
    pImpl->imports_parsed = true;

    auto* import_dir = pImpl->reader.directory(coffi::DIR_IMPORT);
    if (!import_dir)
        return;

    uint32_t import_rva = import_dir->virtual_address;
    uint32_t import_size = import_dir->size;
    if (import_rva == 0 || import_size == 0)
        return;

    const char* import_ptr = rvaToPtr(import_rva);
    if (!import_ptr)
        return;

    const bool is_pe32_plus = pImpl->is_pe32_plus;
    const uint64_t image_base = pImpl->image_base;
    const size_t thunk_size = is_pe32_plus ? 8 : 4;
    const uint64_t ordinal_flag = is_pe32_plus ? (1ULL << 63) : (1ULL << 31);

    // Iterate import descriptors (null-terminated array)
    for (auto desc = reinterpret_cast<const PE_IMPORT_DESCRIPTOR*>(import_ptr);
         desc->Name != 0; ++desc) {
        const char* dll_name = rvaToPtr(desc->Name);
        if (!dll_name)
            continue;

        // Prefer ILT (OriginalFirstThunk), fall back to IAT (FirstThunk)
        uint32_t ilt_rva = desc->OriginalFirstThunk ? desc->OriginalFirstThunk
                                                    : desc->FirstThunk;
        uint32_t iat_rva = desc->FirstThunk;

        const char* ilt_ptr = rvaToPtr(ilt_rva);
        if (!ilt_ptr)
            continue;

        for (size_t idx = 0;; ++idx) {
            uint64_t ilt_entry = 0;
            std::memcpy(&ilt_entry, ilt_ptr + idx * thunk_size, thunk_size);

            if (ilt_entry == 0)
                break;

            // Skip ordinal imports
            if (ilt_entry & ordinal_flag)
                continue;

            // Import by name: entry is RVA to (uint16_t hint + name)
            uint32_t hint_name_rva = static_cast<uint32_t>(ilt_entry);
            const char* hint_name_ptr = rvaToPtr(hint_name_rva);
            if (!hint_name_ptr)
                continue;

            // Skip 2-byte hint, read null-terminated name
            const char* func_name = hint_name_ptr + 2;

            uint64_t iat_va = image_base + iat_rva + idx * thunk_size;

            pImpl->import_entries.push_back(
                {std::string(func_name), iat_va, thunk_size});
        }
    }

    spdlog::info("PE: parsed {} import entries", pImpl->import_entries.size());
}

std::unique_ptr<PEFileHandlerStrategy::Impl>
VMPilot::SDK::Segmentator::make_pe_impl(const std::string& file_name) {
    auto impl = std::make_unique<PEFileHandlerStrategy::Impl>();

    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(file_name);
    if (!loaded) {
        throw std::runtime_error("Failed to parse PE file: " + file_name +
                                 " (" + std::string(coffi::to_string(loaded.error())) + ")");
    }
    impl->reader = std::move(*loaded);

    // Detect PE32+ by checking optional header magic
    auto* opt_hdr = impl->reader.optional_header();
    if (opt_hdr) {
        impl->is_pe32_plus = (opt_hdr->magic == coffi::OH_MAGIC_PE32PLUS);
    }

    auto* win_hdr = impl->reader.win_header();
    if (win_hdr) {
        impl->image_base = win_hdr->image_base;
    }

    return impl;
}

PEFileHandlerStrategy::PEFileHandlerStrategy(const std::string& file_name)
    : pImpl(make_pe_impl(file_name)) {}

PEFileHandlerStrategy::~PEFileHandlerStrategy() = default;

std::vector<uint8_t> PEFileHandlerStrategy::doGetTextSection() noexcept {
    auto* sec = pImpl->reader.find_section(".text");
    if (sec) {
        auto size = sec->data_length();
        const char* data = sec->data_ptr();
        if (!data || size == 0)
            return {};
        return std::vector<uint8_t>(data, data + size);
    }
    spdlog::error("Could not find .text section");
    return {};
}

uint64_t PEFileHandlerStrategy::doGetTextBaseAddr() noexcept {
    if (pImpl->text_base_addr != static_cast<uint64_t>(-1)) {
        return pImpl->text_base_addr;
    }

    auto* sec = pImpl->reader.find_section(".text");
    if (sec) {
        pImpl->text_base_addr =
            pImpl->image_base + sec->virtual_address();
        return pImpl->text_base_addr;
    }

    spdlog::error("Could not find .text section");
    return static_cast<uint64_t>(-1);
}

std::vector<VMPilot::SDK::Core::Section>
PEFileHandlerStrategy::doGetSections() noexcept {
    namespace Core = VMPilot::SDK::Core;
    std::vector<Core::Section> result;

    for (std::size_t i = 0; i < pImpl->reader.section_count(); ++i) {
        auto& sec = pImpl->reader.sections()[i];
        if (sec.data_length() == 0)
            continue;

        Core::Section s;
        s.base_addr = pImpl->image_base + sec.virtual_address();
        s.size = sec.data_length();
        s.name = std::string(sec.name());

        if (s.name == ".text")
            s.kind = Core::SectionKind::Text;
        else if (s.name == ".rdata")
            s.kind = Core::SectionKind::Rodata;
        else if (s.name == ".data")
            s.kind = Core::SectionKind::Data;
        else if (s.name == ".bss")
            s.kind = Core::SectionKind::Bss;
        else if (s.name == ".tls")
            s.kind = Core::SectionKind::Tls;
        else
            s.kind = Core::SectionKind::Unknown;

        // .text data lives in CompilationUnit.code; .bss is zero-initialized
        const bool skip_data =
            s.kind == Core::SectionKind::Text ||
            s.kind == Core::SectionKind::Bss;
        const char* data = sec.data_ptr();
        if (!skip_data && data) {
            s.data.assign(data, data + s.size);
        }

        result.push_back(std::move(s));
    }

    return result;
}

uint64_t PEFileHandlerStrategy::doGetImageBase() noexcept {
    return pImpl->image_base;
}

NativeSymbolTable PEFileHandlerStrategy::doGetSymbols() noexcept {
    return {};
}

std::vector<CallTarget> PEFileHandlerStrategy::doGetStubCallTargets() noexcept {
    parseImports();

    // Build reverse lookup: IAT VA -> function name
    std::unordered_map<uint64_t, std::string> iat_lookup;
    for (const auto& entry : pImpl->import_entries) {
        iat_lookup[entry.iat_va] = entry.func_name;
    }

    // PE has no dedicated stub section (unlike ELF .plt or Mach-O __stubs).
    // MSVC links import thunks inline in .text as `FF 25 <disp32>`:
    //   x64: jmp qword ptr [rip+disp]  (RIP-relative through IAT)
    //   x86: jmp dword ptr [addr]      (absolute through IAT)
    // We scan .text for these byte patterns to discover stub addresses.
    auto text = doGetTextSection();
    uint64_t text_base = doGetTextBaseAddr();
    if (text.empty() || text_base == static_cast<uint64_t>(-1))
        return {};

    constexpr size_t THUNK_SIZE = 6;  // FF 25 + 4-byte operand
    std::vector<CallTarget> targets;

    for (size_t pos = 0; pos + THUNK_SIZE <= text.size(); ++pos) {
        if (text[pos] != 0xFF || text[pos + 1] != 0x25)
            continue;

        int32_t operand;
        std::memcpy(&operand, &text[pos + 2], 4);

        uint64_t thunk_va = text_base + pos;
        uint64_t target_va;

        if (pImpl->is_pe32_plus) {
            // x64: RIP-relative — target = thunk_va + 6 + disp
            target_va = thunk_va + THUNK_SIZE +
                        static_cast<uint64_t>(static_cast<int64_t>(operand));
        } else {
            // x86: absolute address
            target_va = static_cast<uint64_t>(static_cast<uint32_t>(operand));
        }

        auto it = iat_lookup.find(target_va);
        if (it != iat_lookup.end()) {
            targets.push_back({it->second, thunk_va, THUNK_SIZE});
        }
    }

    return targets;
}

std::string PEFileHandlerStrategy::doGetCompilerInfo() noexcept {
    auto* opt_hdr = pImpl->reader.optional_header();
    if (!opt_hdr) return {};
    uint8_t major = opt_hdr->major_linker_version;
    uint8_t minor = opt_hdr->minor_linker_version;
    return "MSVC Linker " + std::to_string(major) + "." +
           std::to_string(minor);
}

std::vector<CallTarget>
PEFileHandlerStrategy::doGetPointerTableTargets() noexcept {
    parseImports();

    std::vector<CallTarget> targets;
    for (const auto& entry : pImpl->import_entries) {
        targets.push_back({entry.func_name, entry.iat_va, entry.iat_size});
    }
    return targets;
}
