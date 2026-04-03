#pragma once
#ifndef __RUNTIME_VM_RUNNER_HPP__
#define __RUNTIME_VM_RUNNER_HPP__

/// @file vm_runner.hpp
/// @brief VmRunner — one-shot factory for building and executing VM programs.
///
/// VmRunner is the primary user-facing API for constructing and running
/// VM programs.  It wraps the entire pipeline:
///
///   1. Native function registration (NativeRegistry)
///   2. Program assembly (ProgramBuilder with injected registry)
///   3. Blob construction (build_blob with auto-computed call_site_ips)
///   4. Engine creation (VmEngine::create)
///   5. Execution (engine->execute() or step-by-step via StepController)
///
/// Usage:
///   auto result = VmRunner<DebugPolicy>()
///       .native("puts", &my_puts, 1)
///       .initial_regs({{0, 42}})
///       .program([](auto& p) {
///           p.bb(1).native_call("puts").halt().end_bb();
///       })
///       .run();
///
///   // Interactive (step-by-step):
///   auto ctrl = VmRunner<DebugPolicy>()
///       .native("draw", &draw, 1)
///       .program([](auto& p) { /* ... */ })
///       .run_interactive();
///   while (!ctrl.is_halted()) ctrl.step();
///
/// Implementation: vm_runner.cpp (explicit instantiations for all Policy×Oram combos).

#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"
#include <native_registry.hpp>
#include "program_builder.hpp"
#include "blob_builder.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/xex_speck64.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;

// Forward declaration
template<typename Policy, typename Oram> class VmRunner;

// ─────────────────────────────────────────────────────────────────────────────
// StepController — wraps a VmEngine for interactive step-by-step execution
// ─────────────────────────────────────────────────────────────────────────────

/// Controller for interactive (step-by-step) VM execution.
///
/// Wraps a fully constructed VmEngine. Intended for programs that need
/// external input between steps (e.g. snake game with key injection).
template<typename Policy, typename Oram = typename DefaultOramFor<Policy>::type>
class StepController {
public:
    StepController() = default;

    /// Check if the VM has halted.
    [[nodiscard]] bool is_halted() const noexcept { return halted_; }

    /// Execute a single VM instruction.
    [[nodiscard]] tl::expected<VmResult, DiagnosticCode> step();

    /// Execute until halt or error.
    [[nodiscard]] tl::expected<VmExecResult, DiagnosticCode> run_to_completion();

    /// Get the return value (decoded r0) after halting.
    /// Only valid after is_halted() returns true.
    /// Doc 16: uses FPE_Decode with current insn_fpe_key (replaces LUT decode).
    [[nodiscard]] uint64_t return_value() const;

    /// Read-only access to the engine (for inspection).
    [[nodiscard]] const VmEngine<Policy, Oram>& engine() const { return *engine_; }

    /// Mutable access to execution state (for key injection etc).
    [[nodiscard]] VmExecution& execution() { return engine_->execution(); }

private:
    template<typename P, typename O> friend class VmRunner;

    std::vector<uint8_t> blob_storage_;  ///< owns the blob bytes
    VmEngine<Policy, Oram>* engine_ = nullptr;
    std::unique_ptr<VmEngine<Policy, Oram>> engine_holder_;
    bool halted_ = false;

    void set_engine(tl::expected<VmEngine<Policy, Oram>, DiagnosticCode>&& eng);
};

// ─────────────────────────────────────────────────────────────────────────────
// VmRunner — the main factory
// ─────────────────────────────────────────────────────────────────────────────

/// One-shot builder for VM programs.
///
/// Template parameters:
///   Policy — security policy (DebugPolicy, StandardPolicy, HighSecPolicy)
///   Oram   — ORAM strategy (DirectOram, RollingKeyOram); defaults per policy
template<typename Policy, typename Oram = typename DefaultOramFor<Policy>::type>
class VmRunner {
public:
    VmRunner() {
        // Default seed (deterministic for testing)
        for (int i = 0; i < 32; ++i)
            seed_[i] = static_cast<uint8_t>(i + 1);
    }

