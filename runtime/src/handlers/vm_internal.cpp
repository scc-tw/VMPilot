#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime::handlers {

/// Stub handler for VM internal opcodes (NOP, CHECK_*, MUTATE_ISA, REKEY, etc.).
/// Also provides the generic stub_handler used for unimplemented opcodes.
tl::expected<void, Common::DiagnosticCode>
vm_internal(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

tl::expected<void, Common::DiagnosticCode>
stub_handler(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
