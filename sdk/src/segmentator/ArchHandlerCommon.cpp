#include "ArchHandlerCommon.hpp"

#include <algorithm>

namespace VMPilot::SDK::Segmentator {

AddrToSymbol buildAddrLookup(const NativeSymbolTable& symbols) {
    AddrToSymbol lookup;
    for (const auto& entry : symbols) {
        lookup[entry.address] = entry.name;
    }
    return lookup;
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
        // ELF: symbol has known size — check containment
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

}  // namespace VMPilot::SDK::Segmentator
