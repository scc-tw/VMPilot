#include <vm_entry.hpp>
#include <encoding.hpp>

namespace VMPilot::Runtime {

using namespace Common;
using namespace Common::VM;

// =========================================================================
// Struct-based API (canonical implementation)
// =========================================================================

tl::expected<VmExecResult, DiagnosticCode>
vm_execute(const VmExecRequest& req) noexcept {

    // 1. Load and validate blob (Phase 2)
    auto loaded = load_blob(req.blob_data, req.blob_size,
                            req.stored_seed, req.config);
    if (!loaded)
        return tl::make_unexpected(loaded.error());

    auto& vm = *loaded;
    auto& ctx = vm.ctx;

    // 1b. Set PIE/ASLR delta (D13§D3, RM§3.D3)
    ctx.load_base_delta = req.load_base_delta;

    // 2. Encode initial register values into the register domain (D2).
    //    The encoding tables for the first BB have already been derived
    //    by load_blob(), so encode_register() uses the correct tables.
    if (req.initial_regs && req.num_regs > 0) {
        for (uint8_t i = 0; i < req.num_regs && i < VM_REG_COUNT; ++i) {
            ctx.encoded_regs[i] = encode_register(ctx, i, req.initial_regs[i]);
        }
    }

    // 3. Execute the 12-step uniform pipeline until HALT or error (Phase 7)
    auto result = execute(ctx, req.config);
    if (!result)
        return tl::make_unexpected(result.error());

    // 4. Decode register 0 to plaintext and build return value.
    VmExecResult exec_result;
    exec_result.status = *result;
    exec_result.return_value = decode_register(ctx, 0, ctx.encoded_regs[0]);

    return exec_result;
}


}  // namespace VMPilot::Runtime
