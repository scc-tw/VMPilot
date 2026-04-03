#pragma once
#ifndef __COMMON_VM_BUILDER_TYPES_HPP__
#define __COMMON_VM_BUILDER_TYPES_HPP__

/// @file builder_types.hpp
/// @brief Shared builder types for VM bytecode assembly.
///
/// These POD types describe instructions, basic blocks, and pool entries
/// in their pre-encryption form.  Used by:
///   - Compiler backend (sdk): emits BuilderBB from IR lowering
///   - Runtime ProgramBuilder (runtime): fluent DSL for test programs
///   - Blob builder (runtime): encrypts BuilderBB → blob sections
///   - Chaff expansion (common): inserts NOP padding for timing normalization
///
/// Extracted from runtime/include/program_builder.hpp to common/ so that
/// both the sdk compiler and runtime can depend on them.

#include <vm/vm_opcode.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Common::VM {

/// Instruction descriptor — one plaintext VM instruction before encryption.
struct BuilderInstruction {
    VmOpcode opcode;
    uint8_t  flags;
    uint8_t  reg_a;
    uint8_t  reg_b;
    uint32_t aux;
};

/// Basic block descriptor — a sequence of instructions with encryption metadata.
struct BuilderBB {
    uint32_t bb_id;
    uint32_t epoch;
    uint16_t live_regs_bitmap;
    uint16_t flags;
    uint8_t  epoch_seed[32];
    std::vector<BuilderInstruction> instructions;
};

/// Pool entry descriptor for per-register encoding.
struct BuilderPoolEntry {
    uint64_t plaintext;
    uint32_t target_bb_index;  ///< which BB's encoding tables to use
    uint8_t  target_reg;       ///< which register's encoding to use
};

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_BUILDER_TYPES_HPP__
