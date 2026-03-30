/// @file test_handlers.cpp
/// @brief Unit tests for all 55 opcode handlers (Phase 6).
///
/// Tests call handlers directly (no dispatcher needed). VMContext is
/// set up via build_test_blob + load_blob, then encoded_regs are
/// populated manually via encode_register().

#include "test_blob_builder.hpp"

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_encoding.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Encoding;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void fill_test_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

/// Build a minimal blob and load it, returning the LoadedVM.
/// The blob_storage must outlive the returned LoadedVM.
static LoadedVM load_test_vm(std::vector<uint8_t>& blob_storage,
                             uint16_t live_regs = 0xFFFF,
                             const std::vector<uint64_t>& pool = {}) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = live_regs;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions.push_back(halt);

    blob_storage = build_test_blob(seed, {bb}, pool);
    VmSecurityConfig config;
    auto result = load_blob(blob_storage.data(), blob_storage.size(),
                            seed, config);
    if (!result.has_value()) {
        ADD_FAILURE() << "load_blob failed with code "
                      << static_cast<uint32_t>(result.error());
        return LoadedVM{};
    }
    return std::move(result.value());
}

/// Build a DecodedInsn with given fields (minimal construction).
static DecodedInsn make_insn(VmOpcode op, uint8_t reg_a = 0,
                              uint8_t reg_b = 1, uint32_t aux = 0,
                              uint8_t condition = 0) {
    DecodedInsn insn{};
    insn.opcode = op;
    insn.reg_a = reg_a;
    insn.reg_b = reg_b;
    insn.aux = aux;
    insn.condition = condition;
    return insn;
}

// ---------------------------------------------------------------------------
// Test 1: MOVE
// ---------------------------------------------------------------------------

TEST(Handlers, Move) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    // Encode known values into r0 and r1
    ctx.encoded_regs[0] = encode_register(ctx, 0, 0xAAAA);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 0xBBBB);

    auto insn = make_insn(VmOpcode::MOVE, 0, 1);
    auto result = handlers::handle_move(ctx, insn);
    ASSERT_TRUE(result.has_value());

    // After MOVE r0, r1: r0 should contain r1's encoded value
    EXPECT_EQ(ctx.encoded_regs[0], ctx.encoded_regs[1]);
}

// ---------------------------------------------------------------------------
// Test 2: LOAD_CONST
// ---------------------------------------------------------------------------

TEST(Handlers, LoadConst) {
    std::vector<uint8_t> blob;
    std::vector<uint64_t> pool = {0xDEADBEEFCAFEBABE};
    auto vm = load_test_vm(blob, 0xFFFF, pool);
    auto& ctx = vm.ctx;

    auto insn = make_insn(VmOpcode::LOAD_CONST, 0, 0, 0);  // pool index 0
    auto result = handlers::handle_load_const(ctx, insn);
    ASSERT_TRUE(result.has_value());

    // Decode what we loaded and verify it matches the pool plaintext
    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0xDEADBEEFCAFEBABEu);
}

// ---------------------------------------------------------------------------
// Test 3: XOR homomorphic
// ---------------------------------------------------------------------------

TEST(Handlers, XorHomomorphic) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 0x123456789ABCDEF0;
    uint64_t b = 0xFEDCBA9876543210;
    ctx.encoded_regs[0] = encode_register(ctx, 0, a);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b);

    auto insn = make_insn(VmOpcode::XOR, 0, 1);
    auto result = handlers::handle_xor(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, a ^ b);
}

// ---------------------------------------------------------------------------
// Test 4: AND homomorphic
// ---------------------------------------------------------------------------

TEST(Handlers, AndHomomorphic) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 0xFF00FF00FF00FF00;
    uint64_t b = 0x0F0F0F0F0F0F0F0F;
    ctx.encoded_regs[0] = encode_register(ctx, 0, a);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b);

    auto insn = make_insn(VmOpcode::AND, 0, 1);
    auto result = handlers::handle_and(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, a & b);
}

// ---------------------------------------------------------------------------
// Test 5: NOT homomorphic
// ---------------------------------------------------------------------------

TEST(Handlers, NotHomomorphic) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 0x123456789ABCDEF0;
    ctx.encoded_regs[0] = encode_register(ctx, 0, a);

    auto insn = make_insn(VmOpcode::NOT, 0, 0);
    auto result = handlers::handle_not(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, ~a);
}

