#pragma once
#ifndef __RUNTIME_VM_ENGINE_HPP__
#define __RUNTIME_VM_ENGINE_HPP__

/// @file vm_engine.hpp
/// @brief VmEngine<Policy, Oram> — doc 16 forward-secrecy execution engine.
///
/// Doc 16 architecture change (vs doc 15):
///
///   The per-BB LUT register encoding (RegTables, DomainTables, CompositionCache)
///   is replaced by Speck-FPE (format-preserving encryption) keyed from
///   VmExecution::insn_fpe_key.  The key ratchets every instruction via a
///   one-way BLAKE3 derivation, providing forward secrecy: a memory dump after
///   instruction N reveals nothing about register encodings before instruction N.
///
///   Pipeline phases per instruction (doc 16 rev.8 section 4):
///
///     A-C: Fetch + decrypt + decode + FPE-decode operands -> handler plaintext
///     D:   Handler computes result (plaintext)
///     E:   FPE-encode handler result register (if opcode writes to reg)
///     F:   Fingerprint all 16 encoded registers (BLAKE3_KEYED_128)
///     G:   Key ratchet: next_key = BLAKE3_KEYED_128(current_key, fingerprint||opcode||aux)
///     H:   Re-encode all 16 registers from current_key to next_key
///     I:   Commit next_key
///     J:   secure_zero all temporaries
///     K:   advance_enc_state (SipHash chain for instruction decryption)
///     L:   Advance IP / BB transition (including enter_basic_block)
///
/// Usage:
///   auto engine = VmEngine<HighSecPolicy>::create(blob, size, seed, delta, regs, nregs);
///   auto result = engine->execute();
///
///   // Reentrant invocation (shares immutable state):
///   auto inner = VmEngine<HighSecPolicy>::create_reentrant(engine->shared_immutable(), ...);
///   auto result2 = inner->execute();

#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"
#include "pipeline.hpp"
#include "handler_impls.hpp"

#include <vm/encoded_value.hpp>
#include <vm/blob_view.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>     // derive_memory_tables (used in create() only)
#include <vm/xex_speck64.hpp>     // FPE_Encode, FPE_Decode, Speck64_KeySchedule, XEX_ComputeTweaks
#include <vm/secure_zero.hpp>     // secure_zero, SecureLocal
#include <vm/hardware_rng.hpp>   // hardware_random_u64 (RDRAND/RNDR nonce)
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <cstring>
#include <memory>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VmSecurityConfig;
using Common::VM::RegVal;
using Common::VM::PlainVal;
using Common::VM::VmOpcode;
using Common::VM::VM_REG_COUNT;
using Common::VM::Crypto::Speck64_RoundKeys;
using Common::VM::Crypto::XEX_Tweaks;
using Common::VM::Crypto::Speck64_KeySchedule;
using Common::VM::Crypto::XEX_ComputeTweaks;
using Common::VM::Crypto::FPE_Encode;
using Common::VM::Crypto::FPE_Decode;
using Common::VM::Crypto::blake3_keyed_128;
using Common::VM::Crypto::blake3_keyed_fingerprint;
using Common::VM::secure_zero;

// ─────────────────────────────────────────────────────────────────────────────
// Result types
// ─────────────────────────────────────────────────────────────────────────────

enum class VmResult : uint8_t {
    Halted,     ///< VM_HALT opcode reached
    Stepped,    ///< Single step completed (for step-by-step debugging)
};

struct VmExecResult {
    VmResult status;
    uint64_t return_value;   ///< Decoded plaintext of register 0
};

// ─────────────────────────────────────────────────────────────────────────────
// opcode_writes_reg — doc 16 Phase E decision table
// ─────────────────────────────────────────────────────────────────────────────

