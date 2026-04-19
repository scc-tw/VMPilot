#ifndef VMPILOT_COMMON_CBOR_STRICT_HPP
#define VMPILOT_COMMON_CBOR_STRICT_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

// Strict subset CBOR decoder for binding-critical metadata.
//
//   • Major types 0 (uint), 2 (bytes), 3 (text), 4 (array), 5 (map).
//   • Shortest-form unsigned integer encoding.
//   • Definite-length for all items.
//   • No floats, tags, negative ints, reserved or indefinite-length items.
//   • Map keys bytewise lexicographic ascending; duplicates rejected.
//   • No trailing bytes.
//   • UTF-8 validity not enforced here — text strings are treated as opaque
//     bytes and any schema-specific text semantics live above this layer.
//
// The parser owns parsed state; the source byte buffer may be released after
// `parse_strict` returns.

namespace VMPilot::Cbor {

// Maximum nesting depth — more than enough for any binding record schema.
constexpr std::size_t kMaxDepth = 16;

enum class ErrorCode : std::uint8_t {
    // Structural / encoding issues.
    TruncatedInput = 1,
    TrailingBytes,
    NonCanonicalInteger,      // integer encoded wider than necessary
    IndefiniteLength,         // any major type with additional-info 31
    ReservedAdditionalInfo,   // additional-info 28..30
    DisallowedMajorType,      // negint, tag, float/simple
    DepthLimitExceeded,

    // Map-specific.
    DuplicateMapKey,
    MapKeyOutOfOrder,

    // Access helpers (non-parse errors, returned by query helpers).
    FieldNotFound,
    FieldTypeMismatch,
};

struct ParseError {
    ErrorCode code;
    std::size_t offset;       // byte position at which the rule fired
};

// Owning CBOR value. Parsed trees are small (binding record size), so a
// straightforward owning variant is simpler than a pool-allocated graph.
class Value {
public:
    enum class Kind : std::uint8_t {
        Uint,
        Bytes,
        Text,
        Array,
        Map,
    };

    Kind kind() const noexcept { return kind_; }

    std::uint64_t as_uint() const noexcept { return uint_; }
    const std::vector<std::uint8_t>& as_bytes() const noexcept { return bytes_; }
    const std::string& as_text() const noexcept { return text_; }
    const std::vector<Value>& as_array() const noexcept { return children_; }

    // Maps are stored flat as [k0, v0, k1, v1, ...] — keeps the declared
    // canonical ordering stable across copies and makes iteration obvious.
    std::size_t map_size() const noexcept { return children_.size() / 2; }
    const Value& map_key_at(std::size_t i) const noexcept { return children_[2 * i]; }
    const Value& map_value_at(std::size_t i) const noexcept { return children_[2 * i + 1]; }

    // Lookup helpers: return pointer so caller can distinguish "absent" from
    // "present but wrong type" without exceptions.
    const Value* find_by_uint_key(std::uint64_t key) const noexcept;
    const Value* find_by_text_key(std::string_view key) const noexcept;

