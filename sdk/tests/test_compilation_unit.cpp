#include <CompilationUnit.hpp>

#include <gtest/gtest.h>

using VMPilot::SDK::Core::CompilationUnit;
using VMPilot::SDK::Segmentator::CompilationContext;

TEST(CompilationUnit, DefaultConstruction) {
    CompilationUnit unit;
    EXPECT_TRUE(unit.name.empty());
    EXPECT_EQ(unit.addr, 0u);
    EXPECT_EQ(unit.size, 0u);
    EXPECT_TRUE(unit.code.empty());
    EXPECT_TRUE(unit.enclosing_symbol.empty());
    EXPECT_FALSE(unit.is_canonical);
    EXPECT_EQ(unit.context, nullptr);
}

TEST(CompilationUnit, PopulatedUnit) {
    CompilationContext ctx_val;
    ctx_val.arch = VMPilot::SDK::Segmentator::Arch::X86;
    ctx_val.mode = VMPilot::SDK::Segmentator::Mode::MODE_32;
    auto ctx = std::make_shared<const CompilationContext>(std::move(ctx_val));

    CompilationUnit unit;
    unit.name = "test_function";
    unit.addr = 0x401000;
    unit.size = 16;
    unit.code = {0x55, 0x89, 0xe5, 0x83, 0xec, 0x10,
                 0xc7, 0x45, 0xfc, 0x00, 0x00, 0x00,
                 0x00, 0x8b, 0x45, 0xfc};
    unit.enclosing_symbol = "_Z13test_functionv";
    unit.is_canonical = true;
    unit.context = ctx;

    EXPECT_EQ(unit.name, "test_function");
    EXPECT_EQ(unit.addr, 0x401000u);
    EXPECT_EQ(unit.size, 16u);
    EXPECT_EQ(unit.code.size(), 16u);
    EXPECT_EQ(unit.enclosing_symbol, "_Z13test_functionv");
    EXPECT_NE(unit.context, nullptr);
    EXPECT_EQ(unit.context->arch, VMPilot::SDK::Segmentator::Arch::X86);
}

TEST(CompilationUnit, CopySemantics) {
    CompilationUnit original;
    original.name = "copy_test";
    original.addr = 0x1000;
    original.code = {0x90, 0x90};

    CompilationUnit copy = original;
    EXPECT_EQ(copy.name, original.name);
    EXPECT_EQ(copy.addr, original.addr);
    EXPECT_EQ(copy.code, original.code);
}

TEST(CompilationUnit, MoveSemantics) {
    CompilationUnit original;
    original.name = "move_test";
    original.code = {0x90, 0x90, 0x90};

    CompilationUnit moved = std::move(original);
    EXPECT_EQ(moved.name, "move_test");
    EXPECT_EQ(moved.code.size(), 3u);
}
