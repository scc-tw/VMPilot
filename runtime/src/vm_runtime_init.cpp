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
/// W^X COMPLIANCE:
///
///   The injected section is mapped RW (writable, NOT executable)
///   at load time.  After writing call_slot, the constructor
///   mprotects the entire section to RX (executable, NOT writable).
///   At no point is a page simultaneously W+X.

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
#include <sys/mman.h>
#include <unistd.h>

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
                // Round section_start down, section_size up to page boundaries.
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

static void vmpilot_runtime_init() {
    _dyld_register_func_for_add_image(fill_call_slots_macho);
}

// =========================================================================
// Platform: Linux / FreeBSD (ELF)
// =========================================================================
#elif defined(__linux__) || defined(__FreeBSD__)

#include <link.h>    // dl_iterate_phdr, ElfW()
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/// dl_iterate_phdr callback: find the main executable's base address.
/// The main executable has an empty dlpi_name.
static int find_main_exe_base(struct dl_phdr_info* info,
                              size_t /*size*/, void* data) {
    if (info->dlpi_name == nullptr || info->dlpi_name[0] == '\0') {
        *static_cast<uintptr_t*>(data) = static_cast<uintptr_t>(info->dlpi_addr);
        return 1;  // stop iteration
    }
    return 0;
}

/// Result of scanning /proc/self/exe for the .vmpilot section.
struct SectionInfo {
    uint64_t va;    ///< Section virtual address (from ELF section header)
    uint64_t size;  ///< Section size in bytes
};

/// Parse /proc/self/exe to find the .vmpilot section.
/// Returns {0,0} on failure.
///
/// Section headers are NOT loaded into memory by the ELF loader, so we
/// must read the on-disk binary to discover the section by name.
static SectionInfo find_vmpilot_section() {
    int fd = open("/proc/self/exe", O_RDONLY | O_CLOEXEC);
    if (fd < 0) return {};

    struct stat st;
    if (fstat(fd, &st) < 0 ||
        st.st_size < static_cast<off_t>(sizeof(ElfW(Ehdr)))) {
        close(fd);
        return {};
    }

    const auto file_size = static_cast<size_t>(st.st_size);
    void* map = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map == MAP_FAILED) return {};

    const auto* file = static_cast<const uint8_t*>(map);
    const auto* ehdr = reinterpret_cast<const ElfW(Ehdr)*>(file);

    // Validate ELF magic
    if (std::memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
        munmap(map, file_size);
        return {};
    }

    // Need section headers and the section-name string table
    if (ehdr->e_shoff == 0 || ehdr->e_shstrndx == SHN_UNDEF ||
        ehdr->e_shoff + ehdr->e_shnum * sizeof(ElfW(Shdr)) > file_size) {
        munmap(map, file_size);
        return {};
    }

    const auto* shdr = reinterpret_cast<const ElfW(Shdr)*>(
        file + ehdr->e_shoff);

    // Bounds-check the string table section
    if (ehdr->e_shstrndx >= ehdr->e_shnum ||
        shdr[ehdr->e_shstrndx].sh_offset +
            shdr[ehdr->e_shstrndx].sh_size > file_size) {
        munmap(map, file_size);
        return {};
    }

    const char* shstrtab = reinterpret_cast<const char*>(
        file + shdr[ehdr->e_shstrndx].sh_offset);
    const size_t shstrtab_size = shdr[ehdr->e_shstrndx].sh_size;

    SectionInfo result{};
    for (uint16_t i = 0; i < ehdr->e_shnum; ++i) {
        if (shdr[i].sh_name < shstrtab_size &&
            std::strcmp(shstrtab + shdr[i].sh_name,
                        VMPILOT_SECTION_NAME) == 0) {
            result.va   = shdr[i].sh_addr;
            result.size = shdr[i].sh_size;
            break;
        }
    }

    munmap(map, file_size);
    return result;
}

static void vmpilot_runtime_init() {
    // Step 1: Find the main executable's base address (0 for non-PIE,
    //         ASLR base for PIE).
    uintptr_t base = 0;
    dl_iterate_phdr(find_main_exe_base, &base);

    // Step 2: Find .vmpilot section VA + size from on-disk ELF headers.
    const auto sec = find_vmpilot_section();
    if (sec.va == 0) return;

    // Step 3: Compute runtime address = base + section VA.
    //         Works for both PIE (base != 0) and non-PIE (base == 0).
    auto* section_start = reinterpret_cast<uint8_t*>(base + sec.va);

    // Step 4: The segment is mapped RW (PF_R|PF_W).  Write call_slot.
    auto* slot = reinterpret_cast<void**>(section_start);
    *slot = reinterpret_cast<void*>(&vm_stub_entry);

    // Step 5: Transition RW → RX for the entire section (W^X).
    //         Round start down and length up to page boundaries.
    const long page_size = sysconf(_SC_PAGESIZE);
    const auto upage = static_cast<uintptr_t>(page_size);
    auto* page_start = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(section_start) & ~(upage - 1));
    const size_t head = static_cast<size_t>(
        section_start - static_cast<uint8_t*>(page_start));
    const size_t prot_len = (sec.size + head + upage - 1) & ~(upage - 1);

    mprotect(page_start, prot_len, PROT_READ | PROT_EXEC);
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
