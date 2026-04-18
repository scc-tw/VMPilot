#include "envelope/outer.hpp"

#include <array>
#include <cctype>
#include <cstring>
#include <string_view>
#include <vector>

#include "cbor/strict.hpp"

namespace VMPilot::Runtime::Envelope {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

// Field ids for the outer envelope metadata map. Small unsigned integer
// keys per the canonical metadata convention (doc 07 §3.1).
constexpr std::uint64_t kField_OuterFormatVersion         = 1;
constexpr std::uint64_t kField_PackageSchemaVersion       = 2;
constexpr std::uint64_t kField_CanonicalEncodingId        = 3;
constexpr std::uint64_t kField_SectionTableShapeClass     = 4;
constexpr std::uint64_t kField_PackageBindingRecordLoc    = 5;
constexpr std::uint64_t kField_InnerMetadataPartitionLoc  = 6;
constexpr std::uint64_t kField_PayloadPartitionLoc        = 7;

// Locator map field ids.
constexpr std::uint64_t kLocator_Offset = 1;
constexpr std::uint64_t kLocator_Length = 2;

// Tokens the outer envelope is forbidden from exposing. A substring match
// (case-insensitive) on any text / byte string inside the metadata map
// trips TierRevealingToken. This is defense-in-depth against mis-built
// artifacts; the canonical rule is enforced by the producer. See doc 15 §3.
constexpr std::array<std::string_view, 13> kForbiddenTokens = {
    "debug", "standard", "highsec",
    "f1", "f2", "f3",
    "tpm", "tee", "premium", "cloud",
    "provider_class",
    "resolved_family_profile_id",
    "policy_id",
};

inline tl::unexpected<ParseError> err(ParseError e) noexcept {
    return tl::make_unexpected(e);
}

std::uint32_t read_u32_be(const std::uint8_t* p) noexcept {
    return (static_cast<std::uint32_t>(p[0]) << 24) |
           (static_cast<std::uint32_t>(p[1]) << 16) |
           (static_cast<std::uint32_t>(p[2]) <<  8) |
           (static_cast<std::uint32_t>(p[3]));
}

// Case-insensitive substring search over ASCII bytes. Forbidden tokens are
// all ASCII; any byte outside [0,0x7f] cannot match so we can keep the
// comparison trivial.
bool contains_token_ci(std::string_view haystack, std::string_view needle) noexcept {
    if (needle.empty() || needle.size() > haystack.size()) return false;
    for (std::size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
        bool match = true;
        for (std::size_t j = 0; j < needle.size(); ++j) {
            const auto hc = static_cast<unsigned char>(haystack[i + j]);
            const auto nc = static_cast<unsigned char>(needle[j]);
            const auto lh = static_cast<unsigned char>(std::tolower(hc));
            const auto ln = static_cast<unsigned char>(std::tolower(nc));
            if (lh != ln) { match = false; break; }
        }
        if (match) return true;
    }
    return false;
}

bool any_forbidden_token(std::string_view s) noexcept {
    for (const auto& t : kForbiddenTokens) {
        if (contains_token_ci(s, t)) return true;
    }
    return false;
}

// Walk the parsed tree and flag TierRevealingToken if any text or byte
// string carries a forbidden token. Depth-limited recursion mirrors the
// decoder cap.
bool tree_contains_forbidden_text(const Value& v, unsigned depth = 0) noexcept {
    if (depth > VMPilot::Cbor::kMaxDepth) return false;
    switch (v.kind()) {
        case Value::Kind::Text:
            return any_forbidden_token(v.as_text());
        case Value::Kind::Bytes: {
            const auto& b = v.as_bytes();
            if (b.empty()) return false;
            // Treat bytes as candidate ASCII text for scan purposes only.
            // Non-ASCII bytes can't match any ASCII forbidden token so
            // this conservatively catches accidental string smuggling.
            std::string_view sv(reinterpret_cast<const char*>(b.data()), b.size());
            return any_forbidden_token(sv);
        }
        case Value::Kind::Array:
            for (const auto& c : v.as_array())
                if (tree_contains_forbidden_text(c, depth + 1)) return true;
            return false;
        case Value::Kind::Map:
            for (std::size_t i = 0; i < v.map_size(); ++i) {
                if (tree_contains_forbidden_text(v.map_key_at(i), depth + 1)) return true;
                if (tree_contains_forbidden_text(v.map_value_at(i), depth + 1)) return true;
            }
            return false;
        case Value::Kind::Uint:
        default:
            return false;
    }
}

tl::expected<Locator, ParseError>
extract_locator(const Value& m, std::uint64_t key) noexcept {
    const Value* loc_v = m.find_by_uint_key(key);
    if (loc_v == nullptr) return err(ParseError::MissingCoreField);
    if (loc_v->kind() != Value::Kind::Map) return err(ParseError::WrongFieldType);

    const Value* off = loc_v->find_by_uint_key(kLocator_Offset);
    const Value* len = loc_v->find_by_uint_key(kLocator_Length);
    if (off == nullptr || len == nullptr) return err(ParseError::MissingCoreField);
    if (off->kind() != Value::Kind::Uint || len->kind() != Value::Kind::Uint) {
        return err(ParseError::WrongFieldType);
    }
    return Locator{off->as_uint(), len->as_uint()};
}

// Checked [offset, offset + length) bounds within [0, buffer_size).
bool locator_fits(const Locator& L, std::uint64_t buffer_size) noexcept {
    if (L.length == 0) return false;
    const std::uint64_t end = L.offset + L.length;
    if (end < L.offset) return false;             // overflow
    if (end > buffer_size) return false;
    return true;
}

bool locators_overlap(const Locator& a, const Locator& b) noexcept {
    const std::uint64_t a_end = a.offset + a.length;
    const std::uint64_t b_end = b.offset + b.length;
    return !(a_end <= b.offset || b_end <= a.offset);
}

}  // namespace

