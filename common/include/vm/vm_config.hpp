#pragma once
#ifndef __COMMON_VM_CONFIG_HPP__
#define __COMMON_VM_CONFIG_HPP__

#include <cstdint>

namespace VMPilot::Common::VM {

/// Security-related tuning knobs (ISA Design v1, Section 0).
struct VmSecurityConfig {
    uint8_t  fusion_granularity   = 2;    ///< handler fusion group size
    uint8_t  encoding_evolution_n = 5;    ///< re-encode every N basic blocks
    uint16_t mutation_interval    = 64;   ///< ISA mutation period (instructions)
    uint16_t anti_debug_interval  = 128;  ///< anti-debug check period (instructions)
    bool     debug_mode           = false; ///< enable debug diagnostics
};

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_CONFIG_HPP__
