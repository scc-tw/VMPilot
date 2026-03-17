#ifndef __SDK_REG_VALUE_RESOLVER_HPP__
#define __SDK_REG_VALUE_RESOLVER_HPP__
#pragma once

#include <capstone.hpp>

#include <cstdint>
#include <variant>
#include <vector>

namespace VMPilot::SDK::Segmentator {

// --- Write classification for backward constant propagation ---

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

struct Unresolvable {};

using WriteClassification =
    std::variant<ResolvedConstant, RegisterForward, ArithmeticAdjust,
                 Unresolvable>;

// --- Helpers ---

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

inline constexpr int kMaxResolveDepth = 5;

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
                         size_t min_idx, int depth = 0) {
    if (depth >= kMaxResolveDepth || from_idx >= insns.size())
        return 0;

    for (size_t i = from_idx + 1; i-- > min_idx;) {
        const auto& insn = insns[i];

        // CALL/RET clobber caller-saved registers
        if (insn.isCall() || insn.isRet()) {
            if (!Traits::isCalleeSaved(reg))
                return 0;
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
                                                   min_idx, depth + 1);
                },
                [&](ArithmeticAdjust aa) -> uint64_t {
                    size_t prev = i > 0 ? i - 1 : 0;
                    uint64_t base = resolveRegValue<Traits>(
                        aa.src_reg, prev, insns, min_idx, depth + 1);
                    if (base != 0)
                        return static_cast<uint64_t>(
                            static_cast<int64_t>(base) + aa.offset);
                    return uint64_t{0};
                },
                [](Unresolvable) -> uint64_t { return 0; },
            },
            c);
    }

    return 0;
}

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_REG_VALUE_RESOLVER_HPP__
