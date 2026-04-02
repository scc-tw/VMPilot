#pragma once
#ifndef __RUNTIME_BLOB_BUILDER_HPP__
#define __RUNTIME_BLOB_BUILDER_HPP__

/// @file blob_builder.hpp
/// @brief Unified blob builder — constructs encrypted blobs from ProgramBuilder output.
///
/// This is the production-quality blob builder for use by VmRunner and examples.
/// It accepts both per-register pool entries AND native call descriptors
/// simultaneously (the test_blob_builder.hpp versions were mutually exclusive).
///
/// Backward compatibility: the old test_blob_builder.hpp functions still work;
/// they delegate to this unified implementation internally.
///
/// Implementation lives in blob_builder.cpp — none of these functions are
/// templates, so there is no reason to keep them inline in the header.

#include "native_registry.hpp"
#include "program_builder.hpp"

#include <vm/vm_blob.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_crypto.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Runtime {

// ─────────────────────────────────────────────────────────────────────────────
// Blob-level native call descriptor (for the transition table)
// ─────────────────────────────────────────────────────────────────────────────

/// Native call descriptor for blob construction.
/// Identical to TestNativeCall in test_blob_builder.hpp.
struct BlobNativeCall {
    uint32_t call_site_ip;   ///< global instruction IP of the NATIVE_CALL
    uint32_t arg_count;      ///< packed arg_count (see TransitionEntry)
    uint64_t target_addr;    ///< absolute function pointer
};

// ─────────────────────────────────────────────────────────────────────────────
// Internal encryption helpers (implemented in blob_builder.cpp)
// ─────────────────────────────────────────────────────────────────────────────

namespace blob_detail {

uint64_t update_enc_state(uint64_t enc_state,
                          uint16_t opcode_val,
                          uint32_t aux) noexcept;

void derive_bb_enc_seed(const uint8_t stored_seed[32],
                        uint32_t bb_id,
                        uint8_t out[8]) noexcept;

uint8_t find_alias_for_opcode(const uint8_t alias_lut[256],
                              uint8_t semantic_op) noexcept;

}  // namespace blob_detail

// ─────────────────────────────────────────────────────────────────────────────
// build_blob — unified builder (implemented in blob_builder.cpp)
// ─────────────────────────────────────────────────────────────────────────────

/// Build a complete encrypted blob from ProgramBuilder output.
///
/// Supports both per-register pool encoding AND native call transition entries
/// simultaneously. This is the unified builder that replaces the two mutually
/// exclusive functions in test_blob_builder.hpp.
///
/// @param stored_seed   32-byte root secret
/// @param bbs           basic blocks with plaintext instructions
/// @param pool_entries  pool entries with per-register encoding info
/// @param native_calls  native call descriptors (call_site_ip auto-computed)
/// @param debug_mode    set BLOB_FLAG_DEBUG if true
/// @return              complete encrypted blob bytes
std::vector<uint8_t> build_blob(
    const uint8_t stored_seed[32],
    const std::vector<BuilderBB>& bbs,
    const std::vector<BuilderPoolEntry>& pool_entries = {},
    const std::vector<BlobNativeCall>& native_calls = {},
    bool debug_mode = false);

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_BLOB_BUILDER_HPP__
