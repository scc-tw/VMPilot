#ifndef VMPILOT_COMMON_CBOR_SCHEMA_HPP
#define VMPILOT_COMMON_CBOR_SCHEMA_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <tl/expected.hpp>

#include "cbor/strict.hpp"
#include "vm/enum_text.hpp"

// Schema-driven strict-CBOR parser.
//
// Declarative: describe a struct's on-wire layout as a std::tuple of
// field descriptors, then feed `root` to `parse_schema` to extract
// every field into a populated target struct. Replaces the long
// imperative `require_* + if (!x_or) return ...` chains that parsers
// used to grow to 100+ lines.
//
// Example:
//
//     struct MyRecord {
//         std::string version;
//         FamilyId family;
//         bool flag;
//         std::uint64_t epoch;
//     };
//
//     using namespace VMPilot::Cbor::Schema;
//     auto schema = std::make_tuple(
//         TextField{1, &MyRecord::version, "my-record-v1"},
//         EnumUintField<FamilyId>{2, &MyRecord::family},
//         BoolField{3, &MyRecord::flag},
//         UintField{4, &MyRecord::epoch}
//     );
//     auto rec = parse_schema<MyRecord, MyParseError>(root_value, schema);
//
// Each descriptor carries the field key, the pointer-to-member of the
// target struct, and an optional validation constraint. The parser
// folds over all descriptors, short-circuiting on the first error.

namespace VMPilot::Cbor::Schema {

// ─── Field descriptors ──────────────────────────────────────────────────

// Text field. `required_value` is optional: when non-empty, the parser
// rejects with `unsupported_version` if the actual value differs.
template <typename T>
struct TextField {
    std::uint64_t key;
    std::string T::*member;
    std::string_view required_value = {};
};

template <typename T>
struct TextViewField {
    std::uint64_t key;
    std::string_view T::*member;  // Not owning; view into parsed Value tree.
};

// Unsigned int field.
template <typename T>
struct UintField {
    std::uint64_t key;
    std::uint64_t T::*member;
};

// Bool encoded as uint {0,1}.
template <typename T>
struct BoolField {
    std::uint64_t key;
    bool T::*member;
};

// Enum encoded as uint. Validated via static_cast against enum range
// (must match an existing EnumTextTraits<E> specialization's entries).
template <typename T, typename E>
struct EnumUintField {
    std::uint64_t key;
    E T::*member;
};

// Fixed-length hash bytes.
template <typename T, std::size_t N = 32>
struct HashField {
    std::uint64_t key;
    std::array<std::uint8_t, N> T::*member;
};

// Array of enum text values, validated via EnumTextTraits<E>.
template <typename T, typename E>
struct EnumTextArrayField {
    std::uint64_t key;
    std::vector<E> T::*member;
    std::size_t min_length;
    std::size_t max_length;
};

// ─── Error-mapping trait ────────────────────────────────────────────────
//
// The parser needs to know which error-enum member to emit for each
// class of failure. Consumers specialize this next to their error enum.

template <typename E>
struct SchemaErrors;
// Expected members (per use — specializations may omit unused ones):
//   static constexpr E bad_cbor;
//   static constexpr E not_a_map;
//   static constexpr E missing_field;
//   static constexpr E wrong_field_type;
//   static constexpr E unknown_core_field;
//   static constexpr E unknown_enum_value;
//   static constexpr E array_too_long;
//   static constexpr E unsupported_version;

// ─── Field extractors ───────────────────────────────────────────────────

namespace detail {

template <typename E>
[[nodiscard]] constexpr tl::unexpected<E> unexpected(E e) noexcept {
    return tl::make_unexpected(e);
}

// One extract overload per field-descriptor type. Each returns
// tl::expected<void, E> — on success it has already written into
// `out`.`member`.

template <typename T, typename E>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const TextField<T>& f) noexcept {
    auto v_or = require_text<E>(m, f.key);
    if (!v_or) return unexpected(v_or.error());
    if (!f.required_value.empty() && *v_or != f.required_value) {
        return unexpected(SchemaErrors<E>::unsupported_version);
    }
    out.*(f.member) = std::string(*v_or);
    return {};
}

template <typename T, typename E>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const TextViewField<T>& f) noexcept {
    auto v_or = require_text<E>(m, f.key);
    if (!v_or) return unexpected(v_or.error());
    out.*(f.member) = *v_or;
    return {};
}

