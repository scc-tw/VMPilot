#include <vm_loader.hpp>

#include <cstring>

namespace VMPilot::Runtime {

tl::expected<Common::VM::VMContext, Common::DiagnosticCode>
load_blob(const uint8_t* /*blob_data*/, size_t /*blob_size*/,
          const Common::VM::VmSecurityConfig& /*config*/) noexcept {
    // Stub: will be implemented in Phase 2
    return tl::make_unexpected(Common::DiagnosticCode::NotImplemented);
}

void init_context(Common::VM::VMContext& ctx) noexcept {
    std::memset(&ctx, 0, sizeof(ctx));
}

}  // namespace VMPilot::Runtime
