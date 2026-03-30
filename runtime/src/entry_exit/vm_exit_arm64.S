// VM Exit Stub - AArch64 (v2)
//
// In v2, this assembly stub handles the transition from VM execution
// back to native code. It is the counterpart to vm_entry_arm64.S.
//
// The stub will:
//   1. Receive the VM execution result (VmExecResult) from vm_execute()
//   2. Extract the plaintext return value
//   3. Place the return value into X0 (integer return) or D0/V0 (float)
//   4. Restore the native stack pointer saved by the entry stub
//   5. Restore all native callee-saved registers (X19-X28, X29/FP)
//   6. Clean up any VM-allocated stack frames
//   7. Restore X30/LR and execute RET to return to the original call site
//
// For multi-value returns (structs), X0 and X1 are used per AAPCS64.
//
// v1: Exit is implicit -- vm_execute() returns the result directly.
//     This file is a placeholder and is NOT compiled into the build.