    /// Set the 32-byte root secret.
    VmRunner& seed(const uint8_t s[32]) {
        std::memcpy(seed_, s, 32);
        return *this;
    }

    /// Register a native function.
    ///
    /// @param name      human-readable name for use in native_call("name")
    /// @param fn        function pointer
    /// @param argc      number of arguments (0-8)
    /// @param fp_mask   floating-point argument mask (default 0)
    /// @param variadic  whether function is variadic (default false)
    template<typename Fn>
    VmRunner& native(const char* name, Fn fn, uint8_t argc,
                     uint8_t fp_mask = 0, bool variadic = false) {
        registry_.add(name,
                      reinterpret_cast<uint64_t>(reinterpret_cast<void*>(fn)),
                      argc, fp_mask, variadic);
        return *this;
    }

    /// Set initial register values.
    /// Map of register_index -> plaintext_value.
    VmRunner& initial_regs(std::initializer_list<std::pair<uint8_t, uint64_t>> regs) {
        for (auto& [idx, val] : regs) {
            if (idx < 16) init_regs_[idx] = val;
        }
        num_init_regs_ = 0;
        for (int i = 15; i >= 0; --i) {
            if (init_regs_[i] != 0) {
                num_init_regs_ = static_cast<uint8_t>(i + 1);
                break;
            }
        }
        return *this;
    }

    /// Set initial register values from a plain array.
    VmRunner& initial_regs(const uint64_t* regs, uint8_t count) {
        std::memset(init_regs_, 0, sizeof(init_regs_));
        for (uint8_t i = 0; i < count && i < 16; ++i)
            init_regs_[i] = regs[i];
        num_init_regs_ = count;
        return *this;
    }

    /// Set debug mode for the blob.
    VmRunner& debug(bool d = true) {
        debug_mode_ = d;
        return *this;
    }

    /// Set the PIE/ASLR load base delta.
    VmRunner& load_base_delta(int64_t delta) {
        load_base_delta_ = delta;
        return *this;
    }

    /// Define the program via a lambda that receives a ProgramBuilder&.
    ///
    /// The ProgramBuilder has the NativeRegistry injected, so
    /// `native_call("name")` works.
    template<typename BuildFn>
    VmRunner& program(BuildFn&& fn) {
        build_fn_ = std::forward<BuildFn>(fn);
        return *this;
    }

    /// Build the blob and execute until halt.
    [[nodiscard]] tl::expected<VmExecResult, DiagnosticCode> run();

    /// Build the blob and return a StepController for interactive execution.
    [[nodiscard]] StepController<Policy, Oram> run_interactive();

private:
    uint8_t seed_[32] = {};
    NativeRegistry registry_;
    uint64_t init_regs_[16] = {};
    uint8_t  num_init_regs_ = 0;
    bool     debug_mode_ = false;
    int64_t  load_base_delta_ = 0;
    std::function<void(ProgramBuilder&)> build_fn_;

    /// Build the encrypted blob from the program definition.
    std::vector<uint8_t> build_blob_internal();

    /// Scan assembled BBs for NATIVE_CALL instructions and build
    /// the BlobNativeCall vector with auto-computed global IPs.
    ///
    /// For each NATIVE_CALL instruction found, its `aux` field contains
    /// the transition entry index (set by ProgramBuilder::native_call()).
    /// We need to:
    ///   a) Compute the global IP of each NATIVE_CALL instruction
    ///   b) Look up the NativeDescriptor from the registry
    ///   c) Build a BlobNativeCall with the correct call_site_ip
    ///
    /// The result is sorted by transition entry index (the blob expects
    /// transition entries in index order).
    std::vector<BlobNativeCall> compute_native_calls(
        const std::vector<BuilderBB>& bbs) const;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_RUNNER_HPP__
