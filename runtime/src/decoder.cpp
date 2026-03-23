#include <decoder.hpp>

namespace VMPilot::Runtime {

tl::expected<DecodedInsn, Common::DiagnosticCode>
fetch_decrypt_decode(Common::VM::VMContext& /*ctx*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

void advance_enc_state(Common::VM::VMContext& /*ctx*/) noexcept {
    // Stub: will be implemented in Phase 2
}

}  // namespace VMPilot::Runtime
