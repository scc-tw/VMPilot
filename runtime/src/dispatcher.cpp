/// @file dispatcher.cpp
/// @brief Phase 7: 12-Step Uniform Pipeline dispatcher (ISA Design v1, Section 5.2).
///
/// The dispatch loop executes every step unconditionally to ensure
/// constant-time execution regardless of instruction type (D3: SNR->0).
/// Steps 5-8 (COMPUTE, SELECT, WRITE, MEMORY) are integrated into
/// the handler functions; the dispatcher handles steps 1-4 and 9-12.

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <handlers.hpp>
#include <encoding.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>

#include <cstring>

namespace VMPilot::Runtime {

using namespace Common;
using namespace Common::VM;

// Static handler table -- initialized once, reused across all execute() calls.
static HandlerTable g_handler_table = init_handler_table();

/// Resolve operand value based on operand type.
///
/// Uniform pipeline step 4 (READ): always resolve both operands,
/// even if the handler only uses one. This ensures constant-time
/// execution regardless of instruction type (D3: SNR->0, spec S11.3).
static uint64_t resolve_operand(const VMContext& ctx,
                                uint8_t operand_type,
                                uint8_t reg_idx,
                                uint32_t aux) noexcept {
    switch (operand_type) {
        case VM_OPERAND_REG:
            return ctx.encoded_regs[reg_idx & 0x0F];
        case VM_OPERAND_POOL: {
            uint64_t val = 0;
            if (aux < ctx.pool_size)
                std::memcpy(&val, ctx.constant_pool + aux * 8, 8);
            return val;
        }
        case VM_OPERAND_MEM: {
            // Guest external memory read (Space 2, direct access)
            // v1: aux is treated as an offset/address
            // For safety in v1, return 0 if address is null-ish
            uint64_t val = 0;
            // NOTE: In production, this would read from the guest's memory space.
            // For v1 testing, we don't dereference arbitrary addresses.
            return val;
        }
        case VM_OPERAND_NONE:
        default:
            return 0;
    }
}

// -- step() -- single instruction execution ------------------------------------

tl::expected<VmResult, DiagnosticCode>
step(VMContext& ctx, const VmSecurityConfig& config) noexcept {

    // =====================================================================
    // Steps 1-3: FETCH + DECRYPT + DECODE
    // =====================================================================
    auto decoded = fetch_decrypt_decode(ctx);
    if (!decoded)
        return tl::make_unexpected(decoded.error());

    // =====================================================================
    // Step 4: READ -- resolve both operands (always both, uniform pipeline)
    //
    // Why always both: D3 requires constant-time execution. Reading only
    // one operand when the instruction is unary would create a timing
    // difference observable via cache side channels. Reading both
    // regardless ensures I(type; cache_trace) ~ 0.
    // =====================================================================
    (void)resolve_operand(ctx, decoded->operand_a_type, decoded->reg_a, decoded->aux);
    (void)resolve_operand(ctx, decoded->operand_b_type, decoded->reg_b, decoded->aux);

    // =====================================================================
    // Steps 5-8: COMPUTE + SELECT + WRITE + MEMORY
    //
    // All integrated into the handler function. The handler performs the
    // semantic operation on encoded values and writes the result.
    // =====================================================================
    auto handler = g_handler_table[static_cast<size_t>(decoded->opcode)];
    auto result = handler(ctx, *decoded);
    if (!result)
        return tl::make_unexpected(result.error());

    // =====================================================================
    // Step 9: UPDATE -- advance enc_state (within-BB SipHash chain)
    //
    // Why this must happen for EVERY instruction including chaff:
    // The enc_state chain is a one-way hash (SipHash, 128-bit PRF).
    // Skipping an instruction would desync the chain, causing all
    // subsequent decryptions to fail. This is the mechanism that makes
    // chaff instructions undeletable (spec S4.3): an attacker who
    // removes a chaff instruction breaks the preimage-resistant chain.
    // =====================================================================
    advance_enc_state(ctx, decoded->plaintext_opcode, decoded->aux);

    // =====================================================================
    // Step 10: ADVANCE -- next instruction pointer
    //
    // If a control flow handler set branch_taken, the dispatcher
    // transitions to the target BB. Otherwise, advance sequentially.
    // =====================================================================
    if (ctx.branch_taken) {
        ctx.branch_taken = false;

        // Step 11 (early): CHECK -- verify MAC of the BB we're leaving
        // BB MAC covers all instructions including chaff. Verifying at
        // BB exit ensures no instruction was tampered during execution.
        auto mac_check = verify_bb_mac(ctx);
        if (!mac_check)
            return tl::make_unexpected(mac_check.error());

        // Enter the target BB (derives encoding tables, resets enc_state)
        auto enter = enter_basic_block(ctx, ctx.branch_target_bb);
        if (!enter)
            return tl::make_unexpected(enter.error());

    } else {
        ctx.vm_ip++;

        // Check if we've reached the end of the current BB
        const BBMetadata& cur_bb = ctx.bb_metadata[ctx.current_bb_index];
        uint32_t bb_end_ip = cur_bb.entry_ip + cur_bb.insn_count_in_bb;

        if (ctx.vm_ip >= bb_end_ip) {
            // Step 11: CHECK -- verify MAC at BB boundary
            auto mac_check = verify_bb_mac(ctx);
            if (!mac_check)
                return tl::make_unexpected(mac_check.error());

            // Advance to next sequential BB (fallthrough)
            if (ctx.current_bb_index + 1 < ctx.bb_count) {
                uint32_t next_bb_id = ctx.bb_metadata[ctx.current_bb_index + 1].bb_id;
                auto enter = enter_basic_block(ctx, next_bb_id);
                if (!enter)
                    return tl::make_unexpected(enter.error());
            }
            // else: past last BB without HALT -- will fail on next fetch
        }
    }

    // =====================================================================
    // Step 12: GUARD -- anti-instrumentation check (amortized)
    //
    // Only runs every anti_debug_interval instructions to reduce overhead.
    // Phase 9 will implement the actual check (ptrace/P_TRACED/etc).
    // =====================================================================
    if (!config.debug_mode && config.anti_debug_interval > 0) {
        if (ctx.vm_ip % config.anti_debug_interval == 0) {
            // Phase 9 stub: anti-debug check would go here
        }
    }

    // Return result
    if (ctx.halted)
        return VmResult::Halted;

    return VmResult::Running;
}

// -- execute() -- main loop ---------------------------------------------------

tl::expected<VmResult, DiagnosticCode>
execute(VMContext& ctx, const VmSecurityConfig& config) noexcept {
    while (true) {
        auto result = step(ctx, config);
        if (!result)
            return tl::make_unexpected(result.error());
        if (*result == VmResult::Halted)
            return VmResult::Halted;
    }
}

}  // namespace VMPilot::Runtime
