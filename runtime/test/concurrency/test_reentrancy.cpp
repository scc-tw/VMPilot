/// @file test_reentrancy.cpp
/// @brief Reentrancy tests for VmEngine create_reentrant.
///
/// Tests: child does not corrupt parent, two children from same parent.

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include <vm/vm_insn.hpp>
#include <gtest/gtest.h>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

TEST(Reentrancy, CreateReentrantDoesNotCorruptParent) {
    uint8_t seed[32]; fill_seed(seed);
    auto bb = make_test_bb(1, 0xA0, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    });
    auto blob = build_test_blob(seed, {bb}, {42});

    auto parent = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    // Snapshot parent state
    auto parent_ip = parent->execution().vm_ip;
    auto parent_enc = parent->execution().enc_state;

    // Create and execute child
    auto child = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm);
    ASSERT_TRUE(child.has_value());
    auto cr = child->execute();
    ASSERT_TRUE(cr.has_value());
    EXPECT_EQ(cr->return_value, 42u);

    // Parent state must be unchanged
    EXPECT_EQ(parent->execution().vm_ip, parent_ip)
        << "Child execution must not corrupt parent vm_ip";
    EXPECT_EQ(parent->execution().enc_state, parent_enc)
        << "Child execution must not corrupt parent enc_state";

    // Parent can still execute correctly
    auto pr = parent->execute();
    ASSERT_TRUE(pr.has_value());
    EXPECT_EQ(pr->return_value, 42u);
}

TEST(Reentrancy, TwoChildrenFromSameParent) {
    uint8_t seed[32]; fill_seed(seed);
    auto bb = make_test_bb(1, 0xA0, {
        {VmOpcode::LOAD_CONST, OP_FLAGS_POOL, 0, 0, 0},
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    });
    auto blob = build_test_blob(seed, {bb}, {77});

    auto parent = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    auto child1 = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm);
    auto child2 = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm);
    ASSERT_TRUE(child1.has_value());
    ASSERT_TRUE(child2.has_value());

    auto r1 = child1->execute();
    auto r2 = child2->execute();
    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r1->return_value, 77u);
    EXPECT_EQ(r2->return_value, 77u);
}

// ############################################################################
// EngineReentrant: shared immutable ref count (from test_engine_comprehensive.cpp)
// ############################################################################

TEST(Reentrancy, SharedImmutableRefCount) {
    uint8_t seed[32]; fill_seed(seed);
    auto bb = make_test_bb(1, 0xF6, {
        {VmOpcode::HALT, OP_FLAGS_NONE, 0, 0, 0},
    });
    auto blob = build_test_blob(seed, {bb});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto imm = engine->shared_immutable();
    EXPECT_EQ(imm.use_count(), 2);

    auto inner = VmEngine<DebugPolicy>::create_reentrant(imm);
    ASSERT_TRUE(inner.has_value());
    EXPECT_EQ(imm.use_count(), 3);

    auto r = inner->execute();
    ASSERT_TRUE(r.has_value());
}
