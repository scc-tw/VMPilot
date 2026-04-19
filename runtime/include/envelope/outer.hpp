#ifndef VMPILOT_RUNTIME_ENVELOPE_OUTER_HPP
#define VMPILOT_RUNTIME_ENVELOPE_OUTER_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

// Parser for the tier-neutral outer envelope wrapping a shipping artifact.
// Envelope layout on disk:
//
//     offset 0            16 bytes   kOuterMagic
//     offset 16           4 bytes    metadata_length (big-endian uint32)
//     offset 20           N bytes    strict-CBOR metadata map
//     offset 20 + N       ... bytes  data partitions referenced by locators
//
// The parser is *parse only*. It does not verify signatures or inner
// metadata. Stage 5 (PackageBindingRecord acceptance) hashes the envelope
// bytes + data partitions and compares against PBR fields to establish
// authenticity.

namespace VMPilot::Runtime::Envelope {

// 16-byte magic. Recognisable in hex dumps, tier-neutral, fixed across all
// shipping artifacts regardless of policy / family / provider.
inline constexpr std::array<std::uint8_t, 16> kOuterMagic = {
    'V', 'M', 'P', 'I', 'L', 'O', 'T', '_',
    'A', 'R', 'T', 'I', 'F', 'A', 'C', 'T',
};

inline constexpr std::size_t kOuterFixedHeaderSize = 20;  // magic + length

// Only the current supported envelope format version. Newer versions must
// bump this constant and land a migration path at the same time.
inline constexpr std::uint64_t kOuterFormatVersionV1 = 1;

// Locator into the artifact byte buffer. Offsets are absolute within the
// buffer passed to parse_outer_envelope; lengths are exclusive ends.
struct Locator {
    std::uint64_t offset;
    std::uint64_t length;
};

// Parsed envelope. Non-owning — the caller retains the byte buffer; the
// parser only returns indices + the small fixed metadata fields.
struct OuterEnvelope {
    std::uint64_t outer_format_version;
    std::string   package_schema_version;
    std::string   canonical_encoding_id;
    std::uint64_t section_table_shape_class;

    Locator package_binding_record;
    Locator inner_metadata_partition;
    Locator payload_partition;

    // Slice of the original buffer that holds the CBOR metadata. Useful for
    // Stage 5 when it needs to rehash the envelope body as part of
    // artifact_layout_hash verification.
    std::size_t metadata_offset;   // 20
    std::size_t metadata_length;
};

enum class ParseError : std::uint8_t {
    // Structural failures.
    BufferTooSmall = 1,
    WrongMagic,
    TruncatedMetadata,
    BadCbor,
    MissingCoreField,
    WrongFieldType,
    BadLocator,
    OverlappingLocators,
    UnsupportedFormatVersion,

    // Skeleton-level leakage: the outer envelope contains text tokens that
    // the artifact skeleton contract forbids (tier / family / provider).
    TierRevealingToken,
};

// Parse the outer envelope from a contiguous byte buffer.
[[nodiscard]] tl::expected<OuterEnvelope, ParseError>
parse_outer_envelope(const std::uint8_t* data, std::size_t size) noexcept;

// Convenience overload for range-friendly inputs.
[[nodiscard]] tl::expected<OuterEnvelope, ParseError>
parse_outer_envelope(std::string_view bytes) noexcept;

inline tl::expected<OuterEnvelope, ParseError>
parse_outer_envelope(const std::vector<std::uint8_t>& bytes) noexcept {
    return parse_outer_envelope(bytes.data(), bytes.size());
}

}  // namespace VMPilot::Runtime::Envelope

#endif  // VMPILOT_RUNTIME_ENVELOPE_OUTER_HPP
