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
/// 64 dense cases — forces all compilers to emit a jump table.
/// Each case writes to a volatile global to prevent merging.
static volatile int g_jt_sink;

#define JT_CASE(N, EXPR) case N: g_jt_sink = (EXPR); result = g_jt_sink; break

NOINLINE int jump_table_showcase(int op) {
    VMPilot_Begin(__FUNCTION__);

    int c = g_counter;
    int result;
    switch (op) {
        JT_CASE(0,  c * 2);       JT_CASE(1,  c + 10);
        JT_CASE(2,  c - 5);       JT_CASE(3,  c ^ 0xFF);
        JT_CASE(4,  c << 2);      JT_CASE(5,  c >> 1);
        JT_CASE(6,  c & 0xF0);    JT_CASE(7,  c | 0x0F);
        JT_CASE(8,  c * 3);       JT_CASE(9,  c - 20);
        JT_CASE(10, c ^ 0xAA);    JT_CASE(11, c << 3);
        JT_CASE(12, c * 5);       JT_CASE(13, c + 99);
        JT_CASE(14, c & 0x7F);    JT_CASE(15, c | 0xF0);
        JT_CASE(16, c * 7);       JT_CASE(17, c - 42);
        JT_CASE(18, c ^ 0x55);    JT_CASE(19, c << 1);
        JT_CASE(20, c >> 2);      JT_CASE(21, c & 0xCC);
        JT_CASE(22, c | 0x33);    JT_CASE(23, c * 11);
        JT_CASE(24, c + 200);     JT_CASE(25, c - 100);
        JT_CASE(26, c ^ 0xFF00);  JT_CASE(27, c << 4);
        JT_CASE(28, c >> 3);      JT_CASE(29, c & 0x0F);
        JT_CASE(30, c | 0xFF);    JT_CASE(31, c * 13);
        JT_CASE(32, c + 1);       JT_CASE(33, c - 1);
        JT_CASE(34, c ^ 1);       JT_CASE(35, c << 5);
        JT_CASE(36, c >> 4);      JT_CASE(37, c & 0xAA);
        JT_CASE(38, c | 0x55);    JT_CASE(39, c * 17);
        JT_CASE(40, c + 500);     JT_CASE(41, c - 250);
        JT_CASE(42, c ^ 0xDEAD);  JT_CASE(43, c << 6);
        JT_CASE(44, c >> 5);      JT_CASE(45, c & 0xF00F);
        JT_CASE(46, c | 0x0FF0);  JT_CASE(47, c * 19);
        JT_CASE(48, c + 1000);    JT_CASE(49, c - 500);
        JT_CASE(50, c ^ 0xBEEF);  JT_CASE(51, c << 7);
        JT_CASE(52, c >> 6);      JT_CASE(53, c & 0x8000);
        JT_CASE(54, c | 0x1234);  JT_CASE(55, c * 23);
        JT_CASE(56, c + 2000);    JT_CASE(57, c - 999);
        JT_CASE(58, c ^ 0xCAFE);  JT_CASE(59, c << 8);
        JT_CASE(60, c >> 7);      JT_CASE(61, c & 0xFFFF);
        JT_CASE(62, c | 0x5678);  JT_CASE(63, c * 29);
        default: result = -1; break;
    }

    VMPilot_End(__FUNCTION__);
    return result;
}

#undef JT_CASE

int main() {
    int r1 = data_ref_showcase(3);
    int r2 = jump_table_showcase(r1 % 64);
    printf("%d %d\n", r1, r2);
    return 0;
}
