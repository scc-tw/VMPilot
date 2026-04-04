/// verify_roundtrip: Verify serializer dump→load round-trip for a binary.
///
/// Usage: verify_roundtrip <binary>
///
/// Exercises: segment → build_units → dump → load → compare
/// Verifies: loaded units match original units (name, addr, size, code, canonical).
/// Exit 0 = pass, Exit 1 = fail.

#include <Serializer.hpp>
#include <segmentator.hpp>
#include <diagnostic_collector.hpp>

#include <cinttypes>
#include <cstdio>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

using namespace VMPilot::SDK;
using VMPilot::Common::DiagnosticCollector;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    DiagnosticCollector diag;

    // 1. Segment
    auto seg = Segmentator::segment(argv[1], diag);
    if (!seg.has_value()) {
        fprintf(stderr, "Segmentation failed: %s\n",
                VMPilot::Common::to_string(seg.error()));
        return 1;
    }

    // 2. Build units
    auto original_units = Serializer::build_units(*seg, diag);
    if (diag.has_errors()) {
        fprintf(stderr, "%s", diag.summary().c_str());
        return 1;
    }

    printf("Built %zu units from segmentation.\n", original_units.size());
    if (original_units.empty()) {
        printf("No units to verify (binary has no protected regions).\n");
        return 0;
    }

    // 3. Dump to temp directory
    auto tmp = fs::temp_directory_path() / "vmpilot_roundtrip_test";
    fs::remove_all(tmp);

    auto dump_result = Serializer::dump(original_units, tmp.string(), diag);
    if (!dump_result.has_value()) {
        fprintf(stderr, "Dump failed: %s\n",
                VMPilot::Common::to_string(dump_result.error()));
        return 1;
    }
    printf("Dumped to %s\n", tmp.string().c_str());

    // 4. Load back
    auto loaded_result = Serializer::load(tmp.string(), diag);
    if (!loaded_result.has_value()) {
        fprintf(stderr, "Load failed: %s\n",
                VMPilot::Common::to_string(loaded_result.error()));
        return 1;
    }

    auto& loaded_units = *loaded_result;
    printf("Loaded %zu units from disk.\n", loaded_units.size());

    // 5. Compare
    if (original_units.size() != loaded_units.size()) {
        fprintf(stderr, "FAIL: unit count mismatch: original=%zu loaded=%zu\n",
                original_units.size(), loaded_units.size());
        fs::remove_all(tmp);
        return 1;
    }

    // Sort both by addr for stable comparison
    auto by_addr = [](const Core::CompilationUnit& a,
                      const Core::CompilationUnit& b) {
        return a.addr < b.addr;
    };
    std::sort(original_units.begin(), original_units.end(), by_addr);
    std::sort(loaded_units.begin(), loaded_units.end(), by_addr);

    int failures = 0;
    for (size_t i = 0; i < original_units.size(); ++i) {
        const auto& orig = original_units[i];
        const auto& load = loaded_units[i];

        bool ok = true;
        if (orig.name != load.name) {
            fprintf(stderr, "  FAIL [%zu]: name mismatch: '%s' vs '%s'\n",
                    i, orig.name.c_str(), load.name.c_str());
            ok = false;
        }
        if (orig.addr != load.addr) {
            fprintf(stderr,
                    "  FAIL [%zu]: addr mismatch: 0x%" PRIx64 " vs 0x%" PRIx64
                    "\n",
                    i, orig.addr, load.addr);
            ok = false;
        }
        if (orig.size != load.size) {
            fprintf(stderr,
                    "  FAIL [%zu]: size mismatch: %" PRIu64 " vs %" PRIu64
                    "\n",
                    i, orig.size, load.size);
            ok = false;
        }
        if (orig.code != load.code) {
            fprintf(stderr,
                    "  FAIL [%zu]: code mismatch (%zu vs %zu bytes)\n", i,
                    orig.code.size(), load.code.size());
            ok = false;
        }
        if (orig.is_canonical != load.is_canonical) {
            fprintf(stderr,
                    "  FAIL [%zu]: is_canonical mismatch: %d vs %d\n", i,
                    orig.is_canonical, load.is_canonical);
            ok = false;
        }
        if (orig.enclosing_symbol != load.enclosing_symbol) {
            fprintf(stderr,
                    "  FAIL [%zu]: enclosing_symbol mismatch: '%s' vs '%s'\n",
                    i, orig.enclosing_symbol.c_str(),
                    load.enclosing_symbol.c_str());
            ok = false;
        }
        if (orig.data_references.size() != load.data_references.size()) {
            fprintf(stderr,
                    "  FAIL [%zu]: data_references count mismatch: %zu vs %zu\n",
                    i, orig.data_references.size(),
                    load.data_references.size());
            ok = false;
        } else {
            for (size_t j = 0; j < orig.data_references.size(); ++j) {
                const auto& o = orig.data_references[j];
                const auto& l = load.data_references[j];
                if (o.insn_offset != l.insn_offset ||
                    o.target_va != l.target_va ||
                    o.kind != l.kind ||
                    o.atomic_op != l.atomic_op ||
                    o.atomic_ordering != l.atomic_ordering ||
                    o.atomic_width != l.atomic_width) {
                    fprintf(stderr,
                            "  FAIL [%zu]: data_references[%zu] mismatch\n",
                            i, j);
                    ok = false;
                    break;
                }
            }
        }
        if (!load.context) {
            fprintf(stderr, "  FAIL [%zu]: loaded unit has null context\n", i);
            ok = false;
        }

        if (ok) {
            printf("  PASS [%zu] %-20s  addr=0x%" PRIx64 "  %zu bytes  %s\n",
                   i, orig.name.c_str(), orig.addr, orig.code.size(),
                   orig.is_canonical ? "canonical" : "inline");
        } else {
            ++failures;
        }
    }

    // Cleanup
    fs::remove_all(tmp);

    printf("\n%zu/%zu units verified.\n", original_units.size() - static_cast<size_t>(failures),
           original_units.size());

    if (failures > 0) {
        fprintf(stderr, "FAIL: %d unit(s) did not round-trip correctly.\n",
                failures);
        return 1;
    }

    printf("ALL PASS\n");
    return 0;
}
