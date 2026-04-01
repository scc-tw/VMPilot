/// @file pipeline.cpp
/// @brief Non-template pipeline function implementations for doc 16 forward secrecy.
///
/// Ports the fetch-decrypt-decode, resolve, advance, MAC verify, and BB
/// transition logic to the doc 16 model:
///
///   - Register encoding: Speck64/128 XEX FPE (replaces per-BB LUT bijections)
///   - Memory encoding:   GlobalMemTables LUT (replaces per-BB DomainTables)
///   - Key evolution:     Per-instruction ratchet + per-BB chain state
///
/// Key changes from doc 15:
///
///   1. resolve_operands: MEM operands decode through imm.mem (GlobalMemTables)
///      instead of epoch.dom (DomainTables).  REG operands are FPE-decoded
///      into plain_a/plain_b using the current insn_fpe_key.
///
///   2. enter_basic_block: derives insn_fpe_key via BLAKE3_KEYED_128, re-encodes
///      all live registers from old FPE key to new FPE key, sanitises dead regs.
///      No longer derives RegTables or DomainTables (those are removed in doc 16).
///
///   3. derive_bb_fpe_key: new function for path-dependent key derivation.

#include "pipeline.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_blob.hpp>
#include <vm/encoded_value.hpp>
#include <vm/xex_speck64.hpp>
#include <vm/secure_zero.hpp>

#include <cstring>
#include <vector>

namespace VMPilot::Runtime::pipeline {

using namespace Common::VM;
using namespace Common::VM::Crypto;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// derive_bb_enc_seed removed: bb_enc_seeds are now pre-derived in VmImmutable
// during VmEngine::create() and stored in BBMetadata.  No stored_seed at runtime.

/// Update enc_state: SipHash(enc_state_as_key_zeropadded_16, opcode(2)||aux(4))
static uint64_t update_enc_state_impl(uint64_t enc_state,
                                      uint16_t opcode_val,
                                      uint32_t aux) noexcept {
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);
    uint8_t msg[6];
    std::memcpy(msg, &opcode_val, 2);
    std::memcpy(msg + 2, &aux, 4);
    return siphash_2_4(key, msg, 6);
}

/// Find BB index by bb_id (linear search -- v1 simplicity).
static int find_bb_index(const VmImmutable& imm, uint32_t bb_id) noexcept {
    for (size_t i = 0; i < imm.bb_metadata.size(); ++i) {
        if (imm.bb_metadata[i].bb_id == bb_id)
            return static_cast<int>(i);
    }
    return -1;
}

// ---------------------------------------------------------------------------
// fetch_decrypt_decode (Steps 1-3)
// ---------------------------------------------------------------------------

tl::expected<DecodedInsn, DiagnosticCode>
fetch_decrypt_decode(const VmImmutable& imm,
                     const VmExecution& exec,
                     const VmEpoch& epoch) noexcept {

    // Step 1: FETCH -- bounds check + read encrypted 8 bytes
    auto insns = imm.blob.instructions();
    if (exec.vm_ip >= insns.size())
        return tl::make_unexpected(DiagnosticCode::InstructionDecryptFailed);

    uint64_t encrypted = 0;
    std::memcpy(&encrypted, &insns[exec.vm_ip], 8);

    // Step 2: DECRYPT -- SipHash keystream
    uint64_t keystream = siphash_keystream(
        imm.fast_key, exec.enc_state, exec.insn_index_in_bb);
    uint64_t plain_u64 = encrypted ^ keystream;

    VmInsn insn{};
    std::memcpy(&insn, &plain_u64, 8);

    // Step 3: DECODE -- two-layer PRP opcode resolution
    // PRP inverse FIRST (undo Layer 2), then alias LUT SECOND (undo Layer 1).
    // This order preserves Shannon perfect secrecy (doc 11 section 6).
    uint8_t encrypted_alias = static_cast<uint8_t>(insn.opcode & 0xFF);
    uint8_t alias = epoch.opcode_perm_inv[encrypted_alias];
    uint8_t semantic_op = imm.alias_lut[alias];

    if (semantic_op >= VM_OPCODE_COUNT)
        return tl::make_unexpected(DiagnosticCode::InvalidOpcodeAlias);

    // Build DecodedInsn
    DecodedInsn decoded{};
    decoded.opcode          = static_cast<VmOpcode>(semantic_op);
    decoded.operand_a_type  = insn.operand_a_type();
    decoded.operand_b_type  = insn.operand_b_type();
    decoded.condition       = insn.condition();
    decoded.reg_a           = insn.reg_a();
    decoded.reg_b           = insn.reg_b();
    decoded.plaintext_opcode = insn.opcode;
    decoded.aux             = insn.aux;

    return decoded;
}

