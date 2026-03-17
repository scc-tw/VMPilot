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

// Forward declarations of protobuf types (avoid heavy pb.h include in header)
// Specializations are implemented in SerializationTraits.cpp where pb.h is included.

/// Specialization: CompilationContext ↔ vmpilot::CompilationContext
template <>
struct SerializationTraits<Segmentator::CompilationContext> {
    // to_proto and from_proto declared here, defined in .cpp
    // We use std::string as serialized form to avoid exposing protobuf types in the header.

    static tl::expected<std::string, std::string>
    to_bytes(const Segmentator::CompilationContext& ctx);

    static tl::expected<Segmentator::CompilationContext, std::string>
    from_bytes(const std::string& bytes);
};

/// Specialization: CompilationUnit ↔ vmpilot::CompilationUnit
template <>
struct SerializationTraits<Core::CompilationUnit> {
    static tl::expected<std::string, std::string>
    to_bytes(const Core::CompilationUnit& unit);

    static tl::expected<Core::CompilationUnit, std::string>
    from_bytes(const std::string& bytes,
               std::shared_ptr<const Segmentator::CompilationContext> ctx);
};

/// Generic serialize: C++ object → protobuf bytes
template <typename T>
tl::expected<std::string, std::string> serialize(const T& obj) {
    return SerializationTraits<T>::to_bytes(obj);
}

/// Generic deserialize: protobuf bytes → C++ object
template <typename T, typename... Args>
tl::expected<T, std::string> deserialize(const std::string& bytes,
                                         Args&&... args) {
    return SerializationTraits<T>::from_bytes(bytes,
                                              std::forward<Args>(args)...);
}

}  // namespace VMPilot::SDK::Serializer

#endif  // __SDK_SERIALIZER_SERIALIZATION_TRAITS_HPP__
