#ifndef __LOADER_TYPES_HPP__
#define __LOADER_TYPES_HPP__
#pragma once

#include <file_type_parser.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Seed size in bytes (32-byte root secret, spec §13.5).
constexpr size_t SEED_SIZE = 32;

/// Describes a single protected region to be patched.
struct RegionPatchInfo {
    std::string name;       // __FUNCTION__ name
    uint64_t addr = 0;      // Original VA of protected region
    uint64_t size = 0;      // Size in bytes
};

/// Input to the Loader's patch operation.
///
/// The Loader is a **passthrough binary mutation tool**.  It does NOT know
/// the blob's internal structure — that is the compiler backend's concern.
/// It receives pre-built blob bytes and injects them into the binary.
struct PatchRequest {
    std::string input_path;     // Path to original binary
    std::string output_path;    // Path for patched output binary

    /// Protected regions to patch (from SegmentationResult).
    std::vector<RegionPatchInfo> regions;

    /// Complete bytecode blob produced by the compiler pipeline.
    /// Format: Common::VM::BlobHeader + sections (see vm_blob.hpp).
    /// Loader injects this verbatim — no interpretation or transformation.
    std::vector<uint8_t> blob_data;

    /// 32-byte root secret (stored_seed).
    /// Embedded alongside the blob in the patched binary so the runtime
    /// can derive encryption/encoding keys at execution time.
    /// NEVER stored inside the blob itself (spec §13.5).
    std::array<uint8_t, SEED_SIZE> stored_seed = {};

    /// Architecture info (from SegmentationResult.context).
    Common::FileArch arch = Common::FileArch::X86;
    Common::FileMode mode = Common::FileMode::MODE_64;
    Common::FileFormat format = Common::FileFormat::ELF;
};

/// Output from a successful patch operation.
struct PatchResult {
    std::string output_path;
    size_t regions_patched = 0;
    size_t blob_bytes_injected = 0;
};

// ---------------------------------------------------------------------------
// Binary editor result types
// ---------------------------------------------------------------------------

struct TextSectionInfo {
    uint64_t base_addr = 0;
    uint64_t size = 0;
};

struct NewSegmentInfo {
    uint64_t va = 0;
    size_t size = 0;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_TYPES_HPP__
