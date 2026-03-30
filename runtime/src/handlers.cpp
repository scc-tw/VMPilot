/// @file handlers.cpp
/// @brief Handler dispatch table initialization and global composition cache.

#include <handlers.hpp>
#include <decoder.hpp>
#include <composition_cache.hpp>

namespace VMPilot::Runtime {

using Common::VM::VmOpcode;

// ---------------------------------------------------------------------------
// Global composition table cache
//
// Cleared on every BB transition (encoding tables change per-epoch).
// Accessed by Class A (bitwise) and Class B (MBA) handler files via
// extern declaration.
// ---------------------------------------------------------------------------

CompositionCache g_comp_cache;

void clear_composition_cache() noexcept {
    g_comp_cache.clear();
}

// ---------------------------------------------------------------------------
// init_handler_table: map every VmOpcode to its handler function
// ---------------------------------------------------------------------------

HandlerTable init_handler_table() noexcept {
    HandlerTable table;

    // Default: all unimplemented opcodes return NotImplemented
    for (auto& fn : table)
        fn = handlers::stub_handler;

    // Cat 0: Data Movement (8)
    table[static_cast<size_t>(VmOpcode::MOVE)]       = handlers::handle_move;
    table[static_cast<size_t>(VmOpcode::LOAD)]       = handlers::handle_load;
    table[static_cast<size_t>(VmOpcode::STORE)]      = handlers::handle_store;
    table[static_cast<size_t>(VmOpcode::PUSH)]       = handlers::handle_push;
    table[static_cast<size_t>(VmOpcode::POP)]        = handlers::handle_pop;
    table[static_cast<size_t>(VmOpcode::LOAD_CONST)] = handlers::handle_load_const;
    table[static_cast<size_t>(VmOpcode::LOAD_CTX)]   = handlers::handle_load_ctx;
    table[static_cast<size_t>(VmOpcode::STORE_CTX)]  = handlers::handle_store_ctx;

    // Cat 1: Arithmetic (8)
    table[static_cast<size_t>(VmOpcode::ADD)]  = handlers::handle_add;
    table[static_cast<size_t>(VmOpcode::SUB)]  = handlers::handle_sub;
    table[static_cast<size_t>(VmOpcode::MUL)]  = handlers::handle_mul;
    table[static_cast<size_t>(VmOpcode::IMUL)] = handlers::handle_imul;
    table[static_cast<size_t>(VmOpcode::DIV)]  = handlers::handle_div;
    table[static_cast<size_t>(VmOpcode::IDIV)] = handlers::handle_idiv;
    table[static_cast<size_t>(VmOpcode::NEG)]  = handlers::handle_neg;
    table[static_cast<size_t>(VmOpcode::MOD)]  = handlers::handle_mod;

    // Cat 2: Logic (9)
    table[static_cast<size_t>(VmOpcode::AND)] = handlers::handle_and;
    table[static_cast<size_t>(VmOpcode::OR)]  = handlers::handle_or;
    table[static_cast<size_t>(VmOpcode::XOR)] = handlers::handle_xor;
    table[static_cast<size_t>(VmOpcode::NOT)] = handlers::handle_not;
    table[static_cast<size_t>(VmOpcode::SHL)] = handlers::handle_shl;
    table[static_cast<size_t>(VmOpcode::SHR)] = handlers::handle_shr;
    table[static_cast<size_t>(VmOpcode::SAR)] = handlers::handle_sar;
    table[static_cast<size_t>(VmOpcode::ROL)] = handlers::handle_rol;
    table[static_cast<size_t>(VmOpcode::ROR)] = handlers::handle_ror;

    // Cat 3: Comparison (4)
    table[static_cast<size_t>(VmOpcode::CMP)]      = handlers::handle_cmp;
    table[static_cast<size_t>(VmOpcode::TEST)]     = handlers::handle_test;
    table[static_cast<size_t>(VmOpcode::SET_FLAG)] = handlers::handle_set_flag;
    table[static_cast<size_t>(VmOpcode::GET_FLAG)] = handlers::handle_get_flag;

    // Cat 4: Control Flow (6)
    table[static_cast<size_t>(VmOpcode::JMP)]         = handlers::handle_jmp;
    table[static_cast<size_t>(VmOpcode::JCC)]         = handlers::handle_jcc;
    table[static_cast<size_t>(VmOpcode::CALL_VM)]     = handlers::handle_call_vm;
    table[static_cast<size_t>(VmOpcode::RET_VM)]      = handlers::handle_ret_vm;
    table[static_cast<size_t>(VmOpcode::NATIVE_CALL)] = handlers::handle_native_call;
    table[static_cast<size_t>(VmOpcode::HALT)]        = handlers::handle_halt;

    // Cat 5: Width/Extension (8)
    table[static_cast<size_t>(VmOpcode::SEXT8)]   = handlers::handle_sext8;
    table[static_cast<size_t>(VmOpcode::SEXT16)]  = handlers::handle_sext16;
    table[static_cast<size_t>(VmOpcode::SEXT32)]  = handlers::handle_sext32;
    table[static_cast<size_t>(VmOpcode::ZEXT8)]   = handlers::handle_zext8;
    table[static_cast<size_t>(VmOpcode::ZEXT16)]  = handlers::handle_zext16;
    table[static_cast<size_t>(VmOpcode::ZEXT32)]  = handlers::handle_zext32;
    table[static_cast<size_t>(VmOpcode::TRUNC8)]  = handlers::handle_trunc8;
    table[static_cast<size_t>(VmOpcode::TRUNC16)] = handlers::handle_trunc16;

    // Cat 6: Atomic (5)
    table[static_cast<size_t>(VmOpcode::LOCK_ADD)]    = handlers::handle_lock_add;
    table[static_cast<size_t>(VmOpcode::XCHG)]        = handlers::handle_xchg;
    table[static_cast<size_t>(VmOpcode::CMPXCHG)]     = handlers::handle_cmpxchg;
    table[static_cast<size_t>(VmOpcode::FENCE)]       = handlers::handle_fence;
    table[static_cast<size_t>(VmOpcode::ATOMIC_LOAD)] = handlers::handle_atomic_load;

    // Cat 7: VM Internal (7)
    table[static_cast<size_t>(VmOpcode::NOP)]             = handlers::handle_nop;
    table[static_cast<size_t>(VmOpcode::CHECK_INTEGRITY)] = handlers::handle_check_integrity;
    table[static_cast<size_t>(VmOpcode::CHECK_DEBUG)]     = handlers::handle_check_debug;
    table[static_cast<size_t>(VmOpcode::MUTATE_ISA)]      = handlers::handle_mutate_isa;
    table[static_cast<size_t>(VmOpcode::REKEY)]           = handlers::handle_rekey;
    table[static_cast<size_t>(VmOpcode::SAVE_EPOCH)]      = handlers::handle_save_epoch;
    table[static_cast<size_t>(VmOpcode::RESYNC)]          = handlers::handle_resync;

    return table;
}

}  // namespace VMPilot::Runtime
