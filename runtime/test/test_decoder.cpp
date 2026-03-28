#include "test_blob_builder.hpp"

#include <vm_loader.hpp>
#include <decoder.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_blob.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Common::VM::Encoding;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Fixed test seed -- deterministic across runs.
static void fill_test_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(i + 1);
}

/// Fixed epoch seed for a test BB.
static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

/// Load a blob with proper PRP tables.
///
/// The test blob builder now correctly simulates the compiler's two-layer PRP:
///   compile: semantic_op -> alias -> opcode_perm[alias] -> encrypted_alias
///   runtime: encrypted_alias -> opcode_perm_inv -> alias -> alias_lut -> semantic_op
///
/// The loader derives the real opcode_perm/opcode_perm_inv from epoch_seed,
/// and the alias_lut is loaded from the blob. No identity overrides needed.
static LoadedVM load_test_blob(const uint8_t seed[32],
                               const std::vector<uint8_t>& blob) {
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    EXPECT_TRUE(result.has_value());
    return std::move(result.value());
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(Decoder, SingleInstructionDecrypt) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0001;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0;
    bb.instructions.push_back(halt);

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    // Fetch, decrypt, decode
    auto result = fetch_decrypt_decode(vm.ctx);
    ASSERT_TRUE(result.has_value())
        << "fetch_decrypt_decode failed with code "
        << static_cast<uint32_t>(result.error());

    const DecodedInsn& decoded = result.value();
    EXPECT_EQ(decoded.opcode, VmOpcode::HALT);
    EXPECT_EQ(decoded.aux, 0u);
}

TEST(Decoder, ThreeInstructionChain) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 5;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x000F;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xBB);

    // Three instructions: NOP, ADD, HALT
    TestInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    nop.flags  = 0;
    nop.reg_a  = 0;
    nop.reg_b  = 0;
    nop.aux    = 0x11111111;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
    add.reg_a  = 1;
    add.reg_b  = 2;
    add.aux    = 0x22222222;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0x33333333;

    bb.instructions = {nop, add, halt};

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    // Decrypt instruction 0: NOP
    auto r0 = fetch_decrypt_decode(vm.ctx);
    ASSERT_TRUE(r0.has_value())
        << "insn 0 failed: " << static_cast<uint32_t>(r0.error());
    EXPECT_EQ(r0->opcode, VmOpcode::NOP);
    EXPECT_EQ(r0->aux, 0x11111111u);

    // Advance enc_state and vm_ip
    advance_enc_state(vm.ctx, r0->plaintext_opcode, r0->aux);
    vm.ctx.vm_ip++;

    // Decrypt instruction 1: ADD
    auto r1 = fetch_decrypt_decode(vm.ctx);
    ASSERT_TRUE(r1.has_value())
        << "insn 1 failed: " << static_cast<uint32_t>(r1.error());
    EXPECT_EQ(r1->opcode, VmOpcode::ADD);
    EXPECT_EQ(r1->operand_a_type, VM_OPERAND_REG);
    EXPECT_EQ(r1->operand_b_type, VM_OPERAND_REG);
    EXPECT_EQ(r1->reg_a, 1u);
    EXPECT_EQ(r1->reg_b, 2u);
    EXPECT_EQ(r1->aux, 0x22222222u);

    // Advance enc_state and vm_ip
    advance_enc_state(vm.ctx, r1->plaintext_opcode, r1->aux);
    vm.ctx.vm_ip++;

    // Decrypt instruction 2: HALT
    auto r2 = fetch_decrypt_decode(vm.ctx);
    ASSERT_TRUE(r2.has_value())
        << "insn 2 failed: " << static_cast<uint32_t>(r2.error());
    EXPECT_EQ(r2->opcode, VmOpcode::HALT);
    EXPECT_EQ(r2->aux, 0x33333333u);
}

