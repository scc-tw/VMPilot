/// @file vm_engine.cpp
/// @brief VmEngine template method implementations + explicit instantiations.
///
/// Extracted from vm_engine.hpp.  The template methods (create, create_reentrant,
/// step, execute) are large enough to warrant out-of-line definitions for
/// debuggability.  Explicit instantiations for all known (Policy, Oram) combos
/// ensure link-time availability without exposing implementation in the header.

#include "vm_engine.hpp"

#include <vm/vm_encoding.hpp>
#include <vm/hardware_rng.hpp>
#include <vm/secure_zero.hpp>

#include <cstring>

namespace VMPilot::Runtime {

using namespace Common::VM;
using namespace Common::VM::Crypto;

// ═════════════════════════════════════════════════════════════════════════════
// VmEngine::create()
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmEngine<Policy, Oram>, DiagnosticCode>
VmEngine<Policy, Oram>::create(
    const uint8_t* blob_data, size_t blob_size,
    const uint8_t stored_seed[32],
    int64_t load_base_delta,
    const uint64_t* initial_regs, uint8_t num_regs) noexcept
{
    using namespace Encoding;

    // 1. Copy blob data so VmImmutable owns it (BlobView points into this copy)
    auto imm = std::make_shared<VmImmutable>();
    auto* m = const_cast<VmImmutable*>(imm.get());

    m->blob_storage.assign(blob_data, blob_data + blob_size);

    // Validate blob via BlobView (pointing into owned storage)
    auto blob_or = BlobView::create(m->blob_storage.data(), m->blob_storage.size());
    if (!blob_or) return tl::make_unexpected(blob_or.error());
    m->blob = *blob_or;

    // 2. Key derivation (doc 16 §6: all via BLAKE3_KEYED, not blake3_kdf)
    //
    // stored_seed (32 bytes) is the BLAKE3 key.  Context strings are the
    // message, producing domain-separated sub-keys.  After all derivations
    // complete, stored_seed is zeroed (doc 16 §1.1: ROOT DESTROYED).
    auto bkh = [&](const char* ctx, size_t clen, uint8_t* out, size_t olen) {
        blake3_keyed_hash(stored_seed,
                          reinterpret_cast<const uint8_t*>(ctx), clen,
                          out, olen);
    };

    bkh("fast", 4, m->fast_key, 16);
    bkh("oram", 4, m->oram_key, 16);
    bkh("integrity", 9, m->integrity_key, 32);
    bkh("chain_evo", 9, m->chain_evolution_key, 32);
    bkh("rekey", 5, m->rekey_key, 32);

    uint8_t meta_key[16], pool_key[16];
    bkh("meta", 4, meta_key, 16);
    bkh("pool", 4, pool_key, 16);

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
        blake3_keyed_hash(stored_seed,
                          reinterpret_cast<const uint8_t*>(enc_msg), 7,
                          md.bb_enc_seed, 8);
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

    // 8. Blob integrity hash (uses integrity_key, NOT stored_seed)
    blake3_keyed_hash(m->integrity_key,
                      m->blob_storage.data(), m->blob_storage.size(),
                      m->blob_integrity_hash, 32);

    // 9. ROOT DESTROYED (doc 16 §1.1).
    //
    // All stored_seed-dependent values have been pre-derived:
    //   fast_key, oram_key, integrity_key, chain_evolution_key, rekey_key,
    //   meta_key (local), pool_key (local), bb_enc_seeds (in bb_metadata),
    //   mem.encode/decode, blob_integrity_hash.
    //
    // stored_seed is the 32-byte root secret.  Zeroing it here ensures no
    // memory dump after this point can recover the root — all Theorems 7.2-7.4
    // depend on this invariant.
    //
    // The LOCAL copy on the caller's stack (stored_seed parameter) is the
    // caller's responsibility.  VmImmutable does NOT store it.
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

// ═════════════════════════════════════════════════════════════════════════════
// VmEngine::create_reentrant()
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmEngine<Policy, Oram>, DiagnosticCode>
VmEngine<Policy, Oram>::create_reentrant(
    std::shared_ptr<const VmImmutable> imm,
    int64_t load_base_delta,
    const uint64_t* initial_regs, uint8_t num_regs) noexcept
{
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

// ═════════════════════════════════════════════════════════════════════════════
// VmEngine::step() — doc 16 Phase A-L pipeline
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmResult, DiagnosticCode>
VmEngine<Policy, Oram>::step() noexcept
{
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

    // Zero plaintext operands after handler (Theorem 7.1).
    // plain_a/plain_b are the decoded register values.  Other insn fields
    // (opcode, reg_a, aux, plaintext_opcode) are still needed by Phases E-K
    // and are NOT secret (they're from the decrypted instruction, not data).
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
        // next_key = BLAKE3_KEYED_128(current_key, fingerprint || full_insn)
        //
        // The ratchet message includes:
        //   - fingerprint (16 bytes): binds next key to ALL register state
        //   - full_insn (8 bytes): binds next key to the entire decrypted instruction
        //     (including opcode, operand types, registers, and aux).
        //
        // One-way property of BLAKE3 means current_key cannot be recovered
        // from next_key -- this is the forward-secrecy guarantee.
        SecureLocal<uint8_t[16]> next_key;
        {
            uint8_t ratchet_msg[24];  // 16 + 8
            std::memcpy(ratchet_msg, fingerprint.val, 16);
            std::memcpy(ratchet_msg + 16, &insn.full_plaintext_insn, 8);
            blake3_keyed_128(exec_.insn_fpe_key, ratchet_msg, 24,
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

            // Doc 16 §4 Phase H: live registers re-encode, dead → Enc(0).
            uint16_t live = imm_->bb_metadata[exec_.current_bb_index].live_regs_bitmap;
            for (uint8_t i = 0; i < 16; ++i) {
                if (live & (1u << i)) {
                    SecureLocal<uint64_t> plain;
                    plain.val = FPE_Decode(rk.val, tw.val, i,
                                           exec_.regs[i].bits);
                    exec_.regs[i] = RegVal(
                        FPE_Encode(new_rk.val, new_tw.val, i, plain.val));
                } else {
                    // Dead register: canonical zero under new key
                    exec_.regs[i] = RegVal(
                        FPE_Encode(new_rk.val, new_tw.val, i, 0));
                }
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
    pipeline::advance_enc_state(exec_, insn.full_plaintext_insn);

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

// ═════════════════════════════════════════════════════════════════════════════
// VmEngine::dispatch_unit() — Doc 19 fixed-width dispatch unit
//
// WHY this exists (Doc 19 §1, Doc 18 §2):
//   The dispatch unit is the structural foundation for timing normalization.
//   It executes N = Policy::fusion_granularity sub-instructions through the
//   FULL Phase A-K crypto pipeline (no shortcuts), then Phase L once.
//
//   For N=1 (DebugPolicy), this is functionally identical to step() + Phase L.
//   For N=4 (HighSecPolicy), it eliminates 3 interpreter loop iterations,
//   3 indirect branches, and 3 Phase L checks — the only safe optimization
//   axis when the VM state is cryptographically entangled (Doc 18 §0).
//
// STATE ISOMORPHISM (Doc 18 Theorem 4.1):
//   dispatch_unit(σ₀) = A(…A(A(σ₀, p₁), p₂)…, pₙ)
//   This is not an approximation — it is the definition.  The dispatch unit
//   is a syntactic transformation (loop unrolling) of the interpreter, not
//   a semantic optimization.
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmResult, DiagnosticCode>
VmEngine<Policy, Oram>::dispatch_unit() noexcept
{
    static const auto table = build_handler_table<Policy, Oram>();
    constexpr uint8_t N = Policy::fusion_granularity;

    // ── N sub-instructions: full Phase A-K each ─────────────────────────
    for (uint8_t j = 0; j < N; ++j) {

        // ── Phases A-C: FETCH + DECRYPT + DECODE ────────────────────────
        auto insn_or = pipeline::fetch_decrypt_decode(*imm_, exec_, *epoch_);
        if (!insn_or) return tl::make_unexpected(insn_or.error());
        auto insn = *insn_or;

        // ── Phase C': RESOLVE operands + FPE decode ─────────────────────
        pipeline::resolve_operands(*imm_, exec_, *epoch_, insn);

        // ── Phase D: HANDLER DISPATCH ───────────────────────────────────
        auto r = table[static_cast<uint8_t>(insn.opcode)](
            exec_, *epoch_, *oram_, *imm_, insn);
        if (!r) return tl::make_unexpected(r.error());

        // Zero plaintext operands after handler (Theorem 7.1).
        secure_zero(&insn.plain_a, sizeof(insn.plain_a));
        secure_zero(&insn.plain_b, sizeof(insn.plain_b));

        // ── Phases E-I: FPE encode + fingerprint + key ratchet ──────────
        {
            SecureLocal<Speck64_RoundKeys> rk;
            Speck64_KeySchedule(exec_.insn_fpe_key, rk.val);
            SecureLocal<XEX_Tweaks> tw;
            XEX_ComputeTweaks(rk.val, tw.val);

            // Phase E: FPE-encode handler result
            if (opcode_writes_reg(insn.opcode)) {
                exec_.regs[insn.reg_a] = RegVal(
                    FPE_Encode(rk.val, tw.val, insn.reg_a,
                               exec_.regs[insn.reg_a].bits));
            }

            // Phase F: Fingerprint all 16 encoded registers
            SecureLocal<uint8_t[16]> fingerprint;
            {
                uint64_t reg_bits[16];
                for (uint8_t i = 0; i < 16; ++i)
                    reg_bits[i] = exec_.regs[i].bits;
                blake3_keyed_fingerprint(exec_.insn_fpe_key, reg_bits, fingerprint.val);
                secure_zero(reg_bits, sizeof(reg_bits));
            }

            // Phase G: Key ratchet
            SecureLocal<uint8_t[16]> next_key;
            {
                uint8_t ratchet_msg[24];
                std::memcpy(ratchet_msg, fingerprint.val, 16);
                std::memcpy(ratchet_msg + 16, &insn.full_plaintext_insn, 8);
                blake3_keyed_128(exec_.insn_fpe_key, ratchet_msg, 24,
                                 next_key.val, 16);
                secure_zero(ratchet_msg, sizeof(ratchet_msg));
            }

            // Phase H: Re-encode all 16 registers (old key → new key)
            {
                SecureLocal<Speck64_RoundKeys> new_rk;
                Speck64_KeySchedule(next_key.val, new_rk.val);
                SecureLocal<XEX_Tweaks> new_tw;
                XEX_ComputeTweaks(new_rk.val, new_tw.val);

                uint16_t live = imm_->bb_metadata[exec_.current_bb_index].live_regs_bitmap;
                for (uint8_t i = 0; i < 16; ++i) {
                    if (live & (1u << i)) {
                        SecureLocal<uint64_t> plain;
                        plain.val = FPE_Decode(rk.val, tw.val, i,
                                               exec_.regs[i].bits);
                        exec_.regs[i] = RegVal(
                            FPE_Encode(new_rk.val, new_tw.val, i, plain.val));
                    } else {
                        exec_.regs[i] = RegVal(
                            FPE_Encode(new_rk.val, new_tw.val, i, 0));
                    }
                }
            }

            // Phase I: Commit new key
            std::memcpy(exec_.insn_fpe_key, next_key.val, 16);
        }
        // Phase J: SecureLocal destructors zero all temporaries.

        // ── Phase K: advance enc_state ──────────────────────────────────
        pipeline::advance_enc_state(exec_, insn.full_plaintext_insn);

        // Advance IP for next fetch (Phase L runs once after all N iterations)
        exec_.vm_ip++;

        // Break on HALT — Phase L must still run for BB MAC verification.
        // In step(), Phase L executes even when halted (the BB boundary
        // check triggers verify_bb_mac).  Returning early here would skip
        // MAC verification, allowing a tampered blob to execute without
        // detection until after HALT.
        if (exec_.halted)
            break;
    }

    // ── Phase L: BB transition (once per dispatch unit) ─────────────────
    //
    // WHY only once: the fusability predicate (Doc 18 Def 3.1) guarantees
    // that only the last of N sub-instructions may be a branch.  For
    // non-branch instructions, Phase L is just vm_ip++ (already done above).
    // The BB boundary check and branch logic need only run once at the end.
    //
    // NOTE: This round uses the current branching Phase L.  Doc 19 Phase 4
    // will replace this with a branchless snapshot/MUX in the next round.
    if (exec_.branch_taken) {
        exec_.branch_taken = false;

        auto mac_r = pipeline::verify_bb_mac(*imm_, exec_, *epoch_);
        if (!mac_r) return tl::make_unexpected(mac_r.error());

        auto enter_r = pipeline::enter_basic_block(
            exec_, *epoch_, *imm_, exec_.branch_target_bb);
        if (!enter_r) return tl::make_unexpected(enter_r.error());

        if (exec_.return_resume_ip != 0) {
            exec_.vm_ip = exec_.return_resume_ip;
            exec_.insn_index_in_bb = exec_.return_resume_insn_idx;
            pipeline::replay_enc_state(exec_, *epoch_, *imm_,
                                       exec_.return_resume_insn_idx);
            exec_.return_resume_ip = 0;
            exec_.return_resume_insn_idx = 0;
        }
    } else {
        // Check if we've reached end of current BB (fallthrough)
        uint32_t bb_end = imm_->bb_metadata[exec_.current_bb_index].entry_ip
                        + pipeline::current_bb_insn_count(*imm_, exec_);

        if (exec_.vm_ip >= bb_end) {
            auto mac_r = pipeline::verify_bb_mac(*imm_, exec_, *epoch_);
            if (!mac_r) return tl::make_unexpected(mac_r.error());

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

// ═════════════════════════════════════════════════════════════════════════════
// VmEngine::execute() — main loop via dispatch_unit
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmExecResult, DiagnosticCode>
VmEngine<Policy, Oram>::execute() noexcept
{
    while (true) {
        auto r = dispatch_unit();
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

// ═════════════════════════════════════════════════════════════════════════════
// Explicit template instantiations — all known (Policy, Oram) combinations
//
// WHY explicit instantiation:
//   VmEngine is a class template whose method implementations now live in this
//   .cpp file (not the header).  Without explicit instantiations, any TU that
//   uses VmEngine<P,O> would get linker errors because the compiler never saw
//   the definitions when compiling that TU.  By instantiating all 6 known
//   combinations here, the linker finds the symbols it needs.
//
//   If a new Policy or Oram type is added, add its instantiation here.
// ═════════════════════════════════════════════════════════════════════════════

template class VmEngine<DebugPolicy,    DirectOram>;
template class VmEngine<DebugPolicy,    RollingKeyOram>;
template class VmEngine<StandardPolicy, DirectOram>;
template class VmEngine<StandardPolicy, RollingKeyOram>;
template class VmEngine<HighSecPolicy,  DirectOram>;
template class VmEngine<HighSecPolicy,  RollingKeyOram>;

}  // namespace VMPilot::Runtime
