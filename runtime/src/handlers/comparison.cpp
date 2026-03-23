#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime::handlers {

/// Stub handler for comparison opcodes (CMP, TEST, SET_FLAG, GET_FLAG).
tl::expected<void, Common::DiagnosticCode>
comparison(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
