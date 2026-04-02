/// @file vm_runtime_init_darwin.cpp
/// @brief Load-time constructor — macOS (Mach-O) implementation.
///
/// Locates the __VMPILOT segment in the loaded Mach-O image and writes
/// &vm_stub_entry to the call_slot at offset 0.  Then transitions the
/// section from RW to RX for W^X compliance.
///
/// WHY _dyld_register_func_for_add_image:
///   On macOS, images are loaded lazily and may be added at any time
///   (e.g. via dlopen).  _dyld_register_func_for_add_image fires the
///   callback for every currently-loaded image AND every future image,
///   so we catch the main executable regardless of initialization order.
///
/// Compiled only on macOS (selected by CMakeLists.txt).

#include <vm_stub_entry.hpp>

#include <cstdint>
#include <cstring>

#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include <sys/mman.h>
#include <unistd.h>

static constexpr const char* VMPILOT_SEGMENT = "__VMPILOT";

extern "C" int64_t vm_stub_entry(
    const VMPilot::Common::VM::VmStubArgs* args) noexcept;

static void fill_call_slots_macho(const struct mach_header* mh,
                                  intptr_t slide) {
    // 64-bit only on modern macOS (Apple dropped 32-bit in Catalina)
    const auto* hdr = reinterpret_cast<const struct mach_header_64*>(mh);
    const uint8_t* base = reinterpret_cast<const uint8_t*>(hdr);
    size_t offset = sizeof(struct mach_header_64);

    for (uint32_t i = 0; i < hdr->ncmds; ++i) {
        const auto* lc = reinterpret_cast<const struct load_command*>(base + offset);
        if (lc->cmd == LC_SEGMENT_64) {
            const auto* seg = reinterpret_cast<const struct segment_command_64*>(lc);
            if (std::strncmp(seg->segname, VMPILOT_SEGMENT, 16) == 0) {
                // Runtime address = ASLR slide + segment's preferred VA.
                auto* section_start = reinterpret_cast<uint8_t*>(
                    static_cast<uintptr_t>(slide) + seg->vmaddr);
                const size_t section_size = seg->vmsize;

                // Section is mapped RW (initprot).  Write call_slot.
                auto* slot = reinterpret_cast<void**>(section_start);
                *slot = reinterpret_cast<void*>(&vm_stub_entry);

                // Transition RW → RX: remove write, add execute (W^X).
                const long page_size = sysconf(_SC_PAGESIZE);
                auto* page_start = reinterpret_cast<void*>(
                    reinterpret_cast<uintptr_t>(section_start)
                        & ~(static_cast<uintptr_t>(page_size) - 1));
                const size_t prot_len =
                    (section_size + (static_cast<size_t>(
                        section_start - static_cast<uint8_t*>(page_start)))
                     + static_cast<size_t>(page_size) - 1)
                    & ~(static_cast<size_t>(page_size) - 1);
                mprotect(page_start, prot_len, PROT_READ | PROT_EXEC);
                return;
            }
        }
        offset += lc->cmdsize;
    }
}

// ── Constructor registration ────────────────────────────────────────────────

__attribute__((constructor))
static void vmpilot_runtime_ctor() {
    _dyld_register_func_for_add_image(fill_call_slots_macho);
}
