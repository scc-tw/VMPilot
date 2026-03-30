// VM Entry Stub - AArch64 (v2)
//
// In v2, this assembly stub will be patched into the original binary
// by the loader (Phase D1-D3 in doc 13). When the program reaches a
// protected region, execution jumps here.
//
// The stub will:
//   1. Save ALL native callee-saved registers to the native stack
//      (X19-X28, X29/FP, X30/LR per AAPCS64)
//   2. Save the native stack pointer (SP)
//   3. Load the VMContext pointer (from a known location or register)
//   4. Set up initial register values from native arguments
//      (X0-X7 -> vm registers 0-7, AAPCS64 allows 8 register args)
//   5. Call vm_execute_with_args() via BL
//   6. Move the return value to X0 (native return register)
//   7. Restore all native callee-saved registers
//   8. Return to the caller (RET via X30/LR)
//
// v1: Entry is through the C++ vm_execute() API directly.
//     This file is a placeholder and is NOT compiled into the build.
