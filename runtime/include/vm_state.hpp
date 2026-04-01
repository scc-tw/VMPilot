#pragma once
#ifndef __RUNTIME_VM_STATE_HPP__
#define __RUNTIME_VM_STATE_HPP__

/// @file vm_state.hpp
/// @brief 4-way split VM state model for the doc 16 forward-secrecy architecture.
///
/// Replaces the monolithic VMContext with four focused structs:
///
///   VmImmutable   — set once at blob load, shared across reentrant calls
///   VmExecution   — hot-path mutable state (registers, IP, SP, flags, FPE key)
///   VmEpoch       — per-BB opcode permutation, trivially copyable
///   VmOramState   — ORAM workspace, cache-isolated from execution state
///
/// Design rationale:
///
///   1. Const-correctness: VmImmutable is const after loading.  Handlers
///      cannot accidentally corrupt the blob, keys, or BB metadata.
///
///   2. Cache locality: VmExecution (hot registers + IP) fits in 2-3 cache
///      lines, separate from VmOramState (4KB workspace) which would
///      otherwise evict execution state during ORAM full-scan.
///
///   3. Reentrancy: VmImmutable is shared (shared_ptr<const VmImmutable>)
///      across nested VM invocations via NATIVE_CALL.  Per-level state
///      (VmExecution on stack, VmEpoch + VmOramState on heap) keeps stack
///      usage low instead of ~138KB/level.
///
///   4. Forward secrecy (doc 16): per-register LUT tables are gone.
///      Encoding is now Speck-FPE (format-preserving encryption), keyed by
///      insn_fpe_key which ratchets every instruction.  VmEpoch holds only
///      the D4 opcode permutation — no RegTables, DomainTables, or
///      CompositionCache.  BB transitions derive new state from
///      bb_chain_state via BLAKE3, providing forward secrecy: compromising
///      the current key reveals nothing about past BB encodings.

#include <vm/encoded_value.hpp>
#include <vm/blob_view.hpp>
#include <vm/vm_context.hpp>      // BBMetadata, EpochCheckpoint, constants

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>

namespace VMPilot::Runtime {

using namespace Common::VM;

// ─────────────────────────────────────────────────────────────────────────────
// GlobalMemTables — memory domain stays LUT-based (fixed for entire execution)
// ─────────────────────────────────────────────────────────────────────────────

/// Global memory encoding tables (derived once from stored_seed at load time).
///
/// Unlike register encoding (which moved to Speck-FPE in doc 16), the memory
/// domain remains LUT-based because memory values are long-lived across BB
/// boundaries and do not benefit from per-instruction ratcheting.
struct GlobalMemTables {
    uint8_t encode[VM_BYTE_LANES][256];
    uint8_t decode[VM_BYTE_LANES][256];

    [[nodiscard]] ByteLaneLUT encode_lut() const noexcept {
        return ByteLaneLUT{encode};
    }