    // Factories used by the parser; public so tests can build expected trees.
    static Value make_uint(std::uint64_t v);
    static Value make_bytes(std::vector<std::uint8_t> b);
    static Value make_text(std::string s);
    static Value make_array(std::vector<Value> children);
    // `flat_kv` must already be in canonical key order with no duplicates.
    static Value make_map(std::vector<Value> flat_kv);

private:
    Kind kind_ = Kind::Uint;
    std::uint64_t uint_ = 0;
    std::vector<std::uint8_t> bytes_;
    std::string text_;
    std::vector<Value> children_;
};

// Parse `[data, data+size)` as strict-subset canonical CBOR.
// Returns the parsed root Value on success or a ParseError with the offset
// at which the first violation was detected.
tl::expected<Value, ParseError>
parse_strict(const std::uint8_t* data, std::size_t size) noexcept;

// Convenience overload.
inline tl::expected<Value, ParseError>
parse_strict(const std::vector<std::uint8_t>& bytes) noexcept {
    return parse_strict(bytes.data(), bytes.size());
}

// Domain-separated hash helper matching the scheme in
//
//   out = SHA-256( length_prefix(domain_label) || canonical_bytes )
//
// `length_prefix` is a single byte carrying the domain label length followed
// by the label bytes; the same rule used by the Ed25519 covered-domain
// prefix so label behavior is shared across hash and signature layers.
//
// `domain_label` must be non-empty and <= 255 bytes; violations are fatal
// (the function aborts with `std::terminate` via `std::abort` — callers
// pass constexpr string_views from `common/include/vm/domain_labels.hpp`
// so this is a programmer error, not a runtime input).
std::array<std::uint8_t, 32>
domain_hash_sha256(std::string_view domain_label,
                   const std::uint8_t* canonical_bytes,
                   std::size_t size) noexcept;

inline std::array<std::uint8_t, 32>
domain_hash_sha256(std::string_view domain_label,
                   const std::vector<std::uint8_t>& canonical_bytes) noexcept {
    return domain_hash_sha256(domain_label, canonical_bytes.data(), canonical_bytes.size());
}

// ─── Field extraction helpers ───────────────────────────────────────────
//
// Before this header, seven runtime TUs each rolled their own
// require_text / require_uint / require_hash / require_bool / require_bytes
// with the same body parameterised only by the consumer's error enum.
// Collapse them into generic templates whose error mapping is declared
// once per consumer by specializing `RequireErrors<E>`.
//
// Usage (consumer side):
//
//   namespace VMPilot::Cbor {
//   template <> struct RequireErrors<MyError> {
//       static constexpr MyError missing_field    = MyError::MissingField;
//       static constexpr MyError wrong_field_type = MyError::WrongFieldType;
//       static constexpr MyError wrong_hash_size  = MyError::WrongHashSize;
//   };
//   }
//
// Then call `require_text<MyError>(map, key)` etc. No error enum has to
// ship every constant — fields that a particular consumer never calls
// don't need to exist in the specialization.

template <typename E>
struct RequireErrors;

namespace detail {

// Uniform error lookup — `RequireErrors<E>::missing_field` etc. must be
// `constexpr static E`. Wrapped behind helper functions so specialization
// is opt-in (specializations that don't need wrong_hash_size just don't
// declare it; callers that never use require_hash don't trip a lookup).
template <typename E>
[[nodiscard]] constexpr E missing_field_code() noexcept {
    return RequireErrors<E>::missing_field;
}
template <typename E>
[[nodiscard]] constexpr E wrong_field_type_code() noexcept {
    return RequireErrors<E>::wrong_field_type;
}
template <typename E>
[[nodiscard]] constexpr E wrong_hash_size_code() noexcept {
    return RequireErrors<E>::wrong_hash_size;
}

}  // namespace detail

template <typename E>
[[nodiscard]] inline tl::expected<const std::string*, E>
require_text_ref(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return tl::make_unexpected(detail::missing_field_code<E>());
    if (v->kind() != Value::Kind::Text) {
        return tl::make_unexpected(detail::wrong_field_type_code<E>());
    }
    return &v->as_text();
}

template <typename E>
[[nodiscard]] inline tl::expected<std::string, E>
require_text(const Value& m, std::uint64_t key) noexcept {
    auto ref = require_text_ref<E>(m, key);
    if (!ref) return tl::make_unexpected(ref.error());
    return **ref;
}

template <typename E>
[[nodiscard]] inline tl::expected<std::uint64_t, E>
require_uint(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return tl::make_unexpected(detail::missing_field_code<E>());
    if (v->kind() != Value::Kind::Uint) {
        return tl::make_unexpected(detail::wrong_field_type_code<E>());
    }
    return v->as_uint();
}

template <typename E>
[[nodiscard]] inline tl::expected<const std::vector<std::uint8_t>*, E>
require_bytes_ref(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return tl::make_unexpected(detail::missing_field_code<E>());
    if (v->kind() != Value::Kind::Bytes) {
        return tl::make_unexpected(detail::wrong_field_type_code<E>());
    }
    return &v->as_bytes();
}

template <typename E>
[[nodiscard]] inline tl::expected<std::vector<std::uint8_t>, E>
require_bytes(const Value& m, std::uint64_t key) noexcept {
    auto ref = require_bytes_ref<E>(m, key);
    if (!ref) return tl::make_unexpected(ref.error());
    return **ref;
}

template <typename E, std::size_t N = 32>
[[nodiscard]] inline tl::expected<std::array<std::uint8_t, N>, E>
require_hash(const Value& m, std::uint64_t key) noexcept {
    auto bytes_or = require_bytes_ref<E>(m, key);
    if (!bytes_or) return tl::make_unexpected(bytes_or.error());
    const auto& b = **bytes_or;
    if (b.size() != N) {
        return tl::make_unexpected(detail::wrong_hash_size_code<E>());
    }
    std::array<std::uint8_t, N> out{};
    std::memcpy(out.data(), b.data(), N);
    return out;
}

// CBOR strict subset has no dedicated bool kind — the convention is
// uint{0,1}. Anything outside that range is WrongFieldType, matching
// the behaviour consumers wrote manually.
template <typename E>
[[nodiscard]] inline tl::expected<bool, E>
require_uint_bool(const Value& m, std::uint64_t key) noexcept {
    auto u = require_uint<E>(m, key);
    if (!u) return tl::make_unexpected(u.error());
    if (*u == 0) return false;
    if (*u == 1) return true;
    return tl::make_unexpected(detail::wrong_field_type_code<E>());
}

}  // namespace VMPilot::Cbor

#endif  // VMPILOT_COMMON_CBOR_STRICT_HPP
