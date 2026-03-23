#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime::handlers {

/// Stub handler for atomic opcodes (LOCK_ADD, XCHG, CMPXCHG, FENCE, ATOMIC_LOAD).
tl::expected<void, Common::DiagnosticCode>
atomic(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