    [[nodiscard]] ByteLaneLUT decode_lut() const noexcept {
        return ByteLaneLUT{decode};
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// VmImmutable — read-only after blob loading, shared across reentrancy levels
// ─────────────────────────────────────────────────────────────────────────────

/// Immutable VM state: blob data, derived keys, BB metadata.
///
/// Shared via shared_ptr<const VmImmutable> across reentrant VM invocations.
/// Thread-safe because it's const after construction.
struct VmImmutable {
    /// Owned copy of the raw blob bytes.
    /// BlobView points into this buffer, so it must not be moved after
    /// blob is created.  The vector's heap allocation stays stable.
    std::vector<uint8_t> blob_storage;

    /// Type-safe view over blob_storage (validated at construction).
    BlobView blob;

    /// Global memory encoding tables (derived once from stored_seed).
    GlobalMemTables mem;

    /// Decrypted BB metadata (decrypted during load_blob).
    std::vector<BBMetadata> bb_metadata;

    /// Decrypted constant pool (blob's pool section is encrypted).
    std::vector<uint8_t> decrypted_pool;

    /// NATIVE_CALL transition entries (copied from blob).
    std::vector<TransitionEntry> native_calls;

    /// SipHash key for instruction decryption (derived from stored_seed).
    uint8_t fast_key[16];

    /// BB MAC verification key (derived from stored_seed).
    uint8_t integrity_key[32];

    /// Root secret — in production, this should live in CPU registers only.
    /// Stored here for reentrancy (shared across levels, not re-derived).
    uint8_t stored_seed[32];

    /// ORAM keystream key (derived from stored_seed).
    uint8_t oram_key[16];

    /// Anti-tamper: BLAKE3_keyed(integrity_key, entire_blob)
    uint8_t blob_integrity_hash[32];

    /// GSS alias LUT (copied from blob, 256 bytes).
    uint8_t alias_lut[256];
};

// ─────────────────────────────────────────────────────────────────────────────
// VmExecution — hot-path mutable state, stack-allocated per reentrancy level
// ─────────────────────────────────────────────────────────────────────────────

/// Mutable execution state for one VM invocation.
///
/// Stack-allocated to keep per-level stack cost low.
/// Contains registers, instruction pointer, flags, branch state,
/// and the per-instruction FPE key + BB forward-secrecy chain state.
struct alignas(64) VmExecution {
    // ── Hot data (first cache line) ──────────────────────────────────────

    /// D2: encoded registers in the current BB's register domain.
    RegVal regs[VM_REG_COUNT];                  // 128 bytes

    /// Current instruction pointer (global index into bytecodes).
    uint64_t vm_ip = 0;

    /// VM stack pointer into ORAM workspace (grows downward from top).
    uint64_t vm_sp = VM_OBLIVIOUS_SIZE;

    /// SipHash encryption chain state (within-BB, resets at BB entry).
    uint64_t enc_state = 0;

    // ── BB tracking ─────────────────────────────────────────────────────

    uint32_t current_bb_id    = 0;
    uint32_t current_bb_index = 0;  ///< index into bb_metadata[] (O(1) lookup)
    uint32_t current_epoch    = 0;
    uint32_t insn_index_in_bb = 0;  ///< for enc_state chain (j within BB)

    // ── Flags and control flow ──────────────────────────────────────────

    /// CMP/TEST flags (plaintext — 1-bit predicates, MCSP encoding gains nothing).
    uint8_t  vm_flags = 0;

    bool     halted      = false;
    bool     branch_taken = false;
    uint32_t branch_target_bb = 0;

    /// If non-zero after a branch, the dispatcher resumes execution
    /// at this vm_ip instead of the BB's entry_ip.  Set by RET_VM to
    /// resume after the CALL_VM instruction.  Reset to 0 after use.
    uint64_t return_resume_ip = 0;
    uint32_t return_resume_insn_idx = 0;

    // ── Shadow stack (CALL_VM / RET_VM) ─────────────────────────────────

    EpochCheckpoint shadow_stack[VM_MAX_NESTING];
    uint8_t shadow_depth = 0;

    // ── Ephemeral encoding nonces ───────────────────────────────────────

    uint64_t native_call_nonce = 0;  ///< monotonic, per NATIVE_CALL invocation
    uint64_t class_c_nonce     = 0;  ///< monotonic, per Class C operation

    // ── ASLR delta ──────────────────────────────────────────────────────

    int64_t load_base_delta = 0;

    // ── GSS chaff (trash register file, fake taint) ─────────────────────

    uint64_t trash_regs[VM_REG_COUNT] = {};

    // ── Doc 16 forward-secrecy state ────────────────────────────────────

    /// Current Speck-FPE key for register encoding/decoding.
    ///
    /// Doc 16 §C1: this key is ratcheted (BLAKE3 one-way derivation) after
    /// every instruction.  Because ratcheting is one-way, compromising the
    /// current key reveals nothing about past per-instruction encodings.
    /// This replaces the per-BB RegTables/DomainTables LUT approach.
    uint8_t insn_fpe_key[16] = {};

    /// BB-level forward-secrecy chain state.
    ///
    /// Doc 16 §B: on each BB transition, the new BB's encoding material
    /// (opcode_perm, initial insn_fpe_key) is derived from this chain state
    /// via BLAKE3, then the chain state itself is ratcheted.  The old chain
    /// state is securely erased, so even a full memory dump after BB N
    /// cannot reconstruct the encoding used in BB N-1.
    uint8_t bb_chain_state[32] = {};
};

// Verify VmExecution starts with regs at offset 0 for cache-line alignment.
static_assert(alignof(VmExecution) >= 64,
              "VmExecution must be 64-byte aligned for cache locality");
static_assert(offsetof(VmExecution, regs) == 0,
              "regs must be at offset 0 (first cache line)");

// ─────────────────────────────────────────────────────────────────────────────
// VmEpoch — per-BB opcode permutation, trivially copyable
// ─────────────────────────────────────────────────────────────────────────────

/// Per-BB opcode permutation and identity.
///
/// Doc 16 stripped VmEpoch down to just the D4 opcode permutation.
/// Register encoding tables (RegTables, DomainTables) and the
/// CompositionCache are gone — register encoding is now handled by
/// Speck-FPE keyed from VmExecution::insn_fpe_key, which ratchets
/// per-instruction for forward secrecy.
///
/// VmEpoch is now trivially copyable and small (~514 bytes), so it
/// can live on the stack alongside VmExecution if desired.
struct VmEpoch {
    /// D4: opcode permutation for this epoch (doc 15 §3.4).
    /// Derived from bb_chain_state on BB entry.
    uint8_t opcode_perm[256];
    uint8_t opcode_perm_inv[256];

    /// Current BB identity.
    uint32_t bb_id    = 0;
    uint32_t epoch    = 0;
    uint16_t live_regs_bitmap = 0;

    /// Derive opcode permutation from BB metadata.
    ///
    /// Doc 16: this is the ONLY derivation that remains in VmEpoch.
    /// Register encoding is handled by Speck-FPE (keyed from
    /// VmExecution::insn_fpe_key), and domain conversion is folded
    /// into the FPE encrypt/decrypt path.  The opcode permutation
    /// still uses a full 256-byte LUT because opcodes are single-byte
    /// indices — FPE on a 1-byte domain is just a permutation anyway.
    ///
    /// Note: the actual derivation call (Encoding::derive_opcode_permutation)
    /// is in the .cpp file, not inlined here, to avoid pulling in
    /// vm_encoding.hpp.
    void enter_bb(const BBMetadata& bb) noexcept;
};

// ─────────────────────────────────────────────────────────────────────────────
// VmOramState — ORAM workspace, cache-isolated, policy-parameterized
// ─────────────────────────────────────────────────────────────────────────────

/// Oblivious workspace state for PUSH/POP and register spills.
///
/// Heap-allocated (unique_ptr) and cache-line aligned.  Separate from
/// VmExecution to prevent false sharing: the ORAM full-scan pattern
/// (R+W all 64 cache lines) would otherwise evict hot execution state.
struct alignas(64) VmOramState {
    /// Rolling-keystream ORAM workspace (4KB = 64 cache lines).
    uint8_t workspace[VM_OBLIVIOUS_SIZE];

    /// Monotonic nonce for keystream derivation.  Incremented on every
    /// ORAM access (both read and write).  Never repeats within a VM
    /// invocation, preventing keystream reuse (two-time pad).
    uint64_t nonce = 0;

    /// SipHash key for ORAM keystream expansion.
    /// Copied from VmImmutable.oram_key at engine creation.
    uint8_t key[16];

    /// Initialize workspace and key from immutable state.
    void init(const VmImmutable& imm) noexcept {
        std::memset(workspace, 0, sizeof(workspace));
        nonce = 0;
        std::memcpy(key, imm.oram_key, sizeof(key));
    }
};

static_assert(alignof(VmOramState) >= 64,
              "VmOramState must be 64-byte aligned for cache-line ORAM access");
static_assert(offsetof(VmOramState, workspace) == 0,
              "workspace must be at offset 0 for aligned ORAM access");

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_STATE_HPP__
