/// @file test_mba_property.cpp
/// @brief Property-based tests for MBA arithmetic correctness (Phase 11+).
///
/// The most critical correctness test: for any random (a, b), the MBA ADD
/// must produce the correct result. This verifies carry propagation,
/// lane-0 bypass, branchless CMOV, and all 64 iterations.
///
/// Property: forall (a,b) in uint64_t x uint64_t:
///   decode(MBA_ADD(encode(a), encode(b))) == a + b
///
/// Why this is sufficient: if this property holds for random inputs,
/// then the MBA decomposition (XOR + AND + MBA_SHL1 loop) correctly
/// implements addition across all 8 byte lanes with full carry
/// propagation. Any bug in:
///   - SHL1_INTRA tables -> wrong intra-lane shift -> wrong carry
///   - CARRY_EXTRACT -> wrong carry bit -> wrong propagation
///   - INJECT_0/INJECT_1 -> wrong bit injection -> wrong sum
///   - Lane 0 bypass -> wrong starting carry -> all lanes wrong
///   - CMOV masking -> data-dependent selection -> random errors
/// would be caught by random testing with high probability.

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <random>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
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

/// Make flags byte from operand types and condition.
static uint8_t make_flags(uint8_t op_a_type, uint8_t op_b_type,
                          uint8_t condition = 0) {
    return static_cast<uint8_t>((op_a_type << 6) | (op_b_type << 4) |
                                (condition & 0x0F));
}

static VmSecurityConfig default_config() {
    VmSecurityConfig cfg;
    cfg.debug_mode = true;  // disable anti-debug for tests
    return cfg;
}

/// Reset VMContext for re-execution with new register values.
///
/// Resets the execution state (IP, enc_state, flags) back to the first BB
/// while preserving the loaded blob data. This allows running the same
/// program 1000s of times without re-loading the blob each time.
static void reset_for_reexecution(LoadedVM& vm) {
    auto& ctx = vm.ctx;

    // Re-enter first BB (resets enc_state, ip, encoding tables if needed)
    auto result = enter_basic_block(ctx, ctx.bb_metadata[0].bb_id);
    (void)result;  // should always succeed for valid blob

    ctx.halted = false;
    ctx.branch_taken = false;
    ctx.vm_flags = 0;

    // Re-initialize ORAM workspace
    std::memset(ctx.oblivious_workspace, 0, sizeof(ctx.oblivious_workspace));
    ctx.oram_nonce = 0;
    ctx.vm_sp = VM_OBLIVIOUS_SIZE;

    // Clear shadow stack
    ctx.shadow_depth = 0;

    // Clear all registers to zero (encoded zero)
    for (uint8_t r = 0; r < VM_REG_COUNT; ++r)
        ctx.encoded_regs[r] = encode_register(ctx, r, 0);
}

// ---------------------------------------------------------------------------
// Blob builders: create once, reuse for all iterations
// ---------------------------------------------------------------------------

/// Build a blob for binary operation: OP r0, r1; HALT
/// The caller loads it once, then reset+re-encode for each test pair.
static std::vector<uint8_t> build_binop_blob(uint8_t seed[32],
                                              VmOpcode op) {
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction binop{};
    binop.opcode = op;
    binop.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    binop.reg_a  = 0;
    binop.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {binop, halt};

    return build_test_blob(seed, {bb});
}

/// Build a blob for unary operation: OP r0; HALT
static std::vector<uint8_t> build_unaryop_blob(uint8_t seed[32],
                                                VmOpcode op) {
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction unop{};
    unop.opcode = op;
    unop.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    unop.reg_a  = 0;
    unop.reg_b  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {unop, halt};

    return build_test_blob(seed, {bb});
}

// ---------------------------------------------------------------------------
// Test 1: ADD random pairs (the "big" property test)
//
// Why this matters: The MBA ADD is the most complex encoded operation,
// involving 64 iterations of XOR + AND + SHL1 across 8 byte lanes.
// Any bug in the carry propagation chain would corrupt addition results,
// breaking all programs that use arithmetic.
// ---------------------------------------------------------------------------

