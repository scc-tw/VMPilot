#pragma once
#ifndef __COMMON_VM_BLOB_VIEW_HPP__
#define __COMMON_VM_BLOB_VIEW_HPP__

/// @file blob_view.hpp
/// @brief Type-safe, validated view over a bytecode blob.
///
/// BlobView provides bounded, type-safe access to every section of a
/// VM bytecode blob.  All pointer arithmetic is validated at construction
/// time — client code never performs manual offset calculations.
///
/// Construction via the factory method `BlobView::create()` validates:
///   - magic number (VM_BLOB_MAGIC)
///   - version compatibility (VM_BLOB_VERSION)
///   - total_size consistency with computed section layout
///   - buffer size ≥ total_size (not truncated)
///   - non-zero instruction and BB counts
///   - reserved fields are zero (forward compatibility)
///
/// If any check fails, create() returns the appropriate DiagnosticCode.
/// Once constructed, all accessors are noexcept and bounds-safe.

#include <vm/vm_blob.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_config.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <cstddef>

// Use our own span-like view for C++17 compatibility.
// In C++20 codebases, replace with std::span.
namespace VMPilot::Common::VM {

/// Lightweight non-owning contiguous view (C++17-friendly std::span substitute).
template<typename T>
struct Span {
    const T* data_ = nullptr;
    size_t   size_ = 0;

    constexpr Span() noexcept = default;
    constexpr Span(const T* d, size_t n) noexcept : data_(d), size_(n) {}

    [[nodiscard]] constexpr const T* data() const noexcept { return data_; }
    [[nodiscard]] constexpr size_t   size() const noexcept { return size_; }
    [[nodiscard]] constexpr bool     empty() const noexcept { return size_ == 0; }
    [[nodiscard]] constexpr const T& operator[](size_t i) const noexcept { return data_[i]; }
    [[nodiscard]] constexpr const T* begin() const noexcept { return data_; }
    [[nodiscard]] constexpr const T* end() const noexcept { return data_ + size_; }
};

/// Type-safe, validated view over a bytecode blob.
///
/// Constructed via `BlobView::create()` which validates the blob format.
/// All section pointers are pre-computed at construction — no pointer
/// arithmetic in accessor calls.
///
/// BlobView is non-owning: the backing buffer must outlive this view.
class BlobView {
public:
    /// Factory method: validate blob and construct a BlobView.
    ///
    /// @param data  pointer to raw blob bytes (must remain valid)
    /// @param size  size of the buffer in bytes
    /// @return      validated BlobView on success, DiagnosticCode on failure
    [[nodiscard]] static tl::expected<BlobView, DiagnosticCode>
    create(const uint8_t* data, size_t size) noexcept {
        // Null check
        if (!data || size < sizeof(BlobHeader))
            return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

        auto& hdr = *reinterpret_cast<const BlobHeader*>(data);

        // Magic
        if (hdr.magic != VM_BLOB_MAGIC)
            return tl::make_unexpected(DiagnosticCode::BlobMagicMismatch);

        // Version
        if (hdr.version != VM_BLOB_VERSION)
            return tl::make_unexpected(DiagnosticCode::BlobVersionMismatch);

        // Reserved must be zero (forward compat)
        if (hdr.reserved != 0)
            return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

        // Sanity: need at least 1 instruction and 1 BB
        if (hdr.insn_count == 0 || hdr.bb_count == 0)
            return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

        // Size consistency
        uint32_t expected = blob_expected_size(hdr);
        if (hdr.total_size != expected)
            return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

        // Buffer not truncated
        if (size < hdr.total_size)
            return tl::make_unexpected(DiagnosticCode::BlobTruncated);

        // All checks passed — compute section pointers.
        BlobView view;
        view.data_ = data;
        view.size_ = hdr.total_size;

        view.insns_  = reinterpret_cast<const VmInsn*>(data + blob_section_insn(hdr));
        view.pool_   = reinterpret_cast<const uint64_t*>(data + blob_section_pool(hdr));
        view.meta_   = reinterpret_cast<const SerializedBBMeta*>(data + blob_section_meta(hdr));
        view.macs_   = data + blob_section_mac(hdr);
        view.trans_  = reinterpret_cast<const TransitionEntry*>(data + blob_section_trans(hdr));
        view.alias_  = data + blob_section_alias(hdr);
        view.config_ = reinterpret_cast<const VmSecurityConfig*>(data + blob_section_config(hdr));

        return view;
    }

