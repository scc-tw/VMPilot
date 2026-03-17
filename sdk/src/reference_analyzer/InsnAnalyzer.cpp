#include "InsnAnalyzer.hpp"
#include "SectionLookup.hpp"

#include <ARM64Traits.hpp>
#include <RegValueResolver.hpp>
#include <X86Traits.hpp>
#include <x86.hpp>

#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <cstring>
#include <unordered_map>

namespace VMPilot::SDK::ReferenceAnalyzer {

namespace {

// Stack register IDs (from capstone x86.h)
constexpr unsigned kX86_RSP = X86_REG_RSP;
constexpr unsigned kX86_RBP = X86_REG_RBP;
constexpr unsigned kX86_ESP = X86_REG_ESP;
constexpr unsigned kX86_EBP = X86_REG_EBP;
constexpr unsigned kX86_SP = X86_REG_SP;
constexpr unsigned kX86_BP = X86_REG_BP;

bool isStackReg(unsigned reg) {
    return reg == kX86_RSP || reg == kX86_RBP || reg == kX86_ESP ||
           reg == kX86_EBP || reg == kX86_SP || reg == kX86_BP;
}

bool isLEA(const Capstone::Instruction& insn) {
    return insn.mnemonic == "lea";
}

Core::DataRefKind classifyBySection(Core::SectionKind sk) {
    switch (sk) {
        case Core::SectionKind::Data:
        case Core::SectionKind::Bss:
            return Core::DataRefKind::GlobalVar;
        case Core::SectionKind::Rodata:
            return Core::DataRefKind::ReadOnlyData;
        case Core::SectionKind::Got:
            return Core::DataRefKind::GotLoad;
        case Core::SectionKind::Tls:
            return Core::DataRefKind::TlsVar;
        default:
            return Core::DataRefKind::Unknown;
    }
}

// --- x86 atomic classification ---

struct X86AtomicInfo {
    Core::AtomicOp op = Core::AtomicOp::None;
    Core::AtomicOrdering ordering = Core::AtomicOrdering::None;
};

/// Classify an x86 instruction as atomic based on LOCK prefix / XCHG.
/// Returns non-None only when the instruction has a memory operand.
X86AtomicInfo classifyX86Atomic(const Capstone::Instruction& insn,
                                 bool has_mem_operand) {
    if (!has_mem_operand)
        return {};

    bool is_locked = Capstone::X86::hasLockPrefix(insn);
    bool is_xchg_mem = (insn.id == X86_INS_XCHG) && has_mem_operand;

    if (!is_locked && !is_xchg_mem)
        return {};

    X86AtomicInfo info;
    info.ordering = Core::AtomicOrdering::AcqRel;  // x86 lock = full barrier

    switch (insn.id) {
        case X86_INS_CMPXCHG:
        case X86_INS_CMPXCHG8B:
        case X86_INS_CMPXCHG16B:
            info.op = Core::AtomicOp::CompareSwap;
            break;
        case X86_INS_XCHG:
            info.op = Core::AtomicOp::Swap;
            break;
        case X86_INS_XADD:
            info.op = Core::AtomicOp::FetchAdd;
            break;
        case X86_INS_ADD:
        case X86_INS_SUB:
        case X86_INS_AND:
        case X86_INS_OR:
        case X86_INS_XOR:
        case X86_INS_INC:
        case X86_INS_DEC:
        case X86_INS_NEG:
        case X86_INS_NOT:
        case X86_INS_BTS:
        case X86_INS_BTR:
        case X86_INS_BTC:
            info.op = Core::AtomicOp::RMW;
            break;
        default:
            info.op = Core::AtomicOp::RMW;  // lock + unknown → generic RMW
            break;
    }
    return info;
}

/// Determine AtomicWidth from operand byte size.
Core::AtomicWidth atomicWidthFromSize(uint8_t size) {
    switch (size) {
        case 1:
            return Core::AtomicWidth::Atomic8;
        case 2:
            return Core::AtomicWidth::Atomic16;
        case 4:
            return Core::AtomicWidth::Atomic32;
        case 8:
            return Core::AtomicWidth::Atomic64;
        case 16:
            return Core::AtomicWidth::Atomic128;
        default:
            return Core::AtomicWidth::None;
    }
}

/// Special-case width for CMPXCHG8B/16B.
Core::AtomicWidth atomicWidthForCmpxchg(uint32_t insn_id) {
    if (insn_id == X86_INS_CMPXCHG16B)
        return Core::AtomicWidth::Atomic128;
    if (insn_id == X86_INS_CMPXCHG8B)
        return Core::AtomicWidth::Atomic64;
    return Core::AtomicWidth::None;  // use operand size
}

/// True if reg is a 64-bit ARM64 X register (X0-X28, X29/FP, X30/LR).
bool isARM64XReg(unsigned reg) {
    return (reg >= ARM64_REG_X0 && reg <= ARM64_REG_X28) ||
           reg == ARM64_REG_FP || reg == ARM64_REG_LR;
}

// --- ARM64 atomic classification ---

struct ARM64AtomicInfo {
    Core::AtomicOp op = Core::AtomicOp::None;
    Core::AtomicOrdering ordering = Core::AtomicOrdering::None;
    Core::AtomicWidth width = Core::AtomicWidth::None;
};

/// Parse ARM64 mnemonic ordering suffix ('a' = acquire, 'l' = release,
/// 'al' = acqrel) and width suffix ('b' = byte, 'h' = half).
/// `base_len` is the length of the base mnemonic (e.g. 3 for "cas").
/// The mnemonic after base_len may contain [a][l][b|h].
void parseARM64Suffix(const std::string& mnemonic, size_t base_len,
                      Core::AtomicOrdering& ordering,
                      Core::AtomicWidth& width,
                      const Capstone::Instruction& insn) {
    ordering = Core::AtomicOrdering::Relaxed;
    // Default width from register size (W=32, X=64)
    width = Core::AtomicWidth::Atomic32;

    std::string suffix = mnemonic.substr(base_len);

    // Parse ordering: 'a' and/or 'l'
    bool has_a = false, has_l = false;
    size_t pos = 0;
    if (pos < suffix.size() && suffix[pos] == 'a') {
        has_a = true;
        ++pos;
    }
    if (pos < suffix.size() && suffix[pos] == 'l') {
        has_l = true;
        ++pos;
    }

    if (has_a && has_l)
        ordering = Core::AtomicOrdering::AcqRel;
    else if (has_a)
        ordering = Core::AtomicOrdering::Acquire;
    else if (has_l)
        ordering = Core::AtomicOrdering::Release;

    // Parse width suffix
    if (pos < suffix.size()) {
        if (suffix[pos] == 'b')
            width = Core::AtomicWidth::Atomic8;
        else if (suffix[pos] == 'h')
            width = Core::AtomicWidth::Atomic16;
    } else {
        // No width suffix → check first register operand for W vs X
        // X registers = 64-bit, W registers = 32-bit
        for (const auto& op : insn.operands) {
            if (op.type == Capstone::OpType::REG) {
                // ARM64 X registers have IDs >= ARM64_REG_X0
                // W registers have IDs >= ARM64_REG_W0
                if (isARM64XReg(op.reg)) {
                    width = Core::AtomicWidth::Atomic64;
                }
                break;
            }
        }
    }
}

/// Classify an ARM64 instruction as atomic based on mnemonic.
ARM64AtomicInfo classifyARM64Atomic(const Capstone::Instruction& insn) {
    const auto& mn = insn.mnemonic;
    ARM64AtomicInfo info;

    // Load-acquire
    if (mn.substr(0, 4) == "ldar") {
        info.op = Core::AtomicOp::LoadAcquire;
        info.ordering = Core::AtomicOrdering::Acquire;
        // ldar / ldarb / ldarh
        if (mn.size() > 4 && mn[4] == 'b')
            info.width = Core::AtomicWidth::Atomic8;
        else if (mn.size() > 4 && mn[4] == 'h')
            info.width = Core::AtomicWidth::Atomic16;
        else {
            info.width = Core::AtomicWidth::Atomic32;
            for (const auto& op : insn.operands) {
                if (op.type == Capstone::OpType::REG &&
                    isARM64XReg(op.reg)) {
                    info.width = Core::AtomicWidth::Atomic64;
                    break;
                }
            }
        }
        return info;
    }

    // Store-release
    if (mn.substr(0, 4) == "stlr") {
        info.op = Core::AtomicOp::StoreRelease;
        info.ordering = Core::AtomicOrdering::Release;
        if (mn.size() > 4 && mn[4] == 'b')
            info.width = Core::AtomicWidth::Atomic8;
        else if (mn.size() > 4 && mn[4] == 'h')
            info.width = Core::AtomicWidth::Atomic16;
        else {
            info.width = Core::AtomicWidth::Atomic32;
            for (const auto& op : insn.operands) {
                if (op.type == Capstone::OpType::REG &&
                    isARM64XReg(op.reg)) {
                    info.width = Core::AtomicWidth::Atomic64;
                    break;
                }
            }
        }
        return info;
    }

    // Exclusive load: ldxr / ldaxr (and b/h variants)
    if (mn.substr(0, 4) == "ldxr" || mn.substr(0, 5) == "ldaxr") {
        info.op = Core::AtomicOp::LoadExclusive;
        if (mn.substr(0, 5) == "ldaxr") {
            parseARM64Suffix(mn, 5, info.ordering, info.width, insn);
            info.ordering = Core::AtomicOrdering::Acquire;
        } else {
            parseARM64Suffix(mn, 4, info.ordering, info.width, insn);
            info.ordering = Core::AtomicOrdering::Relaxed;
        }
        return info;
    }

    // Exclusive store: stxr / stlxr (and b/h variants)
    if (mn.substr(0, 4) == "stxr" || mn.substr(0, 5) == "stlxr") {
        info.op = Core::AtomicOp::StoreExclusive;
        if (mn.substr(0, 5) == "stlxr") {
            parseARM64Suffix(mn, 5, info.ordering, info.width, insn);
            info.ordering = Core::AtomicOrdering::Release;
        } else {
            parseARM64Suffix(mn, 4, info.ordering, info.width, insn);
            info.ordering = Core::AtomicOrdering::Relaxed;
        }
        return info;
    }

    // CAS: cas[a][l][b|h]
    if (mn.substr(0, 3) == "cas" && mn.find("p") == std::string::npos) {
        info.op = Core::AtomicOp::CompareSwap;
        parseARM64Suffix(mn, 3, info.ordering, info.width, insn);
        return info;
    }

    // SWP: swp[a][l][b|h]
    if (mn.substr(0, 3) == "swp") {
        info.op = Core::AtomicOp::Swap;
        parseARM64Suffix(mn, 3, info.ordering, info.width, insn);
        return info;
    }

    // LDADD: ldadd[a][l][b|h]
    if (mn.substr(0, 5) == "ldadd") {
        info.op = Core::AtomicOp::FetchAdd;
        parseARM64Suffix(mn, 5, info.ordering, info.width, insn);
        return info;
    }

    // Other atomic LD ops: ldclr, ldset, ldeor, ldsmax, ldsmin, ldumax, ldumin
    for (const char* base : {"ldclr", "ldset", "ldeor", "ldsmax", "ldsmin",
                              "ldumax", "ldumin"}) {
        size_t blen = std::strlen(base);
        if (mn.size() >= blen && mn.substr(0, blen) == base) {
            info.op = Core::AtomicOp::RMW;
            parseARM64Suffix(mn, blen, info.ordering, info.width, insn);
            return info;
        }
    }

    // Fences: dmb, dsb, isb
    if (mn == "dmb" || mn == "dsb" || mn == "isb") {
        info.op = Core::AtomicOp::Fence;
        info.ordering = Core::AtomicOrdering::AcqRel;
        info.width = Core::AtomicWidth::None;
        return info;
    }

    return info;  // Not atomic
}

/// Determine if a memory operand is a write (destination)
bool isMemWrite(const Capstone::Instruction& insn, size_t op_idx) {
    // For most x86 instructions, operand 0 is destination
    // Exceptions: CMP, TEST, PUSH are read-only on their first operand
    if (insn.mnemonic == "cmp" || insn.mnemonic == "test" ||
        insn.mnemonic == "push")
        return false;

    // For two-operand instructions, op 0 is typically the destination
    return op_idx == 0;
}

/// Build symbol address lookup
using AddrToName = std::unordered_map<uint64_t, std::string>;

AddrToName buildSymbolLookup(
    const Segmentator::NativeSymbolTable& symbols) {
    AddrToName result;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.address != 0)
            result[sym.address] = sym.name;
    }
    return result;
}

