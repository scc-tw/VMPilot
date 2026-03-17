/// data_refs: Test binary exercising all data reference categories.
///
/// Patterns covered inside VMPilot-protected regions:
///   - .rodata access          (const lookup table)
///   - .data  read-modify-write (initialized global)
///   - .bss   write             (zero-initialized array)
///   - std::atomic              (lock xadd / ldadd)
///   - thread_local             (fs: / mrs TPIDR_EL0 / gs: TEB)
///   - Cross-DSO GOT-indirect   (shared library data)
///   - Jump table               (switch → .rodata table)
///
/// Build with -O2 to get realistic codegen patterns.

#include <atomic>
#include <cstdio>

extern void VMPilot_Begin(char const*);
extern void VMPilot_End(char const*);

// ---- Cross-DSO data (from mock_VMPilot shared library) ----
// On x86-64 ELF  : accessed via GOT (mov rax,[rip+sym@GOTPCREL]; mov eax,[rax])
// On ARM64 MachO  : accessed via GOT page (adrp+ldr from __got)
// On Windows PE   : accessed via IAT (__declspec(dllimport))
#ifdef _MSC_VER
#define IMPORT __declspec(dllimport)
#else
#define IMPORT
#endif

extern IMPORT int g_shared_counter;
extern IMPORT int g_shared_config[4];

// ---- Module-local data ----
static int g_counter = 100;                                      // .data
static const int g_table[] = {11, 22, 33, 44, 55, 66, 77, 88}; // .rodata
static int g_bss_buf[8];                                          // .bss
static std::atomic<int> g_atomic_val{0};                          // .data/.bss
static thread_local int g_tls_acc = 0;                            // TLS

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#endif

/// Showcase: local, global, bss, atomic, TLS, cross-DSO accesses.
///
/// Expected references inside protected region:
///   g_table[sel&7]        → ReadOnlyData  (.rodata)
///   g_counter += val      → GlobalVar     (.data,  write)
///   g_bss_buf[sel&7]      → GlobalVar     (.bss,   write)
///   g_atomic_val.fetch_add→ GlobalVar     (.data,  atomic RMW)
///   g_tls_acc += val      → TlsVar        (fs:/TPIDR_EL0)
///   g_shared_counter      → GotLoad       (GOT-indirect, cross-DSO)
///   g_shared_config[i]    → GotLoad       (GOT-indirect, cross-DSO)
NOINLINE int data_ref_showcase(int sel) {
    VMPilot_Begin(__FUNCTION__);

    // .rodata read — const table lookup
    int val = g_table[sel & 7];

    // .data read-modify-write
    g_counter += val;

    // .bss write
    g_bss_buf[sel & 7] = val;

    // Atomic RMW on global
    //   x86-64 : lock xadd dword ptr [rip+g_atomic_val], reg
    //   ARM64  : ldadd w_val, w_old, [x_addr]
    int old = g_atomic_val.fetch_add(val, std::memory_order_relaxed);

    // TLS access
    //   Linux x86-64 : mov eax, fs:[offset]   (Local Exec)
    //   macOS ARM64  : mrs xN, TPIDR_EL0; ldr ...
    //   Windows x64  : mov rax, gs:[0x58]; ...  (TEB TlsSlots)
    g_tls_acc += val;

    // Cross-DSO / GOT-indirect
    //   x86-64 : mov rax, [rip+g_shared_counter@GOTPCREL]; mov eax, [rax]
    //   ARM64  : adrp+ldr from GOT page
    int shared = g_shared_counter + g_shared_config[sel & 3];

    int result = g_counter + old + g_tls_acc + shared;

    VMPilot_End(__FUNCTION__);
    return result;
}

/// Showcase: jump table from switch statement.
///
/// With -O2 and 8+ dense cases, GCC/Clang/MSVC emit a jump table
/// in .rodata with relative (x86-64/ARM64) or absolute (x86-32) offsets.
NOINLINE int jump_table_showcase(int op) {
    VMPilot_Begin(__FUNCTION__);

    int result;
    switch (op) {
        case 0: result = g_counter * 2;     break;
        case 1: result = g_counter + 10;    break;
        case 2: result = g_counter - 5;     break;
        case 3: result = g_counter ^ 0xFF;  break;
        case 4: result = g_counter << 2;    break;
        case 5: result = g_counter >> 1;    break;
        case 6: result = g_counter & 0xF0;  break;
        case 7: result = g_counter | 0x0F;  break;
        default: result = -1;               break;
    }

    VMPilot_End(__FUNCTION__);
    return result;
}

int main() {
    int r1 = data_ref_showcase(3);
    int r2 = jump_table_showcase(r1 % 8);
    printf("%d %d\n", r1, r2);
    return 0;
}
