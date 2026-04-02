/// @file vm_runtime_init_linux.cpp
/// @brief Load-time constructor — Linux / FreeBSD (ELF) implementation.
///
/// Uses dl_iterate_phdr to find the main executable's base address, then
/// parses /proc/self/exe on-disk to locate the .vmpilot section header
/// (section headers are NOT loaded into memory by the ELF loader).
///
/// WHY /proc/self/exe:
///   ELF program headers (loaded by the kernel) do NOT include section
///   names — they only have PT_LOAD segments.  To find a section by name
///   we must read the section header table, which is only available in the
///   on-disk binary.  /proc/self/exe is a symlink to the running binary.
///
/// Compiled only on Linux/FreeBSD (selected by CMakeLists.txt).

#include <vm_stub_entry.hpp>

#include <cstdint>
#include <cstring>

#include <link.h>    // dl_iterate_phdr, ElfW()
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

static constexpr const char* VMPILOT_SECTION_NAME = ".vmpilot";

extern "C" int64_t vm_stub_entry(
    const VMPilot::Common::VM::VmStubArgs* args) noexcept;

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

// ── Constructor ─────────────────────────────────────────────────────────────

__attribute__((constructor))
static void vmpilot_runtime_ctor() {
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