void analyzeX86Insns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const AddrToName& sym_lookup,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];

        // Only consider instructions within our region
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        for (size_t op_idx = 0; op_idx < insn.operands.size(); ++op_idx) {
            const auto& op = insn.operands[op_idx];
            if (op.type != Capstone::OpType::MEM)
                continue;

            // Skip LEA — just address computation, not a memory access
            if (isLEA(insn))
                continue;

            // Skip stack-relative
            if (isStackReg(op.mem.base))
                continue;

            uint64_t target_va = 0;
            bool resolved = false;

            // RIP-relative: target = insn.address + insn.size + disp
            if (op.mem.isRipRelative()) {
                target_va = insn.address + insn.size + op.mem.disp;
                resolved = true;
            }
            // Register-indirect: try to resolve base register
            else if (op.mem.base != 0 && op.mem.index == 0) {
                size_t from = idx > 0 ? idx - 1 : 0;
                uint64_t base_val =
                    Segmentator::resolveRegValue<Segmentator::X86ArchTraits>(
                        op.mem.base, from, insns, 0);
                if (base_val != 0) {
                    target_va = base_val + op.mem.disp;
                    resolved = true;
                }
            }

            if (!resolved)
                continue;

            // Classify by section
            auto sk = sections.classify(target_va);
            if (sk == Core::SectionKind::Text ||
                sk == Core::SectionKind::Plt ||
                sk == Core::SectionKind::Unknown)
                continue;

            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = target_va;
            ref.kind = classifyBySection(sk);
            ref.source = Core::DataRefSource::InsnAnalysis;
            ref.access_size = op.size;
            ref.is_write = isMemWrite(insn, op_idx);
            ref.is_pc_relative = op.mem.isRipRelative();

            // Atomic detection for memory operands
            auto atomic = classifyX86Atomic(insn, true);
            if (atomic.op != Core::AtomicOp::None) {
                ref.atomic_op = atomic.op;
                ref.atomic_ordering = atomic.ordering;
                // Special-case CMPXCHG8B/16B width
                auto special = atomicWidthForCmpxchg(insn.id);
                ref.atomic_width = (special != Core::AtomicWidth::None)
                                       ? special
                                       : atomicWidthFromSize(op.size);
            }

            // Try symbol lookup
            auto sym_it = sym_lookup.find(target_va);
            if (sym_it != sym_lookup.end())
                ref.target_symbol = sym_it->second;

            refs.push_back(std::move(ref));
        }

        // Fence instructions (no memory operand)
        if (insn.id == X86_INS_MFENCE || insn.id == X86_INS_SFENCE ||
            insn.id == X86_INS_LFENCE) {
            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = 0;
            ref.kind = Core::DataRefKind::Unknown;
            ref.source = Core::DataRefSource::InsnAnalysis;
            ref.atomic_op = Core::AtomicOp::Fence;
            ref.atomic_ordering = Core::AtomicOrdering::AcqRel;
            ref.atomic_width = Core::AtomicWidth::None;
            refs.push_back(std::move(ref));
        }
    }
}

