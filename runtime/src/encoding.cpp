#include <encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime {

uint64_t apply_byte_lane_lut(
    const uint8_t lut[][256],
    uint64_t val) noexcept {
    uint64_t result = 0;
    for (int k = 0; k < 8; ++k) {
        uint8_t lane = static_cast<uint8_t>(val >> (k * 8));
        result |= static_cast<uint64_t>(lut[k][lane]) << (k * 8);
    }
    return result;
}

uint64_t encode_register(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t plain) noexcept {
    return apply_byte_lane_lut(ctx.reg_encode[reg], plain);
}

uint64_t decode_register(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t encoded) noexcept {
    return apply_byte_lane_lut(ctx.reg_decode[reg], encoded);
}

uint64_t encode_for_store(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t encoded_reg_val) noexcept {
    return apply_byte_lane_lut(ctx.store_tables[reg], encoded_reg_val);
}

uint64_t decode_for_load(
    const Common::VM::VMContext& ctx,
    uint8_t reg,
    uint64_t encoded_mem_val) noexcept {
    return apply_byte_lane_lut(ctx.load_tables[reg], encoded_mem_val);
}

void apply_re_tables(Common::VM::VMContext& ctx,
                     const uint8_t re_tables[16][8][256],
                     uint16_t live_bitmap) noexcept {
    for (int r = 0; r < Common::VM::VM_REG_COUNT; ++r) {
        if (live_bitmap & (1u << r)) {
            ctx.encoded_regs[r] = apply_byte_lane_lut(re_tables[r],
                                                       ctx.encoded_regs[r]);
        }
    }
}

}  // namespace VMPilot::Runtime
