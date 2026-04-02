/// @file vm_runtime_init_windows.cpp
/// @brief Load-time constructor — Windows (PE) implementation.
///
/// Walks the PE section headers of the main module to find the .vmpltt
/// section, writes &vm_stub_entry to the call_slot at offset 0, then
/// transitions the section from PAGE_READWRITE to PAGE_EXECUTE_READ
/// for W^X compliance.
///
/// WHY .vmpltt (8-char limit):
///   PE section names are limited to 8 bytes.  The full ".vmpilot" name
///   (9 chars including null) would be truncated.  ".vmpltt" fits within
///   the 8-byte IMAGE_SECTION_HEADER.Name field.
///
/// Compiled only on Windows (selected by CMakeLists.txt).

#include <vm_stub_entry.hpp>

#include <cstdint>
#include <cstring>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

static constexpr const char* VMPILOT_SECTION_NAME = ".vmpltt";

extern "C" int64_t vm_stub_entry(
    const VMPilot::Common::VM::VmStubArgs* args) noexcept;

static void vmpilot_runtime_init() {
    HMODULE exe = GetModuleHandle(nullptr);
    if (!exe) return;

    auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(exe);
    auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<const uint8_t*>(dos) + dos->e_lfanew);
    auto* sections = IMAGE_FIRST_SECTION(nt);

    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (std::strncmp(reinterpret_cast<const char*>(sections[i].Name),
                         VMPILOT_SECTION_NAME, 8) == 0) {
            auto* section_start = reinterpret_cast<uint8_t*>(exe)
                                  + sections[i].VirtualAddress;
            const SIZE_T section_size = sections[i].Misc.VirtualSize;

            // Section is mapped RW.  Write call_slot.
            auto* slot = reinterpret_cast<void**>(section_start);
            *slot = reinterpret_cast<void*>(&vm_stub_entry);

            // Transition RW → RX (W^X).
            DWORD old_prot;
            VirtualProtect(section_start, section_size,
                           PAGE_EXECUTE_READ, &old_prot);
            return;
        }
    }
}

// ── Constructor registration ────────────────────────────────────────────────
//
// MSVC: CRT initializer via .CRT$XCU section.
// Functions placed in .CRT$XCU are called by the CRT startup code
// before main(), similar to __attribute__((constructor)).
//
// GCC/Clang (MinGW): use __attribute__((constructor)).

#if defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
static void vmpilot_runtime_crt_init() { vmpilot_runtime_init(); }
__declspec(allocate(".CRT$XCU"))
static void (*p_vmpilot_init)() = vmpilot_runtime_crt_init;
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
static void vmpilot_runtime_ctor() { vmpilot_runtime_init(); }
#endif
