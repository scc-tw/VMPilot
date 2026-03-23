#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime::handlers {

/// Stub handler for Class-C bridge operations (native call bridge).
tl::expected<void, Common::DiagnosticCode>
class_c_bridge(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
