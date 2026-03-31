#pragma once
#ifndef __RUNTIME_VM_ENGINE_HPP__
#define __RUNTIME_VM_ENGINE_HPP__

/// @file vm_engine.hpp
/// @brief VmEngine<Policy, Oram> — the parameterized VM execution engine.
///
/// VmEngine is the central class of the redesigned VM runtime.  It replaces
/// the monolithic vm_execute() + VMContext with a template-parameterized
/// engine that provides:
///
///   - Compile-time security policy selection (D1-D4 dimensions)
///   - Orthogonal ORAM strategy (RollingKeyOram vs DirectOram)
///   - Shared VmImmutable across reentrant NATIVE_CALL invocations
///   - 4-way state split for cache locality and const-correctness
///   - Pipeline as method chain (12-step uniform dispatch, doc 15 §5.2)
///
/// Usage:
///   // First invocation (loads blob, derives keys):
///   auto engine = VmEngine<HighSecPolicy>::create(blob, size, seed, delta, regs, nregs);
///   auto result = engine->execute();
///
///   // Reentrant invocation (shares immutable state):
///   auto inner = VmEngine<HighSecPolicy>::create_reentrant(engine->shared_immutable(), ...);
///   auto result2 = inner->execute();

#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/encoded_value.hpp>
#include <vm/blob_view.hpp>
#include <vm/vm_config.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <memory>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VmSecurityConfig;
using Common::VM::RegVal;
using Common::VM::PlainVal;

// ─────────────────────────────────────────────────────────────────────────────
// Result types
// ─────────────────────────────────────────────────────────────────────────────

enum class VmResult : uint8_t {
    Halted,     ///< VM_HALT opcode reached
    Stepped,    ///< Single step completed (for step-by-step debugging)
};

struct VmExecResult {
    VmResult status;
    uint64_t return_value;   ///< Decoded plaintext of register 0
};

// ─────────────────────────────────────────────────────────────────────────────
// VmEngine<Policy, Oram>
// ─────────────────────────────────────────────────────────────────────────────

/// The parameterized VM execution engine.
///
/// Template parameters:
///   Policy — security policy traits (HighSecPolicy, StandardPolicy, DebugPolicy)
///   Oram   — ORAM access strategy (RollingKeyOram, DirectOram)
///
/// Memory layout for reentrancy:
///   VmImmutable:  shared_ptr (one copy, shared across all nesting levels)
///   VmExecution:  value member (~600 bytes, lives on stack with the engine)
///   VmEpoch:      unique_ptr (heap, ~131KB encoding tables)
///   VmOramState:  unique_ptr (heap, ~4KB ORAM workspace)
///
///   Stack per nesting level: sizeof(VmEngine) ≈ 700 bytes
///   Heap per nesting level: ~135KB
///   100 levels: 70KB stack + 13.5MB heap (vs 13.5MB stack before)
template<typename Policy, typename Oram = typename DefaultOramFor<Policy>::type>
class VmEngine {
    static_assert(Policy::validate(), "Invalid security policy configuration");

public:
    using policy_type = Policy;
    using oram_type   = Oram;

    // ── Factory methods ─────────────────────────────────────────────────

    /// Create a new engine from raw blob data (first invocation).
    ///
    /// Validates the blob, decrypts metadata, derives all keys, and
    /// initialises VmImmutable (shared), VmExecution, VmEpoch, VmOramState.
    ///
    /// @param blob_data       pointer to raw blob bytes (must outlive engine)
    /// @param blob_size       size of the blob in bytes
    /// @param stored_seed     32-byte root secret
    /// @param load_base_delta PIE/ASLR relocation delta
    /// @param initial_regs    initial register values (plaintext)
    /// @param num_regs        number of registers to initialise
    /// @return                ready-to-execute engine, or error
    [[nodiscard]] static tl::expected<VmEngine, DiagnosticCode>
    create(const uint8_t* blob_data, size_t blob_size,
           const uint8_t stored_seed[32],
           int64_t load_base_delta = 0,
           const uint64_t* initial_regs = nullptr,
           uint8_t num_regs = 0) noexcept;

