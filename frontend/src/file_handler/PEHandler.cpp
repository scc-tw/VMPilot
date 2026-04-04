#include <PEHandler.hpp>
#include <HandlerRegistry.hpp>
#include <Section.hpp>
#include <utilities.hpp>

#include <cstring>
#include <optional>
#include <unordered_map>
#include <variant>

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

// ---------------------------------------------------------------------------
// Impl — owns file data and uses coff_file (read-only, zero-copy) for parsing.
//
// Supports both PE32 and PE32+ via std::variant.  Architecture is detected
// once during construction; all accessors dispatch through std::visit.
// ---------------------------------------------------------------------------

struct PEFileHandlerStrategy::Impl {
    coffi::file_buffer buf;
    // Stored as optional because any_coff_file (variant) has no default ctor.
    std::optional<coffi::any_coff_file> file;

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

const char* PEFileHandlerStrategy::rvaToPtr(uint32_t /*rva*/) const noexcept {
    // Kept as stub to satisfy the header declaration.
    // Import parsing now uses coffi-modern's imports() range API.
    return nullptr;
}

// ---------------------------------------------------------------------------
// parseImports — coffi-modern imports() lazy range
// ---------------------------------------------------------------------------

void PEFileHandlerStrategy::parseImports() noexcept {
    if (pImpl->imports_parsed)
        return;
    pImpl->imports_parsed = true;

    std::visit([this](auto& file) {
        using file_t = std::decay_t<decltype(file)>;
        using traits_t = std::conditional_t<
            std::is_same_v<file_t, coffi::coff_file<coffi::pe32_traits>>,
            coffi::pe32_traits, coffi::pe32plus_traits>;

        const size_t thunk_size = traits_t::thunk_size;
        const uint64_t image_base = pImpl->image_base;

        // Walk the import directory through the typed import range.
        // Each import_module_ref provides dll_name() and symbols().
        // The symbol range walks the ILT (OriginalFirstThunk) or IAT.
        //
        // To reconstruct IAT VAs we need the FirstThunk RVA from each
        // descriptor.  The import_module_ref doesn't expose it directly,
        // so we also read the raw import directory for FirstThunk values.

        auto dir = file.data_directory(coffi::DIR_IMPORT);
        if (!dir || dir->virtual_address == 0 || dir->size == 0)
            return;

        const auto& resolver = file.rva();

        // Walk import descriptors manually for FirstThunk, but use the
        // typed symbols() range for name parsing (safe + cross-arch).
        uint32_t idt_rva = dir->virtual_address;
        uint32_t idt_remaining = dir->size;

        for (auto mod : file.imports()) {
            // Read the raw import descriptor to get FirstThunk RVA
            auto desc_view = resolver.to_view(idt_rva, sizeof(coffi::image_import_descriptor));
            if (!desc_view)
                break;
            coffi::image_import_descriptor desc;
            std::memcpy(&desc, desc_view->data(), sizeof(desc));

            // Null terminator check
            if (desc.name == 0 && desc.first_thunk == 0)
                break;

            uint32_t iat_rva = desc.first_thunk;
            size_t idx = 0;

            for (auto sym : mod.symbols()) {
                if (!sym.is_ordinal()) {
                    auto name = sym.name();
                    if (!name.empty()) {
                        uint64_t iat_va = image_base + iat_rva + idx * thunk_size;
                        pImpl->import_entries.push_back(
                            {std::string(name), iat_va, thunk_size});
                    }
                }
                ++idx;
            }

            idt_rva += static_cast<uint32_t>(sizeof(coffi::image_import_descriptor));
            idt_remaining = (idt_remaining >= sizeof(coffi::image_import_descriptor))
                          ? idt_remaining - static_cast<uint32_t>(sizeof(coffi::image_import_descriptor))
                          : 0;
        }

        spdlog::info("PE: parsed {} import entries", pImpl->import_entries.size());
    }, *pImpl->file);
}

// ---------------------------------------------------------------------------
// make_pe_impl — factory
// ---------------------------------------------------------------------------

std::unique_ptr<PEFileHandlerStrategy::Impl>
VMPilot::SDK::Segmentator::make_pe_impl(const std::string& file_name) {
    auto impl = std::make_unique<PEFileHandlerStrategy::Impl>();

    // Load file into an owned buffer (coff_file uses zero-copy byte_view).
    auto buf = coffi::file_buffer::from_file(file_name);
    if (!buf) {
        throw std::runtime_error("Failed to read PE file: " + file_name +
                                 " (" + std::string(coffi::to_string(buf.error())) + ")");
    }
    impl->buf = std::move(*buf);

    // Auto-detect PE32 vs PE32+ and parse with the correct traits.
    auto loaded = coffi::auto_load(impl->buf.view());
    if (!loaded) {
        throw std::runtime_error("Failed to parse PE file: " + file_name +
                                 " (" + std::string(coffi::to_string(loaded.error())) + ")");
    }
    impl->file = std::move(*loaded);

    // Extract common metadata via visit.
    std::visit([&](auto& file) {
        using file_t = std::decay_t<decltype(file)>;

        auto opt = file.optional_header();
        if (opt) {
            using traits_t = std::conditional_t<
                std::is_same_v<file_t, coffi::coff_file<coffi::pe32_traits>>,
                coffi::pe32_traits, coffi::pe32plus_traits>;
            impl->is_pe32_plus = (traits_t::magic == coffi::OH_MAGIC_PE32PLUS);
        }

        auto win = file.win_header();
        if (win) {
            impl->image_base = win->image_base;
        }
    }, *impl->file);

    return impl;
}

PEFileHandlerStrategy::PEFileHandlerStrategy(const std::string& file_name)
    : pImpl(make_pe_impl(file_name)) {}

PEFileHandlerStrategy::~PEFileHandlerStrategy() = default;

// ---------------------------------------------------------------------------
// Section accessors — dispatch through std::visit
// ---------------------------------------------------------------------------

std::vector<uint8_t> PEFileHandlerStrategy::doGetTextSection() noexcept {
    return std::visit([](auto& file) -> std::vector<uint8_t> {
        auto sec = coffi::find_first(file.sections(),
            [](auto s) { return s.name() == ".text"; });
        if (!sec) return {};
        auto data = sec->data();
        if (data.empty()) return {};
        auto* p = reinterpret_cast<const uint8_t*>(data.data());
        return std::vector<uint8_t>(p, p + data.size());
    }, *pImpl->file);
}

uint64_t PEFileHandlerStrategy::doGetTextBaseAddr() noexcept {
    if (pImpl->text_base_addr != static_cast<uint64_t>(-1)) {
        return pImpl->text_base_addr;
    }

    std::visit([this](auto& file) {
        auto sec = coffi::find_first(file.sections(),
            [](auto s) { return s.name() == ".text"; });
        if (sec) {
            pImpl->text_base_addr =
                pImpl->image_base + sec->virtual_address();
        }
    }, *pImpl->file);

    if (pImpl->text_base_addr == static_cast<uint64_t>(-1)) {
        spdlog::error("Could not find .text section");
    }
    return pImpl->text_base_addr;
}

std::vector<VMPilot::SDK::Core::Section>
PEFileHandlerStrategy::doGetSections() noexcept {
    namespace Core = VMPilot::SDK::Core;

    return std::visit([this](auto& file) {
        std::vector<Core::Section> result;

        for (auto sec : file.sections()) {
            auto data = sec.data();
            if (data.empty())
                continue;

            Core::Section s;
            s.base_addr = pImpl->image_base + sec.virtual_address();
            s.size = data.size();
            auto name_sv = sec.name();
            s.name = std::string(name_sv);

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

            const bool skip_data =
                s.kind == Core::SectionKind::Text ||
                s.kind == Core::SectionKind::Bss;
            if (!skip_data) {
                auto* p = reinterpret_cast<const uint8_t*>(data.data());
                s.data.assign(p, p + data.size());
            }

            result.push_back(std::move(s));
        }

        return result;
    }, *pImpl->file);
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
    return std::visit([](auto& file) -> std::string {
        auto opt = file.optional_header();
        if (!opt) return {};
        uint8_t major = opt->major_linker_version;
        uint8_t minor = opt->minor_linker_version;
        return "MSVC Linker " + std::to_string(major) + "." +
               std::to_string(minor);
    }, *pImpl->file);
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
