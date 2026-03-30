// VM Entry Stub - x86_64 (v2)
//
// In v2, this assembly stub will be patched into the original binary
// by the loader (Phase D1-D3 in doc 13). When the program reaches a
// protected region, execution jumps here.
//
// The stub will:
//   1. Save ALL native callee-saved registers to the native stack
//      (RBX, RBP, R12-R15 per System V AMD64 ABI)
//   2. Save the native stack pointer (RSP)
//   3. Load the VMContext pointer (from a known location or register)
//   4. Set up initial register values from native arguments
//      (RDI, RSI, RDX, RCX, R8, R9 -> vm registers 0-5)
//   5. Call vm_execute_with_args()
//   6. Move the return value to RAX (native return register)
//   7. Restore all native callee-saved registers
//   8. Return to the caller (RET)
//
// v1: Entry is through the C++ vm_execute() API directly.
//     This file is a placeholder and is NOT compiled into the build.
