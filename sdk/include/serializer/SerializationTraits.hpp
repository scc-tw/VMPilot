#ifndef __SDK_SERIALIZER_SERIALIZATION_TRAITS_HPP__
#define __SDK_SERIALIZER_SERIALIZATION_TRAITS_HPP__
#pragma once

#include <CompilationContext.hpp>
#include <CompilationUnit.hpp>

#include <tl/expected.hpp>

#include <memory>
#include <string>

namespace VMPilot::SDK::Serializer {

/// Primary template — unspecialized types produce a compile error.
template <typename CppType>
struct SerializationTraits;

/// Specialization: CompilationContext
template <>
struct SerializationTraits<Segmentator::CompilationContext> {

    static tl::expected<std::string, std::string>
    to_bytes(const Segmentator::CompilationContext& ctx);

    static tl::expected<Segmentator::CompilationContext, std::string>
    from_bytes(const std::string& bytes);
};

/// Specialization: CompilationUnit
template <>
struct SerializationTraits<Core::CompilationUnit> {
    static tl::expected<std::string, std::string>
    to_bytes(const Core::CompilationUnit& unit);

    static tl::expected<Core::CompilationUnit, std::string>
    from_bytes(const std::string& bytes,
               std::shared_ptr<const Segmentator::CompilationContext> ctx);
};

/// Generic serialize: C++ object → binary bytes
template <typename T>
tl::expected<std::string, std::string> serialize(const T& obj) {
    return SerializationTraits<T>::to_bytes(obj);
}

/// Generic deserialize: binary bytes → C++ object
template <typename T, typename... Args>
tl::expected<T, std::string> deserialize(const std::string& bytes,
                                         Args&&... args) {
    return SerializationTraits<T>::from_bytes(bytes,
                                              std::forward<Args>(args)...);
}

}  // namespace VMPilot::SDK::Serializer

#endif  // __SDK_SERIALIZER_SERIALIZATION_TRAITS_HPP__
