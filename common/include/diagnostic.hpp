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

/// Unified diagnostic code across all components.
///
/// 32-bit layout: 0xMMMM'CCCC
///   upper 16 bits = module ID
///   lower 16 bits = error code within module
///
/// Module IDs:
///   0x0000 = None / Common
///   0x0001 = Segmentator
///   0x0002 = RegionRefiner
///   0x0003 = Serializer
///   0x0004 = Compiler (SimpleBackend, future LLVM Backend)
///   0x0005 = ReferenceAnalyzer (reserved)
///   0x0006 = Runtime VM
///   0x0007 = Loader
enum class DiagnosticCode : uint32_t {
    None = 0x0000'0000,

    // --- 0x0001: Segmentator ---
    FileNotFound           = 0x0001'0001,
    UnsupportedFormat      = 0x0001'0002,
    UnsupportedArch        = 0x0001'0003,
    TextSectionMissing     = 0x0001'0004,
    DisassemblyFailed      = 0x0001'0005,
    NoRegionsFound         = 0x0001'0006,

    // --- 0x0002: RegionRefiner ---
    ContainedRegionDropped = 0x0002'0001,
    OverlappingRegionMerged= 0x0002'0002,
    InvalidMergedRegion    = 0x0002'0003,

    // --- 0x0003: Serializer ---
    OrphanSiteSkipped      = 0x0003'0001,
    SerializationFailed    = 0x0003'0002,
    LoadFailed             = 0x0003'0003,
    MalformedManifestEntry = 0x0003'0004,
    PartialDumpCleanup     = 0x0003'0005,
    NoUnitsToDump          = 0x0003'0006,
    NullContext            = 0x0003'0007,

    // --- 0x0004: Compiler ---
    InvalidInput           = 0x0004'0001,
    CompilerUnsupportedArch= 0x0004'0002,
    UnsupportedInstruction = 0x0004'0003,
    CompilerInternalError  = 0x0004'0004,
    NotImplemented         = 0x0004'0005,
    BackendCreationFailed  = 0x0004'0006,
    NullBackend            = 0x0004'0007,

    // --- 0x0005: ReferenceAnalyzer (reserved) ---

    // --- 0x0006: Runtime VM ---
    BlobHeaderInvalid          = 0x0006'0001,
    BlobMagicMismatch          = 0x0006'0002,
    BlobVersionMismatch        = 0x0006'0003,
    BlobTruncated              = 0x0006'0004,
    BBMetadataDecryptFailed    = 0x0006'0005,
    BBMacVerificationFailed    = 0x0006'0006,
    InstructionDecryptFailed   = 0x0006'0007,
    OramAddressUnaligned       = 0x0006'0008,
    OramAddressOutOfBounds     = 0x0006'0009,
    NativeCallBridgeFailed     = 0x0006'000A,
    StackOverflow              = 0x0006'000B,
    StackUnderflow             = 0x0006'000C,
    InvalidBBTransition        = 0x0006'000D,
    AntiDebugTriggered         = 0x0006'000E,
    IntegrityCheckFailed       = 0x0006'000F,
    HaltReached                = 0x0006'0010,
    EpochResyncFailed          = 0x0006'0011,
    ShadowStackOverflow        = 0x0006'0012,
    InvalidOpcodeAlias         = 0x0006'0013,

    // --- 0x0007: Loader ---
    PatchInputInvalid          = 0x0007'0001,
    PatchOutputFailed          = 0x0007'0002,
    PatchRegionTooSmall        = 0x0007'0003,
    PatchFormatUnsupported     = 0x0007'0004,
    PatchArchUnsupported       = 0x0007'0005,
    PatchBlobSerializationFailed = 0x0007'0006,
    PatchBinaryReadFailed      = 0x0007'0007,
    PatchBinaryWriteFailed     = 0x0007'0008,
    PatchStubGenerationFailed  = 0x0007'0009,
    PatchSegmentCreationFailed = 0x0007'000A,
};

/// Extract the module ID (upper 16 bits) from a DiagnosticCode.
inline uint16_t module_of(DiagnosticCode code) noexcept {
    return static_cast<uint16_t>(static_cast<uint32_t>(code) >> 16);
}

/// Extract the error code (lower 16 bits) from a DiagnosticCode.
inline uint16_t code_of(DiagnosticCode code) noexcept {
    return static_cast<uint16_t>(static_cast<uint32_t>(code) & 0xFFFF);
}

/// Human-readable module name from a module ID.
const char* module_name(uint16_t module_id) noexcept;

/// Human-readable name for a DiagnosticCode.
const char* to_string(DiagnosticCode code) noexcept;

/// Human-readable name for a DiagnosticLevel.
const char* to_string(DiagnosticLevel level) noexcept;

struct Diagnostic {
    DiagnosticLevel level = DiagnosticLevel::Note;
    DiagnosticCode code = DiagnosticCode::None;
    std::string source;       // "segmentator", "refiner", "serializer", "compiler", "runtime", "loader"
    std::string message;
    std::string unit_name;    // optional: related unit
    uint64_t addr = 0;        // optional: related address
};

}  // namespace VMPilot::Common

#endif  // __COMMON_DIAGNOSTIC_HPP__
