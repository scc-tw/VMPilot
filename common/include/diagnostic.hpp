#ifndef __COMMON_DIAGNOSTIC_HPP__
#define __COMMON_DIAGNOSTIC_HPP__

#include <cstdint>
#include <string>

namespace VMPilot::Common {

enum class DiagnosticLevel : uint8_t {
    Note,       // informational context
    Warning,    // unexpected but not fatal
    Error,      // prevents correct output
};

/// Unified diagnostic code across all SDK components.
/// Ranges:
///   0          = None
///   1000-1999  = Segmentator + RegionRefiner
///   2000-2999  = Serializer
///   3000-3999  = Compiler
enum class DiagnosticCode : uint16_t {
    None = 0,

    // --- Segmentator: 1000-1499 ---
    FileNotFound = 1001,
    UnsupportedFormat = 1002,
    UnsupportedArch = 1003,
    TextSectionMissing = 1004,
    DisassemblyFailed = 1005,
    NoRegionsFound = 1006,

    // --- RegionRefiner: 1500-1999 ---
    ContainedRegionDropped = 1501,
    OverlappingRegionMerged = 1502,
    InvalidMergedRegion = 1503,

    // --- Serializer: 2000-2999 ---
    OrphanSiteSkipped = 2001,
    SerializationFailed = 2002,
    LoadFailed = 2003,
    MalformedManifestEntry = 2004,
    PartialDumpCleanup = 2005,
    NoUnitsToDump = 2006,
    NullContext = 2007,

    // --- Compiler: 3000-3999 ---
    InvalidInput = 3001,
    CompilerUnsupportedArch = 3002,
    UnsupportedInstruction = 3003,
    CompilerInternalError = 3004,
    NotImplemented = 3005,
    BackendCreationFailed = 3006,
    NullBackend = 3007,
};

/// Human-readable name for a DiagnosticCode.
const char* to_string(DiagnosticCode code) noexcept;

/// Human-readable name for a DiagnosticLevel.
const char* to_string(DiagnosticLevel level) noexcept;

struct Diagnostic {
    DiagnosticLevel level = DiagnosticLevel::Note;
    DiagnosticCode code = DiagnosticCode::None;
    std::string source;       // "segmentator", "refiner", "serializer", "compiler"
    std::string message;
    std::string unit_name;    // optional: related unit
    uint64_t addr = 0;        // optional: related address
};

}  // namespace VMPilot::Common

#endif  // __COMMON_DIAGNOSTIC_HPP__
