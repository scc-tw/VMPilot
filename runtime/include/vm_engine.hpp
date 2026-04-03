#pragma once
#ifndef __RUNTIME_VM_ENGINE_HPP__
#define __RUNTIME_VM_ENGINE_HPP__

/// @file vm_engine.hpp
/// @brief VmEngine<Policy, Oram> — doc 16 forward-secrecy execution engine.
///
/// Doc 16 architecture change (vs doc 15):
///
///   The per-BB LUT register encoding (RegTables, DomainTables, CompositionCache)
///   is replaced by Speck-FPE (format-preserving encryption) keyed from
///   VmExecution::insn_fpe_key.  The key ratchets every instruction via a
///   one-way BLAKE3 derivation, providing forward secrecy: a memory dump after
///   instruction N reveals nothing about register encodings before instruction N.
///
///   Pipeline phases per instruction (doc 16 rev.8 section 4):
///
///     A-C: Fetch + decrypt + decode + FPE-decode operands -> handler plaintext
///     D:   Handler computes result (plaintext)
///     E:   FPE-encode handler result register (if opcode writes to reg)
///     F:   Fingerprint all 16 encoded registers (BLAKE3_KEYED_128)
///     G:   Key ratchet: next_key = BLAKE3_KEYED_128(current_key, fingerprint||opcode||aux)
///     H:   Re-encode all 16 registers from current_key to next_key
///     I:   Commit next_key
///     J:   secure_zero all temporaries
///     K:   advance_enc_state (SipHash chain for instruction decryption)
///     L:   Advance IP / BB transition (including enter_basic_block)
///
/// Usage:
///   auto engine = VmEngine<HighSecPolicy>::create(blob, size, seed, delta, regs, nregs);
///   auto result = engine->execute();
///
///   // Reentrant invocation (shares immutable state):
///   auto inner = VmEngine<HighSecPolicy>::create_reentrant(engine->shared_immutable(), ...);
///   auto result2 = inner->execute();
///
/// Implementation: vm_engine.cpp (explicit instantiations for all Policy×Oram combos).

#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"
#include "pipeline.hpp"
#include "handler_impls.hpp"

#include <vm/encoded_value.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/xex_speck64.hpp>     // FPE_Decode (used by execute() return-value decode in StepController)
#include <vm/secure_zero.hpp>     // SecureLocal (used by step() — referenced in handler_impls.hpp)
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <memory>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VmSecurityConfig;
using Common::VM::RegVal;
using Common::VM::PlainVal;
using Common::VM::VmOpcode;
using Common::VM::VM_REG_COUNT;
using Common::VM::Crypto::Speck64_RoundKeys;
using Common::VM::Crypto::XEX_Tweaks;
using Common::VM::Crypto::Speck64_KeySchedule;
using Common::VM::Crypto::XEX_ComputeTweaks;
using Common::VM::Crypto::FPE_Encode;
using Common::VM::Crypto::FPE_Decode;
using Common::VM::Crypto::blake3_keyed_128;
using Common::VM::Crypto::blake3_keyed_fingerprint;
using Common::VM::secure_zero;
using Common::VM::opcode_writes_reg;

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
///   Policy -- security policy traits (HighSecPolicy, StandardPolicy, DebugPolicy)
///   Oram   -- ORAM access strategy (RollingKeyOram, DirectOram)
///
/// Memory layout for reentrancy:
///   VmImmutable:  shared_ptr (one copy, shared across all nesting levels)
///   VmExecution:  value member (~600 bytes, lives on stack with the engine)
///   VmEpoch:      unique_ptr (heap, ~514 bytes -- opcode perm only in doc 16)
///   VmOramState:  unique_ptr (heap, ~4KB ORAM workspace)
///
///   Stack per nesting level: sizeof(VmEngine) ~ 700 bytes
///   Heap per nesting level: ~5KB
///   100 levels: 70KB stack + 0.5MB heap
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
    /// via shared_ptr -- no re-parsing or re-derivation of keys.
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
    /// Runs the doc 16 Phase A-L pipeline in a loop.  Returns when:
    ///   - HALT opcode reached -> VmResult::Halted
    ///   - Error (MAC failure, invalid opcode, etc.) -> DiagnosticCode
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

    /// Execute N sub-instructions as a single dispatch unit (Doc 19 §4).
    ///
    /// Runs Policy::fusion_granularity iterations of Phase A-K, then
    /// Phase L once.  For N=1 (DebugPolicy) this is equivalent to step().
    ///
    /// WHY dispatch_unit instead of step()-in-a-loop:
    ///   Eliminates N-1 interpreter loop iterations (indirect branch,
    ///   decode, Phase L overhead).  More importantly, it is the
    ///   structural foundation for Doc 19's timing normalization:
    ///   every dispatch unit executes the same number of crypto pipeline
    ///   iterations regardless of opcode mix (1 real + N-1 chaff NOP).
    [[nodiscard]] tl::expected<VmResult, DiagnosticCode> dispatch_unit() noexcept;

private:
    // ── Constructor (private -- use create() / create_reentrant()) ───────

    VmEngine(std::shared_ptr<const VmImmutable> imm,
             VmExecution&& exec,
             std::unique_ptr<VmEpoch> epoch,
             std::unique_ptr<VmOramState> oram) noexcept
        : imm_(std::move(imm))
        , exec_(std::move(exec))
        , epoch_(std::move(epoch))
        , oram_(std::move(oram))
    {}

    // ── State ───────────────────────────────────────────────────────────

    /// Shared immutable state (blob, keys, BB metadata).
    /// Reference-counted for reentrancy.
    std::shared_ptr<const VmImmutable> imm_;

    /// Hot-path mutable execution state (registers, IP, flags, FPE key).
    /// Stack-allocated (value member of VmEngine).
    VmExecution exec_;

    /// Per-BB opcode permutation (doc 16: no RegTables/DomainTables).
    /// Heap-allocated via unique_ptr.
    std::unique_ptr<VmEpoch> epoch_;

    /// ORAM workspace.
    /// Heap-allocated (4KB) and cache-line aligned.
    std::unique_ptr<VmOramState> oram_;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_ENGINE_HPP__
