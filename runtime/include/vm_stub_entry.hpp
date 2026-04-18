#pragma once
#ifndef __RUNTIME_VM_STUB_ENTRY_HPP__
#define __RUNTIME_VM_STUB_ENTRY_HPP__

/// @file vm_stub_entry.hpp
/// @brief Thin C-linkage entry point for loader-generated machine-code stubs.
///
/// WHY C LINKAGE (extern "C"):
///
///   The caller of this function is hand-assembled machine code emitted
///   by StubEmitter at binary-patching time.  That machine code reads the
///   return value directly from the platform's return register:
///     - x86_64 : rax
///     - x86_32 : edx:eax  (int64_t returned as register pair)
///     - ARM64  : x0
///
///   C linkage guarantees a stable, predictable ABI across compiler
///   versions — C++ name mangling and calling conventions vary between
///   MSVC, GCC, and Clang and would make the stub <-> runtime boundary
///   fragile.  This is the ONLY function that straddles the boundary
///   between loader-generated machine code and the C++ runtime.  All
///   other runtime APIs are pure C++.
///
/// WHY int64_t RETURN:
///
///   The original protected code expects its return value in the native
///   return register.  After vm_stub_entry returns, the stub's resume
///   JMP/B transfers control back to the original code which reads that
///   register directly.
///
///   On x86_32, int64_t uses the edx:eax register pair.  The original
///   32-bit code typically expects a 32-bit return in eax only — the
///   high 32 bits in edx are naturally ignored by the caller, matching
///   cdecl convention for 32-bit return values.
///
/// ERROR POLICY:
///
///   Production (NDEBUG defined):
///     abort().  The original native caller has no concept of VM errors.
///     Continuing after a tampered blob or integrity failure would
///     violate the anti-tamper guarantee (D15§9, D13§G2).  An attacker
///     who corrupts the blob should not get control back.
///
///   Debug (NDEBUG not defined):
///     Returns INT64_MIN and logs diagnostics via DiagnosticCollector.
///     This allows test code to observe and assert on error conditions
///     without crashing the test harness.

#include <vm/vm_stub_args.hpp>
#include <vm_stub_artifact_args.hpp>

#include <cstdint>

/// Legacy entry point for loader-generated stubs.
///
/// Still uses blob-header-flag dispatch (doc 08 §2 violation) for
/// backward compatibility while existing tests migrate to the artifact
/// path. New callers must target `vm_stub_entry_artifact`.
///
/// @param args  Pointer to VmStubArgs built on the stack by the entry stub.
/// @return      VM return value (plaintext, decoded from register 0),
///              or INT64_MIN on error in debug builds.
extern "C"
int64_t vm_stub_entry(
    const VMPilot::Common::VM::VmStubArgs* args) noexcept;

/// Redesigned entry point: consumes a full signed artifact + unit id,
/// walks Stage 4-7 acceptance, resolves the runtime specialization via
/// signed RuntimeSpecializationRegistry, and dispatches to the
/// appropriate VmEngine template instantiation.
///
/// Policy / family selection flows through:
///   OuterEnvelope -> accept_package -> accept_unit_entry ->
///   parse_resolved_family_profile_header -> Registry::lookup ->
///   (Policy, Oram) template tag -> VmEngine<Policy, Oram>::execute
///
/// Any failure along the chain returns INT64_MIN in debug builds and
/// aborts in production — consistent with the legacy entry's
/// fail-closed policy.
extern "C"
int64_t vm_stub_entry_artifact(
    const VMPilot::Runtime::VmStubArtifactArgs* args) noexcept;

#endif  // __RUNTIME_VM_STUB_ENTRY_HPP__
