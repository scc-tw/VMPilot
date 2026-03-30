/// @file test_vm_stub_entry.cpp
/// @brief Unit tests for vm_stub_entry() — the thin C-linkage shim.
///
/// Tests verify:
///   - Valid VmStubArgs → correct return value
///   - Bad version → INT64_MIN (debug builds)
///   - Null args → INT64_MIN (debug builds)
///   - Null blob → INT64_MIN (debug builds)
///   - Initial register passthrough

#include "test_blob_builder.hpp"

#include <vm_stub_entry.hpp>
#include <vm_entry.hpp>
#include <vm/vm_stub_args.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <climits>
#include <cstring>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ---------------------------------------------------------------------------
// Helpers (same pattern as test_vm_entry.cpp)
// ---------------------------------------------------------------------------

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(0xA0 + i);
}

static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(VmStubEntry, ValidArgsReturnsCorrectly) {
    uint8_t seed[32];
    fill_seed(seed);

    // Build a simple HALT blob
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xA0);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    VmStubArgs args{};
    args.version         = VM_STUB_ABI_VERSION;
    args.num_regs        = 0;
    args.load_base_delta = 0;
    args.blob_data       = blob.data();
    args.blob_size       = static_cast<uint32_t>(blob.size());
    args.stored_seed     = seed;
    args.initial_regs    = nullptr;

    int64_t result = vm_stub_entry(&args);
    // HALT with no register setup → return_value = decoded reg 0.
    // Should NOT be INT64_MIN (error sentinel).
    EXPECT_NE(result, INT64_MIN) << "vm_stub_entry returned error sentinel";
}

TEST(VmStubEntry, BadVersionReturnsError) {
    VmStubArgs args{};
    args.version = VM_STUB_ABI_VERSION + 999;

    int64_t result = vm_stub_entry(&args);
    EXPECT_EQ(result, INT64_MIN);
}

TEST(VmStubEntry, NullArgsReturnsError) {
    int64_t result = vm_stub_entry(nullptr);
    EXPECT_EQ(result, INT64_MIN);
}

TEST(VmStubEntry, NullBlobReturnsError) {
    VmStubArgs args{};
    args.version   = VM_STUB_ABI_VERSION;
    args.blob_data = nullptr;
    args.blob_size = 0;

    int64_t result = vm_stub_entry(&args);
    EXPECT_EQ(result, INT64_MIN);
}

TEST(VmStubEntry, WithInitialRegs) {
    uint8_t seed[32];
    fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xA0);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    uint64_t regs[1] = {42};

    VmStubArgs args{};
    args.version         = VM_STUB_ABI_VERSION;
    args.num_regs        = 1;
    args.load_base_delta = 0;
    args.blob_data       = blob.data();
    args.blob_size       = static_cast<uint32_t>(blob.size());
    args.stored_seed     = seed;
    args.initial_regs    = regs;

    int64_t result = vm_stub_entry(&args);
    EXPECT_NE(result, INT64_MIN) << "vm_stub_entry returned error sentinel";
    // initial_regs[0]=42, encode→HALT→decode should return 42
    EXPECT_EQ(result, 42);
}
