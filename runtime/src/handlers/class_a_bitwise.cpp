#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime::handlers {

/// Stub handler for Class-A bitwise operations (AND, OR, XOR, NOT, shifts, rotates).
tl::expected<void, Common::DiagnosticCode>
class_a_bitwise(Common::VM::VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
