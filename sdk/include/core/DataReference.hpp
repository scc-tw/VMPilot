#ifndef __SDK_CORE_DATA_REFERENCE_HPP__
#define __SDK_CORE_DATA_REFERENCE_HPP__
#pragma once

#include <cstdint>
#include <string>

namespace VMPilot::SDK::Core {

enum class DataRefKind : uint8_t {
    Unknown,
    GlobalVar,     // .data / .bss reference
    ReadOnlyData,  // .rodata / .rdata reference
    GotLoad,       // GOT-relative load
    TlsVar,        // thread-local storage variable
    JumpTable,     // TODO: jump table in .rodata (placeholder)
};

enum class TlsModel : uint8_t {
    None,
    LocalExec,       // fs:/gs: segment or TPIDR_EL0
    InitialExec,     // GOT-indirect TLS (GOTTPOFF)
    LocalDynamic,    // __tls_get_addr with TLSLD reloc
    GeneralDynamic,  // __tls_get_addr with TLSGD reloc
};

enum class DataRefSource : uint8_t {
    Relocation,    // Layer 1: parsed from .rela.text
    InsnAnalysis,  // Layer 2: instruction operand analysis
    PatternMatch,  // Layer 3: pattern-based detection
};

enum class AtomicWidth : uint8_t {
    None,
    Atomic8,   // TODO: reserved for future atomic detection
    Atomic16,  // TODO: reserved for future atomic detection
    Atomic32,  // TODO: reserved for future atomic detection
    Atomic64,  // TODO: reserved for future atomic detection
};

struct RelocationEntry {
    uint64_t offset = 0;        // offset within .text
    uint32_t type = 0;          // ELF relocation type (e.g. R_X86_64_PC32)
    uint32_t symbol_index = 0;
    int64_t addend = 0;
    std::string symbol_name;
};

struct DataReference {
    uint64_t insn_offset = 0;     // instruction address (VA)
    uint64_t target_va = 0;       // resolved target virtual address
    std::string target_symbol;    // symbol name (if known)
    DataRefKind kind = DataRefKind::Unknown;
    TlsModel tls_model = TlsModel::None;
    DataRefSource source = DataRefSource::InsnAnalysis;
    uint32_t access_size = 0;     // bytes accessed
    bool is_write = false;        // true if instruction writes to target
    bool is_pc_relative = false;  // true if RIP/PC-relative addressing
    AtomicWidth atomic_width = AtomicWidth::None;

    /// Original relocation entry (populated by Layer 1)
    RelocationEntry relocation;
};

}  // namespace VMPilot::SDK::Core

#endif  // __SDK_CORE_DATA_REFERENCE_HPP__
