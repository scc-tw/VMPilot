#pragma once
#ifndef __RUNTIME_VM_ENGINE_HPP__
#define __RUNTIME_VM_ENGINE_HPP__

/// @file vm_engine.hpp
/// @brief VmEngine<Policy, Oram> — the parameterized VM execution engine.
///
/// VmEngine is the central class of the redesigned VM runtime.  It replaces
/// the monolithic vm_execute() + VMContext with a template-parameterized
/// engine that provides:
///
///   - Compile-time security policy selection (D1-D4 dimensions)
///   - Orthogonal ORAM strategy (RollingKeyOram vs DirectOram)
///   - Shared VmImmutable across reentrant NATIVE_CALL invocations
///   - 4-way state split for cache locality and const-correctness
///   - Pipeline as method chain (12-step uniform dispatch, doc 15 §5.2)
///
/// Usage:
///   // First invocation (loads blob, derives keys):
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
// VmEngine<Policy, Oram>
// ─────────────────────────────────────────────────────────────────────────────

/// The parameterized VM execution engine.
///
/// Template parameters:
///   Policy — security policy traits (HighSecPolicy, StandardPolicy, DebugPolicy)
///   Oram   — ORAM access strategy (RollingKeyOram, DirectOram)
///
/// Memory layout for reentrancy:
///   VmImmutable:  shared_ptr (one copy, shared across all nesting levels)
///   VmExecution:  value member (~600 bytes, lives on stack with the engine)
///   VmEpoch:      unique_ptr (heap, ~131KB encoding tables)
///   VmOramState:  unique_ptr (heap, ~4KB ORAM workspace)
///
///   Stack per nesting level: sizeof(VmEngine) ≈ 700 bytes
///   Heap per nesting level: ~135KB
///   100 levels: 70KB stack + 13.5MB heap (vs 13.5MB stack before)
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
    /// via shared_ptr — no re-parsing or re-derivation of keys.
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
    /// Runs the 12-step uniform pipeline in a loop.  Returns when:
    ///   - HALT opcode reached → VmResult::Halted
    ///   - Error (MAC failure, invalid opcode, etc.) → DiagnosticCode
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
    // ── Constructor (private — use create() / create_reentrant()) ───────

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

    /// Hot-path mutable execution state (registers, IP, flags).
    /// Stack-allocated (value member of VmEngine).
    VmExecution exec_;

    /// Per-BB encoding tables and composition cache.
    /// Heap-allocated because encoding tables are ~131KB.
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

    // 4. Decrypt constant pool
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

    // 6. Derive global memory encoding
    derive_memory_tables(stored_seed, m->mem.encode, m->mem.decode);

    // 7. Copy alias LUT
    std::memcpy(m->alias_lut, m->blob.alias_lut(), 256);

    // 8. Blob integrity hash
    blake3_keyed_hash(m->integrity_key, blob_data, blob_size,
                      m->blob_integrity_hash, 32);

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

    // Derive epoch tables for first BB
    auto epoch = std::make_unique<VmEpoch>();
    epoch->enter_bb(first, *imm);

    // Encode initial register values into register domain
    if (initial_regs && num_regs > 0) {
        for (uint8_t i = 0; i < num_regs && i < VM_REG_COUNT; ++i) {
            exec.regs[i] = encode_register(epoch->reg.encode_lut(i),
                                            PlainVal(initial_regs[i]));
        }
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

    auto epoch = std::make_unique<VmEpoch>();
    epoch->enter_bb(first, *imm);

    if (initial_regs && num_regs > 0) {
        for (uint8_t i = 0; i < num_regs && i < VM_REG_COUNT; ++i) {
            exec.regs[i] = encode_register(epoch->reg.encode_lut(i),
                                            PlainVal(initial_regs[i]));
        }
    }

    auto oram = std::make_unique<VmOramState>();
    oram->init(*imm);

    return VmEngine(std::move(imm), std::move(exec),
                    std::move(epoch), std::move(oram));
}

// ── VmEngine::step() — 12-step uniform pipeline ────────────────────────────

