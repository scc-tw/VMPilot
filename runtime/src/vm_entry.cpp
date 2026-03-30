#include <vm_entry.hpp>
#include <encoding.hpp>

namespace VMPilot::Runtime {

using namespace Common;
using namespace Common::VM;

tl::expected<VmExecResult, DiagnosticCode>
vm_execute(const uint8_t* blob_data,
           size_t blob_size,
           const uint8_t stored_seed[32],
           int64_t load_base_delta,
           const VmSecurityConfig& config) noexcept {
    return vm_execute_with_args(blob_data, blob_size, stored_seed,
                                nullptr, 0, load_base_delta, config);
}

tl::expected<VmExecResult, DiagnosticCode>
vm_execute_with_args(const uint8_t* blob_data,
                     size_t blob_size,
                     const uint8_t stored_seed[32],
                     const uint64_t* initial_regs,
                     uint8_t num_regs,
                     int64_t load_base_delta,
                     const VmSecurityConfig& config) noexcept {

    // 1. Load and validate blob (Phase 2)
    auto loaded = load_blob(blob_data, blob_size, stored_seed, config);
    if (!loaded)
        return tl::make_unexpected(loaded.error());

    auto& vm = *loaded;
    auto& ctx = vm.ctx;

    // 1b. Set PIE/ASLR delta (D13§D3, RM§3.D3)
    ctx.load_base_delta = load_base_delta;

    // 2. Encode initial register values into the register domain (D2).
    //    The encoding tables for the first BB have already been derived
    //    by load_blob(), so encode_register() uses the correct tables.
    if (initial_regs && num_regs > 0) {
        for (uint8_t i = 0; i < num_regs && i < VM_REG_COUNT; ++i) {
            ctx.encoded_regs[i] = encode_register(ctx, i, initial_regs[i]);
        }
    }

    // 3. Execute the 12-step uniform pipeline until HALT or error (Phase 7)
    auto result = execute(ctx, config);
    if (!result)
        return tl::make_unexpected(result.error());

    // 4. Decode register 0 to plaintext and build return value.
    //    The caller receives a plaintext value, avoiding the lifetime
    //    issue of needing access to encoding tables after vm_execute returns.
    VmExecResult exec_result;
    exec_result.status = *result;
    exec_result.return_value = decode_register(ctx, 0, ctx.encoded_regs[0]);

    return exec_result;
}

}  // namespace VMPilot::Runtime
