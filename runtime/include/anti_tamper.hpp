#pragma once
#ifndef __RUNTIME_ANTI_TAMPER_HPP__
#define __RUNTIME_ANTI_TAMPER_HPP__

/// @file anti_tamper.hpp
/// @brief Phase 9.2: Whole-blob integrity verification (anti-tamper).
///
/// Unlike per-BB MAC verification (verify_bb_mac, which checks individual BBs),
/// anti-tamper verifies the ENTIRE blob has not been modified since load time.
///
/// Mechanism:
///   At load time, load_blob() computes BLAKE3_keyed(integrity_key, blob)
///   and stores the 32-byte hash in VMContext::blob_integrity_hash.
///   verify_blob_integrity() recomputes the hash over the (possibly modified)
///   blob and compares in constant time.
///
/// Why BLAKE3 keyed hash:
///   An attacker who modifies the blob cannot recompute the hash without
///   knowing integrity_key, which is derived from stored_seed via
///   BLAKE3 KDF (spec S13.5). The attacker would need stored_seed
///   (register-only, never in addressable memory) to forge a valid hash.

#include <vm/vm_context.hpp>
#include <diagnostic.hpp>
#include <tl/expected.hpp>

#include <cstdint>
#include <cstddef>

namespace VMPilot::Runtime {

/// Verify the integrity of the entire bytecode blob.
///
/// Computes BLAKE3_keyed(integrity_key, blob_data) and compares against
/// the stored hash (computed at load time). This catches any modification
/// to any part of the blob (instructions, metadata, MACs, alias table,
/// config) -- not just individual BB tampering (which verify_bb_mac
/// covers per-BB).
///
/// @param blob_data   pointer to the raw blob bytes
/// @param blob_size   size of the blob
/// @param ctx         VMContext with integrity_key and blob_integrity_hash
/// @return            success or IntegrityCheckFailed
[[nodiscard]] tl::expected<void, Common::DiagnosticCode>
verify_blob_integrity(const uint8_t* blob_data,
                      size_t blob_size,
                      const Common::VM::VMContext& ctx) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_ANTI_TAMPER_HPP__