TEST(Decoder, BBMacPass) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 10;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0003;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    nop.flags  = 0;
    nop.reg_a  = 0;
    nop.reg_b  = 0;
    nop.aux    = 0;
    bb.instructions = {nop, nop};

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    // MAC should verify successfully
    auto result = verify_bb_mac(vm.ctx);
    ASSERT_TRUE(result.has_value())
        << "verify_bb_mac failed with code "
        << static_cast<uint32_t>(result.error());
}

TEST(Decoder, BBMacFail) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 10;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0003;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    // Corrupt one byte of the encrypted instruction section.
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t insn_off = blob_section_insn(hdr);
    blob[insn_off] ^= 0x42;  // flip some bits

    auto vm = load_test_blob(seed, blob);

    // MAC should fail since instruction bytes were corrupted
    auto result = verify_bb_mac(vm.ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BBMacVerificationFailed);
}

TEST(Decoder, EnterBasicBlock) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Two BBs in the same epoch share the same epoch_seed (and therefore
    // the same opcode permutation). The runtime only re-derives encoding
    // tables on epoch change, so intra-epoch BBs must use consistent seeds.
    TestBB bb0{};
    bb0.bb_id = 100;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0x0003;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    nop.flags  = 0;
    nop.reg_a  = 0;
    nop.reg_b  = 0;
    nop.aux    = 0;
    bb0.instructions = {nop};

    TestBB bb1{};
    bb1.bb_id = 200;
    bb1.epoch = 0;  // same epoch — must use same epoch_seed as bb0
    bb1.live_regs_bitmap = 0x000F;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x10);  // same seed as bb0

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0;
    bb1.instructions = {halt};

    auto blob = build_test_blob(seed, {bb0, bb1});
    auto vm = load_test_blob(seed, blob);

    // Initially in bb0
    EXPECT_EQ(vm.ctx.current_bb_id, 100u);
    EXPECT_EQ(vm.ctx.vm_ip, 0u);
    EXPECT_EQ(vm.ctx.insn_index_in_bb, 0u);

    // Enter bb1
    auto result = enter_basic_block(vm.ctx, 200);
    ASSERT_TRUE(result.has_value())
        << "enter_basic_block failed with code "
        << static_cast<uint32_t>(result.error());

    EXPECT_EQ(vm.ctx.current_bb_id, 200u);
    EXPECT_EQ(vm.ctx.vm_ip, 1u);  // bb1 starts at ip=1 (after bb0's 1 insn)
    EXPECT_EQ(vm.ctx.insn_index_in_bb, 0u);

    // enc_state should be the bb_enc_seed for bb1
    uint8_t expected_seed_bytes[8];
    uint8_t enc_msg[7];
    std::memcpy(enc_msg, "enc", 3);
    uint32_t bb1_id = 200;
    std::memcpy(enc_msg + 3, &bb1_id, 4);
    blake3_keyed_hash(seed, enc_msg, 7, expected_seed_bytes, 8);

    uint64_t expected_enc_state = 0;
    std::memcpy(&expected_enc_state, expected_seed_bytes, 8);
    EXPECT_EQ(vm.ctx.enc_state, expected_enc_state);

    // The PRP tables are maintained by the loader and enter_basic_block —
    // no need to reset to identity since the builder applies real PRP.

    // Should be able to decrypt bb1's instruction (HALT)
    auto r = fetch_decrypt_decode(vm.ctx);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->opcode, VmOpcode::HALT);
}

TEST(Decoder, EnterBasicBlockInvalidId) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0001;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    // Try to enter a non-existent BB
    auto result = enter_basic_block(vm.ctx, 9999);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::InvalidBBTransition);
}