    /// Create a reentrant engine sharing immutable state from a parent.
    ///
    /// Used by NATIVE_CALL handlers when the native function calls back
    /// into the VM.  The immutable state (blob, keys, metadata) is shared
    /// via shared_ptr — no re-parsing or re-derivation of keys.
    ///
    /// @param imm             shared immutable state from parent engine
    /// @param load_base_delta PIE/ASLR delta (same as parent)
    /// @param initial_regs    initial register values for this invocation
    /// @param num_regs        number of registers to initialise
    /// @return                ready-to-execute engine, or error
    [[nodiscard]] static tl::expected<VmEngine, DiagnosticCode>
    create_reentrant(std::shared_ptr<const VmImmutable> imm,
                     int64_t load_base_delta = 0,
                     const uint64_t* initial_regs = nullptr,
                     uint8_t num_regs = 0) noexcept;

    // ── Execution ───────────────────────────────────────────────────────

    /// Execute until HALT or error.
    ///
    /// Runs the 12-step uniform pipeline in a loop.  Returns when:
    ///   - HALT opcode reached → VmResult::Halted
    ///   - Error (MAC failure, invalid opcode, etc.) → DiagnosticCode
    [[nodiscard]] tl::expected<VmExecResult, DiagnosticCode> execute() noexcept;

    /// Execute a single instruction (for debugging / testing).
    [[nodiscard]] tl::expected<VmResult, DiagnosticCode> step() noexcept;

    // ── Accessors (for testing and reentrancy) ──────────────────────────

    /// Get shared immutable state (for create_reentrant).
    [[nodiscard]] std::shared_ptr<const VmImmutable> shared_immutable() const noexcept {
        return imm_;
    }

    /// Read-only access to execution state (for testing).
    [[nodiscard]] const VmExecution& execution() const noexcept { return exec_; }

    /// Read-only access to epoch state (for testing).
    [[nodiscard]] const VmEpoch& epoch() const noexcept { return *epoch_; }

    /// Read-only access to ORAM state (for testing).
    [[nodiscard]] const VmOramState& oram() const noexcept { return *oram_; }

    /// Mutable access to execution state (for test setup).
    [[nodiscard]] VmExecution& execution() noexcept { return exec_; }

private:
    // ── Constructor (private — use create() / create_reentrant()) ───────

    VmEngine(std::shared_ptr<const VmImmutable> imm,
             VmExecution exec,
             std::unique_ptr<VmEpoch> epoch,
             std::unique_ptr<VmOramState> oram) noexcept
        : imm_(std::move(imm))
        , exec_(std::move(exec))
        , epoch_(std::move(epoch))
        , oram_(std::move(oram))
    {}

    // ── 12-step pipeline (doc 15 §5.2) ─────────────────────────────────
    // Implementation in vm_engine.cpp (explicit instantiation per policy).

    // Steps 1-3: Fetch encrypted insn, decrypt via SipHash, decode opcode
    // tl::expected<DecodedInsn, DiagnosticCode> fetch_decrypt_decode() noexcept;

    // Step 4: Resolve operands (always both, even if unused — D3 uniformity)
    // void resolve_operands(DecodedInsn& insn) noexcept;

    // Step 5-8: Dispatch to handler, select result, write, memory
    // tl::expected<void, DiagnosticCode> dispatch(const DecodedInsn& insn) noexcept;

    // Step 9: Update enc_state (SipHash chain advance)
    // void advance_enc_state(const DecodedInsn& insn) noexcept;

    // Step 10: Advance IP (branchless for branches)
    // void advance_ip() noexcept;

    // Step 11: Check BB MAC at boundary
    // tl::expected<void, DiagnosticCode> check_bb_boundary() noexcept;

    // Step 12: Anti-debug guard (amortised)
    // void guard_check() noexcept;

    // ── State ───────────────────────────────────────────────────────────

    /// Shared immutable state (blob, keys, BB metadata).
    /// Reference-counted for reentrancy.
    std::shared_ptr<const VmImmutable> imm_;

    /// Hot-path mutable execution state (registers, IP, flags).
    /// Stack-allocated (value member of VmEngine).
    VmExecution exec_;

    /// Per-BB encoding tables and composition cache.
    /// Heap-allocated because encoding tables are ~131KB.
    std::unique_ptr<VmEpoch> epoch_;

    /// ORAM workspace.
    /// Heap-allocated (4KB) and cache-line aligned.
    std::unique_ptr<VmOramState> oram_;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_ENGINE_HPP__
