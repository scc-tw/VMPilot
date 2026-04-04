#ifndef __SDK_REG_VALUE_RESOLVER_HPP__
#define __SDK_REG_VALUE_RESOLVER_HPP__
#pragma once

#include <capstone.hpp>

#include <cstdint>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace VMPilot::SDK::Segmentator {

// --- Write classification for backward constant propagation ---
//
// These types classify the effect of an instruction on a single register.
// This is a CLOSED set: every scalar integer instruction falls into one of
// these categories regardless of ISA.  Growth happens in the per-arch
// Traits that map native instructions to these six effects.

struct ResolvedConstant {
    uint64_t value;
};

struct RegisterForward {
    unsigned src_reg;
};

struct ArithmeticAdjust {
    unsigned src_reg;
    int64_t offset;
};

/// Memory load: register value = memory[base + offset].
/// When base_reg == 0, offset is an absolute virtual address
/// (e.g. resolved from a RIP-relative or absolute addressing mode).
struct MemoryLoad {
    unsigned base_reg;
    int64_t offset;
    uint8_t load_size;     // 1, 2, 4, 8 bytes
};

/// Opaque value source that cannot be traced further through the
/// instruction stream.  Carries enough metadata for downstream
/// symbolic analysis (Phase 3+) to classify the reference.
struct OpaqueSource {
    enum class Kind : uint8_t {
        SystemReg,     // ARM64 MRS (e.g. TPIDR_EL0)
        SegmentBase,   // x86 FS / GS segment base
        CallResult,    // Return value of a function call
    };
    Kind kind;
    unsigned id = 0;           // Segment register or system register ID
    uint64_t call_target = 0;  // For CallResult: address of the callee
};

struct Unresolvable {};

using WriteClassification =
    std::variant<ResolvedConstant, RegisterForward, ArithmeticAdjust,
                 MemoryLoad, OpaqueSource, Unresolvable>;

// --- Helpers ---

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

inline constexpr int kMaxResolveDepth = 5;

/// Optional callback: given a CALL instruction and the register being
/// resolved, return the value the call sets the register to.
/// Used for patterns like x86-32 PIC thunks (__x86.get_pc_thunk.bx).
using CallValueResolver = std::function<std::optional<uint64_t>(
    const Capstone::Instruction& call_insn, unsigned reg)>;

// ---------------------------------------------------------------------------
// Generic backward constant propagation resolver.
//
// Scans instructions backwards from `from_idx` to `min_idx`, following data
// flow through register writes classified by the arch-specific Traits.
//
// Traits must provide:
//   static bool isCalleeSaved(unsigned reg);
//   static bool writesToReg(const Capstone::Instruction&, unsigned reg);
//   static WriteClassification classifyWrite(const Capstone::Instruction&,
//                                            unsigned reg);
// ---------------------------------------------------------------------------
template <typename Traits>
uint64_t resolveRegValue(unsigned reg, size_t from_idx,
                         const std::vector<Capstone::Instruction>& insns,
                         size_t min_idx, int depth = 0,
                         const CallValueResolver& call_resolver = nullptr) {
    if (depth >= kMaxResolveDepth || from_idx >= insns.size())
        return 0;

    for (size_t i = from_idx + 1; i-- > min_idx;) {
        const auto& insn = insns[i];

        // CALL/RET clobber caller-saved registers
        if (insn.isCall() || insn.isRet()) {
            if (!Traits::isCalleeSaved(reg))
                return 0;
            // Check if this call is known to set our register
            // (e.g., __x86.get_pc_thunk.bx sets ebx to return addr)
            if (insn.isCall() && call_resolver) {
                auto val = call_resolver(insn, reg);
                if (val)
                    return *val;
            }
            continue;
        }

        // Skip instructions that don't write to our register
        if (!Traits::writesToReg(insn, reg))
            continue;

        auto c = Traits::classifyWrite(insn, reg);
        return std::visit(
            overloaded{
                [](ResolvedConstant rc) -> uint64_t { return rc.value; },
                [&](RegisterForward rf) -> uint64_t {
                    return resolveRegValue<Traits>(rf.src_reg,
                                                   i > 0 ? i - 1 : 0, insns,
                                                   min_idx, depth + 1,
                                                   call_resolver);
                },
                [&](ArithmeticAdjust aa) -> uint64_t {
                    size_t prev = i > 0 ? i - 1 : 0;
                    uint64_t base = resolveRegValue<Traits>(
                        aa.src_reg, prev, insns, min_idx, depth + 1,
                        call_resolver);
                    if (base != 0)
                        return static_cast<uint64_t>(
                            static_cast<int64_t>(base) + aa.offset);
                    return uint64_t{0};
                },
                [](MemoryLoad) -> uint64_t {
                    // Cannot resolve without a memory oracle.
                    // Future: MemoryModel integration (Phase 2+).
                    return 0;
                },
                [](OpaqueSource) -> uint64_t {
                    // Opaque sources (system regs, segment bases,
                    // call results) are not derivable from the
                    // instruction stream alone.
                    return 0;
                },
                [](Unresolvable) -> uint64_t { return 0; },
            },
            c);
    }

    return 0;
}

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_REG_VALUE_RESOLVER_HPP__
