#ifndef __SDK_CORE_DATA_REFERENCE_HPP__
#define __SDK_CORE_DATA_REFERENCE_HPP__
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::Core {

enum class DataRefKind : uint8_t {
    Unknown,
    GlobalVar,     // .data / .bss reference
    ReadOnlyData,  // .rodata / .rdata reference
    GotLoad,       // GOT-relative load
    TlsVar,        // thread-local storage variable
    JumpTable,     // indirect jump through .rodata jump table
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
    Atomic8,
    Atomic16,
    Atomic32,
    Atomic64,
    Atomic128,  // lock cmpxchg16b
};

enum class AtomicOrdering : uint8_t {
    None,
    Relaxed,   // ARM64 no suffix (cas, swp, ldadd...)
    Acquire,   // ARM64 'a' suffix (casa, swpa, ldar...)
    Release,   // ARM64 'l' suffix (casl, stlr...)
    AcqRel,    // ARM64 'al' suffix / x86 lock (seq_cst equivalent)
};

enum class AtomicOp : uint8_t {
    None,
    LoadExclusive,   // ARM64 ldxr/ldaxr
    StoreExclusive,  // ARM64 stxr/stlxr
    CompareSwap,     // lock cmpxchg / cas
    Swap,            // xchg / swp
    FetchAdd,        // lock xadd / ldadd
    RMW,             // lock add/sub/and/or/xor/inc/dec/neg/not/bts/btr/btc
    LoadAcquire,     // ARM64 ldar
    StoreRelease,    // ARM64 stlr
    Fence,           // mfence/sfence/lfence / dmb/dsb/isb
};

struct JumpTableRef {
    uint64_t table_base = 0;       // .rodata table start VA
    uint32_t entry_size = 0;       // 4 or 8 bytes per entry
    uint32_t entry_count = 0;      // 0 = unknown
    bool relative_entries = false;  // true = entries are relative offsets
    std::vector<uint64_t> targets;  // resolved code target addresses
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
    AtomicOrdering atomic_ordering = AtomicOrdering::None;
    AtomicOp atomic_op = AtomicOp::None;

    /// Original relocation entry (populated by Layer 1)
    RelocationEntry relocation;

    /// Jump table metadata (populated when kind == JumpTable)
    std::optional<JumpTableRef> jump_table;

    /// True when target is in a mutable section (.data, .bss).
    /// Backend must preserve memory ordering and not constant-fold.
    bool is_mutable_target = false;
};

}  // namespace VMPilot::SDK::Core

#endif  // __SDK_CORE_DATA_REFERENCE_HPP__
