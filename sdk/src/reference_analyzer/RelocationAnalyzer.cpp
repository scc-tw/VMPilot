#include "RelocationAnalyzer.hpp"
#include "SectionLookup.hpp"

namespace VMPilot::SDK::ReferenceAnalyzer {

namespace {

// ELF x86-64 relocation types (from elf.h)
constexpr uint32_t R_X86_64_NONE = 0;
constexpr uint32_t R_X86_64_64 = 1;
constexpr uint32_t R_X86_64_PC32 = 2;
constexpr uint32_t R_X86_64_PLT32 = 4;
constexpr uint32_t R_X86_64_32 = 10;
constexpr uint32_t R_X86_64_32S = 11;
constexpr uint32_t R_X86_64_GOTPCREL = 9;
constexpr uint32_t R_X86_64_GOTPCRELX = 41;
constexpr uint32_t R_X86_64_REX_GOTPCRELX = 42;
constexpr uint32_t R_X86_64_TPOFF32 = 23;
constexpr uint32_t R_X86_64_TPOFF64 = 18;
constexpr uint32_t R_X86_64_GOTTPOFF = 22;
constexpr uint32_t R_X86_64_TLSGD = 19;
constexpr uint32_t R_X86_64_TLSLD = 20;

// AArch64 relocation types
constexpr uint32_t R_AARCH64_ABS64 = 257;
constexpr uint32_t R_AARCH64_ADR_PREL_PG_HI21 = 275;
constexpr uint32_t R_AARCH64_ADD_ABS_LO12_NC = 277;
constexpr uint32_t R_AARCH64_LDST64_ABS_LO12_NC = 286;
constexpr uint32_t R_AARCH64_ADR_GOT_PAGE = 311;
constexpr uint32_t R_AARCH64_LD64_GOT_LO12_NC = 312;
constexpr uint32_t R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21 = 539;
constexpr uint32_t R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC = 540;
constexpr uint32_t R_AARCH64_TLSLE_ADD_TPREL_HI12 = 549;
constexpr uint32_t R_AARCH64_TLSLE_ADD_TPREL_LO12_NC = 551;
constexpr uint32_t R_AARCH64_TLSGD_ADR_PAGE21 = 513;
constexpr uint32_t R_AARCH64_TLSGD_ADD_LO12_NC = 514;
constexpr uint32_t R_AARCH64_TLSLD_ADR_PAGE21 = 517;

}  // namespace

std::vector<Core::DataReference> analyzeRelocations(
    const std::vector<Core::RelocationEntry>& text_relocations,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& /*sections*/) noexcept {
    std::vector<Core::DataReference> result;

    const uint64_t region_end = region_addr + region_size;

    for (const auto& reloc : text_relocations) {
        // Filter to this region
        if (reloc.offset < region_addr || reloc.offset >= region_end)
            continue;

        Core::DataReference ref;
        ref.insn_offset = reloc.offset;
        ref.source = Core::DataRefSource::Relocation;
        ref.relocation = reloc;
        ref.target_symbol = reloc.symbol_name;

        bool skip = false;
        switch (reloc.type) {
            // x86-64 direct references — classify by target section
            case R_X86_64_PC32:
            case R_X86_64_32:
            case R_X86_64_32S:
            case R_X86_64_64:
                ref.is_pc_relative = (reloc.type == R_X86_64_PC32);
                // Target VA would need symbol value + addend; we use symbol
                // name for now and let the backend resolve. Classify if
                // section lookup is available through the symbol.
                ref.kind = Core::DataRefKind::Unknown;
                break;

            // GOT-relative references
            case R_X86_64_GOTPCREL:
            case R_X86_64_GOTPCRELX:
            case R_X86_64_REX_GOTPCRELX:
                ref.kind = Core::DataRefKind::GotLoad;
                ref.is_pc_relative = true;
                break;

            // TLS — Local Exec
            case R_X86_64_TPOFF32:
            case R_X86_64_TPOFF64:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalExec;
                break;

            // TLS — Initial Exec
            case R_X86_64_GOTTPOFF:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::InitialExec;
                ref.is_pc_relative = true;
                break;

            // TLS — General Dynamic
            case R_X86_64_TLSGD:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::GeneralDynamic;
                break;

            // TLS — Local Dynamic
            case R_X86_64_TLSLD:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalDynamic;
                break;

            // PLT32 — code call, skip
            case R_X86_64_PLT32:
                skip = true;
                break;

            // AArch64 — direct references
            case R_AARCH64_ABS64:
            case R_AARCH64_ADR_PREL_PG_HI21:
            case R_AARCH64_ADD_ABS_LO12_NC:
            case R_AARCH64_LDST64_ABS_LO12_NC:
                ref.kind = Core::DataRefKind::Unknown;
                break;

            // AArch64 — GOT references
            case R_AARCH64_ADR_GOT_PAGE:
            case R_AARCH64_LD64_GOT_LO12_NC:
                ref.kind = Core::DataRefKind::GotLoad;
                break;

            // AArch64 — TLS Initial Exec
            case R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21:
            case R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::InitialExec;
                break;

            // AArch64 — TLS Local Exec
            case R_AARCH64_TLSLE_ADD_TPREL_HI12:
            case R_AARCH64_TLSLE_ADD_TPREL_LO12_NC:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalExec;
                break;

            // AArch64 — TLS General Dynamic
            case R_AARCH64_TLSGD_ADR_PAGE21:
            case R_AARCH64_TLSGD_ADD_LO12_NC:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::GeneralDynamic;
                break;

            // AArch64 — TLS Local Dynamic
            case R_AARCH64_TLSLD_ADR_PAGE21:
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalDynamic;
                break;

            case R_X86_64_NONE:
                skip = true;
                break;

            default:
                ref.kind = Core::DataRefKind::Unknown;
                break;
        }

        if (!skip)
            result.push_back(std::move(ref));
    }

    return result;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