// ---------------------------------------------------------------------------
// Test 6: ADD (MBA)
// ---------------------------------------------------------------------------

TEST(Handlers, AddMba) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    struct TestCase { uint64_t a; uint64_t b; };
    TestCase cases[] = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1},
        {0x7F, 1},         // byte carry
        {0xFF, 1},         // byte overflow
        {100, 200},
        {0x12345678, 0x87654321},
    };

    for (const auto& tc : cases) {
        ctx.encoded_regs[0] = encode_register(ctx, 0, tc.a);
        ctx.encoded_regs[1] = encode_register(ctx, 1, tc.b);

        auto insn = make_insn(VmOpcode::ADD, 0, 1);
        auto result = handlers::handle_add(ctx, insn);
        ASSERT_TRUE(result.has_value())
            << "ADD failed for a=" << tc.a << " b=" << tc.b;

        uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
        EXPECT_EQ(decoded, tc.a + tc.b)
            << "ADD mismatch: a=" << tc.a << " b=" << tc.b
            << " expected=" << (tc.a + tc.b) << " got=" << decoded;
    }
}

// ---------------------------------------------------------------------------
// Test 7: SUB (bridge)
// ---------------------------------------------------------------------------

TEST(Handlers, SubBridge) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 1000;
    uint64_t b = 300;
    ctx.encoded_regs[0] = encode_register(ctx, 0, a);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b);

    auto insn = make_insn(VmOpcode::SUB, 0, 1);
    auto result = handlers::handle_sub(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, a - b);
}

// ---------------------------------------------------------------------------
// Test 8: MUL (bridge)
// ---------------------------------------------------------------------------

TEST(Handlers, MulBridge) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 12345;
    uint64_t b = 67890;
    ctx.encoded_regs[0] = encode_register(ctx, 0, a);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b);

    auto insn = make_insn(VmOpcode::MUL, 0, 1);
    auto result = handlers::handle_mul(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, a * b);
}

// ---------------------------------------------------------------------------
// Test 9: CMP + flag check
// ---------------------------------------------------------------------------

TEST(Handlers, Cmp) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    // Test equal
    ctx.encoded_regs[0] = encode_register(ctx, 0, 42);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 42);
    auto insn = make_insn(VmOpcode::CMP, 0, 1);
    ASSERT_TRUE(handlers::handle_cmp(ctx, insn).has_value());
    EXPECT_TRUE(ctx.vm_flags & 0x01);   // ZF set
    EXPECT_FALSE(ctx.vm_flags & 0x02);  // SF clear

    // Test less than (signed)
    ctx.encoded_regs[0] = encode_register(ctx, 0, 10);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 20);
    ASSERT_TRUE(handlers::handle_cmp(ctx, insn).has_value());
    EXPECT_FALSE(ctx.vm_flags & 0x01);  // ZF clear
    EXPECT_TRUE(ctx.vm_flags & 0x02);   // SF set (10-20 = negative)

    // Test unsigned: a < b sets CF
    ctx.encoded_regs[0] = encode_register(ctx, 0, 5);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 100);
    ASSERT_TRUE(handlers::handle_cmp(ctx, insn).has_value());
    EXPECT_TRUE(ctx.vm_flags & 0x04);   // CF set (unsigned borrow)
}

// ---------------------------------------------------------------------------
// Test 10: HALT
// ---------------------------------------------------------------------------

TEST(Handlers, Halt) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    EXPECT_FALSE(ctx.halted);
    auto insn = make_insn(VmOpcode::HALT);
    auto result = handlers::handle_halt(ctx, insn);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(ctx.halted);
}

// ---------------------------------------------------------------------------
// Test 11: JMP
// ---------------------------------------------------------------------------

TEST(Handlers, Jmp) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.branch_taken = false;
    auto insn = make_insn(VmOpcode::JMP, 0, 0, 42);  // target BB = 42
    auto result = handlers::handle_jmp(ctx, insn);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(ctx.branch_taken);
    EXPECT_EQ(ctx.branch_target_bb, 42u);
}

// ---------------------------------------------------------------------------
// Test 12: PUSH + POP round-trip
// ---------------------------------------------------------------------------

