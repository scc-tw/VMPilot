#pragma once

#include "SymExpr.hpp"

#include <RegValueResolver.hpp>  // For WriteClassification types, overloaded, Traits concept

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

inline constexpr int kMaxSymResolveDepth = 8;

/// Symbol lookup: address -> optional symbol name
using SymbolLookup = std::function<std::optional<std::string>(uint64_t addr)>;

/// Backward symbolic resolution.
/// Same scan algorithm as resolveRegValue, same Traits, but returns
/// SymExpr instead of uint64_t.
///
/// NOTE: Single-threaded. Instruction span must not be modified during
/// resolution.
template <typename Traits>
SymExpr resolveSymbolic(
    unsigned reg,
    size_t from_idx,
    const std::vector<Capstone::Instruction>& insns,
    size_t min_idx,
    const SymbolLookup& sym_lookup = nullptr,
    int depth = 0)
{
    using namespace Segmentator;  // For WriteClassification types

    if (depth >= kMaxSymResolveDepth || from_idx >= insns.size())
        return SymExpr::unknown();

    for (size_t i = from_idx + 1; i-- > min_idx;) {
        const auto& insn = insns[i];

        // CALL: check for known function, or clobber caller-saved
        if (insn.isCall()) {
            if (!Traits::isCalleeSaved(reg)) {
                uint64_t target = insn.getDirectTarget();
                if (target == 0)
                    target = insn.getRipRelativeTarget();
                if (target != 0 && sym_lookup) {
                    auto name = sym_lookup(target);
                    if (name)
                        return SymExpr::opaque(
                            SymExpr::OpaqueKind::CallResult, 0,
                            std::move(*name));
                }
                return SymExpr::unknown();
            }
            continue;
        }
        if (insn.isRet()) {
            if (!Traits::isCalleeSaved(reg))
                return SymExpr::unknown();
            continue;
        }

        if (!Traits::writesToReg(insn, reg))
            continue;

        auto c = Traits::classifyWrite(insn, reg);
        return std::visit(
            overloaded{
                [](ResolvedConstant rc) -> SymExpr {
                    return SymExpr::constant(rc.value);
                },
                [&](RegisterForward rf) -> SymExpr {
                    return resolveSymbolic<Traits>(
                        rf.src_reg, i > 0 ? i - 1 : 0, insns, min_idx,
                        sym_lookup, depth + 1);
                },
                [&](ArithmeticAdjust aa) -> SymExpr {
                    auto base = resolveSymbolic<Traits>(
                        aa.src_reg, i > 0 ? i - 1 : 0, insns, min_idx,
                        sym_lookup, depth + 1);
                    return SymExpr::binop(SymExpr::BinOp::Add,
                        std::move(base),
                        SymExpr::constant(
                            static_cast<uint64_t>(aa.offset)));
                },
                [&](MemoryLoad ml) -> SymExpr {
                    SymExpr addr_base;
                    if (ml.base_reg != 0) {
                        addr_base = resolveSymbolic<Traits>(
                            ml.base_reg, i > 0 ? i - 1 : 0, insns,
                            min_idx, sym_lookup, depth + 1);
                    } else {
                        // Absolute address (e.g. RIP-relative resolved)
                        addr_base = SymExpr::constant(0);
                    }
                    auto addr = SymExpr::binop(SymExpr::BinOp::Add,
                        std::move(addr_base),
                        SymExpr::constant(
                            static_cast<uint64_t>(ml.offset)));
                    return SymExpr::load(std::move(addr));
                },
                [](OpaqueSource os) -> SymExpr {
                    switch (os.kind) {
                        case OpaqueSource::Kind::SystemReg:
                            return SymExpr::opaque(
                                SymExpr::OpaqueKind::SystemReg, os.id);
                        case OpaqueSource::Kind::SegmentBase:
                            return SymExpr::opaque(
                                SymExpr::OpaqueKind::SegmentBase, os.id);
                        case OpaqueSource::Kind::CallResult:
                            return SymExpr::opaque(
                                SymExpr::OpaqueKind::CallResult, 0);
                    }
                    return SymExpr::unknown();
                },
                [](Unresolvable) -> SymExpr {
                    return SymExpr::unknown();
                },
            },
            c);
    }
    return SymExpr::unknown();
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
