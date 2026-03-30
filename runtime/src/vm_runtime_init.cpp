/// @file vm_runtime_init.cpp
/// @brief Load-time constructor that fills call_slot with &vm_stub_entry.
///
/// WHY THIS EXISTS:
///
///   Loader-generated entry stubs use an indirect call through a
///   function pointer ("call_slot") to invoke vm_stub_entry().  At
///   patch time the stub cannot know the runtime library's load
///   address, so the slot is zero-initialized in the payload.
///
///   At load time (when libvmpilot_runtime is loaded into the process),
///   this constructor locates the injected payload section and writes
///   &vm_stub_entry to the call_slot at offset 0 of the section.
///
/// WHY NOT PLT/GOT:
///
///   The injected segment has no relocation entries — the loader is a
///   post-link binary patcher, not a linker.  Creating PLT/GOT entries
///   from a patcher is fragile and format-specific.  A load-time
///   constructor is portable and requires no linker cooperation.
///   (D10§GAP7, [DESIGN])
///
/// PAYLOAD LAYOUT (offset 0 = call_slot):
///
///   [call_slot(8)] [blob(N)] [seed(32)] [stubs...]
///
///   The constructor writes to the first 8 bytes of the section.
///   This means it only needs to find the section — no knowledge of
///   blob_size or payload internals required.
///
/// PLATFORM DISPATCH:
///
///   macOS:   _dyld_register_func_for_add_image + iterate load commands
///   Linux:   dl_iterate_phdr + find section by name
///   Windows: GetModuleHandle + PE section header traversal
///
/// TODO (v2): mprotect call_slot page to RX after writing.

#include <vm_stub_entry.hpp>

#include <cstdint>
#include <cstring>

// The section name the loader uses (must match PlatformTraits).
// TODO: make this configurable or discover via a magic marker.
#if defined(__APPLE__)
static constexpr const char* VMPILOT_SEGMENT = "__VMPILOT";
static constexpr const char* VMPILOT_SECTION = ".vmpilot";
#elif defined(_WIN32)
static constexpr const char* VMPILOT_SECTION_NAME = ".vmpltt";
#else  // Linux / FreeBSD / ELF
static constexpr const char* VMPILOT_SECTION_NAME = ".vmpilot";
#endif

// Forward declaration of the function pointer we write into call_slot.
extern "C" int64_t vm_stub_entry(
    const VMPilot::Common::VM::VmStubArgs* args) noexcept;

// =========================================================================
// Platform: macOS (Mach-O)
// =========================================================================
#if defined(__APPLE__)

#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>

static void fill_call_slots_macho(const struct mach_header* mh,
                                  intptr_t /*slide*/) {
    // 64-bit only on modern macOS (Apple dropped 32-bit in Catalina)
    const auto* hdr = reinterpret_cast<const struct mach_header_64*>(mh);
    const uint8_t* base = reinterpret_cast<const uint8_t*>(hdr);
    size_t offset = sizeof(struct mach_header_64);

    for (uint32_t i = 0; i < hdr->ncmds; ++i) {
        const auto* lc = reinterpret_cast<const struct load_command*>(base + offset);
        if (lc->cmd == LC_SEGMENT_64) {
            const auto* seg = reinterpret_cast<const struct segment_command_64*>(lc);
            if (std::strncmp(seg->segname, VMPILOT_SEGMENT, 16) == 0) {
                // call_slot is at offset 0 of the segment's file data
                auto* slot = reinterpret_cast<void**>(
                    const_cast<uint8_t*>(base) + seg->fileoff);
                *slot = reinterpret_cast<void*>(&vm_stub_entry);
                return;
            }
        }
        offset += lc->cmdsize;
    }
}

static void vmpilot_runtime_init() {
    _dyld_register_func_for_add_image(fill_call_slots_macho);
}

// =========================================================================
// Platform: Linux / FreeBSD (ELF)
// =========================================================================
#elif defined(__linux__) || defined(__FreeBSD__)

#include <link.h>    // dl_iterate_phdr
#include <elf.h>

static int fill_call_slots_elf(struct dl_phdr_info* info,
                               size_t /*size*/, void* /*data*/) {
    // Only process the main executable (empty name = main)
    // and our own shared library
    const ElfW(Ehdr)* ehdr = reinterpret_cast<const ElfW(Ehdr)*>(info->dlpi_addr);
    (void)ehdr;

    // Iterate section headers is not directly available via dl_iterate_phdr.
    // Alternative: scan PT_LOAD segments and look for a known marker.
    // For v1, we use a simpler approach: open /proc/self/exe and parse
    // section headers to find the section by name.
    //
    // TODO: Implement proper ELF section discovery via /proc/self/exe
    // or by embedding a magic marker at a known offset.
    (void)info;
    return 0;  // continue iteration
}

static void vmpilot_runtime_init() {
    dl_iterate_phdr(fill_call_slots_elf, nullptr);
}

// =========================================================================
// Platform: Windows (PE)
// =========================================================================
#elif defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

static void vmpilot_runtime_init() {
    HMODULE exe = GetModuleHandle(nullptr);
    if (!exe) return;

    auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(exe);
    auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<const uint8_t*>(dos) + dos->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);

    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (std::strncmp(reinterpret_cast<const char*>(sec[i].Name),
                         VMPILOT_SECTION_NAME, 8) == 0) {
            auto* slot = reinterpret_cast<void**>(
                reinterpret_cast<uint8_t*>(exe) + sec[i].VirtualAddress);
            *slot = reinterpret_cast<void*>(&vm_stub_entry);
            return;
        }
    }
}

#else
// Unknown platform — no-op
static void vmpilot_runtime_init() {}
#endif

// =========================================================================
// Portable constructor registration
// =========================================================================

#if defined(__GNUC__) || defined(__clang__)
// GCC / Clang / MinGW: __attribute__((constructor))
__attribute__((constructor))
static void vmpilot_runtime_ctor() { vmpilot_runtime_init(); }

#elif defined(_MSC_VER)
// MSVC: CRT initializer via .CRT$XCU section.
// Functions placed in .CRT$XCU are called by the CRT startup code
// before main(), similar to __attribute__((constructor)).
#pragma section(".CRT$XCU", read)
static void vmpilot_runtime_crt_init() { vmpilot_runtime_init(); }
__declspec(allocate(".CRT$XCU"))
static void (*p_vmpilot_init)() = vmpilot_runtime_crt_init;
#endif
