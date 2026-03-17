#include <RegionRefiner.hpp>

#include <algorithm>
#include <string>
#include <unordered_map>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define HAS_CXXABI 1
#else
#define HAS_CXXABI 0
#endif

#include <spdlog/spdlog.h>

using NativeFunc = VMPilot::SDK::Segmentator::NativeFunctionBase;

std::vector<NativeFunc> VMPilot::SDK::RegionRefiner::refine(
    std::vector<NativeFunc> regions) noexcept {
    if (regions.size() <= 1) {
        return regions;
    }

    // Sort by start address, then by size descending (larger regions first)
    std::sort(regions.begin(), regions.end(), [](const auto& a, const auto& b) {
        if (a.getAddr() == b.getAddr()) {
            return a.getSize() > b.getSize();
        }
        return a.getAddr() < b.getAddr();
    });

    std::vector<NativeFunc> result;

    // Sweep: maintain the "current" outermost region's end address.
    // Any region that starts within and ends within the current region
    // is contained and gets dropped. Partial overlaps get merged.
    uint64_t current_end = 0;

    for (auto& region : regions) {
        uint64_t start = region.getAddr();
        uint64_t end = start + region.getSize();

        if (start >= current_end) {
            // No overlap with previous — start a new region
            current_end = end;
            result.push_back(std::move(region));
        } else if (end <= current_end) {
            // Fully contained within the current region — drop it
            spdlog::warn(
                "RegionRefiner: dropping contained region '{}' "
                "[0x{:x}, 0x{:x}) inside '{}' [0x{:x}, 0x{:x})",
                region.getName(), start, end, result.back().getName(),
                result.back().getAddr(), current_end);
            continue;
        } else {
            // Partial overlap — extend the current region to cover both
            auto& last = result.back();
            spdlog::warn(
                "RegionRefiner: merging partially overlapping regions "
                "'{}' [0x{:x}, 0x{:x}) and '{}' [0x{:x}, 0x{:x})",
                last.getName(), last.getAddr(), current_end,
                region.getName(), start, end);
            uint64_t merged_start = last.getAddr();
            uint64_t merged_size = end - merged_start;

            // Rebuild the code bytes for the merged region
            auto last_code = last.getCode();
            auto this_code = region.getCode();

            // The overlap portion: region starts at 'start', last ends at
            // 'current_end'. Overlap = current_end - start bytes.
            uint64_t overlap = current_end - start;
            // Append only the non-overlapping tail of the new region
            if (overlap < this_code.size()) {
                last_code.insert(last_code.end(), this_code.begin() + overlap,
                                 this_code.end());
            }

            NativeFunc merged(
                merged_start, merged_size, last.getName(),
                std::move(last_code));

            if (!merged.isValid()) {
                spdlog::error(
                    "RegionRefiner: merged region '{}' at 0x{:x} is invalid: "
                    "size={} but code.size()={}, dropping",
                    merged.getName(), merged_start, merged_size,
                    merged.getCode().size());
                result.pop_back();
                // Don't update current_end — treat as if both were dropped
                continue;
            }

            result.back() = std::move(merged);
            current_end = end;
        }
    }

    return result;
}

// --- Demangling helpers for canonical detection ---

static std::string demangle(const std::string& mangled) {
#if HAS_CXXABI
    int status = 0;
    char* demangled =
        abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
    if (status == 0 && demangled) {
        std::string result(demangled);
        free(demangled);
        return result;
    }
#endif
    return mangled;  // fallback: return as-is
}

/// Extract the bare function name from a demangled string.
/// "foo(int)" → "foo", "MyClass::foo(int)" → "foo"
static std::string extractBareName(const std::string& demangled) {
    // Find the opening paren (if any) and work backwards
    auto paren = demangled.find('(');
    std::string prefix =
        (paren != std::string::npos) ? demangled.substr(0, paren) : demangled;

    // Find the last :: separator
    auto sep = prefix.rfind("::");
    if (sep != std::string::npos)
        return prefix.substr(sep + 2);

    // Handle spaces (e.g., "void foo" → "foo") — take last token
    auto space = prefix.rfind(' ');
    if (space != std::string::npos)
        return prefix.substr(space + 1);

    return prefix;
}

/// Check if source_name matches the enclosing_symbol (canonical site).
static bool isCanonicalMatch(const std::string& source_name,
                             const std::string& enclosing_symbol) {
    // Direct match (C functions, already-demangled)
    if (source_name == enclosing_symbol)
        return true;

    std::string demangled = demangle(enclosing_symbol);
    std::string bare_enclosing = extractBareName(demangled);
    std::string bare_source = extractBareName(source_name);

    return bare_source == bare_enclosing;
}

std::vector<VMPilot::SDK::RegionRefiner::ProtectedRegion>
VMPilot::SDK::RegionRefiner::group(
    const std::vector<Segmentator::NativeFunctionBase>&
        regions) noexcept {
    // Map source_name → index into result vector
    std::unordered_map<std::string, size_t> name_to_group;
    std::vector<ProtectedRegion> groups;

    for (const auto& region : regions) {
        const auto& name = region.getName();

        RegionSite site;
        site.source_name = name;
        site.enclosing_symbol = region.getEnclosingSymbol();
        site.addr = region.getAddr();
        site.size = region.getSize();

        auto it = name_to_group.find(name);
        if (it == name_to_group.end()) {
            size_t idx = groups.size();
            name_to_group[name] = idx;
            ProtectedRegion pr;
            pr.source_name = name;
            pr.sites.push_back(std::move(site));
            groups.push_back(std::move(pr));
        } else {
            groups[it->second].sites.push_back(std::move(site));
        }
    }

    // Determine canonical site for each group
    for (auto& group : groups) {
        bool found = false;
        for (size_t i = 0; i < group.sites.size(); ++i) {
            auto& site = group.sites[i];
            if (site.enclosing_symbol &&
                isCanonicalMatch(group.source_name,
                                 *site.enclosing_symbol)) {
                site.is_canonical = true;
                group.canonical_index = i;
                found = true;
                break;
            }
        }
        // Fallback: first site is canonical
        if (!found) {
            group.sites[0].is_canonical = true;
            group.canonical_index = 0;
        }
    }

    return groups;
}
