#pragma once

#include <DataReference.hpp>
#include <cstdint>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Result of classifying a relocation type.
struct RelocationClassification {
    Core::DataRefKind kind = Core::DataRefKind::Unknown;
    Core::TlsModel tls_model = Core::TlsModel::None;
    bool is_pc_relative = false;
    bool skip = false;  // true for PLT32, NONE -- not a data reference
};

struct X86RelocTraits {
    static RelocationClassification classify(uint32_t reloc_type) noexcept {
        // ELF x86-64 relocation types (from elf.h)
        constexpr uint32_t R_X86_64_NONE = 0;
        constexpr uint32_t R_X86_64_64 = 1;
        constexpr uint32_t R_X86_64_PC32 = 2;
        constexpr uint32_t R_X86_64_PLT32 = 4;
        constexpr uint32_t R_X86_64_GOTPCREL = 9;
        constexpr uint32_t R_X86_64_32 = 10;
        constexpr uint32_t R_X86_64_32S = 11;
        constexpr uint32_t R_X86_64_TPOFF64 = 18;
        constexpr uint32_t R_X86_64_TLSGD = 19;
        constexpr uint32_t R_X86_64_TLSLD = 20;
        constexpr uint32_t R_X86_64_GOTTPOFF = 22;
        constexpr uint32_t R_X86_64_TPOFF32 = 23;
        constexpr uint32_t R_X86_64_GOTPCRELX = 41;
        constexpr uint32_t R_X86_64_REX_GOTPCRELX = 42;

        switch (reloc_type) {
            // Direct references
            case R_X86_64_PC32:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        /*is_pc_relative=*/true, /*skip=*/false};

            case R_X86_64_32:
            case R_X86_64_32S:
            case R_X86_64_64:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        false, false};

            // GOT-relative references
            case R_X86_64_GOTPCREL:
            case R_X86_64_GOTPCRELX:
            case R_X86_64_REX_GOTPCRELX:
                return {Core::DataRefKind::GotLoad, Core::TlsModel::None,
                        /*is_pc_relative=*/true, false};

            // TLS -- Local Exec
            case R_X86_64_TPOFF32:
            case R_X86_64_TPOFF64:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::LocalExec,
                        false, false};

            // TLS -- Initial Exec
            case R_X86_64_GOTTPOFF:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::InitialExec,
                        /*is_pc_relative=*/true, false};

            // TLS -- General Dynamic
            case R_X86_64_TLSGD:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::GeneralDynamic,
                        false, false};

            // TLS -- Local Dynamic
            case R_X86_64_TLSLD:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::LocalDynamic,
                        false, false};

            // PLT32 -- code call, skip
            case R_X86_64_PLT32:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        false, /*skip=*/true};

            // NONE -- skip
            case R_X86_64_NONE:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        false, /*skip=*/true};

            default:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        false, false};
        }
    }
};

struct ARM64RelocTraits {
    static RelocationClassification classify(uint32_t reloc_type) noexcept {
        // AArch64 relocation types
        constexpr uint32_t R_AARCH64_ABS64 = 257;
        constexpr uint32_t R_AARCH64_ADR_PREL_PG_HI21 = 275;
        constexpr uint32_t R_AARCH64_ADD_ABS_LO12_NC = 277;
        constexpr uint32_t R_AARCH64_LDST64_ABS_LO12_NC = 286;
        constexpr uint32_t R_AARCH64_ADR_GOT_PAGE = 311;
        constexpr uint32_t R_AARCH64_LD64_GOT_LO12_NC = 312;
        constexpr uint32_t R_AARCH64_TLSGD_ADR_PAGE21 = 513;
        constexpr uint32_t R_AARCH64_TLSGD_ADD_LO12_NC = 514;
        constexpr uint32_t R_AARCH64_TLSLD_ADR_PAGE21 = 517;
        constexpr uint32_t R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21 = 539;
        constexpr uint32_t R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC = 540;
        constexpr uint32_t R_AARCH64_TLSLE_ADD_TPREL_HI12 = 549;
        constexpr uint32_t R_AARCH64_TLSLE_ADD_TPREL_LO12_NC = 551;

        switch (reloc_type) {
            // Direct references
            case R_AARCH64_ABS64:
            case R_AARCH64_ADR_PREL_PG_HI21:
            case R_AARCH64_ADD_ABS_LO12_NC:
            case R_AARCH64_LDST64_ABS_LO12_NC:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        false, false};

            // GOT references
            case R_AARCH64_ADR_GOT_PAGE:
            case R_AARCH64_LD64_GOT_LO12_NC:
                return {Core::DataRefKind::GotLoad, Core::TlsModel::None,
                        false, false};

            // TLS -- Initial Exec
            case R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21:
            case R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::InitialExec,
                        false, false};

            // TLS -- Local Exec
            case R_AARCH64_TLSLE_ADD_TPREL_HI12:
            case R_AARCH64_TLSLE_ADD_TPREL_LO12_NC:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::LocalExec,
                        false, false};

            // TLS -- General Dynamic
            case R_AARCH64_TLSGD_ADR_PAGE21:
            case R_AARCH64_TLSGD_ADD_LO12_NC:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::GeneralDynamic,
                        false, false};

            // TLS -- Local Dynamic
            case R_AARCH64_TLSLD_ADR_PAGE21:
                return {Core::DataRefKind::TlsVar, Core::TlsModel::LocalDynamic,
                        false, false};

            default:
                return {Core::DataRefKind::Unknown, Core::TlsModel::None,
                        false, false};
        }
    }
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer
