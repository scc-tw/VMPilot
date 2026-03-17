#ifndef __SDK_ARCH_HANDLER_COMMON_HPP__
#define __SDK_ARCH_HANDLER_COMMON_HPP__
#pragma once

#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>

#include <capstone.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::Segmentator {

struct CompilationContext;  // forward declaration

using AddrToSymbol = std::unordered_map<uint64_t, std::string>;

AddrToSymbol buildAddrLookup(const NativeSymbolTable& symbols);

/// Callback type: given an instruction and the lookup table, resolve to
/// a symbol name (or nullopt if unresolvable).
using CallResolver = std::function<std::optional<std::string>(
    const Capstone::Instruction&, const AddrToSymbol&)>;

/// Callback: given the index of a call instruction and the instruction list,
/// extract the VA of the first string argument (from preceding instructions).
/// Returns 0 if unresolvable.
using ArgExtractor = std::function<uint64_t(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions)>;

/// Shared implementation state for X86Handler and ARM64Handler.
struct ArchHandlerImplBase {
    Capstone::Capstone cs;
    AddrToSymbol addr_lookup;
    uint64_t base_addr = static_cast<uint64_t>(-1);
    std::vector<Capstone::Instruction> instructions;
    std::vector<std::unique_ptr<NativeFunctionBase>> native_functions;
    const CompilationContext* compilation_ctx = nullptr;  // non-owning

    ArchHandlerImplBase(Capstone::Capstone&& cs, AddrToSymbol&& lookup)
        : cs(std::move(cs)), addr_lookup(std::move(lookup)) {}
};

/// Extract protected regions from disassembled instructions using the
/// given call resolver. Results are cached in impl->native_functions.
std::vector<std::unique_ptr<NativeFunctionBase>> extractNativeFunctions(
    ArchHandlerImplBase& impl, const CallResolver& resolver,
    const ArgExtractor& arg_extractor = nullptr);

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARCH_HANDLER_COMMON_HPP__
