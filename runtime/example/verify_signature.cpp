/// @file verify_signature.cpp
/// @brief Signature (MAC) verification via NATIVE_CALL to BLAKE3.
///
/// Demonstrates cryptographic verification inside the VM:
///   1. Load message pointer, key pointer, expected MAC pointer into registers
///   2. NATIVE_CALL to a native BLAKE3-HMAC verify function
///   3. CMP result with expected → print VALID or INVALID
///
/// Two runs: one with correct key (VALID), one with wrong key (INVALID).
///
/// Build:  ninja -C build verify_signature_example
/// Run:    ./build/bin/verify_signature_example

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_crypto.hpp>

#include <cstdint>
#include <cstdio>
#include <cstring>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Test;

// ── Native verification function ────────────────────────────────────────────

/// The message to "sign" and verify.
static const char message[] = "VMPilot protected region integrity check";

/// The signing key (32 bytes for BLAKE3_KEYED).
static const uint8_t signing_key[32] = {
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
    0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,
};

/// Pre-computed correct MAC (computed below in main).
static uint8_t correct_mac[16];

/// Native wrapper: verify BLAKE3-HMAC.
///   r0 = pointer to 32-byte key
///   r1 = pointer to message
///   r2 = message length
///   r3 = pointer to 16-byte expected MAC
///   Returns: 1 = valid, 0 = invalid
static uint64_t native_verify_mac(uint64_t key_ptr, uint64_t msg_ptr,
                                   uint64_t msg_len, uint64_t expected_ptr,
                                   uint64_t, uint64_t, uint64_t, uint64_t) {
    auto* key = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(key_ptr));
    auto* msg = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(msg_ptr));
    auto* expected = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(expected_ptr));
    auto  len = static_cast<size_t>(msg_len);

    uint8_t computed[16];
    blake3_keyed_hash(key, msg, len, computed, 16);

    // Constant-time comparison
    uint8_t diff = 0;
    for (int i = 0; i < 16; ++i)
        diff |= computed[i] ^ expected[i];

    return (diff == 0) ? 1 : 0;
}

/// Native wrapper: print result string.
///   r0 = 1 for "SIGNATURE VALID", 0 for "SIGNATURE INVALID"
static uint64_t native_print_result(uint64_t valid,
                                     uint64_t, uint64_t, uint64_t,
                                     uint64_t, uint64_t, uint64_t, uint64_t) {
    if (valid)
        std::puts("  >> SIGNATURE VALID");
    else
        std::puts("  >> SIGNATURE INVALID");
    return valid;
}

// ── Build and run a verification program ────────────────────────────────────

static bool run_verification(const uint8_t seed[32],
                              const uint8_t* key_to_use,
                              const char* label) {
    std::printf("\n--- %s ---\n", label);

    // Program:
    //   NATIVE_CALL[0]  → verify_mac(r0=key, r1=msg, r2=len, r3=expected_mac)
    //   NATIVE_CALL[1]  → print_result(r0)   (r0 = return value from verify)
    //   HALT
    auto none = []() -> uint8_t { return 0; };

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    for (int i = 0; i < 32; ++i)
        bb.epoch_seed[i] = static_cast<uint8_t>(0xCC + i);

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 0},  // verify_mac → result in r0
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 1},  // print_result(r0)
        {VmOpcode::HALT,        none(), 0, 0, 0},
    };

    TestNativeCall nc_verify{};
    nc_verify.call_site_ip = 0;
    nc_verify.arg_count    = 4;
    nc_verify.target_addr  = reinterpret_cast<uint64_t>(&native_verify_mac);

    TestNativeCall nc_print{};
    nc_print.call_site_ip = 1;
    nc_print.arg_count    = 1;
    nc_print.target_addr  = reinterpret_cast<uint64_t>(&native_print_result);

    auto blob = build_test_blob(seed, {bb}, {}, false, {nc_verify, nc_print});

    // Initial registers: r0=key, r1=message, r2=length, r3=expected_mac
    uint64_t regs[16] = {};
    regs[0] = reinterpret_cast<uint64_t>(key_to_use);
    regs[1] = reinterpret_cast<uint64_t>(message);
    regs[2] = static_cast<uint64_t>(std::strlen(message));
    regs[3] = reinterpret_cast<uint64_t>(correct_mac);

    auto engine = VmEngine<StandardPolicy, RollingKeyOram>::create(
        blob.data(), blob.size(), seed, 0, regs, 4);
    if (!engine) { std::fprintf(stderr, "create failed\n"); return false; }

    auto result = engine->execute();
    if (!result) { std::fprintf(stderr, "execute failed\n"); return false; }

    return (result->return_value != 0);
}

int main() {
    std::printf("=== VMPilot Signature Verification Demo ===\n");
    std::printf("Algorithm: BLAKE3-KEYED MAC (128-bit tag)\n");
    std::printf("Message:   \"%s\"\n", message);

    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    // Pre-compute the correct MAC
    blake3_keyed_hash(signing_key,
                      reinterpret_cast<const uint8_t*>(message),
                      std::strlen(message),
                      correct_mac, 16);

    // Test 1: Verify with correct key → should be VALID
    bool valid = run_verification(seed, signing_key, "Correct key");
    if (!valid) {
        std::fprintf(stderr, "FAIL: correct key should verify\n");
        return 1;
    }

    // Test 2: Verify with wrong key → should be INVALID
    uint8_t wrong_key[32];
    std::memcpy(wrong_key, signing_key, 32);
    wrong_key[0] ^= 0xFF;  // flip one byte

    bool invalid = run_verification(seed, wrong_key, "Wrong key (1 byte flipped)");
    if (invalid) {
        std::fprintf(stderr, "FAIL: wrong key should NOT verify\n");
        return 1;
    }

    std::printf("\nSUCCESS: MAC verification correctly accepts/rejects\n");
    return 0;
}
