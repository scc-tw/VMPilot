#ifndef __LOADER_TYPES_HPP__
#define __LOADER_TYPES_HPP__
#pragma once

#include <CompilationOutput.hpp>
#include <file_type_parser.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Describes a single protected region to be patched.
struct RegionPatchInfo {
    std::string name;       // __FUNCTION__ name
    uint64_t addr = 0;      // Original VA of protected region
    uint64_t size = 0;      // Size in bytes
};

/// Input to the Loader's patch operation.
struct PatchRequest {
    std::string input_path;     // Path to original binary
    std::string output_path;    // Path for patched output binary

    /// Protected regions to patch (from SegmentationResult).
    std::vector<RegionPatchInfo> regions;

    /// Compiled bytecodes per region (from CompilationResult).
    std::vector<SDK::BytecodeCompiler::CompilationOutput> compiled_outputs;

    /// Architecture info (from SegmentationResult.context).
    Common::FileArch arch;
    Common::FileMode mode;
    Common::FileFormat format;
};

/// Output from a successful patch operation.
struct PatchResult {
    std::string output_path;
    size_t regions_patched = 0;
    size_t blob_bytes_injected = 0;
};

/// On-disk header for the injected bytecode blob.
/// Layout: BlobHeader | BlobEntry[entry_count] | raw bytecode data
struct BlobHeader {
    uint32_t magic = 0x564D5031;    // "VMP1"
    uint16_t version = 1;
    uint16_t entry_count = 0;
};
static_assert(sizeof(BlobHeader) == 8, "BlobHeader must be 8 bytes");

/// Per-region entry in the bytecode blob.
struct BlobEntry {
    uint64_t original_addr = 0;     // VA of original protected region
    uint32_t bytecode_offset = 0;   // Offset from start of raw data section
    uint32_t bytecode_size = 0;     // Size of serialized bytecodes
};
static_assert(sizeof(BlobEntry) == 16, "BlobEntry must be 16 bytes");

// ---------------------------------------------------------------------------
// Binary editor result types
// ---------------------------------------------------------------------------

/// Info about the .text section in the target binary.
struct TextSectionInfo {
    uint64_t base_addr = 0;
    uint64_t size = 0;
};

/// Info about a newly added loadable segment/section.
struct NewSegmentInfo {
    uint64_t va = 0;       // Virtual address of the new segment
    size_t size = 0;        // Size of injected data
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_TYPES_HPP__
