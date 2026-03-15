#include <bytecode_compiler.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;

TEST(CompilerFactory, CreateX86) {
    auto compiler = CompilerFactory::CreateCompiler("x86");
    ASSERT_NE(compiler, nullptr);
    EXPECT_EQ(compiler->GetName(), "x86");
}

TEST(CompilerFactory, CreateX86_64) {
    auto compiler = CompilerFactory::CreateCompiler("x86_64");
    ASSERT_NE(compiler, nullptr);
    EXPECT_EQ(compiler->GetName(), "x86_64");
}

TEST(CompilerFactory, CreateArm) {
    auto compiler = CompilerFactory::CreateCompiler("arm");
    ASSERT_NE(compiler, nullptr);
}

TEST(CompilerFactory, CreateUnknown) {
    auto compiler = CompilerFactory::CreateCompiler("unknown_arch");
    EXPECT_EQ(compiler, nullptr);
}

TEST(CompilerFactory, CreateEmpty) {
    auto compiler = CompilerFactory::CreateCompiler("");
    EXPECT_EQ(compiler, nullptr);
}
