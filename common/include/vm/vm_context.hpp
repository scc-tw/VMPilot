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
///
/// # Why saved_insn_fpe_key is necessary and safe (doc 16)
///
/// ## 1. Confidentiality: Dead State vs Live State
///
/// Let K_t be the FPE key at time t.  The ratchet is:
///   K_{t+1} = BLAKE3_KEYED(K_t, fingerprint || insn)
///
/// (A) Dead state (past instructions):
///   Given K_call from a memory dump, recovering K_{call-1} requires inverting
///   BLAKE3 — a preimage attack with work >= 2^128.  Forward secrecy of all
///   completed instructions is preserved.
///
/// (B) Callee instructions:
///   CALL_VM triggers enter_basic_block, which derives the callee's key from
///   bb_chain_state (a separate one-way chain):
///     K_callee_entry = BLAKE3_KEYED(epoch_seed, Chain_new)
///   K_call is a "cryptographic island" — it cannot reach forward into the
///   callee's key domain, nor backward into the caller's history.
///
/// (C) The snapshot itself (live state):
///   K_call can decrypt encoded_regs_snapshot.  This is not a leak: the
///   snapshot is live state that the VM must be able to decode at RET_VM
///   time.  In a MATE (Man-At-The-End) model where the attacker has full
///   memory read, any data the algorithm needs at runtime is observable.
///   This is the theoretical lower bound of pure-software obfuscation,
///   not a design flaw.
///
/// ## 2. Why AEAD/MAC on the shadow stack is unnecessary
///
/// In a MATE model the attacker holds OS-level privileges and can dump all
/// process memory, including any shadow_stack_key.  Therefore:
///   Attacker_Memory >= { shadow_stack, shadow_stack_key }
/// The attacker can forge a valid MAC for any tampered snapshot.  Any
/// software-only MAC whose key resides in the same address space provides
/// 0 bits of tamper resistance.  VMPilot stores snapshots in FPE-encoded
/// form without a MAC — this is a deliberate, architecturally correct
/// decision: we do not waste cycles on a guarantee that cannot hold.
///
/// ## 3. Architectural necessity: FPE domain translation closure
///
/// At RET_VM, enter_basic_block evolves bb_chain_state and derives a
/// fresh key K_resume.  Without saved_insn_fpe_key:
///   1. regs[] is overwritten with encoded_regs_snapshot (in K_call domain)
///   2. Pipeline Phase L assumes regs are in K_callee_last domain
///   3. FPE_Decode(K_callee_last, snapshot) = P_garbage
///   4. FPE_Encode(K_resume, P_garbage) = corrupted registers -> crash
///
/// With saved_insn_fpe_key:
///   P_true = FPE_Decode(K_call, snapshot)
///   regs_new = FPE_Encode(K_resume, P_true)
/// The translation group closes algebraically.  After use, K_call is
/// erased via secure_zero — no cryptographic residue survives.
///
/// ## Summary
///
///   1. No forward secrecy violation: K_call is an isolated node in the
///      key ratchet DAG, severed from both past and callee chains.
///   2. Consistent with white-box limits: live state is inherently
///      observable in pure-software MATE environments.
///   3. Mathematically required: without it, the FPE domain translation
///      group is broken and RET_VM produces garbage.
struct EpochCheckpoint {
    uint64_t vm_ip;
    uint32_t bb_id;
    uint8_t  epoch_seed[32];
    uint64_t salt;                ///< TRNG salt for epoch derivation
    uint32_t saved_insn_index;    ///< insn_index_in_bb at CALL_VM time (for resume after RET_VM)
    uint64_t encoded_regs_snapshot[16];

    /// FPE key at CALL_VM time (doc 16 forward-secrecy extension).
    ///
    /// encoded_regs_snapshot is ciphertext under this key.  RET_VM must
    /// decode the snapshot with saved_insn_fpe_key before re-encoding
    /// under the current key.  Zeroed by secure_zero after use.
    uint8_t  saved_insn_fpe_key[16];
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
