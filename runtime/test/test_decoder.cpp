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
#include <numeric>

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

/// Load a blob and set opcode_perm_inv and alias_lut to identity so that
/// the decoder passes the raw opcode through without PRP transformation.
///
/// The test blob builder writes raw semantic opcode values into VmInsn.
/// The decoder applies: final = opcode_perm_inv[alias_lut[insn.opcode & 0xFF]].
/// By setting both to identity, we get: final = insn.opcode & 0xFF = semantic.
///
/// This isolates encryption/decryption testing from PRP testing.
static LoadedVM load_with_identity_prp(const uint8_t seed[32],
                                       const std::vector<uint8_t>& blob) {
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    EXPECT_TRUE(result.has_value());
    LoadedVM vm = std::move(result.value());

    // Set alias_lut to identity: alias_lut[i] = i
    // (only values < VM_OPCODE_COUNT will be valid semantic indices)
    std::iota(vm.ctx.alias_lut, vm.ctx.alias_lut + 256, 0);

    // Set opcode_perm_inv to identity: opcode_perm_inv[i] = i
    std::iota(vm.ctx.opcode_perm_inv, vm.ctx.opcode_perm_inv + 256, 0);

    // Also set opcode_perm to identity for consistency
    std::iota(vm.ctx.opcode_perm, vm.ctx.opcode_perm + 256, 0);

    return vm;
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
    auto vm = load_with_identity_prp(seed, blob);

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
    auto vm = load_with_identity_prp(seed, blob);

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
    auto vm = load_with_identity_prp(seed, blob);

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

    auto vm = load_with_identity_prp(seed, blob);

    // MAC should fail since instruction bytes were corrupted
    auto result = verify_bb_mac(vm.ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BBMacVerificationFailed);
}

TEST(Decoder, EnterBasicBlock) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Two BBs, same epoch for simplicity
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
    bb1.epoch = 0;  // same epoch
    bb1.live_regs_bitmap = 0x000F;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x20);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0;
    bb1.instructions = {halt};

    auto blob = build_test_blob(seed, {bb0, bb1});
    auto vm = load_with_identity_prp(seed, blob);

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

    // Re-set identity PRP since enter_basic_block may have re-derived
    // opcode_perm from the new epoch_seed (same epoch here, but
    // enter_basic_block only re-derives on epoch change, so this is fine)
    std::iota(vm.ctx.alias_lut, vm.ctx.alias_lut + 256, 0);
    std::iota(vm.ctx.opcode_perm_inv, vm.ctx.opcode_perm_inv + 256, 0);

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
    auto vm = load_with_identity_prp(seed, blob);

    // Try to enter a non-existent BB
    auto result = enter_basic_block(vm.ctx, 9999);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::InvalidBBTransition);
}

TEST(Decoder, AliasLutResolution) {
    // Verify that the decoder correctly resolves different opcodes
    // through the alias_lut + opcode_perm_inv chain.
    // With identity PRP, the chain is a passthrough.
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
    auto vm = load_with_identity_prp(seed, blob);

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
    auto vm = load_with_identity_prp(seed, blob);

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
    auto vm = load_with_identity_prp(seed, blob);

    EXPECT_EQ(vm.ctx.current_epoch, 0u);

    // Remember old opcode_perm_inv (identity since we set it)
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

    // To decrypt bb1's instruction, set identity PRP again since the
    // builder wrote raw semantic opcodes (no compiler-side permutation)
    std::iota(vm.ctx.alias_lut, vm.ctx.alias_lut + 256, 0);
    std::iota(vm.ctx.opcode_perm_inv, vm.ctx.opcode_perm_inv + 256, 0);

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
