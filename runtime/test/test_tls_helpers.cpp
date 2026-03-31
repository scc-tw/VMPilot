/// @file test_tls_helpers.cpp
/// @brief Tests for TLS helper functions (vmpilot_tls_read/write).
///
/// TLS access is inherently platform-specific. These tests verify that
/// the inline-asm helpers correctly read and write the thread-local
/// segment register (fs: on x86-64, gs: on x86-32, TPIDR_EL0 on ARM64).
///
/// We test by reading a known TLS variable's value through the helper
/// and comparing it to direct C++ access of the same variable.

#include <tls_helpers.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <thread>

// A thread-local variable we can test against.
// The compiler places this in the TLS segment; we can compare
// direct C++ access vs our asm helper access.
static thread_local uint64_t tls_test_var_64 = 0;
static thread_local uint32_t tls_test_var_32 = 0;

// ============================================================================
// Helper: get TLS offset of a variable
//
// On x86-64 Linux (Local Exec TLS model), a thread-local variable at
// address `&var` has TLS offset = (uintptr_t)&var - fs_base.
// We can get fs_base via arch_prctl(ARCH_GET_FS, &base) on Linux.
//
// On other platforms, getting the raw TLS offset is non-trivial.
// We use a practical approach: write a known value via C++, scan
// a range of offsets via our helper to find where the value lives.
// ============================================================================

#if defined(__x86_64__) && defined(__linux__)
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

static uint64_t get_fs_base() {
    uint64_t base;
    syscall(SYS_arch_prctl, ARCH_GET_FS, &base);
    return base;
}

static uint64_t tls_offset_of(const void* tls_var) {
    return reinterpret_cast<uintptr_t>(tls_var) - get_fs_base();
}

TEST(TlsHelpers, Read64MatchesCppAccess) {
    tls_test_var_64 = 0xDEADBEEFCAFEBABEull;
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    uint64_t result = vmpilot_tls_read64(offset);
    EXPECT_EQ(result, 0xDEADBEEFCAFEBABEull);
}

TEST(TlsHelpers, Write64ThenReadBack) {
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    vmpilot_tls_write64(offset, 0x1234567890ABCDEFull);
    EXPECT_EQ(tls_test_var_64, 0x1234567890ABCDEFull);

    // Also verify read-back through the helper
    EXPECT_EQ(vmpilot_tls_read64(offset), 0x1234567890ABCDEFull);
}

TEST(TlsHelpers, Read32MatchesCppAccess) {
    tls_test_var_32 = 0xCAFEBABE;
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    uint64_t result = vmpilot_tls_read32(offset);
    EXPECT_EQ(result, 0xCAFEBABEu);
}

TEST(TlsHelpers, Write32ThenReadBack) {
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    vmpilot_tls_write32(offset, 0xDEADFACE);
    EXPECT_EQ(tls_test_var_32, 0xDEADFACEu);
    EXPECT_EQ(vmpilot_tls_read32(offset), 0xDEADFACEu);
}

TEST(TlsHelpers, ThreadIsolation) {
    // TLS variables are per-thread. Write in a child thread,
    // verify the main thread's value is unaffected.
    tls_test_var_64 = 42;
    uint64_t offset = tls_offset_of(&tls_test_var_64);

    std::thread t([offset]() {
        // Child thread has its own TLS — write a different value
        vmpilot_tls_write64(offset, 9999);
        EXPECT_EQ(vmpilot_tls_read64(offset), 9999u);
    });
    t.join();

    // Main thread's value must be untouched
    EXPECT_EQ(tls_test_var_64, 42u);
    EXPECT_EQ(vmpilot_tls_read64(offset), 42u);
}

// Layer 2 fallback must produce identical results to Layer 1
TEST(TlsHelpersFallback, Read64MatchesLayer1) {
    tls_test_var_64 = 0xAAAABBBBCCCCDDDDull;
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    EXPECT_EQ(vmpilot_tls_read64_fallback(offset), vmpilot_tls_read64(offset));
}

TEST(TlsHelpersFallback, Write64ThenReadBack) {
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    vmpilot_tls_write64_fallback(offset, 0xFEDCBA9876543210ull);
    EXPECT_EQ(tls_test_var_64, 0xFEDCBA9876543210ull);
    // Read back via Layer 1 — must see the Layer 2 write
    EXPECT_EQ(vmpilot_tls_read64(offset), 0xFEDCBA9876543210ull);
}

TEST(TlsHelpersFallback, Read32MatchesLayer1) {
    tls_test_var_32 = 0x12345678;
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    EXPECT_EQ(vmpilot_tls_read32_fallback(offset), vmpilot_tls_read32(offset));
}

