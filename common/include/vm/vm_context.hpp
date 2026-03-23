#pragma once
#ifndef __COMMON_VM_CONTEXT_HPP__
#define __COMMON_VM_CONTEXT_HPP__

#include <cstdint>
#include <cstddef>

namespace VMPilot::Common::VM {

/// Oblivious workspace size in bytes (Rolling Keystream ORAM).
constexpr uint32_t VM_OBLIVIOUS_SIZE = 4096;

/// ORAM cache line size.
constexpr uint32_t VM_ORAM_LINE_SIZE = 64;

/// Number of ORAM cache lines.
constexpr uint32_t VM_ORAM_NUM_LINES = VM_OBLIVIOUS_SIZE / VM_ORAM_LINE_SIZE;  // 64

/// Number of virtual registers.
constexpr uint8_t VM_REG_COUNT = 16;

/// Number of byte lanes for encoding tables.
constexpr uint8_t VM_BYTE_LANES = 8;

/// Maximum nesting depth for shadow stack.
constexpr uint8_t VM_MAX_NESTING = 8;

/// Forward declaration for platform-specific native context.
struct NativeContext;

/// Coefficients for BB transition encoding (D2 domain).
struct TransitionData {
    uint64_t a;  ///< linear coefficient
    uint64_t b;  ///< constant offset
};

/// Per-BB metadata from the blob.
struct BBMetadata {
    uint32_t bb_id;
    uint32_t epoch;
    uint32_t entry_ip;
    uint32_t insn_count_in_bb;
    uint16_t live_regs_bitmap;
    uint8_t  bb_enc_seed[8];
    uint8_t  epoch_seed[32];
};

/// Epoch checkpoint for shadow stack (CALL_VM / RET_VM).
struct EpochCheckpoint {
    uint64_t vm_ip;
    uint32_t bb_id;
    uint8_t  epoch_seed[32];
    uint64_t salt;
    uint64_t encoded_regs_snapshot[16];
};

/// Full VM state model (ISA Design v1, Section 8).
struct VMContext {
    // D2: encoded registers (register domain, per-BB encoding)
    uint64_t encoded_regs[VM_REG_COUNT];

    // D1: per-BB encryption state
    uint64_t enc_state;
    uint8_t  stored_seed[32];
    uint8_t  fast_key[16];

    // Execution
    uint64_t vm_ip;
    uint64_t vm_sp;
    uint32_t current_bb_id;
    uint32_t current_epoch;
    uint32_t insn_index_in_bb;  ///< for enc_state chain (j within BB)

    // D2+D3: VM Internal Oblivious Workspace (Rolling Keystream ORAM)
    alignas(64) uint8_t oblivious_workspace[VM_OBLIVIOUS_SIZE];
    uint64_t oram_nonce;
    uint8_t  oram_key[16];

    // Bytecode + constant pool (encrypted)
    const uint8_t* bytecodes;
    uint32_t insn_count;
    const uint8_t* constant_pool;
    uint32_t pool_size;

    // Integrity
    const uint8_t* bb_macs;
    uint8_t integrity_key[32];

    // D2: register encoding tables (per-BB, liveness-aware)
    // Pointers into blob's table area -- NOT owned by VMContext
    const uint8_t (*reg_encode)[VM_BYTE_LANES][256];
    const uint8_t (*reg_decode)[VM_BYTE_LANES][256];

    // D2: memory domain conversion tables (per-BB)
    const uint8_t (*store_tables)[VM_BYTE_LANES][256];
    const uint8_t (*load_tables)[VM_BYTE_LANES][256];

    // D4: opcode permutation (per-epoch)
    uint8_t opcode_perm[256];
    uint8_t opcode_perm_inv[256];

    // GSS: 8-bit alias (256 bytes)
    uint8_t alias_lut[256];

    // Native context
    NativeContext* native_ctx;
    const TransitionData* transition_table;

    // Exception: shadow stack
    EpochCheckpoint shadow_stack[VM_MAX_NESTING];
    uint8_t shadow_depth;

    // GSS: trash registers
    uint64_t trash_regs[VM_REG_COUNT];

    // BB metadata
    const BBMetadata* bb_metadata;
    uint32_t bb_count;

    // Flags set by handlers
    bool halted;
    uint32_t branch_target_bb;  ///< set by JMP/JCC/CALL, read by dispatcher
    bool branch_taken;
};

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_CONTEXT_HPP__
