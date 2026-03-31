/// @file vm_state.cpp
/// @brief Out-of-line definitions for vm_state.hpp types.
///
/// VmEpoch::~VmEpoch() must be defined here (not in the header) because
/// unique_ptr<CompositionCache> needs the complete type for deletion,
/// and CompositionCache is forward-declared in vm_state.hpp.

#include "vm_state.hpp"
#include "composition_cache_v2.hpp"

namespace VMPilot::Runtime {

VmEpoch::VmEpoch() noexcept = default;
VmEpoch::~VmEpoch() = default;
VmEpoch::VmEpoch(VmEpoch&&) noexcept = default;
VmEpoch& VmEpoch::operator=(VmEpoch&&) noexcept = default;

}  // namespace VMPilot::Runtime
