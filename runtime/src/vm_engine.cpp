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

        // P5: precompute bb_end_ip (avoids repeated addition on hot path)
        md.bb_end_ip = md.entry_ip + md.insn_count_in_bb;

        // P6: decode bb_enc_seed as uint64_t once (avoids repeated memcpy)
        std::memcpy(&md.bb_enc_seed_u64, md.bb_enc_seed, 8);
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

    // 7b. Compute max BB instruction count for constant-time MAC verification.
    //
    // WHY (Doc 19 §4.2 Fix #2): verify_bb_mac must iterate a fixed number of
    // times regardless of actual BB length, otherwise the O(BB_length) loop
    // leaks BB size via timing.
    {
        uint32_t max_insns = 0;
        for (const auto& md : m->bb_metadata)
            if (md.insn_count_in_bb > max_insns)
                max_insns = md.insn_count_in_bb;
        m->max_bb_insn_count = max_insns;

        // P1: reject blobs exceeding the compile-time MAC scratch buffer cap.
        if (max_insns > VM_MAX_BB_INSN_CAP)
            return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);
    }

    // 7c. Build O(1) bb_id -> bb_index lookup table (P2).
    //
    // Replaces the linear scan in find_bb_index().  Dense vector sized to
    // max_bb_id + 1, filled with UINT32_MAX (invalid sentinel).
    {
        uint32_t max_id = 0;
        for (const auto& md : m->bb_metadata)
            if (md.bb_id > max_id)
                max_id = md.bb_id;
        m->bb_id_to_index.assign(static_cast<size_t>(max_id) + 1, UINT32_MAX);
        for (size_t i = 0; i < m->bb_metadata.size(); ++i)
            m->bb_id_to_index[m->bb_metadata[i].bb_id] = static_cast<uint32_t>(i);
    }

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

    exec.enc_state = first.bb_enc_seed_u64;  // P6: pre-decoded at load time

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

    exec.enc_state = first.bb_enc_seed_u64;  // P6: pre-decoded at load time

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
// execute_one_instruction — THE per-instruction pipeline (Phase A-K)
//
// Single codepath for all instruction execution.  Both step() and
// dispatch_unit() delegate here.  No other execution path exists.
//
// Security invariants (Doc 16 rev.8 §4, Doc 17 §2.2, Doc 19 §4.2):
//   - Phase D.oram: unconditional ORAM scan per instruction (Doc 19)
//   - Phase E: branchless FPE encode via bitmask MUX (Doc 19 §4.2 Fix #3)
//   - Phase F-I: full crypto ratchet — fingerprint, key derivation, 16-reg
//     re-encode, SecureLocal zeroing (Doc 16 Theorem 7.1)
//   - Phase K: 8-byte full-instruction enc_state advance (Doc 17 §3.1)
//
// STATE ISOMORPHISM (Doc 18 Theorem 4.1):
//   This function implements the atomic transition A(σ, p).  Calling it
//   N times produces A(…A(A(σ₀, p₁), p₂)…, pₙ) — the exact definition
//   of a dispatch unit.
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
static tl::expected<void, DiagnosticCode>
execute_one_instruction(VmExecution& exec, VmEpoch& epoch,
                        VmOramState& oram, const VmImmutable& imm) noexcept
{
    // ── Phases A-C: FETCH + DECRYPT + DECODE ────────────────────────────
    auto insn_or = pipeline::fetch_decrypt_decode(imm, exec, epoch);
    if (!insn_or) return tl::make_unexpected(insn_or.error());
    auto insn = *insn_or;

    // ── Phase C': RESOLVE operands + FPE decode ─────────────────────────
    pipeline::resolve_operands(imm, exec, epoch, insn);

    // ── Phase D.oram: Unconditional per-instruction ORAM scan ───────────
    //
    // WHY per-instruction (Doc 19 pipeline-level normalization):
    //   PUSH/POP handlers previously called Oram::read/write inside Phase D,
    //   making their timing ~14K ns higher than NOP/ALU opcodes.  Moving the
    //   scan here ensures every instruction does exactly 1 ORAM scan at the
    //   same cost.  PUSH/POP handlers read exec.oram_read_result instead of
    //   calling Oram directly.
    //
    //   Branchless MUX: address, value, and direction are computed from the
    //   decoded opcode without data-dependent branches.  Non-PUSH/POP opcodes
    //   produce a dummy read at address 0 with value 0 — same ORAM scan cost.
    {
        const uint64_t push_mask = -static_cast<uint64_t>(
            insn.opcode == VmOpcode::PUSH);
        const uint64_t pop_mask  = -static_cast<uint64_t>(
            insn.opcode == VmOpcode::POP);

        const uint64_t oram_addr =
            ((exec.vm_sp - 8) & push_mask) | (exec.vm_sp & pop_mask);

        const uint64_t encoded =
            imm.mem.encode_lut().apply(insn.plain_a);
        const uint64_t write_val = encoded & push_mask;

        const bool is_write = static_cast<bool>(push_mask & 1);

        exec.oram_read_result = Oram::access(
            oram, oram_addr, write_val, is_write);
    }

    // ── Phase D.handler: HANDLER DISPATCH ───────────────────────────────
    static const auto table = build_handler_table<Policy, Oram>();
    auto r = table[static_cast<uint8_t>(insn.opcode)](
        exec, epoch, oram, imm, insn);
    if (!r) return tl::make_unexpected(r.error());

    // Zero plaintext operands after handler (Doc 16 Theorem 7.1).
    secure_zero(&insn.plain_a, sizeof(insn.plain_a));
    secure_zero(&insn.plain_b, sizeof(insn.plain_b));

    // ── Phases E-I: FPE encode + fingerprint + key ratchet ──────────────
    //
    // Core forward-secrecy pipeline (Doc 16 rev.8 §4).
    // All cryptographic temporaries are SecureLocal (RAII zeroed on scope exit).
    {
        SecureLocal<Speck64_RoundKeys> rk;
        Speck64_KeySchedule(exec.insn_fpe_key, rk.val);
        SecureLocal<XEX_Tweaks> tw;
        XEX_ComputeTweaks(rk.val, tw.val);

        // ── Phase E: branchless FPE encode (Doc 19 §4.2 Fix #3) ────────
        //
        // WHY branchless (always execute FPE_Encode):
        //   A data-dependent branch on opcode_writes_reg() leaks 1 bit of
        //   opcode information via EM/DPA side channel — the attacker can
        //   distinguish register-writing opcodes from non-writing ones by
        //   observing the power trace or electromagnetic emissions.
        //
        //   Branchless MUX always computes FPE_Encode and selects the
        //   destination via bitmask:
        //     writes_reg=true:  encoded value → regs[reg_a]
        //     writes_reg=false: original value preserved in regs[reg_a]
        {
            uint64_t encoded = FPE_Encode(rk.val, tw.val, insn.reg_a,
                                          exec.regs[insn.reg_a].bits);
            uint64_t mask = -static_cast<uint64_t>(
                opcode_writes_reg(insn.opcode));
            exec.regs[insn.reg_a] = RegVal(
                (encoded & mask) | (exec.regs[insn.reg_a].bits & ~mask));
        }

        // P4: Pre-expand the 16-byte FPE key to 32 bytes ONCE for both
        // Phase F (fingerprint) and Phase G (ratchet).  Avoids the redundant
        // [K||K] expansion that blake3_keyed_128 does internally.
        SecureLocal<uint8_t[32]> expanded_fpe_key;
        blake3_preexpand_128(exec.insn_fpe_key, expanded_fpe_key.val);

        // ── Phase F: Fingerprint all 16 encoded registers ───────────────
        //
        // WHY fingerprint ALL regs (Doc 16 "entanglement" property):
        //   Modifying any register — even one not touched by this instruction
        //   — changes the fingerprint, which changes the next key, which
        //   corrupts all subsequent decryptions.
        //
        // P3: Hash exec.regs[] directly — static_asserts in vm_state.hpp
        // verify that RegVal[16] is layout-compatible with uint64_t[16],
        // eliminating the 128-byte copy into a temporary array.
        SecureLocal<uint8_t[16]> fingerprint;
        blake3_keyed_fingerprint_preexpanded(
            expanded_fpe_key.val,
            reinterpret_cast<const uint64_t*>(exec.regs),
            fingerprint.val);

        // ── Phase G: Key ratchet ────────────────────────────────────────
        //
        // next_key = BLAKE3_KEYED_128(current_key, fingerprint || full_insn)
        //
        // WHY full_insn in ratchet (Doc 17 entanglement):
        //   The ratchet binds the next key to both the register state
        //   (fingerprint) AND the entire decrypted instruction (8 bytes).
        //   One-way BLAKE3 guarantees forward secrecy: current_key cannot
        //   be recovered from next_key.
        SecureLocal<uint8_t[16]> next_key;
        {
            uint8_t ratchet_msg[24];
            std::memcpy(ratchet_msg, fingerprint.val, 16);
            std::memcpy(ratchet_msg + 16, &insn.full_plaintext_insn, 8);
            blake3_keyed_preexpanded(expanded_fpe_key.val, ratchet_msg, 24,
                                     next_key.val, 16);
            secure_zero(ratchet_msg, sizeof(ratchet_msg));
        }

        // ── Phase H: Re-encode all 16 registers (old key → new key) ────
        //
        // NOTE (Shannon branch): branchless Phase H was attempted but
        // reverted — always-decode-all-16 added ~14 extra FPE_Decode per
        // instruction, creating micro-architectural timing variance that
        // worsened HighSecPolicy ANOVA from p=0.015 to p=2e-25.
        //
        // The live_regs_bitmap branch remains.  It leaks the number of
        // live registers per BB (~150 ns per extra FPE_Decode), visible
        // as a ~300 ns bimodal in StandardPolicy.  For HighSecPolicy (N=4),
        // the crypto pipeline noise masks this signal adequately.
        //
        // Future fix: normalize live_regs_bitmap at blob creation time
        // (serializer/linker) so all BBs declare the same set of live
        // registers, eliminating the timing signal without runtime cost.
        {
            SecureLocal<Speck64_RoundKeys> new_rk;
            Speck64_KeySchedule(next_key.val, new_rk.val);
            SecureLocal<XEX_Tweaks> new_tw;
            XEX_ComputeTweaks(new_rk.val, new_tw.val);

            uint16_t live = imm.bb_metadata[exec.current_bb_index]
                                .live_regs_bitmap;
            for (uint8_t i = 0; i < 16; ++i) {
                if (live & (1u << i)) {
                    SecureLocal<uint64_t> plain;
                    plain.val = FPE_Decode(rk.val, tw.val, i,
                                           exec.regs[i].bits);
                    exec.regs[i] = RegVal(
                        FPE_Encode(new_rk.val, new_tw.val, i, plain.val));
                } else {
                    exec.regs[i] = RegVal(
                        FPE_Encode(new_rk.val, new_tw.val, i, 0));
                }
            }
        }

        // ── Phase I: Commit new key ─────────────────────────────────────
        std::memcpy(exec.insn_fpe_key, next_key.val, 16);
    }
    // Phase J: SecureLocal destructors zero rk, tw, fingerprint, next_key.

    // ── Phase K: advance enc_state (Doc 17 full-instruction ratchet) ────
    pipeline::advance_enc_state(exec, insn.full_plaintext_insn);

    return {};
}

