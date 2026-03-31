#pragma once
#ifndef __RUNTIME_EPHEMERAL_ENCODING_HPP__
#define __RUNTIME_EPHEMERAL_ENCODING_HPP__

/// @file ephemeral_encoding.hpp
/// @brief Per-invocation ephemeral transition encoding for Class C ops.
///
/// Security property: each call to generate_ephemeral_tables() produces
/// a fresh random bijection LUT from BLAKE3(stored_seed, context, nonce).
/// The nonce is monotonic and never repeats, so I(LUT_i; LUT_{i+1}) = 0
/// under the PRF assumption.  See native_call_bridge.cpp header for the
/// full security proof.
///
/// Usage pattern (every Class C decode point):
///   EphemeralTables et;
///   generate_ephemeral_tables(ctx, "c3", extra, extra_len, et);
///   uint64_t plain = ephemeral_decode(ctx, reg, encoded, et);
///   // ... native compute on plain ...
///   ephemeral_zero(et);  // secure wipe

#include <vm/vm_context.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

struct EphemeralTables {
    uint8_t enc[8][256];
    uint8_t dec[8][256];
};

/// Generate per-invocation ephemeral bijection tables.
/// Advances ctx.class_c_nonce (monotonic, never repeats).
/// @param domain  domain tag for BLAKE3 context separation (e.g. "c3")
/// @param extra   additional context bytes (reg_a, reg_b, opcode, etc.)
void generate_ephemeral_tables(
    Common::VM::VMContext& ctx,
    const char* domain, size_t domain_len,
    const uint8_t* extra, size_t extra_len,
    EphemeralTables& out) noexcept;

/// Decode from register domain through ephemeral transition.
/// Equivalent to: apply_byte_lane_lut(et.enc, decode_register(ctx, reg, encoded))
/// but the plaintext exists for exactly one LUT application.
uint64_t ephemeral_decode(
    const Common::VM::VMContext& ctx,
    uint8_t reg, uint64_t encoded,
    const EphemeralTables& et) noexcept;

/// Secure-zero ephemeral tables (volatile write prevents elision).
void ephemeral_zero(EphemeralTables& et) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_EPHEMERAL_ENCODING_HPP__