TEST(Handlers, PushPop) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t val = 0xCAFEBABEDEADBEEF;
    ctx.encoded_regs[0] = encode_register(ctx, 0, val);

    auto push_insn = make_insn(VmOpcode::PUSH, 0);
    auto push_result = handlers::handle_push(ctx, push_insn);
    ASSERT_TRUE(push_result.has_value());

    // Clear r0 to ensure POP actually restores it
    ctx.encoded_regs[0] = encode_register(ctx, 0, 0);

    auto pop_insn = make_insn(VmOpcode::POP, 0);
    auto pop_result = handlers::handle_pop(ctx, pop_insn);
    ASSERT_TRUE(pop_result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, val);
}

// ---------------------------------------------------------------------------
// Test 13: NOP writes to trash_regs
// ---------------------------------------------------------------------------

TEST(Handlers, Nop) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[1] = encode_register(ctx, 1, 0x42);
    ctx.trash_regs[0] = 0;

    auto insn = make_insn(VmOpcode::NOP, 0, 1);
    auto result = handlers::handle_nop(ctx, insn);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(ctx.trash_regs[0], ctx.encoded_regs[1]);
}

// ---------------------------------------------------------------------------
// Test 14: SEXT8
// ---------------------------------------------------------------------------

TEST(Handlers, Sext8) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    // 0xFF sign-extended from 8 bits = -1 = 0xFFFFFFFFFFFFFFFF
    ctx.encoded_regs[0] = encode_register(ctx, 0, 0xFF);

    auto insn = make_insn(VmOpcode::SEXT8, 0);
    auto result = handlers::handle_sext8(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0xFFFFFFFFFFFFFFFFu);

    // 0x7F sign-extended = 0x7F (positive)
    ctx.encoded_regs[0] = encode_register(ctx, 0, 0x7F);
    ASSERT_TRUE(handlers::handle_sext8(ctx, insn).has_value());
    decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0x7Fu);
}

// ---------------------------------------------------------------------------
// Test 15: ADD carry chain (0xFFFFFFFFFFFFFFFF + 1 = 0)
// ---------------------------------------------------------------------------

TEST(Handlers, AddCarryChain) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[0] = encode_register(ctx, 0, 0xFFFFFFFFFFFFFFFF);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 1);

    auto insn = make_insn(VmOpcode::ADD, 0, 1);
    auto result = handlers::handle_add(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0u)
        << "ADD 0xFFFFFFFFFFFFFFFF + 1 should overflow to 0, got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 16: OR homomorphic
// ---------------------------------------------------------------------------

TEST(Handlers, OrHomomorphic) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 0xF0F0F0F0F0F0F0F0;
    uint64_t b = 0x0F0F0F0F0F0F0F0F;
    ctx.encoded_regs[0] = encode_register(ctx, 0, a);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b);

    auto insn = make_insn(VmOpcode::OR, 0, 1);
    auto result = handlers::handle_or(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, a | b);
}

// ---------------------------------------------------------------------------
// Test 17: DIV by zero returns zero
// ---------------------------------------------------------------------------

TEST(Handlers, DivByZero) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[0] = encode_register(ctx, 0, 42);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 0);

    auto insn = make_insn(VmOpcode::DIV, 0, 1);
    auto result = handlers::handle_div(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0u);
}

// ---------------------------------------------------------------------------
// Test 18: JCC conditional branch
// ---------------------------------------------------------------------------

TEST(Handlers, JccConditional) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    // Set ZF
    ctx.vm_flags = 0x01;
    ctx.branch_taken = false;

    // condition=0 => EQ (branch if ZF set)
    auto insn = make_insn(VmOpcode::JCC, 0, 0, 99, 0);
    auto result = handlers::handle_jcc(ctx, insn);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(ctx.branch_taken);
    EXPECT_EQ(ctx.branch_target_bb, 99u);

    // condition=1 => NE (branch if ZF clear) -- should NOT branch
    ctx.branch_taken = false;
    insn.condition = 1;
    ASSERT_TRUE(handlers::handle_jcc(ctx, insn).has_value());
    EXPECT_FALSE(ctx.branch_taken);
}

// ---------------------------------------------------------------------------
// Test 19: NEG
// ---------------------------------------------------------------------------

TEST(Handlers, Neg) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[0] = encode_register(ctx, 0, 42);
    auto insn = make_insn(VmOpcode::NEG, 0);
    auto result = handlers::handle_neg(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, static_cast<uint64_t>(-42LL));
}

// ---------------------------------------------------------------------------
// Test 20: SHL
// ---------------------------------------------------------------------------

TEST(Handlers, Shl) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[0] = encode_register(ctx, 0, 1);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 10);

    auto insn = make_insn(VmOpcode::SHL, 0, 1);
    auto result = handlers::handle_shl(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 1u << 10);
}

