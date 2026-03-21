#include <gtest/gtest.h>

#include "SymExpr.hpp"

using namespace VMPilot::SDK::ReferenceAnalyzer;

// ---- Default / Unknown ----

TEST(SymExpr, UnknownDefault) {
    SymExpr e;
    EXPECT_TRUE(e.isUnknown());
    EXPECT_FALSE(e.isConstant());
    EXPECT_EQ(e.tryEval(), std::nullopt);
}

// ---- Constant ----

TEST(SymExpr, Constant) {
    auto e = SymExpr::constant(42);
    EXPECT_TRUE(e.isConstant());
    EXPECT_FALSE(e.isUnknown());
    ASSERT_TRUE(e.tryEval().has_value());
    EXPECT_EQ(*e.tryEval(), 42u);
}

// ---- BinaryOp constant folding ----

TEST(SymExpr, BinopConstantFolding) {
    auto e = SymExpr::binop(SymExpr::BinOp::Add,
                            SymExpr::constant(3),
                            SymExpr::constant(5));
    // Should fold to Constant(8)
    EXPECT_TRUE(e.isConstant());
    ASSERT_TRUE(e.tryEval().has_value());
    EXPECT_EQ(*e.tryEval(), 8u);
}

TEST(SymExpr, BinopAllOps) {
    // Add
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Add,
                                SymExpr::constant(10),
                                SymExpr::constant(20));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 30u);
    }
    // Sub
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Sub,
                                SymExpr::constant(20),
                                SymExpr::constant(7));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 13u);
    }
    // Mul
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Mul,
                                SymExpr::constant(6),
                                SymExpr::constant(7));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 42u);
    }
    // And
    {
        auto e = SymExpr::binop(SymExpr::BinOp::And,
                                SymExpr::constant(0xFF),
                                SymExpr::constant(0x0F));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 0x0Fu);
    }
    // Or
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Or,
                                SymExpr::constant(0xF0),
                                SymExpr::constant(0x0F));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 0xFFu);
    }
    // Xor
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Xor,
                                SymExpr::constant(0xFF),
                                SymExpr::constant(0x0F));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 0xF0u);
    }
    // Shl
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Shl,
                                SymExpr::constant(1),
                                SymExpr::constant(4));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 16u);
    }
    // Lsr (logical shift right)
    {
        auto e = SymExpr::binop(SymExpr::BinOp::Lsr,
                                SymExpr::constant(256),
                                SymExpr::constant(4));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 16u);
    }
    // Asr (arithmetic shift right)
    {
        // -16 >> 2 = -4 (arithmetic)
        uint64_t neg16 = static_cast<uint64_t>(int64_t{-16});
        auto e = SymExpr::binop(SymExpr::BinOp::Asr,
                                SymExpr::constant(neg16),
                                SymExpr::constant(2));
        ASSERT_TRUE(e.isConstant());
        EXPECT_EQ(static_cast<int64_t>(*e.tryEval()), -4);
    }
}

// ---- Identity / simplification ----

TEST(SymExpr, BinopAddZeroIdentity) {
    auto x = SymExpr::opaque(SymExpr::OpaqueKind::SystemReg, 1);
    auto e = SymExpr::binop(SymExpr::BinOp::Add, std::move(x),
                            SymExpr::constant(0));
    // Should simplify to x (Opaque), not a BinaryOp
    EXPECT_EQ(e.kind, SymExpr::Kind::Opaque);
}

TEST(SymExpr, BinopSubZeroIdentity) {
    auto x = SymExpr::opaque(SymExpr::OpaqueKind::SystemReg, 1);
    auto e = SymExpr::binop(SymExpr::BinOp::Sub, std::move(x),
                            SymExpr::constant(0));
    EXPECT_EQ(e.kind, SymExpr::Kind::Opaque);
}

TEST(SymExpr, BinopMulOneIdentity) {
    auto x = SymExpr::opaque(SymExpr::OpaqueKind::SystemReg, 1);
    auto e = SymExpr::binop(SymExpr::BinOp::Mul, std::move(x),
                            SymExpr::constant(1));
    EXPECT_EQ(e.kind, SymExpr::Kind::Opaque);
}

TEST(SymExpr, BinopMulZero) {
    auto x = SymExpr::opaque(SymExpr::OpaqueKind::SystemReg, 1);
    auto e = SymExpr::binop(SymExpr::BinOp::Mul, std::move(x),
                            SymExpr::constant(0));
    EXPECT_TRUE(e.isConstant());
    EXPECT_EQ(*e.tryEval(), 0u);
}

// ---- Load ----

TEST(SymExpr, LoadConstantAddr) {
    auto e = SymExpr::load(SymExpr::constant(0x401000));
    EXPECT_EQ(e.kind, SymExpr::Kind::Load);
    EXPECT_EQ(e.tryEval(), std::nullopt);
}

// ---- Opaque ----

TEST(SymExpr, OpaqueSystemReg) {
    auto e = SymExpr::opaque(SymExpr::OpaqueKind::SystemReg, 42);
    EXPECT_EQ(e.kind, SymExpr::Kind::Opaque);
    EXPECT_EQ(e.opaque_kind, SymExpr::OpaqueKind::SystemReg);
    EXPECT_EQ(e.reg_id, 42u);
    EXPECT_EQ(e.tryEval(), std::nullopt);
}

TEST(SymExpr, OpaqueCallResult) {
    auto e = SymExpr::opaque(SymExpr::OpaqueKind::CallResult, 0,
                             "__tls_get_addr");
    EXPECT_EQ(e.kind, SymExpr::Kind::Opaque);
    EXPECT_EQ(e.opaque_kind, SymExpr::OpaqueKind::CallResult);
    EXPECT_EQ(e.symbol, "__tls_get_addr");
}

// ---- Tree queries ----

TEST(SymExpr, InvolvesLoad) {
    // Direct Load
    EXPECT_TRUE(SymExpr::load(SymExpr::constant(0x1000)).involvesLoad());

    // Plain constant
    EXPECT_FALSE(SymExpr::constant(42).involvesLoad());

    // Add(Load(...), Const) -- nested
    auto nested = SymExpr::binop(SymExpr::BinOp::Add,
                                 SymExpr::load(SymExpr::constant(0x1000)),
                                 SymExpr::constant(8));
    EXPECT_TRUE(nested.involvesLoad());
}

TEST(SymExpr, InvolvesOpaque) {
    auto e = SymExpr::opaque(SymExpr::OpaqueKind::SegmentBase, 1);
    EXPECT_TRUE(e.involvesOpaque(SymExpr::OpaqueKind::SegmentBase));
    EXPECT_FALSE(e.involvesOpaque(SymExpr::OpaqueKind::SystemReg));
}

TEST(SymExpr, InvolvesCall) {
    auto e = SymExpr::opaque(SymExpr::OpaqueKind::CallResult, 0,
                             "__tls_get_addr");
    EXPECT_TRUE(e.involvesCall("__tls_get_addr"));
    EXPECT_FALSE(e.involvesCall("other"));
    // Empty name = match ANY CallResult
    EXPECT_TRUE(e.involvesCall(""));
}

TEST(SymExpr, InvolvesCallAnyMatchesAll) {
    // A CallResult with empty symbol should also match empty-name query
    auto e = SymExpr::opaque(SymExpr::OpaqueKind::CallResult, 0, "");
    EXPECT_TRUE(e.involvesCall(""));
    // But NOT a specific name
    EXPECT_FALSE(e.involvesCall("foo"));
    // And a non-CallResult opaque should not match
    auto sys = SymExpr::opaque(SymExpr::OpaqueKind::SystemReg, 0);
    EXPECT_FALSE(sys.involvesCall(""));
}

TEST(SymExpr, FindLoadAddr) {
    // Load(Add(Const(A), Const(B)))
    auto addr = SymExpr::binop(SymExpr::BinOp::Add,
                               SymExpr::constant(0x1000),
                               SymExpr::constant(0x200));
    // addr is folded to Const(0x1200), but let's check findLoadAddr returns it
    auto e = SymExpr::load(std::move(addr));
    const SymExpr* found = e.findLoadAddr();
    ASSERT_NE(found, nullptr);
    // The addr was constant-folded to 0x1200
    EXPECT_TRUE(found->isConstant());
    EXPECT_EQ(*found->tryEval(), 0x1200u);
}

TEST(SymExpr, TryEvalNestedBinop) {
    // Add(Sub(Const(10), Const(3)), Mul(Const(2), Const(4)))
    // = 7 + 8 = 15
    // Both Sub and Mul get folded, then Add gets folded too
    auto e = SymExpr::binop(
        SymExpr::BinOp::Add,
        SymExpr::binop(SymExpr::BinOp::Sub,
                       SymExpr::constant(10),
                       SymExpr::constant(3)),
        SymExpr::binop(SymExpr::BinOp::Mul,
                       SymExpr::constant(2),
                       SymExpr::constant(4)));
    ASSERT_TRUE(e.isConstant());
    EXPECT_EQ(*e.tryEval(), 15u);
}

TEST(SymExpr, TryEvalWithUnknownChild) {
    // Add(Unknown, Const(5)) -- cannot evaluate
    auto e = SymExpr::binop(SymExpr::BinOp::Add,
                            SymExpr::unknown(),
                            SymExpr::constant(5));
    EXPECT_EQ(e.tryEval(), std::nullopt);
}

// ---- Extend ----

TEST(SymExpr, ExtendConstantFold) {
    // ZExt of 0xFF to 8 bits: 0xFF & 0xFF = 0xFF
    {
        auto e = SymExpr::extend(SymExpr::ExtOp::ZExt,
                                 SymExpr::constant(0x1FF), 8);
        EXPECT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 0xFFu);
    }
    // SExt of 0x80 to 8 bits: sign bit set, extend to 64-bit
    {
        auto e = SymExpr::extend(SymExpr::ExtOp::SExt,
                                 SymExpr::constant(0x80), 8);
        EXPECT_TRUE(e.isConstant());
        // 0x80 with bit 7 set -> sign-extended to all 1s in upper bits
        EXPECT_EQ(static_cast<int64_t>(*e.tryEval()), -128);
    }
    // Trunc of 0x12345678 to 16 bits
    {
        auto e = SymExpr::extend(SymExpr::ExtOp::Trunc,
                                 SymExpr::constant(0x12345678), 16);
        EXPECT_TRUE(e.isConstant());
        EXPECT_EQ(*e.tryEval(), 0x5678u);
    }
}

// ---- findOpaque nested ----

TEST(SymExpr, FindOpaqueNested) {
    // Add(Opaque(SegmentBase, FS_id), Const(0x28))
    auto e = SymExpr::binop(SymExpr::BinOp::Add,
                            SymExpr::opaque(SymExpr::OpaqueKind::SegmentBase, 99),
                            SymExpr::constant(0x28));
    const SymExpr* found = e.findOpaque(SymExpr::OpaqueKind::SegmentBase);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->kind, SymExpr::Kind::Opaque);
    EXPECT_EQ(found->opaque_kind, SymExpr::OpaqueKind::SegmentBase);
    EXPECT_EQ(found->reg_id, 99u);

    // Should not find SystemReg
    EXPECT_EQ(e.findOpaque(SymExpr::OpaqueKind::SystemReg), nullptr);
}
