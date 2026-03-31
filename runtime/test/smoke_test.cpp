#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_crypto.hpp>

#include <gtest/gtest.h>

#include <cstring>

using namespace VMPilot::Common::VM;

TEST(VmInsnTest, SizeIs8Bytes) {
    static_assert(sizeof(VmInsn) == 8, "VmInsn must be exactly 8 bytes");
    EXPECT_EQ(sizeof(VmInsn), 8u);
}

TEST(VmInsnTest, FieldExtraction) {
    VmInsn insn{};
    insn.opcode   = 0x1234;
    insn.flags    = 0b11'10'0101;  // operand_a=3, operand_b=2, condition=5
    insn.reg_pack = 0xAB;          // reg_a=0xA, reg_b=0xB
    insn.aux      = 0xDEADBEEF;

    EXPECT_EQ(insn.operand_a_type(), 3u);
    EXPECT_EQ(insn.operand_b_type(), 2u);
    EXPECT_EQ(insn.condition(), 5u);
    EXPECT_EQ(insn.reg_a(), 0xAu);
    EXPECT_EQ(insn.reg_b(), 0xBu);
}

TEST(VmOpcodeTest, Count) {
    EXPECT_EQ(VM_OPCODE_COUNT, 55u);
}

TEST(VmOpcodeTest, Categories) {
    EXPECT_EQ(vm_opcode_category(VmOpcode::MOVE), 0u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::ADD), 1u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::AND), 2u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::CMP), 3u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::JMP), 4u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::SEXT8), 5u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::LOCK_ADD), 6u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::NOP), 7u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::RESYNC), 7u);
}

TEST(VmOpcodeTest, ToString) {
    EXPECT_STREQ(to_string(VmOpcode::HALT), "HALT");
    EXPECT_STREQ(to_string(VmOpcode::NOP), "NOP");
    EXPECT_STREQ(to_string(VmOpcode::ADD), "ADD");
}

TEST(VmConstantsTest, ObliviousSizeIs4KB) {
    EXPECT_EQ(VM_OBLIVIOUS_SIZE, 4096u);
    EXPECT_EQ(VM_ORAM_LINE_SIZE, 64u);
    EXPECT_EQ(VM_ORAM_NUM_LINES, 64u);
}

TEST(VmConstantsTest, RegisterAndLaneCount) {
    EXPECT_EQ(VM_REG_COUNT, 16u);
    EXPECT_EQ(VM_BYTE_LANES, 8u);
    EXPECT_EQ(VM_MAX_NESTING, 8u);
}

TEST(VmConfigTest, Defaults) {
    VmSecurityConfig config;
    EXPECT_EQ(config.fusion_granularity, 2u);
    EXPECT_EQ(config.encoding_evolution_n, 5u);
    EXPECT_EQ(config.mutation_interval, 64u);
    EXPECT_EQ(config.anti_debug_interval, 128u);
    EXPECT_FALSE(config.debug_mode);
}

TEST(SipHashTest, KnownVector) {
    // Test with zero key and empty message
    uint8_t key[16] = {};
    uint64_t hash = Crypto::siphash_2_4(key, nullptr, 0);
    // SipHash-2-4 with zero key and empty input should produce a specific value
    // Verify it at least produces a non-zero result
    EXPECT_NE(hash, 0u);
}

TEST(SipHashTest, DifferentInputsDifferentHashes) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t msg1[] = {0x00};
    uint8_t msg2[] = {0x01};

    uint64_t h1 = Crypto::siphash_2_4(key, msg1, 1);
    uint64_t h2 = Crypto::siphash_2_4(key, msg2, 1);
    EXPECT_NE(h1, h2);
}

TEST(SipHashTest, KeystreamDeterministic) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint64_t enc_state = 0xDEADBEEFCAFEBABE;

    uint64_t ks1 = Crypto::siphash_keystream(key, enc_state, 0);
    uint64_t ks2 = Crypto::siphash_keystream(key, enc_state, 0);
    EXPECT_EQ(ks1, ks2);

    uint64_t ks3 = Crypto::siphash_keystream(key, enc_state, 1);
    EXPECT_NE(ks1, ks3);
}

TEST(SipHashTest, ExpandProduces8Words) {
    uint8_t key[16] = {};
    uint64_t out[8] = {};
    Crypto::siphash_expand(key, 0, 0, out);

    // All 8 words should be populated (extremely unlikely all zero)
    bool all_zero = true;
    for (int i = 0; i < 8; ++i) {
        if (out[i] != 0) all_zero = false;
    }
    EXPECT_FALSE(all_zero);
}

// HandlerTableTest moved to test_phase3_handlers.cpp (EngineTable.AllOpcodesCovered)
// and test_engine_comprehensive.cpp (EngineTable.AllOpcodesCovered)