void analyzeARM64Insns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const AddrToName& sym_lookup,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];

        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        // Check for ARM64 atomic classification (including fences)
        auto arm64_atomic = classifyARM64Atomic(insn);

        // Fence instructions (no memory operand needed)
        if (arm64_atomic.op == Core::AtomicOp::Fence) {
            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = 0;
            ref.kind = Core::DataRefKind::Unknown;
            ref.source = Core::DataRefSource::InsnAnalysis;
            ref.atomic_op = arm64_atomic.op;
            ref.atomic_ordering = arm64_atomic.ordering;
            ref.atomic_width = Core::AtomicWidth::None;
            refs.push_back(std::move(ref));
            continue;
        }

        // Skip instructions without MEM operand
        bool has_mem = false;
        for (const auto& op : insn.operands) {
            if (op.type == Capstone::OpType::MEM) {
                has_mem = true;
                break;
            }
        }
        if (!has_mem)
            continue;

        // Skip stack-relative (SP = X31)
        for (size_t op_idx = 0; op_idx < insn.operands.size(); ++op_idx) {
            const auto& op = insn.operands[op_idx];
            if (op.type != Capstone::OpType::MEM)
                continue;

            // Skip stack-relative: SP and FP (frame pointer)
            if (op.mem.base == ARM64_REG_SP ||
                op.mem.base == ARM64_REG_FP)
                continue;

            // Try to resolve base register via ADRP+ADD pattern
            uint64_t target_va = 0;
            bool resolved = false;

            if (op.mem.base != 0 && idx > 0) {
                size_t from = idx > 0 ? idx - 1 : 0;
                uint64_t base_val =
                    Segmentator::resolveRegValue<Segmentator::ARM64ArchTraits>(
                        op.mem.base, from, insns, 0);
                if (base_val != 0) {
                    target_va = base_val + op.mem.disp;
                    resolved = true;
                }
            }

            if (!resolved)
                continue;

            auto sk = sections.classify(target_va);
            if (sk == Core::SectionKind::Text ||
                sk == Core::SectionKind::Plt ||
                sk == Core::SectionKind::Unknown)
                continue;

            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = target_va;
            ref.kind = classifyBySection(sk);
            ref.source = Core::DataRefSource::InsnAnalysis;
            ref.access_size = op.size;
            // ARM64 write detection: str/stp are plain stores;
            // stlr/stxr/stlxr are atomic stores;
            // cas/swp/ldadd/ldclr/ldset/ldeor etc. are RMW (both read+write)
            ref.is_write =
                (insn.mnemonic.substr(0, 3) == "str" ||
                 insn.mnemonic.substr(0, 3) == "stp" ||
                 insn.mnemonic.substr(0, 4) == "stlr" ||
                 insn.mnemonic.substr(0, 4) == "stxr" ||
                 insn.mnemonic.substr(0, 5) == "stlxr");

            // Apply atomic info if classified
            if (arm64_atomic.op != Core::AtomicOp::None) {
                ref.atomic_op = arm64_atomic.op;
                ref.atomic_ordering = arm64_atomic.ordering;
                ref.atomic_width = arm64_atomic.width;
                // Atomic RMW ops (CAS/SWP/LDADD/LD* ops) are writes too
                if (arm64_atomic.op == Core::AtomicOp::CompareSwap ||
                    arm64_atomic.op == Core::AtomicOp::Swap ||
                    arm64_atomic.op == Core::AtomicOp::FetchAdd ||
                    arm64_atomic.op == Core::AtomicOp::RMW ||
                    arm64_atomic.op == Core::AtomicOp::StoreExclusive ||
                    arm64_atomic.op == Core::AtomicOp::StoreRelease) {
                    ref.is_write = true;
                }
            }

            auto sym_it = sym_lookup.find(target_va);
            if (sym_it != sym_lookup.end())
                ref.target_symbol = sym_it->second;

            refs.push_back(std::move(ref));
        }
    }
}

}  // namespace

std::vector<Core::DataReference> analyzeInstructions(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode /*mode*/) noexcept {
    std::vector<Core::DataReference> refs;

    auto sym_lookup = buildSymbolLookup(symbols);

    using FileArch = VMPilot::Common::FileArch;
    if (arch == FileArch::X86) {
        analyzeX86Insns(insns, region_addr, region_size, sections, sym_lookup,
                        refs);
    } else if (arch == FileArch::ARM64) {
        analyzeARM64Insns(insns, region_addr, region_size, sections, sym_lookup,
                          refs);
    }

    return refs;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
