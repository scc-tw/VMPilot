/// @file native_registry.cpp
/// @brief NativeRegistry method implementations.
///
/// Extracted from native_registry.hpp — non-template class, no reason
/// to keep method bodies inline in the header.

#include "native_registry.hpp"

#include <stdexcept>

namespace VMPilot::Runtime {

uint32_t NativeRegistry::add(const std::string& name, uint64_t fn_addr,
                             uint8_t arg_count, uint8_t fp_mask,
                             bool is_variadic) {
    if (name_to_index_.count(name))
        throw std::runtime_error("NativeRegistry: duplicate name '" + name + "'");

    uint32_t idx = static_cast<uint32_t>(entries_.size());
    entries_.push_back({name, fn_addr, arg_count, fp_mask, is_variadic, idx});
    name_to_index_[name] = idx;
    return idx;
}

uint32_t NativeRegistry::index_of(const std::string& name) const {
    auto it = name_to_index_.find(name);
    if (it == name_to_index_.end())
        throw std::runtime_error("NativeRegistry: unknown native '" + name + "'");
    return it->second;
}

bool NativeRegistry::has(const std::string& name) const noexcept {
    return name_to_index_.count(name) > 0;
}

const NativeDescriptor& NativeRegistry::at(uint32_t idx) const {
    return entries_.at(idx);
}

uint32_t NativeRegistry::size() const noexcept {
    return static_cast<uint32_t>(entries_.size());
}

const std::vector<NativeDescriptor>& NativeRegistry::entries() const noexcept {
    return entries_;
}

}  // namespace VMPilot::Runtime