TEST(MbaProperty, AddRandomPairs) {
    uint8_t seed[32];
    fill_test_seed(seed);
    auto blob = build_binop_blob(seed, VmOpcode::ADD);
    auto config = default_config();

    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto& vm = *loaded;
    std::mt19937_64 rng(42);  // fixed seed for reproducibility

    for (int i = 0; i < 1000; ++i) {
        uint64_t a = rng();
        uint64_t b = rng();
        uint64_t expected = a + b;

        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, a);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, b);

        auto result = execute(vm.ctx, config);
        ASSERT_TRUE(result.has_value())
            << "execute failed at iteration " << i
            << " for a=0x" << std::hex << a << " b=0x" << b
            << ": " << static_cast<uint32_t>(result.error());

        uint64_t actual = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);
        EXPECT_EQ(actual, expected)
            << "ADD mismatch at iteration " << i
            << ": a=0x" << std::hex << a << " b=0x" << b
            << " expected=0x" << expected << " got=0x" << actual;
    }
}

// ---------------------------------------------------------------------------
// Test 2: ADD edge cases
//
// Why this matters: Edge cases exercise specific carry propagation paths
// that random testing might miss. Lane boundary carries, all-ones overflow,
// and zero-identity are all critical for correctness.
// ---------------------------------------------------------------------------

TEST(MbaProperty, AddEdgeCases) {
    uint8_t seed[32];
    fill_test_seed(seed);
    auto blob = build_binop_blob(seed, VmOpcode::ADD);
    auto config = default_config();

    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto& vm = *loaded;
    constexpr uint64_t MAX = 0xFFFFFFFFFFFFFFFFULL;

    struct TestCase { uint64_t a; uint64_t b; const char* desc; };
    TestCase cases[] = {
        {0, 0, "zero + zero"},
        {1, 0, "one + zero"},
        {0, 1, "zero + one"},
        {MAX, 0, "MAX + zero"},
        {MAX, 1, "MAX + 1 (full overflow to 0)"},
        {MAX, MAX, "MAX + MAX (overflow to MAX-1)"},
        // Carry across lane boundary: 0x01__0000_0000_0000 + same
        {0x0100000000000000ULL, 0x0100000000000000ULL,
         "carry across lane 6->7 boundary"},
        // Carry within each odd lane
        {0x00FF00FF00FF00FFULL, 0x0001000100010001ULL,
         "carry within each odd lane"},
        // Carry from bit 7 of each lane (0x80 + 0x80 = carry out)
        {0x8080808080808080ULL, 0x8080808080808080ULL,
         "carry from bit 7 of each byte lane"},
        // Carry at every lane boundary
        {0xFF00FF00FF00FF00ULL, 0x0100010001000100ULL,
         "carry at every even->odd lane boundary"},
        // Single byte carry: 0xFF + 1 in lane 0
        {0xFF, 1, "single byte overflow in lane 0"},
        // Large values with no carry
        {0x0101010101010101ULL, 0x0202020202020202ULL,
         "no carry, all lanes independent"},
        // Alternating pattern
        {0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL,
         "alternating bits sum to MAX"},
    };

    for (const auto& tc : cases) {
        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, tc.a);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, tc.b);

        auto result = execute(vm.ctx, config);
        ASSERT_TRUE(result.has_value())
            << "execute failed for " << tc.desc;

        uint64_t actual = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);
        EXPECT_EQ(actual, tc.a + tc.b)
            << tc.desc << ": a=0x" << std::hex << tc.a
            << " b=0x" << tc.b
            << " expected=0x" << (tc.a + tc.b)
            << " got=0x" << actual;
    }
}

// ---------------------------------------------------------------------------
// Test 3: SUB random pairs
//
// Why this matters: SUB is implemented via the bridge (decode, subtract,
// re-encode), so it exercises a different code path than ADD. Verifying
// SUB correctness ensures the bridge pathway handles the full uint64_t
// range without encoding/decoding errors.
// ---------------------------------------------------------------------------

