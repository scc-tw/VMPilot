#include "test_blob_builder.hpp"

#include <oram.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_crypto.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Crypto;
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

/// Build a minimal 1-BB blob with a single HALT instruction and load it.
/// Returns a LoadedVM with a fully initialised VMContext (valid oram_key, etc.).
static LoadedVM make_test_vm() {
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

    static std::vector<uint8_t> blob_storage;
    blob_storage = build_test_blob(seed, {bb});

    VmSecurityConfig config;
    auto result = load_blob(blob_storage.data(), blob_storage.size(), seed, config);
    // Tests calling this helper assume it succeeds.
    return std::move(result.value());
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(Oram, WriteReadRoundTrip) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    auto wr = oram_write(ctx, 0, 0xDEADBEEFull);
    ASSERT_TRUE(wr.has_value()) << "oram_write failed: "
        << static_cast<uint32_t>(wr.error());

    auto rd = oram_read(ctx, 0);
    ASSERT_TRUE(rd.has_value()) << "oram_read failed: "
        << static_cast<uint32_t>(rd.error());
    EXPECT_EQ(rd.value(), 0xDEADBEEFull);
}

TEST(Oram, MultipleAddresses) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    struct Entry { uint32_t addr; uint64_t val; };
    Entry entries[] = {
        {    0, 0x1111111111111111ull },
        {    8, 0x2222222222222222ull },
        {   16, 0x3333333333333333ull },
        { 4088, 0x4444444444444444ull },
    };

    // Write all
    for (const auto& e : entries) {
        auto wr = oram_write(ctx, e.addr, e.val);
        ASSERT_TRUE(wr.has_value()) << "write at " << e.addr << " failed";
    }

    // Read all back and verify
    for (const auto& e : entries) {
        auto rd = oram_read(ctx, e.addr);
        ASSERT_TRUE(rd.has_value()) << "read at " << e.addr << " failed";
        EXPECT_EQ(rd.value(), e.val) << "mismatch at addr " << e.addr;
    }
}

TEST(Oram, RollingKeystream) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    // Snapshot the workspace before a write.
    uint8_t before[VM_OBLIVIOUS_SIZE];
    std::memcpy(before, ctx.oblivious_workspace, VM_OBLIVIOUS_SIZE);

    auto wr = oram_write(ctx, 0, 0xCAFEBABEull);
    ASSERT_TRUE(wr.has_value());

    // After the write, the entire workspace should differ because the fresh
    // nonce re-encrypts all 64 cache lines with new keystream.
    int changed_bytes = 0;
    for (uint32_t i = 0; i < VM_OBLIVIOUS_SIZE; ++i) {
        if (ctx.oblivious_workspace[i] != before[i])
            ++changed_bytes;
    }

    // With SipHash expansion producing distinct keystreams for different
    // nonces, virtually all bytes should change.  We require >90% changed
    // to allow for the astronomically unlikely case of a few colliding bytes.
    EXPECT_GT(changed_bytes, static_cast<int>(VM_OBLIVIOUS_SIZE * 0.9))
        << "Only " << changed_bytes << " of " << VM_OBLIVIOUS_SIZE
        << " bytes changed; rolling keystream re-encryption may be broken";
}

TEST(Oram, AlignmentViolation) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    auto wr = oram_write(ctx, 3, 0);
    ASSERT_FALSE(wr.has_value());
    EXPECT_EQ(wr.error(), DiagnosticCode::OramAddressUnaligned);

    auto rd = oram_read(ctx, 5);
    ASSERT_FALSE(rd.has_value());
    EXPECT_EQ(rd.error(), DiagnosticCode::OramAddressUnaligned);
}

TEST(Oram, BoundsViolation) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    auto wr = oram_write(ctx, 4096, 0);
    ASSERT_FALSE(wr.has_value());
    EXPECT_EQ(wr.error(), DiagnosticCode::OramAddressOutOfBounds);

    auto rd = oram_read(ctx, 4096);
    ASSERT_FALSE(rd.has_value());
    EXPECT_EQ(rd.error(), DiagnosticCode::OramAddressOutOfBounds);
}

TEST(Oram, PushPopLifo) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    // vm_sp starts at VM_OBLIVIOUS_SIZE (4096), grows downward
    ASSERT_EQ(ctx.vm_sp, VM_OBLIVIOUS_SIZE);

    for (uint64_t i = 1; i <= 5; ++i) {
        auto r = oram_push(ctx, i);
        ASSERT_TRUE(r.has_value()) << "push " << i << " failed";
    }

    // Pop and verify LIFO order: 5, 4, 3, 2, 1
    for (uint64_t expected = 5; expected >= 1; --expected) {
        auto r = oram_pop(ctx);
        ASSERT_TRUE(r.has_value()) << "pop failed";
        EXPECT_EQ(r.value(), expected);
    }
}

TEST(Oram, StackOverflow) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    // Fill the entire workspace via push (4096 / 8 = 512 entries)
    const uint32_t max_pushes = VM_OBLIVIOUS_SIZE / 8;
    for (uint32_t i = 0; i < max_pushes; ++i) {
        auto r = oram_push(ctx, static_cast<uint64_t>(i));
        ASSERT_TRUE(r.has_value()) << "push " << i << " failed unexpectedly";
    }

    // vm_sp should now be 0; next push should fail
    ASSERT_EQ(ctx.vm_sp, 0u);
    auto overflow = oram_push(ctx, 0xBAD);
    ASSERT_FALSE(overflow.has_value());
    EXPECT_EQ(overflow.error(), DiagnosticCode::StackOverflow);
}

TEST(Oram, StackUnderflow) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    // vm_sp == VM_OBLIVIOUS_SIZE means the stack is empty
    ASSERT_EQ(ctx.vm_sp, VM_OBLIVIOUS_SIZE);

    auto underflow = oram_pop(ctx);
    ASSERT_FALSE(underflow.has_value());
    EXPECT_EQ(underflow.error(), DiagnosticCode::StackUnderflow);
}

TEST(Oram, ReadDoesNotCorrupt) {
    auto vm = make_test_vm();
    VMContext& ctx = vm.ctx;

    const uint64_t sentinel = 0xFEEDFACE12345678ull;

    auto wr = oram_write(ctx, 0, sentinel);
    ASSERT_TRUE(wr.has_value());

    auto rd1 = oram_read(ctx, 0);
    ASSERT_TRUE(rd1.has_value());
    EXPECT_EQ(rd1.value(), sentinel);

    auto rd2 = oram_read(ctx, 0);
    ASSERT_TRUE(rd2.has_value());
    EXPECT_EQ(rd2.value(), sentinel);
}
