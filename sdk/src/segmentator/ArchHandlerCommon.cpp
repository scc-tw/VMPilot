#include <ArchHandlerCommon.hpp>

#include <CompilationContext.hpp>
#include <utilities.hpp>

#include <algorithm>
#include <cstdio>

#include <spdlog/spdlog.h>

namespace VMPilot::SDK::Segmentator {

AddrToSymbol buildAddrLookup(const NativeSymbolTable& symbols) {
    AddrToSymbol lookup;
    for (const auto& entry : symbols) {
        lookup[entry.address] = entry.name;
    }
    return lookup;
}

/// Resolve the __FUNCTION__ string argument at the given call site.
static std::string resolveStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions,
    const CompilationContext& ctx,
    const AddrToSymbol& lookup,
    const ArchCallbacks& callbacks) {
    if (ctx.rodata_sections.empty())
        return {};

    auto va = callbacks.extract_string_arg(call_idx, instructions, lookup);
    if (!va)
        return {};

    for (const auto& section : ctx.rodata_sections) {
        if (section.contains(*va))
            return section.readCString(*va);
    }
    return {};
}

std::optional<std::string> findEnclosingSymbol(
    uint64_t addr, const NativeSymbolTable& symbols) {
    // Build sorted index of FUNC symbols (excluding stubs)
    std::vector<size_t> sorted_func_indices;
    for (size_t i = 0; i < symbols.size(); ++i) {
        const auto& sym = symbols[i];
        if (sym.type != SymbolType::FUNC)
            continue;
        if (sym.getAttribute<std::string>("entry_type", "") == "stub")
            continue;
        sorted_func_indices.push_back(i);
    }
    std::sort(sorted_func_indices.begin(),
              sorted_func_indices.end(),
              [&symbols](size_t a, size_t b) {
                  return symbols[a].address < symbols[b].address;
              });

    const auto& idx = sorted_func_indices;
    if (idx.empty())
        return std::nullopt;

    // upper_bound finds first index whose symbol address > addr
    auto it = std::upper_bound(
        idx.begin(), idx.end(), addr,
        [&symbols](uint64_t a, size_t i) {
            return a < symbols[i].address;
        });

    if (it == idx.begin())
        return std::nullopt;

    --it;  // candidate: largest address <= addr
    const auto& candidate = symbols[*it];

    if (candidate.size > 0) {
        // ELF: symbol has known size -- check containment
        if (addr >= candidate.address &&
            addr < candidate.address + candidate.size)
            return candidate.name;
    } else {
        // Mach-O (size == 0): use nearest-next-symbol heuristic
        auto next = it + 1;
        uint64_t upper = (next != idx.end())
                             ? symbols[*next].address
                             : static_cast<uint64_t>(-1);
        if (addr >= candidate.address && addr < upper)
            return candidate.name;
    }

    return std::nullopt;
}

std::vector<NativeFunctionBase> extractNativeFunctions(
    ArchHandlerImplBase& impl,
    const CompilationContext& ctx,
    const ArchCallbacks& callbacks) {
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

        auto sym = callbacks.resolve_call(insn, lookup);
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
        std::string begin_name = resolveStringArg(
            region.begin_idx, instructions, ctx, lookup, callbacks);
        std::string end_name = resolveStringArg(
            region.end_idx, instructions, ctx, lookup, callbacks);

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