TEST(TlsHelpersFallback, CrossLayerWriteRead) {
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    // Write via Layer 2, read via Layer 1
    vmpilot_tls_write32_fallback(offset, 0xDEADBEEF);
    EXPECT_EQ(vmpilot_tls_read32(offset), 0xDEADBEEFu);
    // Write via Layer 1, read via Layer 2
    vmpilot_tls_write32(offset, 0xCAFEBABE);
    EXPECT_EQ(vmpilot_tls_read32_fallback(offset), 0xCAFEBABEu);
}

#elif defined(__aarch64__) && defined(__linux__)

// ARM64: TPIDR_EL0 holds the thread pointer.
static uint64_t get_tp_base() {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    return base;
}

static uint64_t tls_offset_of(const void* tls_var) {
    return reinterpret_cast<uintptr_t>(tls_var) - get_tp_base();
}

TEST(TlsHelpers, Read64MatchesCppAccess) {
    tls_test_var_64 = 0xDEADBEEFCAFEBABEull;
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    EXPECT_EQ(vmpilot_tls_read64(offset), 0xDEADBEEFCAFEBABEull);
}

TEST(TlsHelpers, Write64ThenReadBack) {
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    vmpilot_tls_write64(offset, 0x1234567890ABCDEFull);
    EXPECT_EQ(tls_test_var_64, 0x1234567890ABCDEFull);
    EXPECT_EQ(vmpilot_tls_read64(offset), 0x1234567890ABCDEFull);
}

TEST(TlsHelpers, Read32MatchesCppAccess) {
    tls_test_var_32 = 0xCAFEBABE;
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    EXPECT_EQ(vmpilot_tls_read32(offset), 0xCAFEBABEu);
}

TEST(TlsHelpers, Write32ThenReadBack) {
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    vmpilot_tls_write32(offset, 0xDEADFACE);
    EXPECT_EQ(tls_test_var_32, 0xDEADFACEu);
}

TEST(TlsHelpers, ThreadIsolation) {
    tls_test_var_64 = 42;
    uint64_t offset = tls_offset_of(&tls_test_var_64);

    std::thread t([offset]() {
        vmpilot_tls_write64(offset, 9999);
        EXPECT_EQ(vmpilot_tls_read64(offset), 9999u);
    });
    t.join();

    EXPECT_EQ(tls_test_var_64, 42u);
}

// Layer 2 fallback must produce identical results to Layer 1
TEST(TlsHelpersFallback, Read64MatchesLayer1) {
    tls_test_var_64 = 0xAAAABBBBCCCCDDDDull;
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    EXPECT_EQ(vmpilot_tls_read64_fallback(offset), vmpilot_tls_read64(offset));
}

TEST(TlsHelpersFallback, Write64ThenReadBack) {
    uint64_t offset = tls_offset_of(&tls_test_var_64);
    vmpilot_tls_write64_fallback(offset, 0xFEDCBA9876543210ull);
    EXPECT_EQ(tls_test_var_64, 0xFEDCBA9876543210ull);
    EXPECT_EQ(vmpilot_tls_read64(offset), 0xFEDCBA9876543210ull);
}

TEST(TlsHelpersFallback, Read32MatchesLayer1) {
    tls_test_var_32 = 0x12345678;
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    EXPECT_EQ(vmpilot_tls_read32_fallback(offset), vmpilot_tls_read32(offset));
}

TEST(TlsHelpersFallback, CrossLayerWriteRead) {
    uint64_t offset = tls_offset_of(&tls_test_var_32);
    vmpilot_tls_write32_fallback(offset, 0xDEADBEEF);
    EXPECT_EQ(vmpilot_tls_read32(offset), 0xDEADBEEFu);
    vmpilot_tls_write32(offset, 0xCAFEBABE);
    EXPECT_EQ(vmpilot_tls_read32_fallback(offset), 0xCAFEBABEu);
}

#else
// Platforms without known TLS offset discovery get a compile-only test.
// The functions link but we can't verify correctness without
// knowing how to compute the TLS offset from a C++ variable address.
TEST(TlsHelpers, CompileAndLinkOnly) {
    // Verify the symbols exist and link. Can't test correctness
    // without platform-specific TLS offset discovery.
    EXPECT_NE(reinterpret_cast<void*>(&vmpilot_tls_read64), nullptr);
    EXPECT_NE(reinterpret_cast<void*>(&vmpilot_tls_write64), nullptr);
    EXPECT_NE(reinterpret_cast<void*>(&vmpilot_tls_read32), nullptr);
    EXPECT_NE(reinterpret_cast<void*>(&vmpilot_tls_write32), nullptr);
}
#endif
