#include <capstone.hpp>

#include <gtest/gtest.h>

class CapstoneX64Test : public ::testing::Test {
   protected:
    Capstone::Capstone cs{Capstone::Arch::X86, Capstone::Mode::MODE_64};
};

TEST_F(CapstoneX64Test, DisasmNop) {
    // 0x90 = nop
    auto insns = cs.disasm({0x90}, 0x1000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_EQ(insns[0].mnemonic, "nop");
    EXPECT_EQ(insns[0].address, 0x1000u);
    EXPECT_EQ(insns[0].size, 1u);
}

TEST_F(CapstoneX64Test, DisasmRet) {
    // 0xC3 = ret
    auto insns = cs.disasm({0xC3}, 0x2000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_TRUE(insns[0].isRet());
    EXPECT_FALSE(insns[0].isCall());
    EXPECT_FALSE(insns[0].isJump());
}

TEST_F(CapstoneX64Test, DisasmDirectCall) {
    // e8 00 00 00 00 = call +0 (calls next instruction)
    auto insns = cs.disasm({0xE8, 0x00, 0x00, 0x00, 0x00}, 0x1000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_TRUE(insns[0].isCall());
    EXPECT_EQ(insns[0].getDirectTarget(), 0x1005u);  // 0x1000 + 5 + 0
}

TEST_F(CapstoneX64Test, DisasmCallWithOffset) {
    // e8 fb ff ff ff = call -5 (calls itself at 0x1000)
    auto insns = cs.disasm({0xE8, 0xFB, 0xFF, 0xFF, 0xFF}, 0x1000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_TRUE(insns[0].isCall());
    EXPECT_EQ(insns[0].getDirectTarget(), 0x1000u);
}

TEST_F(CapstoneX64Test, DisasmRipRelativeCall) {
    // ff 15 XX XX XX XX = call [rip + disp32]
    // At address 0x1000, call [rip + 0x2000]
    // Effective addr = 0x1000 + 6 + 0x2000 = 0x3006
    auto insns = cs.disasm({0xFF, 0x15, 0x00, 0x20, 0x00, 0x00}, 0x1000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_TRUE(insns[0].isCall());
    EXPECT_EQ(insns[0].getDirectTarget(), 0u);  // not a direct call
    EXPECT_EQ(insns[0].getRipRelativeTarget(), 0x3006u);
}

TEST_F(CapstoneX64Test, DisasmJmp) {
    // eb fe = jmp -2 (infinite loop)
    auto insns = cs.disasm({0xEB, 0xFE}, 0x1000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_TRUE(insns[0].isJump());
    EXPECT_FALSE(insns[0].isCall());
}

TEST_F(CapstoneX64Test, DisasmMultipleInstructions) {
    // push rbp; mov rbp, rsp; pop rbp; ret
    std::vector<uint8_t> code = {0x55, 0x48, 0x89, 0xE5, 0x5D, 0xC3};
    auto insns = cs.disasm(code, 0x1000);
    ASSERT_EQ(insns.size(), 4u);
    EXPECT_EQ(insns[0].mnemonic, "push");
    EXPECT_EQ(insns[3].mnemonic, "ret");
    EXPECT_TRUE(insns[3].isRet());
}

TEST_F(CapstoneX64Test, DisasmBaseAddrMatters) {
    auto insns_a = cs.disasm({0x90}, 0x0);
    auto insns_b = cs.disasm({0x90}, 0xDEAD);
    EXPECT_EQ(insns_a[0].address, 0x0u);
    EXPECT_EQ(insns_b[0].address, 0xDEADu);
}

TEST_F(CapstoneX64Test, DisasmEmpty) {
    auto insns = cs.disasm({}, 0);
    EXPECT_TRUE(insns.empty());
}

TEST_F(CapstoneX64Test, OperandDetail) {
    // mov rax, 0x42  =>  48 c7 c0 42 00 00 00
    auto insns = cs.disasm({0x48, 0xC7, 0xC0, 0x42, 0x00, 0x00, 0x00}, 0);
    ASSERT_EQ(insns.size(), 1u);
    ASSERT_GE(insns[0].operands.size(), 2u);
    EXPECT_EQ(insns[0].operands[0].type, Capstone::OpType::REG);
    EXPECT_EQ(insns[0].operands[1].type, Capstone::OpType::IMM);
    EXPECT_EQ(insns[0].operands[1].imm, 0x42);
}

class CapstoneX86Test : public ::testing::Test {
   protected:
    Capstone::Capstone cs{Capstone::Arch::X86, Capstone::Mode::MODE_32};
};

TEST_F(CapstoneX86Test, DisasmCall32) {
    // e8 fb ff ff ff = call -5
    auto insns = cs.disasm({0xE8, 0xFB, 0xFF, 0xFF, 0xFF}, 0x1000);
    ASSERT_EQ(insns.size(), 1u);
    EXPECT_TRUE(insns[0].isCall());
    EXPECT_EQ(insns[0].getDirectTarget(), 0x1000u);
}

TEST(CapstoneMove, MoveConstructor) {
    Capstone::Capstone a(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    auto insns = a.disasm({0x90}, 0);
    ASSERT_FALSE(insns.empty());

    Capstone::Capstone b(std::move(a));
    insns = b.disasm({0xC3}, 0);
    ASSERT_FALSE(insns.empty());
    EXPECT_TRUE(insns[0].isRet());
}