// ---------------------------------------------------------------------------
// resolve_operands (Step 4) -- doc 16 version with FPE decode
// ---------------------------------------------------------------------------

/// Resolve a single operand to its raw value.
///
/// Doc 16 encoding model:
///   REG  -> exec.regs[reg_x]  (FPE-encoded, caller will FPE_Decode)
///   POOL -> constant_pool[aux] (plaintext after blob decryption)
///   MEM  -> guest memory -> GlobalMemTables::decode -> plaintext
///   NONE -> 0
///
/// WHY MEM uses GlobalMemTables directly (not DomainTables):
///   Doc 16 removes per-BB DomainTables (store/load).  Memory encoding is
///   a FIXED global LUT (derived once from stored_seed at blob load time).
///   This simplifies BB transitions: no store/load table derivation needed.
///   The security cost is acceptable because memory is already protected by
///   ORAM (oblivious access patterns) -- the LUT only provides value encoding.
static RegVal resolve_one(const VmImmutable& imm,
                          const VmExecution& exec,
                          uint8_t type, uint8_t reg_idx,
                          uint32_t aux) noexcept {
    switch (type) {
        case VM_OPERAND_REG:
            // Return FPE-encoded value as-is.  The caller FPE_Decodes it.
            return exec.regs[reg_idx & 0x0F];

        case VM_OPERAND_POOL: {
            // Constant pool: decrypted at blob load time, values are PLAINTEXT.
            // Doc 16 change: pool is no longer "pre-encoded in target BB's domain"
            // because there are no per-BB LUT domains.  Pool values are plain.
            if (imm.decrypted_pool.empty() || aux * 8 >= imm.decrypted_pool.size())
                return RegVal(0);
            uint64_t val = 0;
            std::memcpy(&val, imm.decrypted_pool.data() + aux * 8, 8);
            return RegVal(val);
        }

        case VM_OPERAND_MEM: {
            // Guest external memory (Space 2, direct access).
            // Read raw value -> decode through GlobalMemTables -> plaintext.
            //
            // WHY decode to plaintext here (not FPE-encode into register domain):
            //   Doc 16 handlers expect plain_a/plain_b as plaintext.  For MEM
            //   operands, the GlobalMemTables LUT decode IS the final decode step.
            //   There's no FPE involvement for memory -- memory uses LUT encoding,
            //   registers use FPE encoding.  These are separate domains.
            uintptr_t guest_addr = static_cast<uintptr_t>(
                static_cast<int64_t>(aux) + exec.load_base_delta);
            uint64_t mem_val = 0;
            std::memcpy(&mem_val, reinterpret_cast<const uint8_t*>(guest_addr), 8);

            // Decode through GlobalMemTables: MemVal -> PlainVal -> store as RegVal bits
            PlainVal plain = decode_memory(imm.mem.decode_lut(), MemVal(mem_val));
            return RegVal(plain.bits);
        }

        case VM_OPERAND_NONE:
        default:
            return RegVal(0);
    }
}

