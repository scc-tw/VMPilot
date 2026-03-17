#include <PEHandler.hpp>
#include <HandlerRegistry.hpp>
#include <utilities.hpp>

#include <cstring>
#include <unordered_map>

// clang-format off
// spdlog must be included before coffi. On Windows, spdlog transitively
// pulls in <windows.h> which defines IMAGE_* macros via winnt.h. COFFI's
// coffi_types.hpp defines the same constants with #ifndef guards, so if
// winnt.h is included first the guards prevent C4005 (macro redefinition)
// warnings. Reversing the order causes ~50 C4005 warnings on MSVC.
#include <spdlog/spdlog.h>
#include <coffi/coffi.hpp>
// clang-format on

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
constexpr uint32_t PE_DIRECTORY_IMPORT = 1;

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
    COFFI::coffi reader;
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
    auto& sections = pImpl->reader.get_sections();
    for (size_t i = 0; i < sections.get_count(); ++i) {
        auto* sec = sections[i];
        uint32_t sec_va = sec->get_virtual_address();
        uint32_t sec_size = sec->get_data_size();
        if (sec_size == 0 || !sec->get_data())
            continue;
        if (rva >= sec_va && rva < sec_va + sec_size) {
            return sec->get_data() + (rva - sec_va);
        }
    }
    return nullptr;
}

void PEFileHandlerStrategy::parseImports() noexcept {
    if (pImpl->imports_parsed)
        return;
    pImpl->imports_parsed = true;

    auto& dirs = pImpl->reader.get_directories();
    if (dirs.get_count() <= PE_DIRECTORY_IMPORT)
        return;

    auto* import_dir = dirs[PE_DIRECTORY_IMPORT];
    if (!import_dir)
        return;

    uint32_t import_rva = import_dir->get_virtual_address();
    uint32_t import_size = import_dir->get_size();
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

    if (!impl->reader.load(file_name)) {
        throw std::runtime_error("Failed to parse PE file: " + file_name);
    }

    impl->is_pe32_plus = impl->reader.is_PE32_plus();

    auto* win_hdr = impl->reader.get_win_header();
    if (win_hdr) {
        impl->image_base = win_hdr->get_image_base();
    }

    return impl;
}

PEFileHandlerStrategy::PEFileHandlerStrategy(const std::string& file_name)
    : pImpl(make_pe_impl(file_name)) {}

PEFileHandlerStrategy::~PEFileHandlerStrategy() = default;

std::vector<uint8_t> PEFileHandlerStrategy::doGetTextSection() noexcept {
    auto& sections = pImpl->reader.get_sections();
    for (size_t i = 0; i < sections.get_count(); ++i) {
        auto* sec = sections[i];
        if (sec->get_name() == ".text") {
            auto size = sec->get_data_size();
            const char* data = sec->get_data();
            if (!data || size == 0)
                return {};
            return std::vector<uint8_t>(data, data + size);
        }
    }
    spdlog::error("Could not find .text section");
    return {};
}

uint64_t PEFileHandlerStrategy::doGetTextBaseAddr() noexcept {
    if (pImpl->text_base_addr != static_cast<uint64_t>(-1)) {
        return pImpl->text_base_addr;
    }

    auto& sections = pImpl->reader.get_sections();
    for (size_t i = 0; i < sections.get_count(); ++i) {
        auto* sec = sections[i];
        if (sec->get_name() == ".text") {
            pImpl->text_base_addr =
                pImpl->image_base + sec->get_virtual_address();
            return pImpl->text_base_addr;
        }
    }

    spdlog::error("Could not find .text section");
    return static_cast<uint64_t>(-1);
}

std::vector<ReadOnlySection>
PEFileHandlerStrategy::doGetReadOnlySections() noexcept {
    std::vector<ReadOnlySection> result;
    auto& sections = pImpl->reader.get_sections();
    for (size_t i = 0; i < sections.get_count(); ++i) {
        auto* sec = sections[i];
        if (sec->get_name() == ".rdata") {
            auto size = sec->get_data_size();
            const char* data = sec->get_data();
            if (data && size > 0) {
                result.push_back(
                    {std::vector<uint8_t>(data, data + size),
                     pImpl->image_base + sec->get_virtual_address()});
            }
            break;
        }
    }
    return result;
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

std::vector<CallTarget>
PEFileHandlerStrategy::doGetPointerTableTargets() noexcept {
    parseImports();

    std::vector<CallTarget> targets;
    for (const auto& entry : pImpl->import_entries) {
        targets.push_back({entry.func_name, entry.iat_va, entry.iat_size});
    }
    return targets;
}