template <typename T, typename E>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const UintField<T>& f) noexcept {
    auto v_or = require_uint<E>(m, f.key);
    if (!v_or) return unexpected(v_or.error());
    out.*(f.member) = *v_or;
    return {};
}

template <typename T, typename E>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const BoolField<T>& f) noexcept {
    auto v_or = require_uint_bool<E>(m, f.key);
    if (!v_or) return unexpected(v_or.error());
    out.*(f.member) = *v_or;
    return {};
}

template <typename T, typename E, typename EnumE>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const EnumUintField<T, EnumE>& f) noexcept {
    auto v_or = require_uint<E>(m, f.key);
    if (!v_or) return unexpected(v_or.error());
    // Validate against the enum's declared entries.
    bool known = false;
    for (const auto& entry : EnumTextTraits<EnumE>::entries) {
        if (static_cast<std::uint64_t>(entry.first) == *v_or) {
            known = true;
            out.*(f.member) = entry.first;
            break;
        }
    }
    if (!known) return unexpected(SchemaErrors<E>::unknown_enum_value);
    return {};
}

template <typename T, typename E, std::size_t N>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const HashField<T, N>& f) noexcept {
    auto v_or = require_hash<E, N>(m, f.key);
    if (!v_or) return unexpected(v_or.error());
    out.*(f.member) = *v_or;
    return {};
}

template <typename T, typename E, typename EnumE>
[[nodiscard]] inline tl::expected<void, E>
extract_one(const Value& m, T& out, const EnumTextArrayField<T, EnumE>& f) noexcept {
    const Value* v = m.find_by_uint_key(f.key);
    if (v == nullptr) return unexpected(SchemaErrors<E>::missing_field);
    if (v->kind() != Value::Kind::Array) return unexpected(SchemaErrors<E>::wrong_field_type);
    const auto& items = v->as_array();
    if (items.size() < f.min_length || items.size() > f.max_length) {
        return unexpected(SchemaErrors<E>::array_too_long);
    }
    std::vector<EnumE> tmp;
    tmp.reserve(items.size());
    for (const Value& item : items) {
        if (item.kind() != Value::Kind::Text) {
            return unexpected(SchemaErrors<E>::wrong_field_type);
        }
        auto parsed = VMPilot::enum_from_text<EnumE>(item.as_text());
        if (!parsed.has_value()) {
            return unexpected(SchemaErrors<E>::unknown_enum_value);
        }
        tmp.push_back(*parsed);
    }
    out.*(f.member) = std::move(tmp);
    return {};
}

}  // namespace detail

// Unknown-key rejection. Takes a list of every key declared in the
// schema (call it with the same `known_keys` array at the schema's
// declaration site) and rejects any map entry whose key isn't in the
// list. Kept separate from parse_schema so sites that want flexible
// unknown-key policies can opt out.
template <typename E>
[[nodiscard]] inline tl::expected<void, E>
reject_unknown_keys(const Value& m,
                    std::initializer_list<std::uint64_t> known_keys) noexcept {
    for (std::size_t i = 0; i < m.map_size(); ++i) {
        const Value& key = m.map_key_at(i);
        if (key.kind() != Value::Kind::Uint) {
            return detail::unexpected(SchemaErrors<E>::wrong_field_type);
        }
        const auto k = key.as_uint();
        bool known = false;
        for (auto allowed : known_keys) {
            if (allowed == k) { known = true; break; }
        }
        if (!known) {
            return detail::unexpected(SchemaErrors<E>::unknown_core_field);
        }
    }
    return {};
}

// Core parser. `fields` is a std::tuple of any mixture of field
// descriptors declared above. The parser folds over the tuple,
// short-circuiting on the first error.
template <typename T, typename E, typename... Fs>
[[nodiscard]] inline tl::expected<T, E>
parse_schema(const Value& m, const std::tuple<Fs...>& fields) noexcept {
    if (m.kind() != Value::Kind::Map) {
        return detail::unexpected(SchemaErrors<E>::not_a_map);
    }
    T out{};
    tl::expected<void, E> result{};
    std::apply([&](const auto&... f) {
        auto try_field = [&](const auto& descriptor) {
            if (!result) return;
            auto r = detail::extract_one<T, E>(m, out, descriptor);
            if (!r) result = tl::make_unexpected(r.error());
        };
        (try_field(f), ...);
    }, fields);
    if (!result) return detail::unexpected(result.error());
    return out;
}

}  // namespace VMPilot::Cbor::Schema

#endif  // VMPILOT_COMMON_CBOR_SCHEMA_HPP