void resolve_operands(const VmImmutable& imm,
                      const VmExecution& exec,
                      const VmEpoch& /*epoch*/,
                      DecodedInsn& insn) noexcept {

    // Phase 1: resolve raw values (always both -- D3 uniform pipeline)
    insn.resolved_a = resolve_one(imm, exec,
                                   insn.operand_a_type, insn.reg_a, insn.aux);
    insn.resolved_b = resolve_one(imm, exec,
                                   insn.operand_b_type, insn.reg_b, insn.aux);

    // Phase 2: FPE decode to plaintext.
    //
    // Pre-compute Speck key schedule + XEX tweaks ONCE for both operands.
    // This avoids redundant key expansion (27-round Speck key schedule is
    // ~100 cycles; doing it twice wastes ~3% of per-instruction budget).
    //
    // WHY we use exec.insn_fpe_key:
    //   The per-instruction FPE key is derived from the BB chain state and
    //   ratcheted after each instruction.  This key determines the Speck
    //   permutation used to encode register values.  Only REG operands are
    //   in this domain -- POOL, MEM, and NONE are already plaintext.
    // SecureLocal ensures round keys and tweaks are zeroed on scope exit.
    // Without this, an attacker could recover insn_fpe_key[j] by inverting
    // Speck's key schedule from stale round keys on the stack (Theorem 7.1).
    SecureLocal<Speck64_RoundKeys> rk;
    Speck64_KeySchedule(exec.insn_fpe_key, rk.val);
    SecureLocal<XEX_Tweaks> tw;
    XEX_ComputeTweaks(rk.val, tw.val);

    // Operand A: FPE decode if REG, passthrough otherwise
    if (insn.operand_a_type == VM_OPERAND_REG) {
        insn.plain_a = FPE_Decode(rk.val, tw.val, insn.reg_a, insn.resolved_a.bits);
    } else {
        // POOL, MEM, NONE: resolved value IS the plaintext
        insn.plain_a = insn.resolved_a.bits;
    }

    // Operand B: same logic
    if (insn.operand_b_type == VM_OPERAND_REG) {
        insn.plain_b = FPE_Decode(rk.val, tw.val, insn.reg_b, insn.resolved_b.bits);
    } else {
        insn.plain_b = insn.resolved_b.bits;
    }
    // rk, tw auto-zeroed by SecureLocal destructor
}

// ---------------------------------------------------------------------------
// advance_enc_state (Step 9)
// ---------------------------------------------------------------------------

void advance_enc_state(VmExecution& exec,
                       uint16_t plaintext_opcode,
                       uint32_t plaintext_aux) noexcept {
    exec.enc_state = update_enc_state_impl(
        exec.enc_state, plaintext_opcode, plaintext_aux);
    ++exec.insn_index_in_bb;
}

// ---------------------------------------------------------------------------
// derive_bb_fpe_key
// ---------------------------------------------------------------------------

void derive_bb_fpe_key(const BBMetadata& bb,
                       const uint8_t bb_chain_state[32],
                       uint8_t out_key[16]) noexcept {
    // key = BLAKE3_KEYED(epoch_seed_as_32byte_key, bb_chain_state)[0:16]
    //
    // WHY BLAKE3_KEYED (not BLAKE3_KDF):
    //   BLAKE3_KEYED provides a PRF: for a random key (epoch_seed), the output
    //   is indistinguishable from random for any input.  This is exactly what
    //   we need -- the FPE key must be unpredictable given the chain state.
    //
    //   BLAKE3_KDF would also work but is semantically wrong: KDF is for
    //   deriving keys from non-uniform entropy.  epoch_seed is already a
    //   uniform 256-bit secret, so keyed hash is the correct primitive.
    //
    // OUTPUT SIZE = 16 bytes:
    //   Matches Speck64/128's key size.  The FPE security level is 128 bits
    //   (determined by the cipher), so 128 bits of key material is optimal --
    //   more would be wasted, less would reduce the security level.
    blake3_keyed_hash(bb.epoch_seed, bb_chain_state, 32, out_key, 16);
}

// ---------------------------------------------------------------------------
// enter_basic_block -- doc 16 version with FPE re-encoding
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
enter_basic_block(VmExecution& exec,
                  VmEpoch& epoch,
                  const VmImmutable& imm,
                  uint32_t target_bb_id) noexcept {

    // 1. Find target BB
    int bb_idx = find_bb_index(imm, target_bb_id);
    if (bb_idx < 0)
        return tl::make_unexpected(DiagnosticCode::InvalidBBTransition);

    const BBMetadata& target = imm.bb_metadata[static_cast<size_t>(bb_idx)];

    // 2. Use pre-derived bb_enc_seed from BBMetadata (no stored_seed needed)
    uint64_t enc_seed_u64 = 0;
    std::memcpy(&enc_seed_u64, target.bb_enc_seed, 8);
    exec.enc_state = enc_seed_u64;

    // 3. Reset instruction tracking
    exec.insn_index_in_bb = 0;
    exec.vm_ip = target.entry_ip;

    // 4. Derive opcode permutation for the new BB's epoch.
    //
    // Doc 16 change: enter_bb() ONLY derives opcode permutations now.
    // RegTables and DomainTables are no longer derived (FPE replaces LUTs,
    // GlobalMemTables replaces DomainTables).
    //
    // NOTE: epoch.enter_bb() derives opcode_perm and opcode_perm_inv
    // which are needed by fetch_decrypt_decode().  This is the ONLY thing
    // VmEpoch does now -- RegTables, DomainTables, and CompositionCache
    // are removed (doc 16 replaces them with Speck-FPE).
    epoch.enter_bb(target);

    // 5. Evolve bb_chain_state BEFORE deriving the new key.
    //
    // chain_state_new = BLAKE3_KEYED(chain_evolution_key, old_state || bb_id)
    //
    // Doc 16 §5 step 2: chain evolution uses a pre-derived key (NOT stored_seed,
    // which was zeroed during create()).  The chain_evolution_key is derived
    // from stored_seed once during init and stored in VmImmutable.
    //
    // WHY evolve before key derivation:
    //   The chain state must incorporate the target BB identity so that
    //   entering the same BB via different paths yields different states.
    {
        uint8_t evolve_msg[36];  // 32 (old chain state) + 4 (bb_id)
        std::memcpy(evolve_msg, exec.bb_chain_state, 32);
        std::memcpy(evolve_msg + 32, &target.bb_id, 4);

        uint8_t new_chain_state[32];
        blake3_keyed_hash(imm.chain_evolution_key, evolve_msg, 36,
                          new_chain_state, 32);

        // Zero old chain state before overwriting (forward secrecy)
        secure_zero(exec.bb_chain_state, 32);
        std::memcpy(exec.bb_chain_state, new_chain_state, 32);
        secure_zero(evolve_msg, sizeof(evolve_msg));
        secure_zero(new_chain_state, sizeof(new_chain_state));
    }

    // 6. Derive new insn_fpe_key from evolved chain state
    //
    // Save old key for register re-encoding (step 7).
    uint8_t old_fpe_key[16];
    std::memcpy(old_fpe_key, exec.insn_fpe_key, 16);

    derive_bb_fpe_key(target, exec.bb_chain_state, exec.insn_fpe_key);

    // 7. Re-encode all registers from old FPE key to new FPE key.
    //
    // For each register:
    //   - Live: plain = FPE_Decode(old_key, old_tweak, reg, encoded)
    //           encoded_new = FPE_Encode(new_key, new_tweak, reg, plain)
    //   - Dead: encoded_new = FPE_Encode(new_key, new_tweak, reg, 0)
    //
    // WHY re-encode instead of just changing the key:
    //   The register file stores FPE-encoded values.  If we change the key
    //   without re-encoding, every subsequent FPE_Decode would produce garbage
    //   because the ciphertext was encrypted under the old key.
    //
    // WHY sanitise dead registers:
    //   Dead registers might hold stale encoded values from a previous BB.
    //   Under the new key, those stale values decode to random garbage -- but
    //   that garbage is PREDICTABLE if an attacker knows the old key.  By
    //   explicitly encoding 0, we ensure dead registers hold a consistent
    //   known-plaintext encoding, revealing no information about the old key.
    //
    // PLAINTEXT EXPOSURE:
    //   Yes, plaintext values are transiently in CPU registers during re-encoding.
    //   This is unavoidable -- FPE is a block cipher, so decode-then-encode
    //   necessarily passes through plaintext.  The exposure is bounded to the
    //   re-encoding loop (~16 iterations, ~microseconds).  Doc 16 accepts this
    //   as the same threat model as the original LUT-based RE_TABLE (which also
    //   computes encode_new(decode_old(x)) through intermediate values).
    {
        // Pre-compute key schedules + tweaks for both keys
        Speck64_RoundKeys old_rk, new_rk;
        Speck64_KeySchedule(old_fpe_key, old_rk);
        Speck64_KeySchedule(exec.insn_fpe_key, new_rk);

        XEX_Tweaks old_tw, new_tw;
        XEX_ComputeTweaks(old_rk, old_tw);
        XEX_ComputeTweaks(new_rk, new_tw);

        for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
            if (target.live_regs_bitmap & (1u << r)) {
                // Live register: decode with old key, encode with new key.
                // SecureLocal ensures plaintext is zeroed after use (Theorem 7.1).
                SecureLocal<uint64_t> plain;
                plain.val = FPE_Decode(old_rk, old_tw, r,
                                       exec.regs[r].bits);
                exec.regs[r] = RegVal(FPE_Encode(new_rk, new_tw, r, plain.val));
            } else {
                // Dead register: encode zero with new key (sanitise)
                exec.regs[r] = RegVal(FPE_Encode(new_rk, new_tw, r, 0));
            }
        }

        // Erase old key material from stack.
        // new_rk/new_tw are re-derivable from exec.insn_fpe_key, but old_rk/old_tw
        // must not linger because old_fpe_key is about to be zeroed.
        secure_zero(&old_rk, sizeof(old_rk));
        secure_zero(&old_tw, sizeof(old_tw));
    }

    // Erase old FPE key -- forward secrecy requires that old keys are destroyed.
    // After this point, the old FPE key cannot be recovered, and any register
    // values from the old BB cannot be decoded without it.
    secure_zero(old_fpe_key, sizeof(old_fpe_key));

    // 8. Update BB tracking
    exec.current_bb_id    = target.bb_id;
    exec.current_bb_index = static_cast<uint32_t>(bb_idx);
    exec.current_epoch    = target.epoch;

    return {};
}

// ---------------------------------------------------------------------------
// verify_bb_mac (Step 11)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
verify_bb_mac(const VmImmutable& imm,
              const VmExecution& exec,
              const VmEpoch& epoch) noexcept {

    uint32_t bb_idx = exec.current_bb_index;
    if (bb_idx >= imm.bb_metadata.size())
        return tl::make_unexpected(DiagnosticCode::InvalidBBTransition);

    const BBMetadata& bb = imm.bb_metadata[bb_idx];

    // Derive bb_enc_seed for this BB
    uint8_t enc_seed_bytes[8];
    std::memcpy(enc_seed_bytes, bb.bb_enc_seed, 8);

    uint64_t enc_state = 0;
    std::memcpy(&enc_state, enc_seed_bytes, 8);

    // Re-decrypt all instructions in this BB
    auto insns = imm.blob.instructions();
    std::vector<uint8_t> plaintext_bytes(bb.insn_count_in_bb * 8);

    for (uint32_t j = 0; j < bb.insn_count_in_bb; ++j) {
        uint64_t encrypted = 0;
        std::memcpy(&encrypted, &insns[bb.entry_ip + j], 8);

        uint64_t ks = siphash_keystream(imm.fast_key, enc_state, j);
        uint64_t plain = encrypted ^ ks;

        std::memcpy(plaintext_bytes.data() + j * 8, &plain, 8);

        // Check for REKEY instruction -- must replay its enc_state mutation
        VmInsn vinst{};
        std::memcpy(&vinst, &plain, 8);
        {
            uint8_t enc_alias = static_cast<uint8_t>(vinst.opcode & 0xFF);
            uint8_t alias = epoch.opcode_perm_inv[enc_alias];
            uint8_t sem_op = imm.alias_lut[alias];
            if (sem_op == static_cast<uint8_t>(VmOpcode::REKEY)) {
                uint32_t rekey_counter = vinst.aux;
                uint8_t rk_ctx[9];
                std::memcpy(rk_ctx, "rekey", 5);
                std::memcpy(rk_ctx + 5, &rekey_counter, 4);
                uint8_t rk_mat[16];
                // Use pre-derived rekey_key (stored_seed was zeroed)
                blake3_keyed_hash(imm.rekey_key,
                                  rk_ctx, 9, rk_mat, 16);
                uint8_t es[8];
                std::memcpy(es, &enc_state, 8);
                enc_state = siphash_2_4(rk_mat, es, 8);
            }
        }
        enc_state = update_enc_state_impl(enc_state, vinst.opcode, vinst.aux);
    }

    // Compute expected MAC
    uint8_t computed_mac[8];
    blake3_keyed_hash(imm.integrity_key,
                      plaintext_bytes.data(),
                      plaintext_bytes.size(),
                      computed_mac, 8);

    // Compare with stored MAC (constant-time)
    uint64_t stored_mac = 0;
    std::memcpy(&stored_mac, imm.blob.bb_mac(bb_idx), 8);

    uint64_t computed_mac_u64 = 0;
    std::memcpy(&computed_mac_u64, computed_mac, 8);

    // Constant-time comparison: XOR is data-independent; the compiler
    // cannot short-circuit it.  Prevents timing side-channel on MAC check.
    uint64_t diff = computed_mac_u64 ^ stored_mac;
    if (diff != 0)
        return tl::make_unexpected(DiagnosticCode::BBMacVerificationFailed);

    return {};
}

