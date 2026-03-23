#include <encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime {

uint64_t apply_byte_lane_lut(
    const uint8_t (*/*lut*/)[Common::VM::VM_BYTE_LANES][256],
    uint64_t val) noexcept {
    // Stub: will be implemented in Phase 2
    return val;
}

uint64_t encode_register(
    const Common::VM::VMContext& /*ctx*/,
    uint8_t /*reg*/,
    uint64_t plain) noexcept {
    // Stub: will be implemented in Phase 2
    return plain;
}

uint64_t decode_register(
    const Common::VM::VMContext& /*ctx*/,
    uint8_t /*reg*/,
    uint64_t encoded) noexcept {
    // Stub: will be implemented in Phase 2
    return encoded;
}

uint64_t encode_for_store(
    const Common::VM::VMContext& /*ctx*/,
    uint64_t val) noexcept {
    // Stub: will be implemented in Phase 2
    return val;
}

uint64_t decode_for_load(
    const Common::VM::VMContext& /*ctx*/,
    uint64_t val) noexcept {
    // Stub: will be implemented in Phase 2
    return val;
}

tl::expected<void, Common::DiagnosticCode>
transition_registers(Common::VM::VMContext& /*ctx*/,
                     const uint8_t (*/*old_decode*/)[Common::VM::VM_BYTE_LANES][256],
                     const uint8_t (*/*new_encode*/)[Common::VM::VM_BYTE_LANES][256],
                     uint16_t /*live_bitmap*/,
                     const Common::VM::TransitionData& /*transition*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime
