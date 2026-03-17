#include <diagnostic_collector.hpp>

#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace VMPilot::Common {

// ---------------------------------------------------------------------------
// to_string helpers
// ---------------------------------------------------------------------------

const char* to_string(DiagnosticLevel level) noexcept {
    switch (level) {
        case DiagnosticLevel::Note:    return "note";
        case DiagnosticLevel::Warning: return "WARN";
        case DiagnosticLevel::Error:   return "ERROR";
    }
    return "???";
}

const char* to_string(DiagnosticCode code) noexcept {
    switch (code) {
        case DiagnosticCode::None:                    return "none";
        // Segmentator
        case DiagnosticCode::FileNotFound:            return "file_not_found";
        case DiagnosticCode::UnsupportedFormat:       return "unsupported_format";
        case DiagnosticCode::UnsupportedArch:         return "unsupported_arch";
        case DiagnosticCode::TextSectionMissing:      return "text_section_missing";
        case DiagnosticCode::DisassemblyFailed:       return "disassembly_failed";
        case DiagnosticCode::NoRegionsFound:          return "no_regions_found";
        // RegionRefiner
        case DiagnosticCode::ContainedRegionDropped:  return "contained_region_dropped";
        case DiagnosticCode::OverlappingRegionMerged: return "overlapping_region_merged";
        case DiagnosticCode::InvalidMergedRegion:     return "invalid_merged_region";
        // Serializer
        case DiagnosticCode::OrphanSiteSkipped:       return "orphan_site_skipped";
        case DiagnosticCode::SerializationFailed:     return "serialization_failed";
        case DiagnosticCode::LoadFailed:              return "load_failed";
        case DiagnosticCode::MalformedManifestEntry:  return "malformed_manifest_entry";
        case DiagnosticCode::PartialDumpCleanup:      return "partial_dump_cleanup";
        case DiagnosticCode::NoUnitsToDump:           return "no_units_to_dump";
        case DiagnosticCode::NullContext:             return "null_context";
        // Compiler
        case DiagnosticCode::InvalidInput:            return "invalid_input";
        case DiagnosticCode::CompilerUnsupportedArch: return "compiler_unsupported_arch";
        case DiagnosticCode::UnsupportedInstruction:  return "unsupported_instruction";
        case DiagnosticCode::CompilerInternalError:   return "compiler_internal_error";
        case DiagnosticCode::NotImplemented:          return "not_implemented";
        case DiagnosticCode::BackendCreationFailed:   return "backend_creation_failed";
        case DiagnosticCode::NullBackend:             return "null_backend";
    }
    return "unknown";
}

// ---------------------------------------------------------------------------
// noop singleton
// ---------------------------------------------------------------------------

DiagnosticCollector& DiagnosticCollector::noop() {
    static DiagnosticCollector instance(/*is_noop=*/true);
    return instance;
}

// ---------------------------------------------------------------------------
// add / convenience methods
// ---------------------------------------------------------------------------

void DiagnosticCollector::add(Diagnostic diag) {
    if (is_noop_) return;
    std::lock_guard<std::mutex> lock(mtx_);
    diagnostics_.push_back(std::move(diag));
}

void DiagnosticCollector::note(const std::string& source, DiagnosticCode code,
                               const std::string& message,
                               const std::string& unit_name, uint64_t addr) {
    add({DiagnosticLevel::Note, code, source, message, unit_name, addr});
}

void DiagnosticCollector::warn(const std::string& source, DiagnosticCode code,
                               const std::string& message,
                               const std::string& unit_name, uint64_t addr) {
    add({DiagnosticLevel::Warning, code, source, message, unit_name, addr});
}

void DiagnosticCollector::error(const std::string& source, DiagnosticCode code,
                                const std::string& message,
                                const std::string& unit_name, uint64_t addr) {
    add({DiagnosticLevel::Error, code, source, message, unit_name, addr});
}

// ---------------------------------------------------------------------------
// queries
// ---------------------------------------------------------------------------

bool DiagnosticCollector::has_errors() const {
    std::lock_guard<std::mutex> lock(mtx_);
    for (const auto& d : diagnostics_)
        if (d.level == DiagnosticLevel::Error) return true;
    return false;
}

bool DiagnosticCollector::has_warnings() const {
    std::lock_guard<std::mutex> lock(mtx_);
    for (const auto& d : diagnostics_)
        if (d.level == DiagnosticLevel::Warning) return true;
    return false;
}

size_t DiagnosticCollector::error_count() const {
    std::lock_guard<std::mutex> lock(mtx_);
    size_t n = 0;
    for (const auto& d : diagnostics_)
        if (d.level == DiagnosticLevel::Error) ++n;
    return n;
}

size_t DiagnosticCollector::warning_count() const {
    std::lock_guard<std::mutex> lock(mtx_);
    size_t n = 0;
    for (const auto& d : diagnostics_)
        if (d.level == DiagnosticLevel::Warning) ++n;
    return n;
}

size_t DiagnosticCollector::size() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return diagnostics_.size();
}

const std::vector<Diagnostic>& DiagnosticCollector::all() const {
    return diagnostics_;
}

// ---------------------------------------------------------------------------
// summary
// ---------------------------------------------------------------------------

std::string DiagnosticCollector::summary() const {
    std::lock_guard<std::mutex> lock(mtx_);

    if (diagnostics_.empty())
        return "No diagnostics.\n";

    // Group by unit_name (empty = global)
    std::unordered_map<std::string, std::vector<const Diagnostic*>> by_unit;
    std::vector<std::string> unit_order;

    for (const auto& d : diagnostics_) {
        const auto& key = d.unit_name;
        if (by_unit.find(key) == by_unit.end())
            unit_order.push_back(key);
        by_unit[key].push_back(&d);
    }

    // Sort each group: Error first, then Warning, then Note
    auto severity_rank = [](DiagnosticLevel l) -> int {
        switch (l) {
            case DiagnosticLevel::Error:   return 0;
            case DiagnosticLevel::Warning: return 1;
            case DiagnosticLevel::Note:    return 2;
        }
        return 3;
    };

    std::ostringstream ss;
    ss << "=== VMPilot Diagnostics ===\n\n";

    for (const auto& unit_key : unit_order) {
        auto& diags = by_unit[unit_key];
        std::sort(diags.begin(), diags.end(),
                  [&](const Diagnostic* a, const Diagnostic* b) {
                      return severity_rank(a->level) < severity_rank(b->level);
                  });

        if (unit_key.empty()) {
            ss << "Global:\n";
        } else {
            ss << "Unit \"" << unit_key << "\"";
            // Find first addr
            for (const auto* d : diags) {
                if (d->addr != 0) {
                    ss << " at 0x" << std::hex << d->addr << std::dec;
                    break;
                }
            }
            ss << ":\n";
        }

        for (const auto* d : diags) {
            ss << "  " << to_string(d->level)
               << "  [" << d->source << ":" << to_string(d->code) << "] "
               << d->message << "\n";
        }
        ss << "\n";
    }

    size_t errs = 0, warns = 0;
    for (const auto& d : diagnostics_) {
        if (d.level == DiagnosticLevel::Error) ++errs;
        else if (d.level == DiagnosticLevel::Warning) ++warns;
    }
    ss << "Summary: " << diagnostics_.size() << " diagnostics, "
       << errs << " error(s), " << warns << " warning(s)\n";

    return ss.str();
}

}  // namespace VMPilot::Common
