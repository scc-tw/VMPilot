/// @file test_native_bridge.cpp
/// @brief Unit tests for Phase 8 native call bridge.
///
/// Tests call_native() directly with simple native functions defined
/// in this file. VMContext is set up via the standard test blob builder
/// so encoding tables are populated.

#include "test_blob_builder.hpp"

#include <native_bridge.hpp>
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
// Test native functions (plain C-compatible, no exceptions)
// ---------------------------------------------------------------------------

static uint64_t native_add(uint64_t a, uint64_t b,
                           uint64_t, uint64_t,
                           uint64_t, uint64_t,
                           uint64_t, uint64_t) {
    return a + b;
}

static uint64_t native_get_42(uint64_t, uint64_t,
                              uint64_t, uint64_t,
                              uint64_t, uint64_t,
                              uint64_t, uint64_t) {
    return 42;
}

static uint64_t native_six_args(uint64_t a, uint64_t b,
                                uint64_t c, uint64_t d,
                                uint64_t e, uint64_t f,
                                uint64_t, uint64_t) {
    return a + b + c + d + e + f;
}

static uint64_t native_identity(uint64_t a, uint64_t,
                                uint64_t, uint64_t,
                                uint64_t, uint64_t,
                                uint64_t, uint64_t) {
    return a;
}

static uint64_t native_multiply(uint64_t a, uint64_t b,
                                uint64_t, uint64_t,
                                uint64_t, uint64_t,
                                uint64_t, uint64_t) {
    return a * b;
}

// ---------------------------------------------------------------------------
// Helpers (same pattern as test_handlers.cpp)
// ---------------------------------------------------------------------------

static void fill_test_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

static LoadedVM load_test_vm(std::vector<uint8_t>& blob_storage,
                             uint16_t live_regs = 0xFFFF) {
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

    blob_storage = build_test_blob(seed, {bb});
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

// ---------------------------------------------------------------------------
// Test 1: Call a simple two-argument function (add)
// ---------------------------------------------------------------------------

TEST(NativeBridge, CallSimpleFunction) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 100;
    uint64_t b = 200;

    // Encode arguments into registers
    uint64_t enc_a = encode_register(ctx, 0, a);
    uint64_t enc_b = encode_register(ctx, 1, b);

    uint64_t encoded_args[] = {enc_a, enc_b};
    uint8_t  arg_regs[] = {0, 1};

    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_add),
        encoded_args, arg_regs, 2);

    ASSERT_TRUE(result.has_value())
        << "call_native failed with code "
        << static_cast<uint32_t>(result.error());

    // Result is plaintext (caller re-encodes if needed)
    EXPECT_EQ(result.value(), 300u);
}

// ---------------------------------------------------------------------------
// Test 2: Call with zero arguments
// ---------------------------------------------------------------------------

TEST(NativeBridge, CallWithZeroArgs) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_get_42),
        nullptr, nullptr, 0);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42u);
}

// ---------------------------------------------------------------------------
// Test 3: Call with six arguments (max System V integer args)
// ---------------------------------------------------------------------------

TEST(NativeBridge, CallWithSixArgs) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    // Use values 10, 20, 30, 40, 50, 60 -- sum = 210
    uint64_t values[] = {10, 20, 30, 40, 50, 60};
    uint64_t encoded_args[6];
    uint8_t  arg_regs[6];

    for (uint8_t i = 0; i < 6; ++i) {
        arg_regs[i] = i;
        encoded_args[i] = encode_register(ctx, i, values[i]);
    }

    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_six_args),
        encoded_args, arg_regs, 6);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 210u);
}

// ---------------------------------------------------------------------------
// Test 4: Full round-trip: encode args -> call_native -> re-encode result
// ---------------------------------------------------------------------------

TEST(NativeBridge, ResultReEncoding) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t input_val = 0xDEADBEEF;

    // Encode input into register 0
    uint64_t enc_input = encode_register(ctx, 0, input_val);

    uint64_t encoded_args[] = {enc_input};
    uint8_t  arg_regs[] = {0};

    // Call identity function: returns plaintext input unchanged
    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_identity),
        encoded_args, arg_regs, 1);

    ASSERT_TRUE(result.has_value());
    uint64_t plain_result = result.value();
    EXPECT_EQ(plain_result, input_val);

    // Re-encode the result into register 2
    uint64_t enc_result = encode_register(ctx, 2, plain_result);

    // Decode and verify round-trip
    uint64_t decoded = decode_register(ctx, 2, enc_result);
    EXPECT_EQ(decoded, input_val);
}

// ---------------------------------------------------------------------------
// Test 5: Null target returns error
// ---------------------------------------------------------------------------

TEST(NativeBridge, NullTargetFails) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    auto result = call_native(ctx, 0, nullptr, nullptr, 0);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeCallBridgeFailed);
}

// ---------------------------------------------------------------------------
// Test 6: Too many arguments returns error
// ---------------------------------------------------------------------------

TEST(NativeBridge, TooManyArgsFails) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t encoded_args[9] = {};
    uint8_t  arg_regs[9] = {};

    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_get_42),
        encoded_args, arg_regs, 9);  // 9 > NATIVE_CALL_MAX_ARGS

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeCallBridgeFailed);
}

// ---------------------------------------------------------------------------
// Test 7: Multiply through bridge (mirrors class_c_bridge pattern)
// ---------------------------------------------------------------------------

TEST(NativeBridge, MultiplyThroughBridge) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 12345;
    uint64_t b = 67890;

    uint64_t enc_a = encode_register(ctx, 0, a);
    uint64_t enc_b = encode_register(ctx, 1, b);

    uint64_t encoded_args[] = {enc_a, enc_b};
    uint8_t  arg_regs[] = {0, 1};

    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_multiply),
        encoded_args, arg_regs, 2);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), a * b);

    // Re-encode and verify full round-trip
    uint64_t enc_result = encode_register(ctx, 0, result.value());
    uint64_t decoded = decode_register(ctx, 0, enc_result);
    EXPECT_EQ(decoded, a * b);
}

// ---------------------------------------------------------------------------
// Test 8: Large values survive encoding round-trip through bridge
// ---------------------------------------------------------------------------

TEST(NativeBridge, LargeValues) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    auto& ctx = vm.ctx;

    uint64_t a = 0xFFFFFFFFFFFFFFFF;
    uint64_t b = 1;

    uint64_t enc_a = encode_register(ctx, 0, a);
    uint64_t enc_b = encode_register(ctx, 1, b);

    uint64_t encoded_args[] = {enc_a, enc_b};
    uint8_t  arg_regs[] = {0, 1};

    auto result = call_native(
        ctx,
        reinterpret_cast<uintptr_t>(&native_add),
        encoded_args, arg_regs, 2);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0u);  // overflow wraps to 0
}