/// Determine whether an opcode's handler writes a plaintext result to regs[reg_a].
///
/// Doc 16 Phase E needs to know which handlers left plaintext in regs[reg_a]
/// (needing FPE-encode) vs which left the existing FPE-encoded value untouched.
/// Encoding an already-encoded value would produce garbage, so we must
/// distinguish the two cases.
///
/// The list of non-writing opcodes is exhaustive: any new opcode that writes
/// to regs must NOT appear in the false cases.
static constexpr bool opcode_writes_reg(VmOpcode op) noexcept {
    switch (op) {
        // Control flow: these modify IP / flags / branch state, not regs.
        case VmOpcode::JMP:
        case VmOpcode::JCC:
        case VmOpcode::HALT:
        case VmOpcode::NOP:

        // Comparison: writes to vm_flags, not regs.
        case VmOpcode::CMP:
        case VmOpcode::TEST:
        case VmOpcode::SET_FLAG:

        // Memory store: writes to guest memory or ORAM, not regs.
        case VmOpcode::STORE:
        case VmOpcode::PUSH:
        case VmOpcode::STORE_CTX:

        // Synchronisation: issues a fence, no register write.
        case VmOpcode::FENCE:

        // VM internal: integrity/debug checks produce no register output.
        case VmOpcode::CHECK_INTEGRITY:
        case VmOpcode::CHECK_DEBUG:
        case VmOpcode::MUTATE_ISA:

        // REKEY: triggers key ratchet externally, handler writes no register.
        case VmOpcode::REKEY:

        // SAVE_EPOCH / RESYNC: snapshot/restore mechanics, not a reg write.
        // NOTE: RESYNC does overwrite regs[] with a saved snapshot, but those
        // values are already FPE-encoded (under the snapshot-time key).
        // Treating RESYNC as "not writing" avoids double-encoding.
        // The ratchet's Phase H will re-encode under the new key, which is
        // wrong for snapshot values -- RESYNC needs redesign for doc 16.
        case VmOpcode::SAVE_EPOCH:
        case VmOpcode::RESYNC:
            return false;

        // Everything else writes a plaintext result to regs[reg_a]:
        // MOVE, LOAD, POP, LOAD_CONST, LOAD_CTX, GET_FLAG,
        // ADD..MOD, AND..ROR, SEXT8..TRUNC16,
        // CALL_VM, RET_VM, NATIVE_CALL,
        // LOCK_ADD, XCHG, CMPXCHG, ATOMIC_LOAD
        default:
            return true;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// VmEngine<Policy, Oram>
// ─────────────────────────────────────────────────────────────────────────────

/// The parameterized VM execution engine.
///
/// Template parameters:
///   Policy -- security policy traits (HighSecPolicy, StandardPolicy, DebugPolicy)
///   Oram   -- ORAM access strategy (RollingKeyOram, DirectOram)
///
/// Memory layout for reentrancy:
///   VmImmutable:  shared_ptr (one copy, shared across all nesting levels)
///   VmExecution:  value member (~600 bytes, lives on stack with the engine)
///   VmEpoch:      unique_ptr (heap, ~514 bytes -- opcode perm only in doc 16)
///   VmOramState:  unique_ptr (heap, ~4KB ORAM workspace)
///
///   Stack per nesting level: sizeof(VmEngine) ~ 700 bytes
///   Heap per nesting level: ~5KB
///   100 levels: 70KB stack + 0.5MB heap
template<typename Policy, typename Oram = typename DefaultOramFor<Policy>::type>
class VmEngine {
    static_assert(Policy::validate(), "Invalid security policy configuration");

public:
    using policy_type = Policy;
    using oram_type   = Oram;

    // ── Factory methods ─────────────────────────────────────────────────

    /// Create a new engine from raw blob data (first invocation).
    ///
    /// Validates the blob, decrypts metadata, derives all keys, and
    /// initialises VmImmutable (shared), VmExecution, VmEpoch, VmOramState.
    ///
    /// @param blob_data       pointer to raw blob bytes (must outlive engine)
    /// @param blob_size       size of the blob in bytes
    /// @param stored_seed     32-byte root secret
    /// @param load_base_delta PIE/ASLR relocation delta
    /// @param initial_regs    initial register values (plaintext)
    /// @param num_regs        number of registers to initialise
    /// @return                ready-to-execute engine, or error
    [[nodiscard]] static tl::expected<VmEngine, DiagnosticCode>
    create(const uint8_t* blob_data, size_t blob_size,
           const uint8_t stored_seed[32],
           int64_t load_base_delta = 0,
           const uint64_t* initial_regs = nullptr,
           uint8_t num_regs = 0) noexcept;

    /// Create a reentrant engine sharing immutable state from a parent.
    ///
    /// Used by NATIVE_CALL handlers when the native function calls back
    /// into the VM.  The immutable state (blob, keys, metadata) is shared
    /// via shared_ptr -- no re-parsing or re-derivation of keys.
    ///
    /// @param imm             shared immutable state from parent engine
    /// @param load_base_delta PIE/ASLR delta (same as parent)
    /// @param initial_regs    initial register values for this invocation
    /// @param num_regs        number of registers to initialise
    /// @return                ready-to-execute engine, or error
    [[nodiscard]] static tl::expected<VmEngine, DiagnosticCode>
    create_reentrant(std::shared_ptr<const VmImmutable> imm,
                     int64_t load_base_delta = 0,
                     const uint64_t* initial_regs = nullptr,
                     uint8_t num_regs = 0) noexcept;

    // ── Execution ───────────────────────────────────────────────────────

    /// Execute until HALT or error.
    ///
    /// Runs the doc 16 Phase A-L pipeline in a loop.  Returns when:
    ///   - HALT opcode reached -> VmResult::Halted
    ///   - Error (MAC failure, invalid opcode, etc.) -> DiagnosticCode
    [[nodiscard]] tl::expected<VmExecResult, DiagnosticCode> execute() noexcept;

    /// Execute a single instruction (for debugging / testing).
    [[nodiscard]] tl::expected<VmResult, DiagnosticCode> step() noexcept;

    // ── Accessors (for testing and reentrancy) ──────────────────────────

    /// Get shared immutable state (for create_reentrant).
    [[nodiscard]] std::shared_ptr<const VmImmutable> shared_immutable() const noexcept {
        return imm_;
    }

    /// Read-only access to execution state (for testing).
    [[nodiscard]] const VmExecution& execution() const noexcept { return exec_; }

    /// Read-only access to epoch state (for testing).
    [[nodiscard]] const VmEpoch& epoch() const noexcept { return *epoch_; }

    /// Read-only access to ORAM state (for testing).
    [[nodiscard]] const VmOramState& oram() const noexcept { return *oram_; }

    /// Mutable access to execution state (for test setup).
    [[nodiscard]] VmExecution& execution() noexcept { return exec_; }

private:
    // ── Constructor (private -- use create() / create_reentrant()) ───────

    VmEngine(std::shared_ptr<const VmImmutable> imm,
             VmExecution&& exec,
             std::unique_ptr<VmEpoch> epoch,
             std::unique_ptr<VmOramState> oram) noexcept
        : imm_(std::move(imm))
        , exec_(std::move(exec))
        , epoch_(std::move(epoch))
        , oram_(std::move(oram))
    {}

    // ── State ───────────────────────────────────────────────────────────

    /// Shared immutable state (blob, keys, BB metadata).
    /// Reference-counted for reentrancy.
    std::shared_ptr<const VmImmutable> imm_;

    /// Hot-path mutable execution state (registers, IP, flags, FPE key).
    /// Stack-allocated (value member of VmEngine).
    VmExecution exec_;

    /// Per-BB opcode permutation (doc 16: no RegTables/DomainTables).
    /// Heap-allocated via unique_ptr.
    std::unique_ptr<VmEpoch> epoch_;

    /// ORAM workspace.
    /// Heap-allocated (4KB) and cache-line aligned.
    std::unique_ptr<VmOramState> oram_;
};

// ═════════════════════════════════════════════════════════════════════════════
// Template method implementations (must be in header for template classes)
// ═════════════════════════════════════════════════════════════════════════════

// ── VmEngine::create() ──────────────────────────────────────────────────────

template<typename Policy, typename Oram>
tl::expected<VmEngine<Policy, Oram>, DiagnosticCode>
VmEngine<Policy, Oram>::create(
    const uint8_t* blob_data, size_t blob_size,
    const uint8_t stored_seed[32],
    int64_t load_base_delta,
    const uint64_t* initial_regs, uint8_t num_regs) noexcept
{
    using namespace Common::VM;
    using namespace Common::VM::Crypto;
    using namespace Common::VM::Encoding;

    // 1. Copy blob data so VmImmutable owns it (BlobView points into this copy)
    auto imm = std::make_shared<VmImmutable>();
    auto* m = const_cast<VmImmutable*>(imm.get());

    m->blob_storage.assign(blob_data, blob_data + blob_size);

    // Validate blob via BlobView (pointing into owned storage)
    auto blob_or = BlobView::create(m->blob_storage.data(), m->blob_storage.size());
    if (!blob_or) return tl::make_unexpected(blob_or.error());
    m->blob = *blob_or;

    std::memcpy(m->stored_seed, stored_seed, 32);

    // 2. Key derivation
    blake3_kdf(stored_seed, "fast", 4, m->fast_key, 16);
    blake3_kdf(stored_seed, "oram", 4, m->oram_key, 16);
    blake3_kdf(stored_seed, "integrity", 9, m->integrity_key, 32);

    uint8_t meta_key[16], pool_key[16];
    blake3_kdf(stored_seed, "meta", 4, meta_key, 16);
    blake3_kdf(stored_seed, "pool", 4, pool_key, 16);

    // 3. Decrypt BB metadata
    auto raw_meta = m->blob.bb_metadata_raw();
    m->bb_metadata.resize(raw_meta.size());

    for (size_t bb = 0; bb < raw_meta.size(); ++bb) {
        uint64_t words[8];
        std::memcpy(words, &raw_meta[bb], 64);
        for (uint32_t w = 0; w < 8; ++w) {
            uint64_t nonce = static_cast<uint64_t>(bb) * 8 + w;
            uint8_t nonce_bytes[8];
            std::memcpy(nonce_bytes, &nonce, 8);
            words[w] ^= siphash_2_4(meta_key, nonce_bytes, 8);
        }
        SerializedBBMeta smeta;
        std::memcpy(&smeta, words, 64);

        auto& md = m->bb_metadata[bb];
        md.bb_id = smeta.bb_id;
        md.epoch = smeta.epoch;
        md.entry_ip = smeta.entry_ip;
        md.insn_count_in_bb = smeta.insn_count_in_bb;
        md.live_regs_bitmap = smeta.live_regs_bitmap;
        std::memcpy(md.epoch_seed, smeta.epoch_seed, 32);

        // Derive bb_enc_seed = BLAKE3_keyed(stored_seed, "enc" || bb_id)[0:8]
        uint8_t enc_msg[7];
        std::memcpy(enc_msg, "enc", 3);
        std::memcpy(enc_msg + 3, &md.bb_id, 4);
        blake3_keyed_hash(stored_seed, enc_msg, 7, md.bb_enc_seed, 8);
    }

    // 4. Decrypt constant pool (SipHash XOR -- pool content is plaintext after this)
    auto raw_pool = m->blob.constant_pool();
    m->decrypted_pool.resize(raw_pool.size() * 8);
    if (!raw_pool.empty()) {
        std::memcpy(m->decrypted_pool.data(), raw_pool.data(), raw_pool.size() * 8);
        auto* pool_words = reinterpret_cast<uint64_t*>(m->decrypted_pool.data());
        for (size_t i = 0; i < raw_pool.size(); ++i) {
            uint64_t idx = static_cast<uint64_t>(i);
            uint8_t idx_bytes[8];
            std::memcpy(idx_bytes, &idx, 8);
            pool_words[i] ^= siphash_2_4(pool_key, idx_bytes, 8);
        }
    }

    // 5. Copy native call entries
    auto raw_trans = m->blob.native_calls();
    m->native_calls.assign(raw_trans.begin(), raw_trans.end());

    // 6. Derive global memory encoding (LUT-based, unchanged from doc 15)
    derive_memory_tables(stored_seed, m->mem.encode, m->mem.decode);

    // 7. Copy alias LUT
    std::memcpy(m->alias_lut, m->blob.alias_lut(), 256);

    // 8. Blob integrity hash
    blake3_keyed_hash(m->integrity_key, blob_data, blob_size,
                      m->blob_integrity_hash, 32);

    // 9. Destroy ephemeral derivation keys (doc 16 E2).
    //
    // WHY: meta_key and pool_key were used only for metadata/pool decryption.
    // Leaving them on the stack allows a memory dump to decrypt the blob's
    // metadata and constant pool, which are meant to be confidential.
    //
    // stored_seed is NOT zeroed here — it's needed at runtime for BB MAC
    // verification (verify_bb_mac) and bb_chain_state evolution.  The stub
    // zeros the payload-section copy (E1); VmImmutable's copy persists until
    // the engine is destroyed.
    secure_zero(meta_key, sizeof(meta_key));
    secure_zero(pool_key, sizeof(pool_key));

    // ── Initialize mutable state ────────────────────────────────────────

    VmExecution exec{};
    exec.load_base_delta = load_base_delta;

    // Enter first BB
    const auto& first = m->bb_metadata[0];
    exec.current_bb_id = first.bb_id;
    exec.current_bb_index = 0;
    exec.current_epoch = first.epoch;
    exec.vm_ip = first.entry_ip;
    exec.insn_index_in_bb = 0;

    uint64_t enc_seed_u64 = 0;
    std::memcpy(&enc_seed_u64, first.bb_enc_seed, 8);
    exec.enc_state = enc_seed_u64;

    // Doc 16: opcode permutation only (no RegTables/DomainTables)
    auto epoch = std::make_unique<VmEpoch>();
    epoch->enter_bb(first);

    // Doc 16 forward-secrecy: seed bb_chain_state with hardware RNG nonce.
    //
    // WHY RDRAND NONCE:
    //   Without a per-execution nonce, two runs of the same blob+seed produce
    //   identical FPE key sequences — an attacker who records one execution's
    //   encoded register file can decode all future executions.  RDRAND provides
    //   non-deterministic diversification: same blob, same seed, different keys.
    //
    // POSITION: only the first 8 bytes are the nonce; the rest is zeroed.
    //   The BLAKE3 chain evolution in enter_basic_block will propagate the nonce
    //   into all 32 bytes after the first BB transition.
    std::memset(exec.bb_chain_state, 0, 32);
    uint64_t nonce = hardware_random_u64();
    std::memcpy(exec.bb_chain_state, &nonce, 8);

    // Derive initial insn_fpe_key from the first BB's epoch_seed and chain state.
    // key = BLAKE3_KEYED_128(epoch_seed, bb_chain_state)[0:16]
    pipeline::derive_bb_fpe_key(first, exec.bb_chain_state, exec.insn_fpe_key);

    // Encode initial register values using Speck-FPE (replaces LUT encode_register)
    if (initial_regs && num_regs > 0) {
        // Pre-compute key schedule + tweaks once for all registers.
        Speck64_RoundKeys rk;
        Speck64_KeySchedule(exec.insn_fpe_key, rk);
        XEX_Tweaks tw;
        XEX_ComputeTweaks(rk, tw);

        for (uint8_t i = 0; i < num_regs && i < VM_REG_COUNT; ++i) {
            exec.regs[i] = RegVal(FPE_Encode(rk, tw, i, initial_regs[i]));
        }

        // Encode remaining registers as FPE(0) -- deterministic "dead" value.
        for (uint8_t i = num_regs; i < VM_REG_COUNT; ++i) {
            exec.regs[i] = RegVal(FPE_Encode(rk, tw, i, 0));
        }

        secure_zero(&rk, sizeof(rk));
        secure_zero(&tw, sizeof(tw));
    } else {
        // No initial regs: encode all as FPE(0).
        Speck64_RoundKeys rk;
        Speck64_KeySchedule(exec.insn_fpe_key, rk);
        XEX_Tweaks tw;
        XEX_ComputeTweaks(rk, tw);

        for (uint8_t i = 0; i < VM_REG_COUNT; ++i) {
            exec.regs[i] = RegVal(FPE_Encode(rk, tw, i, 0));
        }

        secure_zero(&rk, sizeof(rk));
        secure_zero(&tw, sizeof(tw));
    }

    // Initialize ORAM workspace
    auto oram = std::make_unique<VmOramState>();
    oram->init(*imm);

    return VmEngine(std::move(imm), std::move(exec),
                    std::move(epoch), std::move(oram));
}

// ── VmEngine::create_reentrant() ────────────────────────────────────────────

template<typename Policy, typename Oram>
tl::expected<VmEngine<Policy, Oram>, DiagnosticCode>
VmEngine<Policy, Oram>::create_reentrant(
    std::shared_ptr<const VmImmutable> imm,
    int64_t load_base_delta,
    const uint64_t* initial_regs, uint8_t num_regs) noexcept
{
    using namespace Common::VM;
    using namespace Common::VM::Crypto;

    if (!imm || imm->bb_metadata.empty())
        return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

    VmExecution exec{};
    exec.load_base_delta = load_base_delta;

    const auto& first = imm->bb_metadata[0];
    exec.current_bb_id = first.bb_id;
    exec.current_bb_index = 0;
    exec.current_epoch = first.epoch;
    exec.vm_ip = first.entry_ip;
    exec.insn_index_in_bb = 0;

    uint64_t enc_seed_u64 = 0;
    std::memcpy(&enc_seed_u64, first.bb_enc_seed, 8);
    exec.enc_state = enc_seed_u64;

    // Doc 16: opcode permutation only
    auto epoch = std::make_unique<VmEpoch>();
    epoch->enter_bb(first);

    // Fresh chain state for this invocation level (zero for test determinism)
    std::memset(exec.bb_chain_state, 0, 32);

    // Derive initial insn_fpe_key for the first BB
    pipeline::derive_bb_fpe_key(first, exec.bb_chain_state, exec.insn_fpe_key);

    // Encode initial registers with Speck-FPE
    {
        Speck64_RoundKeys rk;
        Speck64_KeySchedule(exec.insn_fpe_key, rk);
        XEX_Tweaks tw;
        XEX_ComputeTweaks(rk, tw);

        if (initial_regs && num_regs > 0) {
            for (uint8_t i = 0; i < num_regs && i < VM_REG_COUNT; ++i) {
                exec.regs[i] = RegVal(FPE_Encode(rk, tw, i, initial_regs[i]));
            }
            for (uint8_t i = num_regs; i < VM_REG_COUNT; ++i) {
                exec.regs[i] = RegVal(FPE_Encode(rk, tw, i, 0));
            }
        } else {
            for (uint8_t i = 0; i < VM_REG_COUNT; ++i) {
                exec.regs[i] = RegVal(FPE_Encode(rk, tw, i, 0));
            }
        }

        secure_zero(&rk, sizeof(rk));
        secure_zero(&tw, sizeof(tw));
    }

    auto oram = std::make_unique<VmOramState>();
    oram->init(*imm);

    return VmEngine(std::move(imm), std::move(exec),
                    std::move(epoch), std::move(oram));
}

// ── VmEngine::step() — doc 16 Phase A-L pipeline ──────────────────────────

template<typename Policy, typename Oram>
tl::expected<VmResult, DiagnosticCode>
VmEngine<Policy, Oram>::step() noexcept
{
    using namespace Common::VM::Crypto;

    // ── Phases A-C: FETCH + DECRYPT + DECODE ────────────────────────────
    auto insn_or = pipeline::fetch_decrypt_decode(*imm_, exec_, *epoch_);
    if (!insn_or) return tl::make_unexpected(insn_or.error());
    auto insn = *insn_or;

    // ── Phase C (cont): RESOLVE operands + FPE decode ───────────────────
    // resolve_operands fills both resolved_a/b (encoded) and plain_a/b (decoded).
    // REG operands: FPE_Decode(insn_fpe_key, reg, encoded) -> plain.
    // POOL/MEM/NONE: already plaintext, copied as-is.
    pipeline::resolve_operands(*imm_, exec_, *epoch_, insn);

    // ── Phase D: HANDLER DISPATCH ───────────────────────────────────────
    // Handler operates on insn.plain_a, insn.plain_b (plaintext).
    // If the opcode writes to a register, the handler stores plaintext
    // directly into exec_.regs[insn.reg_a].  This value is "naked" in
    // memory for ~nanoseconds until Phase E re-encodes it.
    static const auto table = build_handler_table<Policy, Oram>();
    auto r = table[static_cast<uint8_t>(insn.opcode)](
        exec_, *epoch_, *oram_, *imm_, insn);
    if (!r) return tl::make_unexpected(r.error());

    // Zero plaintext operands immediately after handler (Theorem 7.1).
    // These existed in CPU registers during handler execution; zeroing
    // prevents stack residue if the compiler spilled them.
    secure_zero(&insn.plain_a, sizeof(insn.plain_a));
    secure_zero(&insn.plain_b, sizeof(insn.plain_b));

    // ── Phases E-I: FPE encode + fingerprint + key ratchet ──────────────
    //
    // This is the core forward-secrecy pipeline from doc 16 rev.8 section 4.
    // All cryptographic temporaries are SecureLocal (RAII zeroed on scope exit)
    // or explicitly zeroed in Phase J.
    {
        // Pre-compute Speck key schedule + XEX tweaks for the CURRENT key.
        SecureLocal<Speck64_RoundKeys> rk;
        Speck64_KeySchedule(exec_.insn_fpe_key, rk.val);
        SecureLocal<XEX_Tweaks> tw;
        XEX_ComputeTweaks(rk.val, tw.val);

        // ── Phase E: FPE-encode handler result ──────────────────────────
        // If the handler wrote plaintext to regs[reg_a], encode it back into
        // the FPE domain under the current key.  For non-writing opcodes
        // (JMP, HALT, CMP, etc.), regs[reg_a] still holds its old encoded
        // value -- encoding it again would produce garbage.
        if (opcode_writes_reg(insn.opcode)) {
            exec_.regs[insn.reg_a] = RegVal(
                FPE_Encode(rk.val, tw.val, insn.reg_a,
                           exec_.regs[insn.reg_a].bits));
        }
        // All 16 registers are now FPE-encoded under the current key.

        // ── Phase F: Fingerprint all 16 encoded registers ───────────────
        // The fingerprint entangles ALL register state into the key ratchet.
        // Modifying any register (even one not touched by this instruction)
        // changes the fingerprint, which changes the next key, which corrupts
        // all subsequent decryptions.  This is doc 16's "entanglement" property.
        SecureLocal<uint8_t[16]> fingerprint;
        {
            uint64_t reg_bits[16];
            for (uint8_t i = 0; i < 16; ++i)
                reg_bits[i] = exec_.regs[i].bits;
            blake3_keyed_fingerprint(exec_.insn_fpe_key, reg_bits, fingerprint.val);
            secure_zero(reg_bits, sizeof(reg_bits));
        }

        // ── Phase G: Key ratchet ────────────────────────────────────────
        // next_key = BLAKE3_KEYED_128(current_key, fingerprint || opcode || aux)
        //
        // The ratchet message includes:
        //   - fingerprint (16 bytes): binds next key to ALL register state
        //   - opcode (2 bytes): binds next key to the instruction executed
        //   - aux (4 bytes): binds next key to the instruction's immediate
        //
        // One-way property of BLAKE3 means current_key cannot be recovered
        // from next_key -- this is the forward-secrecy guarantee.
        SecureLocal<uint8_t[16]> next_key;
        {
            uint8_t ratchet_msg[22];  // 16 + 2 + 4
            std::memcpy(ratchet_msg, fingerprint.val, 16);
            std::memcpy(ratchet_msg + 16, &insn.plaintext_opcode, 2);
            std::memcpy(ratchet_msg + 18, &insn.aux, 4);
            blake3_keyed_128(exec_.insn_fpe_key, ratchet_msg, 22,
                             next_key.val, 16);
            secure_zero(ratchet_msg, sizeof(ratchet_msg));
        }

        // ── Phase H: Re-encode all 16 registers from old key to new key ─
        // Decode each register under the current key, re-encode under the
        // next key.  After this, all registers are in the new key's domain.
        {
            SecureLocal<Speck64_RoundKeys> new_rk;
            Speck64_KeySchedule(next_key.val, new_rk.val);
            SecureLocal<XEX_Tweaks> new_tw;
            XEX_ComputeTweaks(new_rk.val, new_tw.val);

            for (uint8_t i = 0; i < 16; ++i) {
                SecureLocal<uint64_t> plain;
                plain.val = FPE_Decode(rk.val, tw.val, i,
                                       exec_.regs[i].bits);
                exec_.regs[i] = RegVal(
                    FPE_Encode(new_rk.val, new_tw.val, i, plain.val));
                // plain auto-zeroed by SecureLocal destructor (Theorem 7.1)
            }
            // new_rk, new_tw zeroed by SecureLocal destructor.
        }

        // ── Phase I: Commit new key ─────────────────────────────────────
        std::memcpy(exec_.insn_fpe_key, next_key.val, 16);

        // ── Phase J: secure_zero temporaries ────────────────────────────
        // SecureLocal destructors handle rk, tw, fingerprint, next_key.
        // Explicit zeros handled inline above for ratchet_msg, reg_bits.
    }
    // All SecureLocal destructors fire here -- rk, tw, fingerprint, next_key
    // are zeroed.  No FPE temporaries survive past this scope.

    // ── Phase K: advance enc_state (SipHash chain for instruction decryption) ─
    pipeline::advance_enc_state(exec_, insn.plaintext_opcode, insn.aux);

    // ── Phase L: Advance IP / BB transition ─────────────────────────────
    if (exec_.branch_taken) {
        exec_.branch_taken = false;

        // Verify MAC of the BB we're leaving
        auto mac_r = pipeline::verify_bb_mac(*imm_, exec_, *epoch_);
        if (!mac_r) return tl::make_unexpected(mac_r.error());

        auto enter_r = pipeline::enter_basic_block(
            exec_, *epoch_, *imm_, exec_.branch_target_bb);
        if (!enter_r) return tl::make_unexpected(enter_r.error());

        // If RET_VM set a resume point, override enter_basic_block's
        // default vm_ip = entry_ip.  This resumes after the CALL_VM.
        if (exec_.return_resume_ip != 0) {
            exec_.vm_ip = exec_.return_resume_ip;
            exec_.insn_index_in_bb = exec_.return_resume_insn_idx;
            pipeline::replay_enc_state(exec_, *epoch_, *imm_,
                                       exec_.return_resume_insn_idx);
            exec_.return_resume_ip = 0;
            exec_.return_resume_insn_idx = 0;
        }

    } else {
        exec_.vm_ip++;

        // Check if we've reached end of current BB
        uint32_t bb_end = imm_->bb_metadata[exec_.current_bb_index].entry_ip
                        + pipeline::current_bb_insn_count(*imm_, exec_);

        if (exec_.vm_ip >= bb_end) {
            // Verify MAC at BB boundary
            auto mac_r = pipeline::verify_bb_mac(*imm_, exec_, *epoch_);
            if (!mac_r) return tl::make_unexpected(mac_r.error());

            // Fallthrough to next sequential BB
            if (exec_.current_bb_index + 1 < imm_->bb_metadata.size()) {
                uint32_t next_id = imm_->bb_metadata[exec_.current_bb_index + 1].bb_id;
                auto enter_r = pipeline::enter_basic_block(
                    exec_, *epoch_, *imm_, next_id);
                if (!enter_r) return tl::make_unexpected(enter_r.error());
            }
        }
    }

    return exec_.halted ? VmResult::Halted : VmResult::Stepped;
}

// ── VmEngine::execute() — main loop ────────────────────────────────────────

template<typename Policy, typename Oram>
tl::expected<VmExecResult, DiagnosticCode>
VmEngine<Policy, Oram>::execute() noexcept
{
    while (true) {
        auto r = step();
        if (!r) return tl::make_unexpected(r.error());
        if (*r == VmResult::Halted) {
            // Decode return value from register 0 using current FPE key.
            // After the last instruction's ratchet, regs[0] is encoded
            // under exec_.insn_fpe_key.  Convenience one-shot FPE_Decode.
            uint64_t ret = FPE_Decode(exec_.insn_fpe_key, 0,
                                      exec_.regs[0].bits);
            return VmExecResult{VmResult::Halted, ret};
        }
    }
}

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_ENGINE_HPP__
