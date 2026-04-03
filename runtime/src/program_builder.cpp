/// @file program_builder.cpp
/// @brief BBBuilder method implementations.
///
/// Extracted from program_builder.hpp — the constructor, native_call(string),
/// and end_bb() are non-trivial and benefit from being in a .cpp for debugging.
/// The trivial one-liner chainable methods (halt(), add(), cmp(), etc.) remain
/// inline in the header.

#include "program_builder.hpp"
#include <native_registry.hpp>

#include <stdexcept>

namespace VMPilot::Runtime {

BBBuilder::BBBuilder(ProgramBuilder& parent, uint32_t bb_id,
                     uint32_t epoch, uint8_t epoch_seed_base)
    : parent_(parent)
{
    bb_.bb_id = bb_id;
    bb_.epoch = epoch;
    bb_.live_regs_bitmap = 0xFFFF;  // all registers live by default
    bb_.flags = 0;
    for (int i = 0; i < 32; ++i)
        bb_.epoch_seed[i] = static_cast<uint8_t>(epoch_seed_base + i);
}

BBBuilder& BBBuilder::native_call(const std::string& name) {
    NativeRegistry* reg = parent_.registry();
    if (!reg)
        throw std::runtime_error("ProgramBuilder: native_call(\"" + name +
                                 "\") requires a NativeRegistry");
    uint32_t idx = reg->index_of(name);
    return native_call_raw(idx);
}

ProgramBuilder& BBBuilder::end_bb() {
    parent_.push_bb(std::move(bb_));
    return parent_;
}

}  // namespace VMPilot::Runtime
