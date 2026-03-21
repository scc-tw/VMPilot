# Test Data Binaries

Pre-compiled binaries for integration and smoke tests.

## Building Test Data

Each subdirectory has its own `CMakeLists.txt`. Build with `-O0` to ensure
the compiler does not reorder instructions across `VMPilot_Begin`/`VMPilot_End`
markers:

```bash
# Linux x86_64
cmake -B build_x64 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-O0"
ninja -C build_x64

# Linux x86 (32-bit cross-compile)
cmake -B build_x86 -G Ninja -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-m32 -O0" -DCMAKE_C_FLAGS="-m32"
ninja -C build_x86
```

## Known Limitation: Instruction Scheduling at -O2/-O3

When compiling user code with optimizations enabled, the compiler may
**reorder pure arithmetic** across `VMPilot_Begin`/`VMPilot_End` boundaries.
These marker functions are opaque calls that act as barriers for side effects,
but the compiler is free to move operations that have no data dependency on
the call's side effects.

Example (GCC -O2, `compute_beta`):

```asm
compute_beta:
  mov   ebx, edi
  imul  ebx, ebx              ;; x*x moved BEFORE Begin
  call  VMPilot_Begin
  call  VMPilot_End
  mov   eax, ebx              ;; just returns the pre-computed value
```

The protected region `[Begin, End]` contains no actual user computation.

### Recommendations for Users

To ensure the protected region contains all intended code:

1. **Use `__attribute__((noinline))` on protected functions** to prevent
   the compiler from inlining them into callers (which creates nested
   marker pairs).

2. **Use a compiler barrier** inside the protected region to prevent
   instruction scheduling across the markers:

   ```cpp
   void my_protected_function(int x) {
       VMPilot_Begin(__FUNCTION__);
       asm volatile("" ::: "memory");  // GCC/Clang: prevent reordering
       int result = x * x;            // stays inside [Begin, End]
       asm volatile("" ::: "memory");
       VMPilot_End(__FUNCTION__);
   }
   ```

   On MSVC, use `_ReadWriteBarrier()` or `MemoryBarrier()` for the same
   effect.

3. **Compile with `-O0` or `-Og`** during development to verify that
   all intended code falls within the protected region.
