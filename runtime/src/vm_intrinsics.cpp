/// @file vm_intrinsics.cpp
/// @brief Intrinsic ID to function pointer resolution.

#include "vm_intrinsics.hpp"
#include <tls_helpers.hpp>

namespace VMPilot::Runtime {

void* resolve_intrinsic(IntrinsicId id) noexcept {
    switch (id) {
    case IntrinsicId::TLS_READ64:
        return reinterpret_cast<void*>(&vmpilot_tls_read64);
    case IntrinsicId::TLS_WRITE64:
        return reinterpret_cast<void*>(&vmpilot_tls_write64);
    case IntrinsicId::TLS_READ32:
        return reinterpret_cast<void*>(&vmpilot_tls_read32);
    case IntrinsicId::TLS_WRITE32:
        return reinterpret_cast<void*>(&vmpilot_tls_write32);
    default:
        return nullptr;
    }
}

}  // namespace VMPilot::Runtime
