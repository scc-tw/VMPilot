#pragma once
#ifndef __RUNTIME_VM_STATE_HPP__
#define __RUNTIME_VM_STATE_HPP__

/// @file vm_state.hpp
/// @brief 4-way split VM state model for the redesigned architecture.
///
/// Replaces the monolithic VMContext with four focused structs:
///
///   VmImmutable   — set once at blob load, shared across reentrant calls
///   VmExecution   — hot-path mutable state (registers, IP, SP, flags)
///   VmEpoch       — per-BB derived tables, rebuilt on every BB transition
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
///      usage to ~600 bytes/level instead of ~138KB/level.
///
///   4. Policy parameterization: VmOramState is accessed through the Oram
///      template parameter, enabling compile-time selection between
///      RollingKeyOram (full security) and DirectOram (fast testing).

#include <vm/encoded_value.hpp>
#include <vm/blob_view.hpp>
#include <vm/vm_context.hpp>      // BBMetadata, EpochCheckpoint, constants
#include <vm/vm_encoding.hpp>

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

namespace VMPilot::Runtime {

using namespace Common::VM;

// ─────────────────────────────────────────────────────────────────────────────
// Table type aliases (non-owning views are ByteLaneLUT; these are storage)
// ─────────────────────────────────────────────────────────────────────────────

/// Per-register encoding/decoding tables: 16 registers × 8 lanes × 256 entries.
struct RegTables {
    uint8_t encode[VM_REG_COUNT][VM_BYTE_LANES][256];
    uint8_t decode[VM_REG_COUNT][VM_BYTE_LANES][256];

    /// Get a ByteLaneLUT view for encoding register `reg`.
    [[nodiscard]] ByteLaneLUT encode_lut(uint8_t reg) const noexcept {
        return ByteLaneLUT{encode[reg]};
    }

    /// Get a ByteLaneLUT view for decoding register `reg`.
    [[nodiscard]] ByteLaneLUT decode_lut(uint8_t reg) const noexcept {
        return ByteLaneLUT{decode[reg]};
    }
};

/// Domain conversion tables: register ↔ memory encoding.
struct DomainTables {
    uint8_t store[VM_REG_COUNT][VM_BYTE_LANES][256];  // reg→mem
    uint8_t load[VM_REG_COUNT][VM_BYTE_LANES][256];   // mem→reg

    [[nodiscard]] ByteLaneLUT store_lut(uint8_t reg) const noexcept {
        return ByteLaneLUT{store[reg]};
    }

    [[nodiscard]] ByteLaneLUT load_lut(uint8_t reg) const noexcept {
        return ByteLaneLUT{load[reg]};
    }
};

/// Global memory encoding tables (fixed for entire execution).
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
/// Stack-allocated (~600 bytes) to keep per-level stack cost low.
/// Contains registers, instruction pointer, flags, and branch state.
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
};

// Verify VmExecution starts with regs at offset 0 for cache-line alignment.
static_assert(alignof(VmExecution) >= 64,
              "VmExecution must be 64-byte aligned for cache locality");
static_assert(offsetof(VmExecution, regs) == 0,
              "regs must be at offset 0 (first cache line)");

// ─────────────────────────────────────────────────────────────────────────────
// VmEpoch — per-BB derived state, heap-allocated, rebuilt on BB transitions
// ─────────────────────────────────────────────────────────────────────────────

// Forward declaration — full definition in composition_cache.hpp.
// CompositionCache is ~8MB worst case (16 × 512KB binary entries +
// 8 × 2KB unary + 8 × 8KB MBA).  It lives behind a unique_ptr in
// VmEpoch so that (a) VmEpoch itself stays ~131KB and (b) cache memory
// is only allocated when handlers first need it.
struct CompositionCache;

/// Per-BB encoding tables and composition cache.
///
/// Heap-allocated (unique_ptr from VmEngine) because encoding tables are
/// ~131KB total.  Rebuilt on every BB transition via enter_bb().
struct VmEpoch {
    VmEpoch() noexcept;
    ~VmEpoch();
    VmEpoch(VmEpoch&&) noexcept;
    VmEpoch& operator=(VmEpoch&&) noexcept;

    // Non-copyable (owns unique_ptr)
    VmEpoch(const VmEpoch&) = delete;
    VmEpoch& operator=(const VmEpoch&) = delete;

    /// Per-register encoding/decoding bijections.
    RegTables reg;

    /// Register↔Memory domain conversion tables.
    DomainTables dom;

    /// D4: opcode permutation for this epoch.
    uint8_t opcode_perm[256];
    uint8_t opcode_perm_inv[256];

    /// Current BB identity.
    uint32_t bb_id    = 0;
    uint32_t epoch    = 0;
    uint16_t live_regs_bitmap = 0;

    /// Lazily-built composition tables for Class A (bitwise) and Class B
    /// (MBA) operations.  Allocated on first handler use, cleared on every
    /// BB transition.  nullptr until first Class A/B handler executes.
    ///
    /// Owned by VmEpoch but heap-allocated separately because the cache
    /// can grow to ~8MB while VmEpoch itself is ~131KB.
    std::unique_ptr<CompositionCache> cache;

    /// Derive all tables from BB metadata + immutable state.
    ///
    /// Called on every BB transition.  If epoch changes from the previous BB,
    /// the caller must first call transition_regs() with the OLD tables
    /// before this method overwrites them.
    void enter_bb(const BBMetadata& bb, const VmImmutable& imm) noexcept {
        bb_id = bb.bb_id;
        epoch = bb.epoch;
        live_regs_bitmap = bb.live_regs_bitmap;

        // Derive per-register encoding tables from epoch_seed
        Encoding::derive_register_tables(
            bb.epoch_seed, bb.live_regs_bitmap,
            reg.encode, reg.decode);

        // Derive store/load domain conversion tables
        Encoding::derive_store_load_tables(
            reg.encode, reg.decode,
            imm.mem.encode, imm.mem.decode,
            bb.live_regs_bitmap,
            dom.store, dom.load);

        // Derive opcode permutation for this epoch
        Encoding::derive_opcode_permutation(
            bb.epoch_seed, opcode_perm, opcode_perm_inv);
    }

    /// Apply RE_TABLE to live registers on epoch change.
    ///
    /// Must be called BEFORE enter_bb() overwrites the old tables.
    /// RE_TABLE[r] = encode_new ∘ decode_old — no plaintext intermediate.
    void transition_regs(VmExecution& exec,
                         const BBMetadata& new_bb,
                         const VmImmutable& /*imm*/) noexcept {
        // Derive the NEW encoding tables into a temporary
        uint8_t new_encode[VM_REG_COUNT][VM_BYTE_LANES][256];
        uint8_t new_decode[VM_REG_COUNT][VM_BYTE_LANES][256];
        Encoding::derive_register_tables(
            new_bb.epoch_seed, new_bb.live_regs_bitmap,
            new_encode, new_decode);

        // Derive RE_TABLE = new_encode ∘ old_decode
        uint8_t re_tables[VM_REG_COUNT][VM_BYTE_LANES][256];
        Encoding::derive_re_tables(
            reg.decode, new_encode,
            new_bb.live_regs_bitmap, re_tables);

        // Apply RE_TABLE to each live register (no plaintext intermediate)
        for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
            if (new_bb.live_regs_bitmap & (1u << r)) {
                ByteLaneLUT re_lut{re_tables[r]};
                exec.regs[r] = reencode(re_lut, exec.regs[r]);
            }
        }
    }
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
