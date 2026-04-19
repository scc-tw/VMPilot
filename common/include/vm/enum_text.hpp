#ifndef VMPILOT_COMMON_VM_ENUM_TEXT_HPP
#define VMPILOT_COMMON_VM_ENUM_TEXT_HPP

#include <array>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

// EnumTextTraits<E> — single source of truth for enum ↔ canonical
// on-wire text mapping. Specialize once per enum; the generic
// `enum_to_text(E)` / `enum_from_text<E>(string_view)` helpers below
// then work for every specialization without per-enum switch
// duplication.
//
// Usage:
//
//   enum class MyEnum : std::uint8_t { Foo = 1, Bar, Baz };
//
//   template <>
//   struct EnumTextTraits<MyEnum> {
//       static constexpr std::array<std::pair<MyEnum, std::string_view>, 3>
//           entries{{
//               {MyEnum::Foo, "foo"},
//               {MyEnum::Bar, "bar"},
//               {MyEnum::Baz, "baz"},
//           }};
//   };
//
//   // Callers:
//   VMPilot::enum_to_text(MyEnum::Bar);        // -> "bar"
//   VMPilot::enum_from_text<MyEnum>("baz");    // -> MyEnum::Baz
//
// Per-enum `to_text(E)` / `parse_*_id(sv)` free functions are kept
// for backwards-compat at call sites; they delegate into the generic
// helpers here.

namespace VMPilot {

template <typename E>
struct EnumTextTraits;

namespace detail {

template <typename E, typename = void>
struct has_enum_text_traits : std::false_type {};

template <typename E>
struct has_enum_text_traits<E,
    std::void_t<decltype(EnumTextTraits<E>::entries)>> : std::true_type {};

}  // namespace detail

// `enum_to_text(e)`: returns the canonical on-wire text for a known
// enumerator, or an empty string_view for values outside the declared
// entries (should be unreachable with valid enum values; matches the
// empty-sv fallback that the hand-rolled `to_text` overloads used).
template <typename E,
          typename = std::enable_if_t<detail::has_enum_text_traits<E>::value>>
[[nodiscard]] constexpr std::string_view enum_to_text(E e) noexcept {
    for (const auto& entry : EnumTextTraits<E>::entries) {
        if (entry.first == e) return entry.second;
    }
    return {};
}

// `enum_from_text<E>(sv)`: returns the enumerator matching `sv`, or
// std::nullopt if no entry matches. Used at CBOR parse boundaries
// where the producer might ship a text value the runtime doesn't
// know — the runtime converts std::nullopt into an UnknownEnumValue
// reject.
template <typename E,
          typename = std::enable_if_t<detail::has_enum_text_traits<E>::value>>
[[nodiscard]] constexpr std::optional<E> enum_from_text(std::string_view s) noexcept {
    for (const auto& entry : EnumTextTraits<E>::entries) {
        if (entry.second == s) return entry.first;
    }
    return std::nullopt;
}

}  // namespace VMPilot

#endif  // VMPILOT_COMMON_VM_ENUM_TEXT_HPP
