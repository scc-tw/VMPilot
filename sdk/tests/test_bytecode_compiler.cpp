#include <bytecode_compiler.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;

TEST(CompilerFactory, CreateX86) {
    auto compiler = CompilerFactory::CreateCompiler(Arch::X86);
    ASSERT_NE(compiler, nullptr);
    EXPECT_EQ(compiler->GetArch(), Arch::X86);
}

TEST(CompilerFactory, CreateX86_64) {
    auto compiler = CompilerFactory::CreateCompiler(Arch::X86_64);
    ASSERT_NE(compiler, nullptr);
    EXPECT_EQ(compiler->GetArch(), Arch::X86_64);
}

TEST(CompilerFactory, CreateArmReturnsNull) {
    auto compiler = CompilerFactory::CreateCompiler(Arch::ARM);
    EXPECT_EQ(compiler, nullptr);
}

TEST(CompilerFactory, CreateArm64ReturnsNull) {
    auto compiler = CompilerFactory::CreateCompiler(Arch::ARM64);
    EXPECT_EQ(compiler, nullptr);
}
