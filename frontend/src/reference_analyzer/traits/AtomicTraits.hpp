#pragma once

#include <DataReference.hpp>
#include <capstone.hpp>

#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <cstring>
#include <string>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Unified result for atomic instruction classification.
struct AtomicInfo {
    Core::AtomicOp op = Core::AtomicOp::None;
    Core::AtomicOrdering ordering = Core::AtomicOrdering::None;
    Core::AtomicWidth width = Core::AtomicWidth::None;
};

/// Map operand byte size to AtomicWidth enum.
inline Core::AtomicWidth atomicWidthFromSize(uint8_t size) noexcept {
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
inline Core::AtomicWidth atomicWidthForCmpxchg(uint32_t insn_id) noexcept {
    if (insn_id == X86_INS_CMPXCHG16B)
        return Core::AtomicWidth::Atomic128;
    if (insn_id == X86_INS_CMPXCHG8B)
        return Core::AtomicWidth::Atomic64;
    return Core::AtomicWidth::None;  // use operand size
}

namespace detail {

/// True if reg is a 64-bit ARM64 X register (X0-X28, X29/FP, X30/LR).
inline bool isARM64XReg(unsigned reg) noexcept {
    return (reg >= ARM64_REG_X0 && reg <= ARM64_REG_X28) ||
           reg == ARM64_REG_FP || reg == ARM64_REG_LR;
}

/// Parse ARM64 mnemonic ordering suffix ('a' = acquire, 'l' = release,
/// 'al' = acqrel) and width suffix ('b' = byte, 'h' = half).
/// `base_len` is the length of the base mnemonic (e.g. 3 for "cas").
/// The mnemonic after base_len may contain [a][l][b|h].
inline void parseARM64Suffix(const std::string& mnemonic, size_t base_len,
                              Core::AtomicOrdering& ordering,
                              Core::AtomicWidth& width,
                              const Capstone::Instruction& insn) noexcept {
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
        // No width suffix -> check first register operand for W vs X
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

}  // namespace detail

struct X86AtomicTraits {
    /// Classify an x86 instruction as atomic based on LOCK prefix / XCHG.
    /// Only returns non-None when the instruction has a memory operand.
    static AtomicInfo classify(const Capstone::Instruction& insn,
                               bool has_mem_operand) noexcept {
        if (!has_mem_operand)
            return {};

        bool is_locked =
            (insn.x86_prefix[0] == X86_PREFIX_LOCK);
        bool is_xchg_mem = (insn.id == X86_INS_XCHG) && has_mem_operand;

        if (!is_locked && !is_xchg_mem)
            return {};

        AtomicInfo info;
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
                info.op = Core::AtomicOp::RMW;  // lock + unknown -> generic RMW
                break;
        }
        return info;
    }

    /// Determine if a memory operand is a write (destination).
    /// Exceptions: CMP, TEST, PUSH are read-only on their first operand.
    static bool isMemWrite(const Capstone::Instruction& insn,
                           size_t op_idx) noexcept {
        // For most x86 instructions, operand 0 is destination
        // Exceptions: CMP, TEST, PUSH are read-only on their first operand
        if (insn.mnemonic == "cmp" || insn.mnemonic == "test" ||
            insn.mnemonic == "push")
            return false;

        // For two-operand instructions, op 0 is typically the destination
        return op_idx == 0;
    }
};

struct ARM64AtomicTraits {
    /// Classify an ARM64 instruction as atomic based on mnemonic.
    static AtomicInfo classify(const Capstone::Instruction& insn) noexcept {
        const auto& mn = insn.mnemonic;
        AtomicInfo info;

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
                        detail::isARM64XReg(op.reg)) {
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
                        detail::isARM64XReg(op.reg)) {
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
                detail::parseARM64Suffix(mn, 5, info.ordering, info.width,
                                         insn);
                info.ordering = Core::AtomicOrdering::Acquire;
            } else {
                detail::parseARM64Suffix(mn, 4, info.ordering, info.width,
                                         insn);
                info.ordering = Core::AtomicOrdering::Relaxed;
            }
            return info;
        }

        // Exclusive store: stxr / stlxr (and b/h variants)
        if (mn.substr(0, 4) == "stxr" || mn.substr(0, 5) == "stlxr") {
            info.op = Core::AtomicOp::StoreExclusive;
            if (mn.substr(0, 5) == "stlxr") {
                detail::parseARM64Suffix(mn, 5, info.ordering, info.width,
                                         insn);
                info.ordering = Core::AtomicOrdering::Release;
            } else {
                detail::parseARM64Suffix(mn, 4, info.ordering, info.width,
                                         insn);
                info.ordering = Core::AtomicOrdering::Relaxed;
            }
            return info;
        }

        // CAS: cas[a][l][b|h]
        if (mn.substr(0, 3) == "cas" &&
            mn.find("p") == std::string::npos) {
            info.op = Core::AtomicOp::CompareSwap;
            detail::parseARM64Suffix(mn, 3, info.ordering, info.width, insn);
            return info;
        }

        // SWP: swp[a][l][b|h]
        if (mn.substr(0, 3) == "swp") {
            info.op = Core::AtomicOp::Swap;
            detail::parseARM64Suffix(mn, 3, info.ordering, info.width, insn);
            return info;
        }

        // LDADD: ldadd[a][l][b|h]
        if (mn.substr(0, 5) == "ldadd") {
            info.op = Core::AtomicOp::FetchAdd;
            detail::parseARM64Suffix(mn, 5, info.ordering, info.width, insn);
            return info;
        }

        // Other atomic LD ops: ldclr, ldset, ldeor, ldsmax, ldsmin, ldumax,
        // ldumin
        for (const char* base : {"ldclr", "ldset", "ldeor", "ldsmax",
                                  "ldsmin", "ldumax", "ldumin"}) {
            size_t blen = std::strlen(base);
            if (mn.size() >= blen && mn.substr(0, blen) == base) {
                info.op = Core::AtomicOp::RMW;
                detail::parseARM64Suffix(mn, blen, info.ordering, info.width,
                                         insn);
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

    /// Determine if an ARM64 instruction writes to memory.
    /// str/stp are plain stores; stlr/stxr/stlxr are atomic stores;
    /// cas/swp/ldadd etc. are RMW (both read+write).
    static bool isMemWrite(const Capstone::Instruction& insn,
                           const AtomicInfo& atomic) noexcept {
        const auto& mn = insn.mnemonic;

        // Plain stores and atomic stores by mnemonic prefix
        if (mn.substr(0, 3) == "str" || mn.substr(0, 3) == "stp" ||
            mn.substr(0, 4) == "stlr" || mn.substr(0, 4) == "stxr" ||
            mn.substr(0, 5) == "stlxr")
            return true;

        // Atomic RMW ops are also writes
        if (atomic.op == Core::AtomicOp::CompareSwap ||
            atomic.op == Core::AtomicOp::Swap ||
            atomic.op == Core::AtomicOp::FetchAdd ||
            atomic.op == Core::AtomicOp::RMW ||
            atomic.op == Core::AtomicOp::StoreExclusive ||
            atomic.op == Core::AtomicOp::StoreRelease)
            return true;

        return false;
    }
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer
