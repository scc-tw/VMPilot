/// @file test_boundary_values.cpp
/// @brief Boundary value tests for arithmetic operations.
///
/// Tests: wrapping ADD, underflow SUB, shift edge cases, DIV max by 1,
/// NEG of zero, NEG of one.

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_policy.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <climits>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ============================================================================
// ADD(UINT64_MAX, 1) wraps to 0
// ============================================================================

TEST(BoundaryValues, AddWrapsAtMax) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA0, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 1, 0, 1},
        {VmOpcode::ADD, OP_FLAGS_REG_REG, 0, 1, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {UINT64_MAX, 1});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u);
}

// ============================================================================
// SUB(0, 1) underflows to UINT64_MAX
// ============================================================================

TEST(BoundaryValues, SubUnderflowsAtZero) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA1, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 1, 0, 1},
        {VmOpcode::SUB, OP_FLAGS_REG_REG, 0, 1, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {0, 1});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, UINT64_MAX);
}

// ============================================================================
// SHL(42, 0) is identity
// ============================================================================

TEST(BoundaryValues, ShiftByZeroIsIdentity) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA2, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 1, 0, 1},
        {VmOpcode::SHL, OP_FLAGS_REG_REG, 0, 1, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {42, 0});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ============================================================================
// SHL(1, 63) sets MSB → 0x8000000000000000
// ============================================================================

TEST(BoundaryValues, ShiftBy63SetsMsb) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA3, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 1, 0, 1},
        {VmOpcode::SHL, OP_FLAGS_REG_REG, 0, 1, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {1, 63});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000000ull);
}

// ============================================================================
// DIV(UINT64_MAX, 1) → UINT64_MAX
// ============================================================================

TEST(BoundaryValues, DivMaxByOne) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA4, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 1, 0, 1},
        {VmOpcode::DIV, OP_FLAGS_REG_REG, 0, 1, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {UINT64_MAX, 1});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, UINT64_MAX);
}

// ============================================================================
// NEG(0) → 0
// ============================================================================

TEST(BoundaryValues, NegZero) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA5, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::NEG, OP_FLAGS_REG_NONE, 0, 0, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {0});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u);
}

// ============================================================================
// NEG(1) → UINT64_MAX
// ============================================================================

TEST(BoundaryValues, NegOne) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = make_single_bb_engine(seed, 0xA6, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::NEG, OP_FLAGS_REG_NONE, 0, 0, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    }, {1});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, UINT64_MAX);
}