TEST(Decoder, AliasLutResolution) {
    // Verify that the decoder correctly resolves different opcodes
    // through the two-layer PRP chain:
    //   encrypted_alias -> opcode_perm_inv -> alias -> alias_lut -> semantic_op
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 42;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xDD);

    // Test several different opcodes
    VmOpcode test_opcodes[] = {
        VmOpcode::MOVE, VmOpcode::ADD, VmOpcode::XOR,
        VmOpcode::CMP, VmOpcode::JMP, VmOpcode::HALT
    };

    for (auto op : test_opcodes) {
        TestInstruction ti{};
        ti.opcode = op;
        ti.flags  = 0;
        ti.reg_a  = 0;
        ti.reg_b  = 0;
        ti.aux    = static_cast<uint32_t>(op);  // unique aux per opcode
        bb.instructions.push_back(ti);
    }

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    // Decrypt and verify each instruction's resolved opcode
    for (size_t i = 0; i < sizeof(test_opcodes) / sizeof(test_opcodes[0]); ++i) {
        auto r = fetch_decrypt_decode(vm.ctx);
        ASSERT_TRUE(r.has_value())
            << "Instruction " << i << " decrypt failed";
        EXPECT_EQ(r->opcode, test_opcodes[i])
            << "Instruction " << i << ": expected "
            << to_string(test_opcodes[i]) << " got "
            << to_string(r->opcode);
        EXPECT_EQ(r->aux, static_cast<uint32_t>(test_opcodes[i]));

        advance_enc_state(vm.ctx, r->plaintext_opcode, r->aux);
        vm.ctx.vm_ip++;
    }
}

TEST(Decoder, OutOfBoundsIP) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0001;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    // Set vm_ip past the end
    vm.ctx.vm_ip = vm.ctx.insn_count;

    auto result = fetch_decrypt_decode(vm.ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::InstructionDecryptFailed);
}

TEST(Decoder, EpochChangeOnEnter) {
    // Test that enter_basic_block handles epoch transitions correctly.
    // The epoch change re-derives encoding tables and opcode permutation.
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb0{};
    bb0.bb_id = 1;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0x0003;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    bb0.instructions = {nop};

    TestBB bb1{};
    bb1.bb_id = 2;
    bb1.epoch = 1;  // different epoch
    bb1.live_regs_bitmap = 0x0003;
    bb1.flags = BB_FLAG_EPOCH_CHANGED;
    fill_epoch_seed(bb1.epoch_seed, 0x20);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb1.instructions = {halt};

    auto blob = build_test_blob(seed, {bb0, bb1});
    auto vm = load_test_blob(seed, blob);

    EXPECT_EQ(vm.ctx.current_epoch, 0u);

    // Remember old opcode_perm_inv (derived from bb0's epoch_seed by the loader)
    uint8_t old_perm_inv[256];
    std::memcpy(old_perm_inv, vm.ctx.opcode_perm_inv, 256);

    // Enter bb1 (epoch changes from 0 to 1)
    auto enter_result = enter_basic_block(vm.ctx, 2);
    ASSERT_TRUE(enter_result.has_value());

    EXPECT_EQ(vm.ctx.current_epoch, 1u);

    // opcode_perm_inv should have been re-derived from bb1's epoch_seed
    // (it's no longer identity since enter_basic_block called
    // derive_opcode_permutation)
    bool perm_changed = false;
    for (int i = 0; i < 256; ++i) {
        if (vm.ctx.opcode_perm_inv[i] != old_perm_inv[i]) {
            perm_changed = true;
            break;
        }
    }
    EXPECT_TRUE(perm_changed)
        << "opcode_perm_inv should change after epoch transition";

    // The builder applies real PRP encoding and enter_basic_block re-derives
    // the opcode permutation from the new epoch_seed. The decoder's corrected
    // PRP order (perm_inv FIRST, alias_lut SECOND) handles this correctly.

    // Decrypt instruction in bb1 -- should get HALT
    auto r = fetch_decrypt_decode(vm.ctx);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->opcode, VmOpcode::HALT);
}

