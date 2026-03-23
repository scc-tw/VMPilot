#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime::handlers {

/// Stub handler for control flow opcodes (JMP, JCC, CALL_VM, RET_VM, NATIVE_CALL, HALT).
tl::expected<void, Common::DiagnosticCode>
control_flow(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
