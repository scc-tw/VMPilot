#pragma once
#ifndef __RUNTIME_ENCODING_HPP__
#define __RUNTIME_ENCODING_HPP__

#include <vm/vm_context.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Apply a byte-lane substitution table to a 64-bit value.
///
/// Each of the 8 bytes in the value is independently substituted
/// using the corresponding lane's 256-byte LUT.
///
/// @param lut   pointer to VM_BYTE_LANES x 256 table
/// @param val   64-bit value to transform
/// @return      transformed 64-bit value
[[nodiscard]] uint64_t apply_byte_lane_lut(
    const uint8_t (*lut)[Common::VM::VM_BYTE_LANES][256],
    uint64_t val) noexcept;

/// Encode a plaintext register value into the register domain (D2).
///
/// @param ctx    VMContext with current reg_encode tables
/// @param reg    register index (0-15)
/// @param plain  plaintext 64-bit value
/// @return       encoded 64-bit value
[[nodiscard]] uint64_t encode_register(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t plain) noexcept;

/// Decode an encoded register value back to plaintext.
///
/// @param ctx      VMContext with current reg_decode tables
/// @param reg      register index (0-15)
/// @param encoded  encoded 64-bit value
/// @return         plaintext 64-bit value
[[nodiscard]] uint64_t decode_register(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t encoded) noexcept;

/// Apply store-domain encoding (register domain -> memory domain).
///
/// @param ctx  VMContext with current store_tables
/// @param val  register-domain 64-bit value
/// @return     memory-domain 64-bit value
[[nodiscard]] uint64_t encode_for_store(
    const Common::VM::VMContext& ctx,
    uint64_t val) noexcept;

/// Apply load-domain decoding (memory domain -> register domain).
///
/// @param ctx  VMContext with current load_tables
/// @param val  memory-domain 64-bit value
/// @return     register-domain 64-bit value
[[nodiscard]] uint64_t decode_for_load(
    const Common::VM::VMContext& ctx,
    uint64_t val) noexcept;

/// Perform BB transition: re-encode live registers for new BB tables.
///
/// @param ctx             VMContext (registers updated in place)
/// @param old_decode      decode table from previous BB
/// @param new_encode      encode table for new BB
/// @param live_bitmap     16-bit bitmap of live registers
/// @param transition      transition coefficients
/// @return                success or DiagnosticCode
[[nodiscard]] tl::expected<void, Common::DiagnosticCode>
transition_registers(Common::VM::VMContext& ctx,
                     const uint8_t (*old_decode)[Common::VM::VM_BYTE_LANES][256],
                     const uint8_t (*new_encode)[Common::VM::VM_BYTE_LANES][256],
                     uint16_t live_bitmap,
                     const Common::VM::TransitionData& transition) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_ENCODING_HPP__