TEST(MbaProperty, SubRandomPairs) {
    uint8_t seed[32];
    fill_test_seed(seed);
    auto blob = build_binop_blob(seed, VmOpcode::SUB);
    auto config = default_config();

    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto& vm = *loaded;
    std::mt19937_64 rng(42);

    for (int i = 0; i < 500; ++i) {
        uint64_t a = rng();
        uint64_t b = rng();
        uint64_t expected = a - b;  // unsigned wrapping subtraction

        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, a);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, b);

        auto result = execute(vm.ctx, config);
        ASSERT_TRUE(result.has_value())
            << "execute failed at iteration " << i;

        uint64_t actual = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);
        EXPECT_EQ(actual, expected)
            << "SUB mismatch at iteration " << i
            << ": a=0x" << std::hex << a << " b=0x" << b;
    }
}

// ---------------------------------------------------------------------------
// Test 4: NEG random values
//
// Why this matters: NEG is the two's complement negation. It is implemented
// via the bridge (decode, negate, re-encode). Testing with random values
// ensures the bridge round-trip is lossless for all bit patterns.
// ---------------------------------------------------------------------------

TEST(MbaProperty, NegRandom) {
    uint8_t seed[32];
    fill_test_seed(seed);
    auto blob = build_unaryop_blob(seed, VmOpcode::NEG);
    auto config = default_config();

    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto& vm = *loaded;
    std::mt19937_64 rng(42);

    for (int i = 0; i < 500; ++i) {
        uint64_t a = rng();
        uint64_t expected = static_cast<uint64_t>(-static_cast<int64_t>(a));

        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, a);

        auto result = execute(vm.ctx, config);
        ASSERT_TRUE(result.has_value())
            << "execute failed at iteration " << i;

        uint64_t actual = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);
        EXPECT_EQ(actual, expected)
            << "NEG mismatch at iteration " << i
            << ": a=0x" << std::hex << a;
    }
}

// ---------------------------------------------------------------------------
// Test 5: MUL random pairs (smaller values to keep test time reasonable)
//
// Why this matters: MUL uses the bridge pathway and is the most
// resource-intensive arithmetic operation. Testing with random values
// ensures that the decode-multiply-encode round-trip works for the
// full input space.
// ---------------------------------------------------------------------------

TEST(MbaProperty, MulRandomPairs) {
    uint8_t seed[32];
    fill_test_seed(seed);
    auto blob = build_binop_blob(seed, VmOpcode::MUL);
    auto config = default_config();

    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto& vm = *loaded;
    std::mt19937_64 rng(42);

    for (int i = 0; i < 500; ++i) {
        // Use full 64-bit random values; multiplication wraps naturally
        uint64_t a = rng();
        uint64_t b = rng();
        uint64_t expected = a * b;  // unsigned wrapping multiplication

        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, a);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, b);

        auto result = execute(vm.ctx, config);
        ASSERT_TRUE(result.has_value())
            << "execute failed at iteration " << i;

        uint64_t actual = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);
        EXPECT_EQ(actual, expected)
            << "MUL mismatch at iteration " << i
            << ": a=0x" << std::hex << a << " b=0x" << b;
    }
}

// ---------------------------------------------------------------------------
// Test 6: ADD commutativity
//
// Why this matters: Commutativity (a+b == b+a) is a fundamental property
// of addition. If the MBA implementation has an asymmetry in how it
// processes left vs right operands (e.g., different register encoding
// tables), this test would catch it. It also verifies that the lane-0
// bypass and CMOV masking are operand-order-independent.
// ---------------------------------------------------------------------------

TEST(MbaProperty, AddCommutative) {
    uint8_t seed[32];
    fill_test_seed(seed);
    auto blob = build_binop_blob(seed, VmOpcode::ADD);
    auto config = default_config();

    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto& vm = *loaded;
    std::mt19937_64 rng(42);

    for (int i = 0; i < 200; ++i) {
        uint64_t a = rng();
        uint64_t b = rng();

        // Compute ADD(a, b)
        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, a);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, b);
        auto r1 = execute(vm.ctx, config);
        ASSERT_TRUE(r1.has_value());
        uint64_t ab = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);

        // Compute ADD(b, a)
        reset_for_reexecution(vm);
        vm.ctx.encoded_regs[0] = encode_register(vm.ctx, 0, b);
        vm.ctx.encoded_regs[1] = encode_register(vm.ctx, 1, a);
        auto r2 = execute(vm.ctx, config);
        ASSERT_TRUE(r2.has_value());
        uint64_t ba = decode_register(vm.ctx, 0, vm.ctx.encoded_regs[0]);

        EXPECT_EQ(ab, ba)
            << "ADD not commutative at iteration " << i
            << ": a=0x" << std::hex << a << " b=0x" << b
            << " ADD(a,b)=0x" << ab << " ADD(b,a)=0x" << ba;
    }
}