tl::expected<OuterEnvelope, ParseError>
parse_outer_envelope(const std::uint8_t* data, std::size_t size) noexcept {
    if (data == nullptr || size < kOuterFixedHeaderSize) {
        return err(ParseError::BufferTooSmall);
    }
    if (std::memcmp(data, kOuterMagic.data(), kOuterMagic.size()) != 0) {
        return err(ParseError::WrongMagic);
    }
    const std::uint32_t metadata_len = read_u32_be(data + 16);
    if (static_cast<std::uint64_t>(metadata_len) >
        static_cast<std::uint64_t>(size) - kOuterFixedHeaderSize) {
        return err(ParseError::TruncatedMetadata);
    }

    const std::uint8_t* meta_bytes = data + kOuterFixedHeaderSize;
    auto root_or = parse_strict(meta_bytes, metadata_len);
    if (!root_or) return err(ParseError::BadCbor);
    const Value& root = *root_or;

    if (root.kind() != Value::Kind::Map) return err(ParseError::WrongFieldType);

    // Format version gate. Reject unknown versions up-front so later-stage
    // consumers can rely on the parsed struct matching kOuterFormatVersionV1
    // semantics.
    const Value* version_v = root.find_by_uint_key(kField_OuterFormatVersion);
    if (version_v == nullptr) return err(ParseError::MissingCoreField);
    if (version_v->kind() != Value::Kind::Uint) return err(ParseError::WrongFieldType);
    const std::uint64_t version = version_v->as_uint();
    if (version != kOuterFormatVersionV1) {
        return err(ParseError::UnsupportedFormatVersion);
    }

    const Value* schema_v = root.find_by_uint_key(kField_PackageSchemaVersion);
    const Value* encoding_v = root.find_by_uint_key(kField_CanonicalEncodingId);
    const Value* shape_v = root.find_by_uint_key(kField_SectionTableShapeClass);
    if (schema_v == nullptr || encoding_v == nullptr || shape_v == nullptr) {
        return err(ParseError::MissingCoreField);
    }
    if (schema_v->kind() != Value::Kind::Text ||
        encoding_v->kind() != Value::Kind::Text ||
        shape_v->kind() != Value::Kind::Uint) {
        return err(ParseError::WrongFieldType);
    }

    OuterEnvelope out;
    out.outer_format_version = version;
    out.package_schema_version = schema_v->as_text();
    out.canonical_encoding_id = encoding_v->as_text();
    out.section_table_shape_class = shape_v->as_uint();
    out.metadata_offset = kOuterFixedHeaderSize;
    out.metadata_length = metadata_len;

    auto pbr_or   = extract_locator(root, kField_PackageBindingRecordLoc);
    auto inner_or = extract_locator(root, kField_InnerMetadataPartitionLoc);
    auto payload_or = extract_locator(root, kField_PayloadPartitionLoc);
    if (!pbr_or)     return err(pbr_or.error());
    if (!inner_or)   return err(inner_or.error());
    if (!payload_or) return err(payload_or.error());

    out.package_binding_record = *pbr_or;
    out.inner_metadata_partition = *inner_or;
    out.payload_partition = *payload_or;

    if (!locator_fits(out.package_binding_record, size) ||
        !locator_fits(out.inner_metadata_partition, size) ||
        !locator_fits(out.payload_partition, size)) {
        return err(ParseError::BadLocator);
    }

    // All three data partitions must sit strictly after the fixed header +
    // metadata, and must not overlap each other. Callers expect the
    // envelope bytes themselves to be exactly `[0, kOuterFixedHeaderSize +
    // metadata_length)`, so a locator that straddles into that region
    // would either shadow metadata bytes or imply a malformed layout.
    const std::uint64_t data_start =
        static_cast<std::uint64_t>(kOuterFixedHeaderSize) + metadata_len;
    if (out.package_binding_record.offset < data_start ||
        out.inner_metadata_partition.offset < data_start ||
        out.payload_partition.offset < data_start) {
        return err(ParseError::BadLocator);
    }
    if (locators_overlap(out.package_binding_record,
                         out.inner_metadata_partition) ||
        locators_overlap(out.package_binding_record, out.payload_partition) ||
        locators_overlap(out.inner_metadata_partition, out.payload_partition)) {
        return err(ParseError::OverlappingLocators);
    }

    // Tier-leakage scan. Runs after the structural checks so we only scan
    // well-formed CBOR trees (bounded nesting, length-checked).
    if (tree_contains_forbidden_text(root)) {
        return err(ParseError::TierRevealingToken);
    }

    return out;
}

tl::expected<OuterEnvelope, ParseError>
parse_outer_envelope(std::string_view bytes) noexcept {
    return parse_outer_envelope(
        reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size());
}

}  // namespace VMPilot::Runtime::Envelope
