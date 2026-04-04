#ifndef __LOADER_STUB_EMITTER_HPP__
#define __LOADER_STUB_EMITTER_HPP__
#pragma once

/// @file StubEmitter.hpp
/// @brief Abstract strategy for architecture-specific code generation.
///
/// Each concrete emitter (X86_64StubEmitter, ARM64StubEmitter, etc.)
/// generates machine code stubs and performs fixups using its ArchTraits.
///
/// Source: D10§GAP7 (entry/exit stub spec), D13§D2 (region replacement),
///         D13§D3 (ASLR delta computation).

#include <diagnostic.hpp>
#include <file_type_parser.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace VMPilot::Loader {

/// Machine-code entry stub with fixup metadata.
///
/// Fixup offsets mark positions in `code` where PayloadBuilder patches
/// values after the final payload layout is known.
struct Stub {
    std::vector<uint8_t> code;

    // --- blob_data pointer (LEA disp32 / ADR imm21) ---
    size_t blob_fixup_offset = 0;

    // --- blob_size immediate (MOV imm32 / MOVZ imm16) ---
    size_t size_fixup_offset = 0;

    // --- stored_seed pointer (LEA disp32 / ADR imm21) ---
    size_t seed_fixup_offset = 0;

    // --- call slot pointer for indirect call (LEA disp32 / ADR imm21) ---
    size_t call_slot_fixup_offset = 0;

    // --- resume jump to region.addr + region.size (JMP rel32 / B imm26) ---
    size_t resume_fixup_offset = 0;

    // --- ASLR delta: static VA fixup (MOV imm64 / MOVZ+MOVK×3) ---
    size_t delta_fixup_offset = 0;
    size_t delta_fixup_size = 0;    // 8 for x86-64, 16 for ARM64

    // --- ASLR delta: reference point offset within stub ---
    // The instruction whose runtime VA is compared against the static VA.
    size_t delta_ref_offset = 0;
};

/// Abstract strategy for architecture-specific code generation.
class StubEmitter {
public:
    virtual ~StubEmitter() = default;

    /// Emit a full mid-function entry stub (D10§GAP7).
    /// Saves callee-saved regs, computes ASLR delta, builds initial_regs,
    /// indirect-calls through slot, restores regs, jumps to resume.
    /// All displacements are placeholders — call fixup_*() after layout.
    [[nodiscard]] virtual tl::expected<Stub, Common::DiagnosticCode>
    emit_entry_stub() noexcept = 0;

    /// Emit JMP/B + NOP fill to overwrite a protected region (D13§D2).
    [[nodiscard]] virtual tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
    emit_region_patch(uint64_t region_size,
                      uint64_t from_addr, uint64_t to_addr) noexcept = 0;

    /// Fix up a pointer-loading displacement (LEA disp32 / ADR imm21).
    ///
    /// The emitter computes the correct encoded displacement internally:
    ///   x86_64/x86_32: disp32 = target_va - (fixup_va + 4)
    ///   ARM64:          imm21  = target_va - fixup_va
    ///
    /// PayloadBuilder provides virtual addresses only — no arch knowledge.
    [[nodiscard]] virtual tl::expected<void, Common::DiagnosticCode>
    fixup_ptr(std::vector<uint8_t>& code, std::size_t offset,
              uint64_t fixup_va, uint64_t target_va) noexcept = 0;

    /// Fix up a branch displacement (JMP rel32 / B imm26).
    /// Same ownership model as fixup_ptr — emitter owns the encoding.
    [[nodiscard]] virtual tl::expected<void, Common::DiagnosticCode>
    fixup_branch(std::vector<uint8_t>& code, std::size_t offset,
                 uint64_t fixup_va, uint64_t target_va) noexcept = 0;

    /// Fix up a size/count immediate.  Not PC-relative — no VA needed.
    virtual void fixup_immediate(std::vector<uint8_t>& code,
                                 std::size_t offset,
                                 uint64_t value) noexcept = 0;

    /// Fix up the static VA for ASLR delta computation (D13§D3).
    /// Absolute value, not PC-relative.
    virtual void fixup_static_va(std::vector<uint8_t>& code,
                                 std::size_t offset, std::size_t size,
                                 uint64_t va) noexcept = 0;

    /// Constants from ArchTraits, exposed for PayloadBuilder.
    [[nodiscard]] virtual std::size_t min_region_size() const noexcept = 0;
    [[nodiscard]] virtual int64_t max_branch_distance() const noexcept = 0;
};

/// Factory. Creates the appropriate emitter for the given {arch, mode}.
[[nodiscard]] std::unique_ptr<StubEmitter>
create_emitter(Common::FileArch arch, Common::FileMode mode) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_STUB_EMITTER_HPP__
