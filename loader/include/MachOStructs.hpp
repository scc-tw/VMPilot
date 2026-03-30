#ifndef __LOADER_MACHO_STRUCTS_HPP__
#define __LOADER_MACHO_STRUCTS_HPP__
#pragma once

/// Mach-O structure definitions for cross-platform use.
/// On macOS these mirror <mach-o/loader.h>; on Linux/Windows CI
/// they provide the same layout without system headers.

#include <cstdint>

namespace VMPilot::Loader::MachO {

// --- Magic numbers ---
constexpr uint32_t MH_MAGIC_64    = 0xFEEDFACF;
constexpr uint32_t MH_CIGAM_64    = 0xCFFAEDFE;  // byte-swapped

// --- Load command types ---
constexpr uint32_t LC_SEGMENT_64  = 0x19;
constexpr uint32_t LC_LOAD_DYLIB  = 0xC;

// --- VM protection flags ---
constexpr int32_t VM_PROT_READ    = 0x01;
constexpr int32_t VM_PROT_WRITE   = 0x02;
constexpr int32_t VM_PROT_EXECUTE = 0x04;

// --- Section types (flags & SECTION_TYPE mask) ---
constexpr uint32_t S_REGULAR      = 0x0;

// --- dylib_command (LC_LOAD_DYLIB) ---
struct dylib_command {
    uint32_t cmd;               // LC_LOAD_DYLIB = 0xC
    uint32_t cmdsize;           // sizeof(dylib_command) + string length, aligned to 8
    uint32_t name_offset;       // offset from cmd start to string
    uint32_t timestamp;         // 0
    uint32_t current_version;   // encoded version (e.g., 0x00010000 = 1.0.0)
    uint32_t compat_version;    // encoded version (e.g., 0x00010000 = 1.0.0)
    // Followed by null-terminated install name string
};
static_assert(sizeof(dylib_command) == 24, "dylib_command must be 24 bytes");

// --- Header (64-bit) ---
struct mach_header_64 {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
};
static_assert(sizeof(mach_header_64) == 32, "mach_header_64 must be 32 bytes");

// --- Generic load command header ---
struct load_command {
    uint32_t cmd;
    uint32_t cmdsize;
};

// --- Segment command (64-bit) ---
struct segment_command_64 {
    uint32_t cmd;           // LC_SEGMENT_64
    uint32_t cmdsize;       // sizeof(segment_command_64) + nsects * sizeof(section_64)
    char     segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    int32_t  maxprot;
    int32_t  initprot;
    uint32_t nsects;
    uint32_t flags;
};
static_assert(sizeof(segment_command_64) == 72, "segment_command_64 must be 72 bytes");

// --- Section (64-bit) ---
struct section_64 {
    char     sectname[16];
    char     segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;        // file offset
    uint32_t align;         // section alignment (power of 2)
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
};
static_assert(sizeof(section_64) == 80, "section_64 must be 80 bytes");

}  // namespace VMPilot::Loader::MachO

#endif  // __LOADER_MACHO_STRUCTS_HPP__