// ═════════════════════════════════════════════════════════════════════════════
// VmEngine::step() — delegates to dispatch_unit (no separate codepath)
//
// WHY no separate implementation:
//   The old step() lacked Phase D.oram (PUSH/POP broken) and used branching
//   Phase L (timing side-channel leak).  Delegating to dispatch_unit()
//   ensures step() gets the same security guarantees: per-instruction ORAM
//   scan, branchless Phase E, branchless Phase L.  For DebugPolicy (N=1),
//   dispatch_unit executes exactly 1 instruction + Phase L — identical
//   semantics to the old step(), but with no security loopholes.
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmResult, DiagnosticCode>
VmEngine<Policy, Oram>::step() noexcept
{
    return dispatch_unit();
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
    constexpr uint8_t N = Policy::fusion_granularity;

    // ── N sub-instructions: full Phase A-K each ─────────────────────────
    for (uint8_t j = 0; j < N; ++j) {

        auto r = execute_one_instruction<Policy, Oram>(
            exec_, *epoch_, *oram_, *imm_);
        if (!r) return tl::make_unexpected(r.error());

        exec_.vm_ip++;

        // Break on HALT — Phase L must still run for BB MAC verification.
        if (exec_.halted)
            break;

        // Break at BB boundary — prevents fetching into the next BB's
        // instruction space when the current BB is shorter than N.
        // Once Phase 6 chaff expansion is in place, BB lengths are always
        // multiples of N and this check becomes dead code.
        {
            // P5: use precomputed bb_end_ip (avoids repeated addition)
            uint32_t bb_end_check = imm_->bb_metadata[exec_.current_bb_index].bb_end_ip;
            if (exec_.vm_ip >= bb_end_check || exec_.branch_taken)
                break;
        }
    }

    // ── Phase L: Unified branchless BB transition (Doc 19 §4.2) ────────
    //
    // WHY branchless: the branching if/else in Phase L leaks whether a BB
    // transition occurred via timing (verify_bb_mac + enter_basic_block are
    // ~8000 ns; skipping them is measurably different).  The branchless
    // approach always executes both operations, then uses bitwise MUX to
    // decide whether to keep the new state or restore the snapshot.
    //
    // Three cases handled without data-dependent branches:
    //   (a) branch_taken → target = branch_target_bb, commit
    //   (b) at_bb_end && !branch_taken → target = next_seq_bb, commit
    //   (c) !at_bb_end && !branch_taken → target = current_bb (identity), restore
    {
        // ── L.1: Snapshot state before transition ────────────────────
        auto snap = ExecSnapshot::capture(exec_);
        VmEpoch epoch_snap = *epoch_;

        // ── L.2: Determine target BB (branchless 3-way select) ──────
        // P5: use precomputed bb_end_ip
        uint32_t bb_end = imm_->bb_metadata[exec_.current_bb_index].bb_end_ip;
        bool at_end = (exec_.vm_ip >= bb_end);
        bool need_transition = exec_.branch_taken | at_end;

        uint32_t next_seq_id = (exec_.current_bb_index + 1 < imm_->bb_metadata.size())
            ? imm_->bb_metadata[exec_.current_bb_index + 1].bb_id
            : exec_.current_bb_id;

        uint32_t m_br  = -static_cast<uint32_t>(exec_.branch_taken);
        uint32_t m_end = -static_cast<uint32_t>(at_end) & ~m_br;
        uint32_t m_id  = ~m_br & ~m_end;

        uint32_t target = (exec_.branch_target_bb & m_br)
                        | (next_seq_id & m_end)
                        | (exec_.current_bb_id & m_id);

        // ── L.3: Always verify BB MAC ───────────────────────────────
        auto mac_r = pipeline::verify_bb_mac(*imm_, exec_, *epoch_);
        if (!mac_r) return tl::make_unexpected(mac_r.error());

        // ── L.4: Always enter_basic_block ───────────────────────────
        exec_.branch_taken = false;
        auto enter_r = pipeline::enter_basic_block(
            exec_, *epoch_, *imm_, target);
        if (!enter_r) return tl::make_unexpected(enter_r.error());

        // ── L.5: RET_VM resume — branchless apply via MUX ───────────
        {
            uint64_t rmask = -static_cast<uint64_t>(exec_.return_resume_ip != 0);
            exec_.vm_ip = (exec_.return_resume_ip & rmask)
                        | (exec_.vm_ip & ~rmask);
            exec_.insn_index_in_bb = static_cast<uint32_t>(
                (exec_.return_resume_insn_idx & static_cast<uint32_t>(rmask))
              | (exec_.insn_index_in_bb & static_cast<uint32_t>(~rmask)));
            pipeline::replay_enc_state(exec_, *epoch_, *imm_,
                                       exec_.return_resume_insn_idx);
            exec_.return_resume_ip = 0;
            exec_.return_resume_insn_idx = 0;
        }

        // ── L.6: Branchless commit/restore MUX ──────────────────────
        snap.branchless_restore(exec_, need_transition);
        epoch_->branchless_select(epoch_snap, need_transition);

        secure_zero(&snap, sizeof(snap));
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
