/// @file hello_world.cpp
/// @brief Hello World via NATIVE_CALL to puts().
///
/// Demonstrates the complete native bridge path:
///   VM register (FPE-encoded) → decode → ABI marshalling → puts() → return
///
/// The string address is passed as an initial register value (r0).
/// NATIVE_CALL invokes puts(r0), which prints "Hello, World!".
///
/// Build:  ninja -C build hello_world_example
/// Run:    ./build/bin/hello_world_example

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <cstdint>
#include <cstdio>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

/// Native wrapper: puts() with the 8-arg ABI signature the trampoline expects.
/// We use puts (not printf) to avoid variadic complexity — puts takes 1 arg.
static uint64_t native_puts_wrapper(uint64_t str_ptr,
                                     uint64_t, uint64_t, uint64_t,
                                     uint64_t, uint64_t, uint64_t, uint64_t) {
    return static_cast<uint64_t>(
        std::puts(reinterpret_cast<const char*>(static_cast<uintptr_t>(str_ptr))));
}

/// The string to print — static so its address is stable for the VM's lifetime.
static const char hello_str[] = "Hello, World!";

int main() {
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    // ── Build program: NATIVE_CALL[0] → puts(r0), then HALT ────────────
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    for (int i = 0; i < 32; ++i)
        bb.epoch_seed[i] = static_cast<uint8_t>(0xBB + i);

    auto none = []() -> uint8_t { return 0; };

    bb.instructions = {
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 0},   // call transition entry 0
        {VmOpcode::HALT,        none(), 0, 0, 0},
    };

    // Register native_puts_wrapper as transition entry 0
    TestNativeCall nc{};
    nc.call_site_ip = 0;
    nc.arg_count    = 1;   // 1 integer argument (the string pointer)
    nc.target_addr  = reinterpret_cast<uint64_t>(&native_puts_wrapper);

    auto blob = build_test_blob(seed, {bb}, {}, false, {nc});

    // ── Create engine with r0 = string address ──────────────────────────
    uint64_t initial_regs[16] = {};
    initial_regs[0] = reinterpret_cast<uint64_t>(hello_str);

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        /*load_base_delta=*/0,
        initial_regs, 1);

    if (!engine) {
        std::fprintf(stderr, "ERROR: VmEngine::create() failed\n");
        return 1;
    }

    // ── Execute: should print "Hello, World!" via puts() ────────────────
    auto result = engine->execute();

    if (!result) {
        std::fprintf(stderr, "ERROR: execute() failed\n");
        return 1;
    }

    // puts() returns non-negative on success
    std::printf("[VM returned %ld]\n",
                static_cast<long>(result->return_value));

    return 0;
}
