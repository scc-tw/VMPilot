#ifndef __SDK_ARCH_HANDLER_COMMON_HPP__
#define __SDK_ARCH_HANDLER_COMMON_HPP__
#pragma once

#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>
#include <capstone.hpp>

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::Segmentator {

struct CompilationContext;  // forward declaration

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

/// Type-erased callback bundle for extractNativeFunctions.
struct ArchCallbacks {
    std::function<std::optional<std::string>(
        const Capstone::Instruction&, const AddrToSymbol&)> resolve_call;

    std::function<std::optional<uint64_t>(
        size_t, const std::vector<Capstone::Instruction>&,
        const AddrToSymbol&)> extract_string_arg;
};

/// Shared implementation state for X86Handler and ARM64Handler.
struct ArchHandlerImplBase {
    Capstone::Capstone cs;
    AddrToSymbol addr_lookup;
    uint64_t base_addr = static_cast<uint64_t>(-1);
    std::vector<Capstone::Instruction> instructions;

    ArchHandlerImplBase(Capstone::Capstone&& cs_handle, AddrToSymbol&& lookup)
        : cs(std::move(cs_handle)), addr_lookup(std::move(lookup)) {}
};

/// Find the function symbol that encloses the given address.
std::optional<std::string> findEnclosingSymbol(
    uint64_t addr, const NativeSymbolTable& symbols);

/// Extract protected regions (non-template, algorithm lives in .cpp).
std::vector<NativeFunctionBase> extractNativeFunctions(
    ArchHandlerImplBase& impl,
    const CompilationContext& ctx,
    const ArchCallbacks& callbacks);

/// Thin template bridge: traits -> std::function -> algorithm.
template <typename CallbackTraits>
std::vector<NativeFunctionBase> extractNativeFunctions(
    ArchHandlerImplBase& impl, const CompilationContext& ctx) {
    return extractNativeFunctions(impl, ctx, {
        CallbackTraits::resolveCall,
        CallbackTraits::extractStringArg
    });
}

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARCH_HANDLER_COMMON_HPP__
