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

/// Forward declaration (defined in vm_blob.hpp).
struct TransitionEntry;

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
    uint64_t salt;                ///< TRNG salt for epoch derivation
    uint32_t saved_insn_index;    ///< insn_index_in_bb at CALL_VM time (for resume after RET_VM)
    uint64_t encoded_regs_snapshot[16];
};

// ─────────────────────────────────────────────────────────────────────────────
// NOTE: The monolithic VMContext struct has been REMOVED.
//
// It is replaced by the 4-way state split defined in runtime/include/vm_state.hpp:
//   VmImmutable   — blob, keys, BB metadata (shared via shared_ptr)
//   VmExecution   — registers, IP, SP, flags (stack, ~2KB)
//   VmEpoch       — per-BB encoding tables (heap, ~131KB)
//   VmOramState   — ORAM workspace (heap, ~4KB)
//
// The constants, BBMetadata, and EpochCheckpoint above are shared types
// used by both the compiler (SDK) and the runtime.
// ─────────────────────────────────────────────────────────────────────────────

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_CONTEXT_HPP__
