/// @file vm_state.cpp
/// @brief Out-of-line definitions for vm_state.hpp types.
///
/// VmEpoch::enter_bb() is defined here to avoid pulling vm_encoding.hpp
/// into the header.  The old special member functions (destructor, move
/// ctor/assign) are gone — VmEpoch no longer owns a unique_ptr, so the
/// compiler-generated defaults are correct and trivially copyable.

#include "vm_state.hpp"
#include <vm/vm_encoding.hpp>

namespace VMPilot::Runtime {

void VmEpoch::enter_bb(const BBMetadata& bb) noexcept {
    bb_id = bb.bb_id;
    epoch = bb.epoch;
    live_regs_bitmap = bb.live_regs_bitmap;

    // D4: derive opcode permutation from epoch_seed.
    // This is the only per-BB derivation that remains after doc 16.
    // Register encoding is now Speck-FPE, keyed from VmExecution::insn_fpe_key.
    Encoding::derive_opcode_permutation(
        bb.epoch_seed, opcode_perm, opcode_perm_inv);
}

}  // namespace VMPilot::Runtime