// ---------------------------------------------------------------------------
// Test 21: LOAD_CTX and STORE_CTX
// ---------------------------------------------------------------------------

TEST(Handlers, LoadStoreCtx) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    // LOAD_CTX: read vm_sp (field 1) into r0
    auto load_insn = make_insn(VmOpcode::LOAD_CTX, 0, 0, 1);
    ASSERT_TRUE(handlers::handle_load_ctx(ctx, load_insn).has_value());
    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, ctx.vm_sp);

    // STORE_CTX: write new vm_sp from r0
    ctx.encoded_regs[0] = encode_register(ctx, 0, 2048);
    auto store_insn = make_insn(VmOpcode::STORE_CTX, 0, 0, 1);
    ASSERT_TRUE(handlers::handle_store_ctx(ctx, store_insn).has_value());
    EXPECT_EQ(ctx.vm_sp, 2048u);
}

// ---------------------------------------------------------------------------
// Test 22: SET_FLAG / GET_FLAG
// ---------------------------------------------------------------------------

TEST(Handlers, SetGetFlag) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.vm_flags = 0;

    // SET_FLAG: set CF (bit 2)
    auto set_insn = make_insn(VmOpcode::SET_FLAG, 0, 0, 1, 2);  // flag=CF, value=1
    ASSERT_TRUE(handlers::handle_set_flag(ctx, set_insn).has_value());
    EXPECT_TRUE(ctx.vm_flags & 0x04);

    // GET_FLAG: read CF into r0
    auto get_insn = make_insn(VmOpcode::GET_FLAG, 0, 0, 0, 2);  // flag=CF
    ASSERT_TRUE(handlers::handle_get_flag(ctx, get_insn).has_value());
    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 1u);
}

// ---------------------------------------------------------------------------
// Test 23: ZEXT32
// ---------------------------------------------------------------------------

TEST(Handlers, Zext32) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[0] = encode_register(ctx, 0, 0xFFFFFFFFDEADBEEF);
    auto insn = make_insn(VmOpcode::ZEXT32, 0);
    ASSERT_TRUE(handlers::handle_zext32(ctx, insn).has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0xDEADBEEFu);
}

// ---------------------------------------------------------------------------
// Test 24: MOD
// ---------------------------------------------------------------------------

TEST(Handlers, Mod) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    ctx.encoded_regs[0] = encode_register(ctx, 0, 17);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 5);

    auto insn = make_insn(VmOpcode::MOD, 0, 1);
    auto result = handlers::handle_mod(ctx, insn);
    ASSERT_TRUE(result.has_value());

    uint64_t decoded = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 2u);
}

// ---------------------------------------------------------------------------
// Test 25: FENCE (just verify it doesn't crash)
// ---------------------------------------------------------------------------

TEST(Handlers, Fence) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    auto insn = make_insn(VmOpcode::FENCE);
    auto result = handlers::handle_fence(ctx, insn);
    ASSERT_TRUE(result.has_value());
}

// ---------------------------------------------------------------------------
// Test 26: NATIVE_CALL returns error
// ---------------------------------------------------------------------------

TEST(Handlers, NativeCallFails) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    auto insn = make_insn(VmOpcode::NATIVE_CALL);
    auto result = handlers::handle_native_call(ctx, insn);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeCallBridgeFailed);
}

// ---------------------------------------------------------------------------
// Test 27: Handler table has no nulls and all 55 entries are populated
// ---------------------------------------------------------------------------

TEST(Handlers, TableComplete) {
    auto table = init_handler_table();
    for (size_t i = 0; i < VM_OPCODE_COUNT; ++i) {
        EXPECT_NE(table[i], nullptr)
            << "Handler table entry " << i << " is null";
    }
    // Verify specific entries are not the stub
    EXPECT_NE(table[static_cast<size_t>(VmOpcode::MOVE)],
              handlers::stub_handler);
    EXPECT_NE(table[static_cast<size_t>(VmOpcode::HALT)],
              handlers::stub_handler);
    EXPECT_NE(table[static_cast<size_t>(VmOpcode::ADD)],
              handlers::stub_handler);
    EXPECT_NE(table[static_cast<size_t>(VmOpcode::XOR)],
              handlers::stub_handler);
    EXPECT_NE(table[static_cast<size_t>(VmOpcode::NOP)],
              handlers::stub_handler);
}
