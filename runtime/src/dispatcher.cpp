#include <dispatcher.hpp>

namespace VMPilot::Runtime {

tl::expected<VmResult, Common::DiagnosticCode>
execute(Common::VM::VMContext& /*ctx*/,
        const Common::VM::VmSecurityConfig& /*config*/) noexcept {
    // Stub: will be implemented in Phase 3
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

tl::expected<VmResult, Common::DiagnosticCode>
step(Common::VM::VMContext& /*ctx*/,
     const Common::VM::VmSecurityConfig& /*config*/) noexcept {
    // Stub: will be implemented in Phase 3
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime
