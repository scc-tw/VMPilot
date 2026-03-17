#include <segmentator.hpp>

#include <cinttypes>
#include <cstdio>

using VMPilot::SDK::Segmentator::segment;
using VMPilot::SDK::Segmentator::SegmentError;
using VMPilot::SDK::Segmentator::to_string;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    auto result = segment(argv[1]);
    if (!result) {
        if (result.error() == SegmentError::NoRegionsFound) {
            printf("No protected regions found.\n");
            return 0;
        }
        fprintf(stderr, "Error: %s\n", to_string(result.error()));
        return 1;
    }

    size_t total_regions = 0;
    for (const auto& g : result->groups)
        total_regions += g.sites.size();

    printf("Found %zu protected region(s) in %zu group(s):\n\n",
           total_regions, result->groups.size());

    for (size_t gi = 0; gi < result->groups.size(); ++gi) {
        const auto& g = result->groups[gi];
        size_t inlined = g.sites.size() - 1;
        printf("  Group [%zu] \"%s\"  (%zu cop%s%s)\n", gi,
               g.source_name.c_str(), g.sites.size(),
               g.sites.size() == 1 ? "y" : "ies",
               inlined > 0
                   ? (", " + std::to_string(inlined) + " inlined").c_str()
                   : "");

        for (size_t si = 0; si < g.sites.size(); ++si) {
            const auto& s = g.sites[si];
            uint64_t end = s.addr + s.size;
            printf("    [%s] 0x%" PRIx64 " - 0x%" PRIx64
                   "  (%" PRIu64 " bytes)",
                   s.is_canonical ? "canonical" : "inline  ",
                   s.addr, end, s.size);
            if (s.enclosing_symbol)
                printf("  in %s", s.enclosing_symbol->c_str());
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}
