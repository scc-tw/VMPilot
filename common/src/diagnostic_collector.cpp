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

const char* module_name(uint16_t module_id) noexcept {
    switch (module_id) {
        case 0x0000: return "common";
        case 0x0001: return "segmentator";
        case 0x0002: return "refiner";
        case 0x0003: return "serializer";
        case 0x0004: return "compiler";
        case 0x0005: return "reference_analyzer";
        case 0x0006: return "runtime";
        case 0x0007: return "loader";
    }
    return "unknown_module";
}

const char* to_string(DiagnosticCode code) noexcept {
    switch (code) {
        case DiagnosticCode::None:                    return "none";
        // 0x0001: Segmentator
        case DiagnosticCode::FileNotFound:            return "SEG:file_not_found";
        case DiagnosticCode::UnsupportedFormat:       return "SEG:unsupported_format";
        case DiagnosticCode::UnsupportedArch:         return "SEG:unsupported_arch";
        case DiagnosticCode::TextSectionMissing:      return "SEG:text_section_missing";
        case DiagnosticCode::DisassemblyFailed:       return "SEG:disassembly_failed";
        case DiagnosticCode::NoRegionsFound:          return "SEG:no_regions_found";
        // 0x0002: RegionRefiner
        case DiagnosticCode::ContainedRegionDropped:  return "REF:contained_region_dropped";
        case DiagnosticCode::OverlappingRegionMerged: return "REF:overlapping_region_merged";
        case DiagnosticCode::InvalidMergedRegion:     return "REF:invalid_merged_region";
        // 0x0003: Serializer
        case DiagnosticCode::OrphanSiteSkipped:       return "SER:orphan_site_skipped";
        case DiagnosticCode::SerializationFailed:     return "SER:serialization_failed";
        case DiagnosticCode::LoadFailed:              return "SER:load_failed";
        case DiagnosticCode::MalformedManifestEntry:  return "SER:malformed_manifest_entry";
        case DiagnosticCode::PartialDumpCleanup:      return "SER:partial_dump_cleanup";
        case DiagnosticCode::NoUnitsToDump:           return "SER:no_units_to_dump";
        case DiagnosticCode::NullContext:             return "SER:null_context";
        // 0x0004: Compiler
        case DiagnosticCode::InvalidInput:            return "CMP:invalid_input";
        case DiagnosticCode::CompilerUnsupportedArch: return "CMP:unsupported_arch";
        case DiagnosticCode::UnsupportedInstruction:  return "CMP:unsupported_instruction";
        case DiagnosticCode::CompilerInternalError:   return "CMP:internal_error";
        case DiagnosticCode::NotImplemented:          return "CMP:not_implemented";
        case DiagnosticCode::BackendCreationFailed:   return "CMP:backend_creation_failed";
        case DiagnosticCode::NullBackend:             return "CMP:null_backend";
        // 0x0006: Runtime VM
        case DiagnosticCode::BlobHeaderInvalid:        return "RT:blob_header_invalid";
        case DiagnosticCode::BlobMagicMismatch:        return "RT:blob_magic_mismatch";
        case DiagnosticCode::BlobVersionMismatch:      return "RT:blob_version_mismatch";
        case DiagnosticCode::BlobTruncated:            return "RT:blob_truncated";
        case DiagnosticCode::BBMetadataDecryptFailed:  return "RT:bb_metadata_decrypt_failed";
        case DiagnosticCode::BBMacVerificationFailed:  return "RT:bb_mac_verification_failed";
        case DiagnosticCode::InstructionDecryptFailed: return "RT:instruction_decrypt_failed";
        case DiagnosticCode::OramAddressUnaligned:     return "RT:oram_address_unaligned";
        case DiagnosticCode::OramAddressOutOfBounds:   return "RT:oram_address_out_of_bounds";
        case DiagnosticCode::NativeCallBridgeFailed:   return "RT:native_call_bridge_failed";
        case DiagnosticCode::StackOverflow:            return "RT:stack_overflow";
        case DiagnosticCode::StackUnderflow:           return "RT:stack_underflow";
        case DiagnosticCode::InvalidBBTransition:      return "RT:invalid_bb_transition";
        case DiagnosticCode::AntiDebugTriggered:       return "RT:anti_debug_triggered";
        case DiagnosticCode::IntegrityCheckFailed:     return "RT:integrity_check_failed";
        case DiagnosticCode::HaltReached:              return "RT:halt_reached";
        case DiagnosticCode::EpochResyncFailed:        return "RT:epoch_resync_failed";
        case DiagnosticCode::ShadowStackOverflow:      return "RT:shadow_stack_overflow";
        case DiagnosticCode::InvalidOpcodeAlias:       return "RT:invalid_opcode_alias";
        case DiagnosticCode::NativeBoundaryUnwindTrapped:
            return "RT:native_boundary_unwind_trapped";
        // 0x0007: Loader
        case DiagnosticCode::PatchInputInvalid:          return "LDR:input_invalid";
        case DiagnosticCode::PatchOutputFailed:          return "LDR:output_failed";
        case DiagnosticCode::PatchRegionTooSmall:        return "LDR:region_too_small";
        case DiagnosticCode::PatchFormatUnsupported:     return "LDR:format_unsupported";
        case DiagnosticCode::PatchArchUnsupported:       return "LDR:arch_unsupported";
        case DiagnosticCode::PatchBlobSerializationFailed: return "LDR:blob_serialization_failed";
        case DiagnosticCode::PatchBinaryReadFailed:      return "LDR:binary_read_failed";
        case DiagnosticCode::PatchBinaryWriteFailed:     return "LDR:binary_write_failed";
        case DiagnosticCode::PatchStubGenerationFailed:  return "LDR:stub_generation_failed";
        case DiagnosticCode::PatchSegmentCreationFailed: return "LDR:segment_creation_failed";
        case DiagnosticCode::PatchRuntimeDepFailed:      return "LDR:runtime_dep_failed";
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