// ---------------------------------------------------------------------------
// Test 7: ADD associativity
//
// Why this matters: Associativity ((a+b)+c == a+(b+c)) tests that the
// MBA carry propagation is consistent across multiple additions. Any
// rounding error or lost carry in the first addition would compound
// in the second, making associativity a strong correctness check.
// ---------------------------------------------------------------------------

TEST(MbaProperty, AddAssociative) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Build a blob with: ADD r0, r1; ADD r0, r2; HALT
    // This computes (r0 + r1) + r2
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction add01{};
    add01.opcode = VmOpcode::ADD;
    add01.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add01.reg_a  = 0;
    add01.reg_b  = 1;

    TestInstruction add02{};
    add02.opcode = VmOpcode::ADD;
    add02.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add02.reg_a  = 0;
    add02.reg_b  = 2;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {add01, add02, halt};

    auto blob_ab_c = build_test_blob(seed, {bb});
    auto config = default_config();
    auto loaded_ab_c = load_blob(blob_ab_c.data(), blob_ab_c.size(),
                                  seed, config);
    ASSERT_TRUE(loaded_ab_c.has_value());

    // Build a blob for a + (b + c):
    // ADD r1, r2; ADD r0, r1; HALT
    TestBB bb2{};
    bb2.bb_id = 1;
    bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF;
    bb2.flags = 0;
    fill_epoch_seed(bb2.epoch_seed, 0xAA);

    TestInstruction add12{};
    add12.opcode = VmOpcode::ADD;
    add12.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add12.reg_a  = 1;
    add12.reg_b  = 2;

    TestInstruction add01b{};
    add01b.opcode = VmOpcode::ADD;
    add01b.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add01b.reg_a  = 0;
    add01b.reg_b  = 1;

    bb2.instructions = {add12, add01b, halt};

    auto blob_a_bc = build_test_blob(seed, {bb2});
    auto loaded_a_bc = load_blob(blob_a_bc.data(), blob_a_bc.size(),
                                  seed, config);
    ASSERT_TRUE(loaded_a_bc.has_value());

    std::mt19937_64 rng(42);

    for (int i = 0; i < 100; ++i) {
        uint64_t a = rng();
        uint64_t b = rng();
        uint64_t c = rng();

        // Compute (a + b) + c
        reset_for_reexecution(*loaded_ab_c);
        loaded_ab_c->ctx.encoded_regs[0] = encode_register(loaded_ab_c->ctx, 0, a);
        loaded_ab_c->ctx.encoded_regs[1] = encode_register(loaded_ab_c->ctx, 1, b);
        loaded_ab_c->ctx.encoded_regs[2] = encode_register(loaded_ab_c->ctx, 2, c);
        auto r1 = execute(loaded_ab_c->ctx, config);
        ASSERT_TRUE(r1.has_value());
        uint64_t ab_c = decode_register(loaded_ab_c->ctx, 0,
                                         loaded_ab_c->ctx.encoded_regs[0]);

        // Compute a + (b + c)
        reset_for_reexecution(*loaded_a_bc);
        loaded_a_bc->ctx.encoded_regs[0] = encode_register(loaded_a_bc->ctx, 0, a);
        loaded_a_bc->ctx.encoded_regs[1] = encode_register(loaded_a_bc->ctx, 1, b);
        loaded_a_bc->ctx.encoded_regs[2] = encode_register(loaded_a_bc->ctx, 2, c);
        auto r2 = execute(loaded_a_bc->ctx, config);
        ASSERT_TRUE(r2.has_value());
        uint64_t a_bc = decode_register(loaded_a_bc->ctx, 0,
                                         loaded_a_bc->ctx.encoded_regs[0]);

        EXPECT_EQ(ab_c, a_bc)
            << "ADD not associative at iteration " << i
            << ": a=0x" << std::hex << a << " b=0x" << b << " c=0x" << c
            << " (a+b)+c=0x" << ab_c << " a+(b+c)=0x" << a_bc;
    }
}
