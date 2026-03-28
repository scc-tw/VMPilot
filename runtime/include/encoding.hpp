#pragma once
#ifndef __RUNTIME_ENCODING_HPP__
#define __RUNTIME_ENCODING_HPP__

#include <vm/vm_context.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Apply a byte-lane substitution table to a 64-bit value.
///
/// Each of the 8 bytes in the value is independently substituted
/// using the corresponding lane's 256-byte LUT.
///
/// @param lut   one register's worth of 8 lane tables [8][256]
/// @param val   64-bit value to transform
/// @return      transformed 64-bit value
[[nodiscard]] uint64_t apply_byte_lane_lut(
    const uint8_t lut[][256],
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
/// @param ctx              VMContext with current store_tables
/// @param reg              register index (0-15)
/// @param encoded_reg_val  register-domain 64-bit value
/// @return                 memory-domain 64-bit value
[[nodiscard]] uint64_t encode_for_store(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t encoded_reg_val) noexcept;

/// Apply load-domain decoding (memory domain -> register domain).
///
/// @param ctx              VMContext with current load_tables
/// @param reg              register index (0-15)
/// @param encoded_mem_val  memory-domain 64-bit value
/// @return                 register-domain 64-bit value
[[nodiscard]] uint64_t decode_for_load(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t encoded_mem_val) noexcept;

/// Apply RE tables to live registers at epoch change.
///
/// For each live register r (bit set in live_bitmap):
///     ctx.encoded_regs[r] = apply_byte_lane_lut(re_tables[r], ctx.encoded_regs[r])
///
/// @param ctx          VMContext (registers updated in place)
/// @param re_tables    re-encoding tables [16][8][256]
/// @param live_bitmap  16-bit bitmap of live registers
void apply_re_tables(Common::VM::VMContext& ctx,
                     const uint8_t re_tables[16][8][256],
                     uint16_t live_bitmap) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_ENCODING_HPP__
