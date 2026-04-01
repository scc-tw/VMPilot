#pragma once
/// @file coffi_adapter.hpp
/// @brief Bridges coffi-modern and elfio-modern result types to tl::expected,
///        and provides type-erased PE/ELF editors that handle 32/64-bit.

#include <coffi/coffi.hpp>
#include <elfio/elfio.hpp>
#include <diagnostic.hpp>
#include <tl/expected.hpp>

#include <string>
#include <variant>

namespace VMPilot::Loader {

// ─────────────────────────────────────────────────────────────────────────────
// coffi::result<T> → tl::expected<T, DiagnosticCode>
// ─────────────────────────────────────────────────────────────────────────────

inline Common::DiagnosticCode to_diagnostic(coffi::error_code ec) noexcept {
    using DC = Common::DiagnosticCode;
    using EC = coffi::error_code;
    switch (ec) {
        case EC::success:            return DC::None;
        case EC::file_too_small:     return DC::PatchBinaryReadFailed;
        case EC::invalid_magic:      return DC::PatchInputInvalid;
        case EC::invalid_pe_signature: return DC::PatchInputInvalid;
        case EC::truncated_header:   return DC::PatchInputInvalid;
        case EC::write_failed:       return DC::PatchBinaryWriteFailed;
        case EC::not_found:          return DC::PatchInputInvalid;
        default:                     return DC::PatchInputInvalid;
    }
}

template <typename T>
tl::expected<T, Common::DiagnosticCode>
to_expected(coffi::result<T, coffi::error_code>&& r) noexcept {
    if (r) return std::move(*r);
    return tl::make_unexpected(to_diagnostic(r.error()));
}

inline tl::expected<void, Common::DiagnosticCode>
to_expected(coffi::result<void, coffi::error_code>&& r) noexcept {
    if (r) return {};
    return tl::make_unexpected(to_diagnostic(r.error()));
}

// ─────────────────────────────────────────────────────────────────────────────
// Type-erased PE editor — handles both PE32 and PE32+ via std::variant
// ─────────────────────────────────────────────────────────────────────────────

using PeEditorVariant = std::variant<
    coffi::coff_editor<coffi::pe32_traits>,
    coffi::coff_editor<coffi::pe32plus_traits>
>;

/// Open a PE file for editing, auto-detecting PE32 vs PE32+.
inline tl::expected<PeEditorVariant, Common::DiagnosticCode>
open_pe_editor(const std::string& path) noexcept {
    auto buf = coffi::file_buffer::from_file(path.c_str());
    if (!buf) return tl::make_unexpected(to_diagnostic(buf.error()));

    auto arch = coffi::detect_architecture(buf->view());
    if (!arch) return tl::make_unexpected(to_diagnostic(arch.error()));

    if (*arch == coffi::detected_arch::pe32plus) {
        auto ed = coffi::coff_editor<coffi::pe32plus_traits>::from_path(path.c_str());
        if (!ed) return tl::make_unexpected(to_diagnostic(ed.error()));
        return PeEditorVariant{std::move(*ed)};
    } else {
        auto ed = coffi::coff_editor<coffi::pe32_traits>::from_path(path.c_str());
        if (!ed) return tl::make_unexpected(to_diagnostic(ed.error()));
        return PeEditorVariant{std::move(*ed)};
    }
}

/// Visit helper — apply a generic lambda to whichever editor is active.
template <typename F>
decltype(auto) visit_pe(PeEditorVariant& v, F&& f) {
    return std::visit(std::forward<F>(f), v);
}

template <typename F>
decltype(auto) visit_pe(const PeEditorVariant& v, F&& f) {
    return std::visit(std::forward<F>(f), v);
}

// ─────────────────────────────────────────────────────────────────────────────
// elfio-modern: result<T> → tl::expected<T, DiagnosticCode>
// ─────────────────────────────────────────────────────────────────────────────

inline Common::DiagnosticCode to_diagnostic(elfio::error_code ec) noexcept {
    using DC = Common::DiagnosticCode;
    using EC = elfio::error_code;
    switch (ec) {
        case EC::success:            return DC::None;
        case EC::file_too_small:     return DC::PatchBinaryReadFailed;
        case EC::invalid_magic:      return DC::PatchInputInvalid;
        case EC::truncated_header:   return DC::PatchInputInvalid;
        case EC::write_failed:       return DC::PatchBinaryWriteFailed;
        default:                     return DC::PatchInputInvalid;
    }
}

template <typename T>
tl::expected<T, Common::DiagnosticCode>
to_expected(elfio::result<T, elfio::error_code>&& r) noexcept {
    if (r) return std::move(*r);
    return tl::make_unexpected(to_diagnostic(r.error()));
}

inline tl::expected<void, Common::DiagnosticCode>
to_expected(elfio::result<void, elfio::error_code>&& r) noexcept {
    if (r) return {};
    return tl::make_unexpected(to_diagnostic(r.error()));
}

// ─────────────────────────────────────────────────────────────────────────────
// Type-erased ELF editor — handles both ELF32 and ELF64 via std::variant
// ─────────────────────────────────────────────────────────────────────────────

using ElfEditorVariant = std::variant<
    elfio::elf_editor<elfio::elf32_traits>,
    elfio::elf_editor<elfio::elf64_traits>
>;

template <typename F>
decltype(auto) visit_elf(ElfEditorVariant& v, F&& f) {
    return std::visit(std::forward<F>(f), v);
}

template <typename F>
decltype(auto) visit_elf(const ElfEditorVariant& v, F&& f) {
    return std::visit(std::forward<F>(f), v);
}

}  // namespace VMPilot::Loader
