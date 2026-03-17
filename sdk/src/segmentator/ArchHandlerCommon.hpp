#ifndef __SDK_ARCH_HANDLER_COMMON_HPP__
#define __SDK_ARCH_HANDLER_COMMON_HPP__
#pragma once

#include <CompilationContext.hpp>
#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>
#include <utilities.hpp>

#include <capstone.hpp>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::Segmentator {

using AddrToSymbol = std::unordered_map<uint64_t, std::string>;

AddrToSymbol buildAddrLookup(const NativeSymbolTable& symbols);

// ---------------------------------------------------------------------------
// ArchCallbackTraits concept (duck-typed):
//
// Traits must provide:
//   static std::optional<std::string> resolveCall(
//       const Capstone::Instruction&, const AddrToSymbol&);
//   static std::optional<uint64_t> extractStringArg(
//       size_t call_idx, const std::vector<Capstone::Instruction>&,
//       const AddrToSymbol&);
// ---------------------------------------------------------------------------

/// Shared implementation state for X86Handler and ARM64Handler.
struct ArchHandlerImplBase {
    Capstone::Capstone cs;
    AddrToSymbol addr_lookup;
    uint64_t base_addr = static_cast<uint64_t>(-1);
    std::vector<Capstone::Instruction> instructions;

    ArchHandlerImplBase(Capstone::Capstone&& cs, AddrToSymbol&& lookup)
        : cs(std::move(cs)), addr_lookup(std::move(lookup)) {}
};

/// Find the function symbol that encloses the given address.
/// Returns the mangled symbol name, or nullopt if not found.
std::optional<std::string> findEnclosingSymbol(
    uint64_t addr, const NativeSymbolTable& symbols);

/// Resolve the __FUNCTION__ string argument at the given call site.
template <typename CallbackTraits>
std::string resolveStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions,
    const CompilationContext& ctx,
    const AddrToSymbol& lookup) {
    if (ctx.rodata_sections.empty())
        return {};

    auto va = CallbackTraits::extractStringArg(call_idx, instructions, lookup);
    if (!va)
        return {};

    for (const auto& section : ctx.rodata_sections) {
        if (section.contains(*va))
            return section.readCString(*va);
    }
    return {};
}

/// Extract protected regions from disassembled instructions using the
/// given arch-specific callback traits.
template <typename CallbackTraits>
std::vector<NativeFunctionBase> extractNativeFunctions(
    ArchHandlerImplBase& impl, const CompilationContext& ctx) {
    std::vector<NativeFunctionBase> result;

    const auto& begin_sigs = VMPilot::Common::BEGIN_VMPILOT_SIGNATURES;
    const auto& end_sigs = VMPilot::Common::END_VMPILOT_SIGNATURES;
    const auto& instructions = impl.instructions;
    const auto& lookup = impl.addr_lookup;

    auto matchesAny = [](const std::string& name,
                         const std::vector<std::string>& sigs) {
        for (const auto& sig : sigs)
            if (name == sig)
                return true;
        return false;
    };

    // Scan for pairs of VMPilot_Begin / VMPilot_End calls
    struct Region {
        size_t begin_idx;
        size_t end_idx;
    };
    std::vector<Region> regions;
    size_t pending_begin = static_cast<size_t>(-1);

    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto& insn = instructions[i];
        // VMPilot_Begin is always a call.
        // VMPilot_End may be a jmp (tail-call optimization under -O2/-O3).
        if (!insn.isCall() && !insn.isJump())
            continue;

        auto sym = CallbackTraits::resolveCall(insn, lookup);
        if (!sym)
            continue;

        if (insn.isCall() && matchesAny(*sym, begin_sigs)) {
            if (pending_begin != static_cast<size_t>(-1)) {
                spdlog::warn(
                    "Nested VMPilot_Begin at 0x{:x}, previous begin at 0x{:x}",
                    insn.address, instructions[pending_begin].address);
            }
            pending_begin = i;
        } else if (matchesAny(*sym, end_sigs)) {
            if (pending_begin == static_cast<size_t>(-1)) {
                spdlog::warn("VMPilot_End at 0x{:x} without matching Begin",
                             insn.address);
                continue;
            }
            regions.push_back({pending_begin, i});
            pending_begin = static_cast<size_t>(-1);
        }
    }

    if (pending_begin != static_cast<size_t>(-1)) {
        spdlog::warn("Unmatched VMPilot_Begin at 0x{:x}",
                     instructions[pending_begin].address);
    }

    // Extract each region as a NativeFunctionBase
    for (const auto& region : regions) {
        const auto& begin_insn = instructions[region.begin_idx];
        const auto& end_insn = instructions[region.end_idx];

        uint64_t start_addr = begin_insn.address;
        uint64_t end_addr = end_insn.address + end_insn.size;
        uint64_t size = end_addr - start_addr;

        std::vector<uint8_t> code;
        code.reserve(size);
        for (size_t i = region.begin_idx; i <= region.end_idx; ++i) {
            const auto& insn = instructions[i];
            code.insert(code.end(), insn.bytes.begin(), insn.bytes.end());
        }

        // Try to extract __FUNCTION__ name from Begin and End call sites
        std::string begin_name = resolveStringArg<CallbackTraits>(
            region.begin_idx, instructions, ctx, lookup);
        std::string end_name = resolveStringArg<CallbackTraits>(
            region.end_idx, instructions, ctx, lookup);

        if (!begin_name.empty() && !end_name.empty() &&
            begin_name != end_name) {
            spdlog::warn(
                "__FUNCTION__ mismatch: begin='{}' end='{}' at 0x{:x}",
                begin_name, end_name, start_addr);
        }

        std::string name;
        if (!begin_name.empty()) {
            name = std::move(begin_name);
        } else if (!end_name.empty()) {
            name = std::move(end_name);
        } else {
            name = "vmpilot_region_0x" + ([&] {
                char buf[17];
                snprintf(buf, sizeof(buf), "%llx",
                         static_cast<unsigned long long>(start_addr));
                return std::string(buf);
            })();
        }

        NativeFunctionBase nf(
            start_addr, size, std::move(name), std::move(code));

        if (auto enclosing = findEnclosingSymbol(start_addr, ctx.symbols))
            nf.setEnclosingSymbol(std::move(*enclosing));

        if (!nf.isValid()) {
            spdlog::error(
                "Skipping invalid region at 0x{:x}: "
                "size={} but code.size()={}",
                start_addr, size, nf.getCode().size());
            continue;
        }

        spdlog::info("Found protected region: 0x{:x} - 0x{:x} ({} bytes)",
                     start_addr, end_addr, size);
        result.push_back(std::move(nf));
    }

    return result;
}

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARCH_HANDLER_COMMON_HPP__