TEST(Decoder, AdvanceEncStateMatchesBuilder) {
    // Verify that advance_enc_state produces the same enc_state
    // as the test blob builder's update_enc_state.
    uint64_t initial_state = 0xDEADBEEFCAFEBABE;
    uint16_t opcode_val = static_cast<uint16_t>(VmOpcode::ADD);
    uint32_t aux_val = 0x12345678;

    // Compute via the builder's helper
    uint64_t expected = VMPilot::Test::detail::update_enc_state(
        initial_state, opcode_val, aux_val);

    // Compute via the runtime's advance_enc_state
    VMContext ctx{};
    ctx.enc_state = initial_state;
    ctx.insn_index_in_bb = 0;

    advance_enc_state(ctx, opcode_val, aux_val);

    EXPECT_EQ(ctx.enc_state, expected);
    EXPECT_EQ(ctx.insn_index_in_bb, 1u);
}

TEST(Decoder, ChaffInstructionProcessing) {
    // Chaff (NOP) instructions must participate in the enc_state chain and
    // BB MAC to prevent an attacker from deleting them without breaking the
    // SipHash preimage-resistant chain (spec §4.3). If chaff were excluded
    // from the chain, deletion would be undetectable, leaking program structure.
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 77;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x000F;  // R0-R3 live
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xEE);

    // Interleave real instructions with NOP chaff:
    // LOAD_CONST, NOP(chaff), ADD, NOP(chaff), HALT
    TestInstruction load_const{};
    load_const.opcode = VmOpcode::LOAD_CONST;
    load_const.flags  = static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_POOL << 4));
    load_const.reg_a  = 0;
    load_const.reg_b  = 0;
    load_const.aux    = 0;  // pool index 0

    TestInstruction chaff0{};
    chaff0.opcode = VmOpcode::NOP;
    chaff0.flags  = 0;
    chaff0.reg_a  = 0;
    chaff0.reg_b  = 0;
    chaff0.aux    = 0xCAFE0001;  // chaff aux is random noise from compiler

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
    add.reg_a  = 1;
    add.reg_b  = 2;
    add.aux    = 0;

    TestInstruction chaff1{};
    chaff1.opcode = VmOpcode::NOP;
    chaff1.flags  = 0;
    chaff1.reg_a  = 0;
    chaff1.reg_b  = 0;
    chaff1.aux    = 0xCAFE0002;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0;

    bb.instructions = {load_const, chaff0, add, chaff1, halt};

    auto blob = build_test_blob(seed, {bb});
    auto vm = load_test_blob(seed, blob);

    VmOpcode expected_ops[] = {
        VmOpcode::LOAD_CONST, VmOpcode::NOP, VmOpcode::ADD,
        VmOpcode::NOP, VmOpcode::HALT
    };

    // Decrypt all 5 instructions (including chaff) and verify the
    // enc_state chain is continuous across all of them.
    uint64_t prev_enc_state = vm.ctx.enc_state;

    for (int i = 0; i < 5; ++i) {
        auto r = fetch_decrypt_decode(vm.ctx);
        ASSERT_TRUE(r.has_value())
            << "Instruction " << i << " decrypt failed: "
            << static_cast<uint32_t>(r.error());
        EXPECT_EQ(r->opcode, expected_ops[i])
            << "Instruction " << i << " opcode mismatch";

        // Verify enc_state advances for every instruction (chaff included).
        // This ensures chaff cannot be removed without breaking the chain.
        advance_enc_state(vm.ctx, r->plaintext_opcode, r->aux);
        EXPECT_NE(vm.ctx.enc_state, prev_enc_state)
            << "enc_state did not advance for instruction " << i;
        prev_enc_state = vm.ctx.enc_state;
        vm.ctx.vm_ip++;
    }

    // Verify BB MAC covers all 5 instructions (including chaff).
    // If chaff were excluded from the MAC, an attacker could delete
    // them and forge a valid MAC for the shorter sequence.
    auto mac_result = verify_bb_mac(vm.ctx);
    ASSERT_TRUE(mac_result.has_value())
        << "BB MAC verification failed — chaff may not be covered by MAC";
}
