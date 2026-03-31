#ifndef __LOADER_EDITOR_BASE_HPP__
#define __LOADER_EDITOR_BASE_HPP__
#pragma once

/// @file editor_base.hpp
/// @brief CRTP base for format-specific binary editors.
///
/// EditorBase<Derived> provides a common interface via compile-time
/// polymorphism.  Each concrete editor (ELFEditor, PEEditor, MachOEditor)
/// inherits from EditorBase and implements *_impl() methods.
///
/// At the factory boundary, type erasure is done via std::variant:
///   using AnyEditor = std::variant<ELFEditor, PEEditor, MachOEditor>;
///
/// This gives zero-overhead dispatch inside each editor (all calls
/// resolved at compile time and inlinable) while still supporting
/// runtime format selection at the factory.
///
/// Compared to the old virtual BinaryEditor:
///   - No vtable indirection for editor method calls
///   - No virtual destructor overhead
///   - Each editor is a value type (moveable, no heap allocation for the editor itself)
///   - The variant's visit() generates specialised code per editor type

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace VMPilot::Loader {

/// CRTP base for binary editors.
///
/// Derived must implement:
///   TextSectionInfo text_section_impl() const noexcept;
///   uint64_t next_segment_va_impl(uint64_t alignment) const noexcept;
///   bool cfi_enforced_impl() const noexcept;
///   vector<TextGap> find_text_gaps_impl(size_t min_size) const noexcept;
///   expected<NewSegmentInfo, DC> add_segment_impl(string_view name, const vector<uint8_t>& data, uint64_t align, DiagCollector& diag) noexcept;
///   expected<void, DC> overwrite_text_impl(uint64_t va, const uint8_t* data, size_t len, DiagCollector& diag) noexcept;
///   expected<void, DC> add_runtime_dep_impl(string_view lib, DiagCollector& diag) noexcept;
///   void invalidate_signature_impl() noexcept;
///   expected<void, DC> save_impl(const string& path, DiagCollector& diag) noexcept;
template<typename Derived>
class EditorBase {
protected:
    Derived& self() noexcept { return static_cast<Derived&>(*this); }
    const Derived& self() const noexcept { return static_cast<const Derived&>(*this); }

public:
    // ── Query (read-only) ───────────────────────────────────────────────

    [[nodiscard]] TextSectionInfo text_section() const noexcept {
        return self().text_section_impl();
    }

    [[nodiscard]] uint64_t next_segment_va(uint64_t alignment) const noexcept {
        return self().next_segment_va_impl(alignment);
    }

    [[nodiscard]] bool cfi_enforced() const noexcept {
        return self().cfi_enforced_impl();
    }

    [[nodiscard]] std::vector<TextGap> find_text_gaps(std::size_t min_size) const noexcept {
        return self().find_text_gaps_impl(min_size);
    }

    // ── Mutate ──────────────────────────────────────────────────────────

    [[nodiscard]] auto add_segment(std::string_view name,
                                    const std::vector<uint8_t>& data,
                                    uint64_t alignment,
                                    Common::DiagnosticCollector& diag) noexcept {
        return self().add_segment_impl(name, data, alignment, diag);
    }

    [[nodiscard]] auto overwrite_text(uint64_t va, const uint8_t* data,
                                       size_t len,
                                       Common::DiagnosticCollector& diag) noexcept {
        return self().overwrite_text_impl(va, data, len, diag);
    }

    [[nodiscard]] auto add_runtime_dep(std::string_view lib,
                                        Common::DiagnosticCollector& diag) noexcept {
        return self().add_runtime_dep_impl(lib, diag);
    }

    void invalidate_signature() noexcept {
        self().invalidate_signature_impl();
    }

    // ── Persist ─────────────────────────────────────────────────────────

    [[nodiscard]] auto save(const std::string& path,
                             Common::DiagnosticCollector& diag) noexcept {
        return self().save_impl(path, diag);
    }
};

/// CRTP base for architecture-specific code emitters.
///
/// Derived must implement:
///   expected<Stub, DC> emit_entry_stub_impl() noexcept;
///   expected<vector<uint8_t>, DC> emit_region_patch_impl(uint64_t size, uint64_t from, uint64_t to) noexcept;
///   expected<void, DC> fixup_ptr_impl(vector<uint8_t>& code, size_t off, uint64_t fixup_va, uint64_t target_va) noexcept;
///   expected<void, DC> fixup_branch_impl(vector<uint8_t>& code, size_t off, uint64_t fixup_va, uint64_t target_va) noexcept;
///   void fixup_immediate_impl(vector<uint8_t>& code, size_t off, uint64_t value) noexcept;
///   void fixup_static_va_impl(vector<uint8_t>& code, size_t off, size_t size, uint64_t va) noexcept;
///   size_t min_region_size_impl() const noexcept;
///   int64_t max_branch_distance_impl() const noexcept;
template<typename Derived>
class EmitterBase {
protected:
    Derived& self() noexcept { return static_cast<Derived&>(*this); }
    const Derived& self() const noexcept { return static_cast<const Derived&>(*this); }

public:
    [[nodiscard]] auto emit_entry_stub() noexcept {
        return self().emit_entry_stub_impl();
    }

    [[nodiscard]] auto emit_region_patch(uint64_t region_size,
                                          uint64_t from_addr,
                                          uint64_t to_addr) noexcept {
        return self().emit_region_patch_impl(region_size, from_addr, to_addr);
    }

    [[nodiscard]] auto fixup_ptr(std::vector<uint8_t>& code, std::size_t offset,
                                  uint64_t fixup_va, uint64_t target_va) noexcept {
        return self().fixup_ptr_impl(code, offset, fixup_va, target_va);
    }

    [[nodiscard]] auto fixup_branch(std::vector<uint8_t>& code, std::size_t offset,
                                     uint64_t fixup_va, uint64_t target_va) noexcept {
        return self().fixup_branch_impl(code, offset, fixup_va, target_va);
    }

    void fixup_immediate(std::vector<uint8_t>& code, std::size_t offset,
                          uint64_t value) noexcept {
        self().fixup_immediate_impl(code, offset, value);
    }

    void fixup_static_va(std::vector<uint8_t>& code, std::size_t offset,
                          std::size_t size, uint64_t va) noexcept {
        self().fixup_static_va_impl(code, offset, size, va);
    }

    [[nodiscard]] std::size_t min_region_size() const noexcept {
        return self().min_region_size_impl();
    }

    [[nodiscard]] int64_t max_branch_distance() const noexcept {
        return self().max_branch_distance_impl();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations for variant types (concrete editors defined elsewhere)
// ─────────────────────────────────────────────────────────────────────────────

// NOTE: These forward declarations are here so that AnyEditor/AnyEmitter
// can be used as return types.  The actual class definitions are in their
// respective headers (elf_editor_v2.hpp, etc.).  For now, during the
// incremental migration, the existing ELFEditor/PEEditor/MachOEditor
// still use the old virtual BinaryEditor base.  When a concrete editor
// is ported to CRTP, it will be added to AnyEditor.

// Future:
// class ELFEditorV2;
// class PEEditorV2;
// class MachOEditorV2;
// using AnyEditor = std::variant<ELFEditorV2, PEEditorV2, MachOEditorV2>;

}  // namespace VMPilot::Loader

#endif  // __LOADER_EDITOR_BASE_HPP__
