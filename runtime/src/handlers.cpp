#include <handlers.hpp>
#include <decoder.hpp>

namespace VMPilot::Runtime {

// Forward declarations for handler category files
namespace handlers {
tl::expected<void, Common::DiagnosticCode>
stub_handler(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;
}  // namespace handlers

HandlerTable init_handler_table() noexcept {
    HandlerTable table;
    // Fill all entries with the stub handler
    for (auto& fn : table) {
        fn = handlers::stub_handler;
    }
    return table;
}

}  // namespace VMPilot::Runtime
