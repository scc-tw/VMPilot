#include <oram.hpp>

namespace VMPilot::Runtime {

tl::expected<uint64_t, Common::DiagnosticCode>
oram_read(Common::VM::VMContext& /*ctx*/, uint32_t /*offset*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

tl::expected<void, Common::DiagnosticCode>
oram_write(Common::VM::VMContext& /*ctx*/, uint32_t /*offset*/, uint64_t /*value*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

tl::expected<void, Common::DiagnosticCode>
oram_push(Common::VM::VMContext& /*ctx*/, uint64_t /*value*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

tl::expected<uint64_t, Common::DiagnosticCode>
oram_pop(Common::VM::VMContext& /*ctx*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime
