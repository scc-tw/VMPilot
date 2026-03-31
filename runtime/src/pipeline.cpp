/// @file pipeline.cpp
/// @brief Non-template pipeline function implementations for the new state model.
///
/// Ports the fetch-decrypt-decode, resolve, advance, MAC verify, and BB
/// transition logic from the old VMContext-based code to the 4-way split
/// (VmImmutable, VmExecution, VmEpoch, VmOramState).

#include "pipeline.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_blob.hpp>
#include <vm/encoded_value.hpp>

#include <cstring>
#include <vector>

namespace VMPilot::Runtime::pipeline {

using namespace Common::VM;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers
// ─────────────────────────────────────────────────────────────────────────────

/// Derive bb_enc_seed = BLAKE3_keyed(stored_seed, "enc" || bb_id_le32)[0:8]
static void derive_bb_enc_seed(const uint8_t stored_seed[32],
                               uint32_t bb_id,
                               uint8_t out[8]) noexcept {
    uint8_t msg[7];
    std::memcpy(msg, "enc", 3);
    std::memcpy(msg + 3, &bb_id, 4);
    blake3_keyed_hash(stored_seed, msg, 7, out, 8);
}

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

/// Find BB index by bb_id (linear search — v1 simplicity).
static int find_bb_index(const VmImmutable& imm, uint32_t bb_id) noexcept {
    for (size_t i = 0; i < imm.bb_metadata.size(); ++i) {
        if (imm.bb_metadata[i].bb_id == bb_id)
            return static_cast<int>(i);
    }
    return -1;
}

// ─────────────────────────────────────────────────────────────────────────────
// fetch_decrypt_decode (Steps 1-3)
// ─────────────────────────────────────────────────────────────────────────────

tl::expected<DecodedInsn, DiagnosticCode>
fetch_decrypt_decode(const VmImmutable& imm,
                     const VmExecution& exec,
                     const VmEpoch& epoch) noexcept {

    // Step 1: FETCH — bounds check + read encrypted 8 bytes
    auto insns = imm.blob.instructions();
    if (exec.vm_ip >= insns.size())
        return tl::make_unexpected(DiagnosticCode::InstructionDecryptFailed);

    uint64_t encrypted = 0;
    std::memcpy(&encrypted, &insns[exec.vm_ip], 8);

    // Step 2: DECRYPT — SipHash keystream
    uint64_t keystream = siphash_keystream(
        imm.fast_key, exec.enc_state, exec.insn_index_in_bb);
    uint64_t plain_u64 = encrypted ^ keystream;

    VmInsn insn{};
    std::memcpy(&insn, &plain_u64, 8);

    // Step 3: DECODE — two-layer PRP opcode resolution
    // PRP inverse FIRST (undo Layer 2), then alias LUT SECOND (undo Layer 1).
    // This order preserves Shannon perfect secrecy (§11.6).
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

// ─────────────────────────────────────────────────────────────────────────────
// resolve_operands (Step 4)
// ─────────────────────────────────────────────────────────────────────────────

static RegVal resolve_one(const VmImmutable& imm,
                          const VmExecution& exec,
                          const VmEpoch& epoch,
                          uint8_t type, uint8_t reg_idx,
                          uint32_t aux) noexcept {
    switch (type) {
        case VM_OPERAND_REG:
            return exec.regs[reg_idx & 0x0F];

        case VM_OPERAND_POOL: {
            // Constant pool: pre-encoded by compiler in target BB's domain.
            // Read as RegVal directly (no domain conversion needed).
            if (imm.decrypted_pool.empty() || aux * 8 >= imm.decrypted_pool.size())
                return RegVal(0);
            uint64_t val = 0;
            std::memcpy(&val, imm.decrypted_pool.data() + aux * 8, 8);
            return RegVal(val);
        }

        case VM_OPERAND_MEM: {
            // Guest external memory (Space 2, direct access).
            // Read raw value → convert memory→register domain via LOAD_TABLE.
            uintptr_t guest_addr = static_cast<uintptr_t>(
                static_cast<int64_t>(aux) + exec.load_base_delta);
            uint64_t mem_val = 0;
            std::memcpy(&mem_val, reinterpret_cast<const uint8_t*>(guest_addr), 8);
            return load_convert(epoch.dom.load_lut(reg_idx & 0x0F), MemVal(mem_val));
        }

        case VM_OPERAND_NONE:
        default:
            return RegVal(0);
    }
}

void resolve_operands(const VmImmutable& imm,
                      const VmExecution& exec,
                      const VmEpoch& epoch,
                      DecodedInsn& insn) noexcept {
    // Always resolve BOTH operands — D3 uniform pipeline.
    insn.resolved_a = resolve_one(imm, exec, epoch,
                                   insn.operand_a_type, insn.reg_a, insn.aux);
    insn.resolved_b = resolve_one(imm, exec, epoch,
                                   insn.operand_b_type, insn.reg_b, insn.aux);
}

// ─────────────────────────────────────────────────────────────────────────────
// advance_enc_state (Step 9)
// ─────────────────────────────────────────────────────────────────────────────

void advance_enc_state(VmExecution& exec,
                       uint16_t plaintext_opcode,
                       uint32_t plaintext_aux) noexcept {
    exec.enc_state = update_enc_state_impl(
        exec.enc_state, plaintext_opcode, plaintext_aux);
    ++exec.insn_index_in_bb;
}

// ─────────────────────────────────────────────────────────────────────────────
// enter_basic_block
// ─────────────────────────────────────────────────────────────────────────────

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

    // 2. Derive bb_enc_seed and reset enc_state
    uint8_t enc_seed_bytes[8];
    derive_bb_enc_seed(imm.stored_seed, target.bb_id, enc_seed_bytes);

    uint64_t enc_seed_u64 = 0;
    std::memcpy(&enc_seed_u64, enc_seed_bytes, 8);
    exec.enc_state = enc_seed_u64;

    // 3. Reset instruction tracking
    exec.insn_index_in_bb = 0;
    exec.vm_ip = target.entry_ip;

    // 4. Handle epoch change: apply RE_TABLE before deriving new tables
    if (target.epoch != exec.current_epoch) {
        epoch.transition_regs(exec, target, imm);
    }

    // 5. Derive new tables for this BB
    epoch.enter_bb(target, imm);

    // 6. Update tracking
    exec.current_bb_id    = target.bb_id;
    exec.current_bb_index = static_cast<uint32_t>(bb_idx);
    exec.current_epoch    = target.epoch;

    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
// verify_bb_mac (Step 11)
// ─────────────────────────────────────────────────────────────────────────────

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
    derive_bb_enc_seed(imm.stored_seed, bb.bb_id, enc_seed_bytes);

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

        // Check for REKEY instruction — must replay its enc_state mutation
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
                blake3_kdf(imm.stored_seed,
                           reinterpret_cast<const char*>(rk_ctx), 9,
                           rk_mat, 16);
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

    if (computed_mac_u64 != stored_mac)
        return tl::make_unexpected(DiagnosticCode::BBMacVerificationFailed);

    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
// current_bb_insn_count
// ─────────────────────────────────────────────────────────────────────────────

uint32_t current_bb_insn_count(const VmImmutable& imm,
                                const VmExecution& exec) noexcept {
    if (exec.current_bb_index >= imm.bb_metadata.size())
        return 0;
    return imm.bb_metadata[exec.current_bb_index].insn_count_in_bb;
}

}  // namespace VMPilot::Runtime::pipeline