    // ─── Type-safe section accessors ─────────────────────────────────────

    /// Blob header (always 32 bytes at offset 0).
    [[nodiscard]] const BlobHeader& header() const noexcept {
        return *reinterpret_cast<const BlobHeader*>(data_);
    }

    /// Encrypted instruction stream.
    [[nodiscard]] Span<const VmInsn> instructions() const noexcept {
        return {insns_, header().insn_count};
    }

    /// Encrypted constant pool (8 bytes per entry).
    [[nodiscard]] Span<const uint64_t> constant_pool() const noexcept {
        return {pool_, header().pool_entry_count};
    }

    /// Encrypted BB metadata (64 bytes per BB, must be decrypted with meta_key).
    [[nodiscard]] Span<const SerializedBBMeta> bb_metadata_raw() const noexcept {
        return {meta_, header().bb_count};
    }

    /// Per-BB MAC entries (8 bytes per BB).
    [[nodiscard]] const uint8_t* bb_macs() const noexcept {
        return macs_;
    }

    /// MAC for a specific BB (8-byte slice).
    [[nodiscard]] const uint8_t* bb_mac(uint32_t bb_index) const noexcept {
        return macs_ + bb_index * 8;
    }

    /// NATIVE_CALL transition entries (32 bytes per entry).
    [[nodiscard]] Span<const TransitionEntry> native_calls() const noexcept {
        return {trans_, header().native_call_count};
    }

    /// GSS 8-bit alias LUT (exactly 256 bytes).
    [[nodiscard]] const uint8_t* alias_lut() const noexcept {
        return alias_;
    }

    /// Security configuration (embedded at end of blob).
    [[nodiscard]] const VmSecurityConfig& config() const noexcept {
        return *config_;
    }

    // ─── Raw access (for integrity hashing, etc.) ────────────────────────

    /// Raw blob data pointer.
    [[nodiscard]] const uint8_t* raw_data() const noexcept { return data_; }

    /// Raw blob size in bytes.
    [[nodiscard]] size_t raw_size() const noexcept { return size_; }

    /// Parse hint only; MUST NOT be used for policy dispatch in the
    /// redesigned runtime.
    ///
    /// Legacy `vm_stub_entry(VmStubArgs*)` still branches on this for
    /// backward compatibility while existing tests migrate. New code
    /// must reach policy/family selection exclusively through the
    /// signed `PackageBindingRecord` → `UnitBindingRecord` →
    /// `ResolvedFamilyProfile` → `RuntimeSpecializationRegistry` chain
    /// via `vm_stub_entry_artifact`. `BLOB_FLAG_DEBUG` survives as a
    /// forward-compat payload variant hint; any path that dispatches on
    /// it is a downgrade vector (see doc 08 §2).
    [[nodiscard]] bool is_debug() const noexcept {
        return (header().flags & BLOB_FLAG_DEBUG) != 0;
    }

    /// Default-construct an empty (invalid) BlobView.
    /// Must be assigned from create() before use.
    BlobView() noexcept = default;

private:

    const uint8_t*           data_   = nullptr;
    size_t                   size_   = 0;
    const VmInsn*            insns_  = nullptr;
    const uint64_t*          pool_   = nullptr;
    const SerializedBBMeta*  meta_   = nullptr;
    const uint8_t*           macs_   = nullptr;
    const TransitionEntry*   trans_  = nullptr;
    const uint8_t*           alias_  = nullptr;
    const VmSecurityConfig*  config_ = nullptr;
};

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_BLOB_VIEW_HPP__