template<typename Policy, typename Oram>
tl::expected<VmResult, DiagnosticCode>
VmEngine<Policy, Oram>::step() noexcept
{
    // Steps 1-3: FETCH + DECRYPT + DECODE
    auto insn_or = pipeline::fetch_decrypt_decode(*imm_, exec_, *epoch_);
    if (!insn_or) return tl::make_unexpected(insn_or.error());
    auto insn = *insn_or;

    // Step 4: RESOLVE operands (always both — D3 uniformity)
    pipeline::resolve_operands(*imm_, exec_, *epoch_, insn);

    // Steps 5-8: COMPUTE + SELECT + WRITE + MEMORY (handler dispatch)
    static const auto table = build_handler_table<Policy, Oram>();
    auto r = table[static_cast<uint8_t>(insn.opcode)](
        exec_, *epoch_, *oram_, *imm_, insn);
    if (!r) return tl::make_unexpected(r.error());

    // Step 9: UPDATE enc_state
    pipeline::advance_enc_state(exec_, insn.plaintext_opcode, insn.aux);

    // Step 10: ADVANCE IP
    if (exec_.branch_taken) {
        exec_.branch_taken = false;

        // Step 11 (early): verify MAC of BB we're leaving
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
            // Re-derive enc_state to match the resume position:
            // We need to replay the SipHash chain from entry_ip to resume_ip.
            // The enc_state was reset by enter_basic_block to bb_enc_seed.
            // We replay by re-decrypting instructions [entry..resume) to
            // advance enc_state.  This is expensive but correct (Performance Never).
            uint8_t enc_seed[8];
            const auto& bb = imm_->bb_metadata[exec_.current_bb_index];
            uint8_t msg[7];
            std::memcpy(msg, "enc", 3);
            std::memcpy(msg + 3, &bb.bb_id, 4);
            Common::VM::Crypto::blake3_keyed_hash(imm_->stored_seed, msg, 7, enc_seed, 8);
            uint64_t es = 0;
            std::memcpy(&es, enc_seed, 8);

            auto insns = imm_->blob.instructions();
            for (uint32_t j = 0; j < exec_.return_resume_insn_idx; ++j) {
                uint64_t encrypted = 0;
                std::memcpy(&encrypted, &insns[bb.entry_ip + j], 8);
                uint64_t ks = Common::VM::Crypto::siphash_keystream(
                    imm_->fast_key, es, j);
                uint64_t plain_u64 = encrypted ^ ks;
                Common::VM::VmInsn vi{};
                std::memcpy(&vi, &plain_u64, 8);

                // Check for REKEY
                uint8_t enc_alias = static_cast<uint8_t>(vi.opcode & 0xFF);
                uint8_t alias = epoch_->opcode_perm_inv[enc_alias];
                uint8_t sem = imm_->alias_lut[alias];
                if (sem == static_cast<uint8_t>(Common::VM::VmOpcode::REKEY)) {
                    uint32_t cnt = vi.aux;
                    uint8_t rk[9]; std::memcpy(rk, "rekey", 5);
                    std::memcpy(rk+5, &cnt, 4);
                    uint8_t mat[16];
                    Common::VM::Crypto::blake3_kdf(imm_->stored_seed,
                        reinterpret_cast<const char*>(rk), 9, mat, 16);
                    uint8_t esb[8]; std::memcpy(esb, &es, 8);
                    es = Common::VM::Crypto::siphash_2_4(mat, esb, 8);
                }

                uint8_t key16[16] = {};
                std::memcpy(key16, &es, 8);
                uint8_t msg6[6];
                std::memcpy(msg6, &vi.opcode, 2);
                std::memcpy(msg6+2, &vi.aux, 4);
                es = Common::VM::Crypto::siphash_2_4(key16, msg6, 6);
            }
            exec_.enc_state = es;
            exec_.return_resume_ip = 0;
            exec_.return_resume_insn_idx = 0;
        }

    } else {
        exec_.vm_ip++;

        // Check if we've reached end of current BB
        uint32_t bb_end = imm_->bb_metadata[exec_.current_bb_index].entry_ip
                        + pipeline::current_bb_insn_count(*imm_, exec_);

        if (exec_.vm_ip >= bb_end) {
            // Step 11: verify MAC at BB boundary
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

    // Step 12: GUARD (anti-debug, amortised — future Phase 9)

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
            PlainVal ret = decode_register(
                epoch_->reg.decode_lut(0), exec_.regs[0]);
            return VmExecResult{VmResult::Halted, ret.bits};
        }
    }
}

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_ENGINE_HPP__