// ---------------------------------------------------------------------------
// replay_enc_state (RET_VM resume)
// ---------------------------------------------------------------------------

void replay_enc_state(VmExecution& exec, const VmEpoch& epoch,
                      const VmImmutable& imm,
                      uint32_t target_insn_idx) noexcept {

    const auto& bb = imm.bb_metadata[exec.current_bb_index];

    // Derive bb_enc_seed from scratch (enter_basic_block already set enc_state,
    // but we need the seed for the keystream replay).
    uint8_t enc_seed_bytes[8];
    std::memcpy(enc_seed_bytes, bb.bb_enc_seed, 8);

    uint64_t es = 0;
    std::memcpy(&es, enc_seed_bytes, 8);

    // Replay SipHash chain: decrypt each instruction [0..target_insn_idx) and
    // advance enc_state, handling REKEY mutations along the way.
    auto insns = imm.blob.instructions();
    for (uint32_t j = 0; j < target_insn_idx; ++j) {
        uint64_t encrypted = 0;
        std::memcpy(&encrypted, &insns[bb.entry_ip + j], 8);

        uint64_t ks = siphash_keystream(imm.fast_key, es, j);
        uint64_t plain_u64 = encrypted ^ ks;

        VmInsn vi{};
        std::memcpy(&vi, &plain_u64, 8);

        // Check for REKEY -- must replay its enc_state mutation
        uint8_t enc_alias = static_cast<uint8_t>(vi.opcode & 0xFF);
        uint8_t alias = epoch.opcode_perm_inv[enc_alias];
        uint8_t sem = imm.alias_lut[alias];
        if (sem == static_cast<uint8_t>(VmOpcode::REKEY)) {
            uint32_t cnt = vi.aux;
            uint8_t rk_ctx[9];
            std::memcpy(rk_ctx, "rekey", 5);
            std::memcpy(rk_ctx + 5, &cnt, 4);
            uint8_t rk_mat[16];
            // Use pre-derived rekey_key (stored_seed was zeroed)
            blake3_keyed_hash(imm.rekey_key,
                              rk_ctx, 9, rk_mat, 16);
            uint8_t esb[8];
            std::memcpy(esb, &es, 8);
            es = siphash_2_4(rk_mat, esb, 8);
        }

        es = update_enc_state_impl(es, vi.opcode, vi.aux);
    }

    exec.enc_state = es;
}

// ---------------------------------------------------------------------------
// current_bb_insn_count
// ---------------------------------------------------------------------------

uint32_t current_bb_insn_count(const VmImmutable& imm,
                                const VmExecution& exec) noexcept {
    if (exec.current_bb_index >= imm.bb_metadata.size())
        return 0;
    return imm.bb_metadata[exec.current_bb_index].insn_count_in_bb;
}

}  // namespace VMPilot::Runtime::pipeline
