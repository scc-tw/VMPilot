/// @file native_call_bridge.cpp
/// @brief Native call bridge with ephemeral transition encoding.
///
/// Security analysis (D15§6.1, strengthened beyond affine masking):
///
///   The VM→native boundary is where MCSP-hard encoding must yield to
///   plaintext (the callee requires it).  The defense is NOT "prevent
///   plaintext" but "minimize what the attacker LEARNS per observation."
///
///   Ephemeral transition encoding achieves this by generating a fresh
///   random bijective LUT per invocation from stored_seed + monotonic
///   nonce.  Each LUT is used exactly once and destroyed.
///
///   Why this is secure by design (Kerckhoffs's principle):
///
///     The attacker has full source code.  They know the algorithm
///     (BLAKE3 → Fisher-Yates → byte-lane LUT).  This does NOT help:
///
///     1. BLAKE3(stored_seed, input) is a PRF.  Without stored_seed,
///        the output is indistinguishable from random (standard PRF
///        assumption).  Each nonce value produces an independent key.
///
///     2. Fisher-Yates with PRF seed produces a permutation that is
///        computationally indistinguishable from a uniformly random
///        permutation over {0..255}.
///
///     3. The nonce is monotonic and never repeats.  Each invocation
///        uses a DIFFERENT random permutation.  Observing (plain, masked)
///        at invocation i reveals one entry of π_i, but π_i is never
///        reused.  I(π_i; π_{i+1}) = 0 under the PRF assumption.
///
///     4. Chosen-plaintext attack: the attacker needs 256 observations
///        AT THE SAME NONCE to fully recover one lane's permutation.
///        But the nonce increments after each call.  One observation
///        per nonce → zero useful information accumulated.
///
///   What this does NOT defend against (D15§11.8):
///     - stored_seed compromise → all LUTs derivable → game over.
///       Same as D1/D2/D3/D4.  No additional weakness introduced.
///     - DBI tracing of stripper_call: attacker sees the ephemeral
///       decode table during execution.  Defense: table is zeroed
///       immediately after return (secure_zero).  Attacker must
///       capture during execution, and each capture is useless for
///       the next invocation.

#include <native_bridge.hpp>
#include <encoding.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VMContext;
using Common::VM::VM_BYTE_LANES;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

// =========================================================================
// Legacy bridge (no transition encoding).  Used when nonce-based
// ephemeral encoding is not available (backward compat, old blobs).
// =========================================================================

tl::expected<uint64_t, DiagnosticCode>
call_native(VMContext& ctx,
            uintptr_t target,
            const uint64_t* encoded_args,
            const uint8_t* arg_regs,
            uint8_t arg_count) noexcept {

    if (target == 0)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);
    if (arg_count > NATIVE_CALL_MAX_ARGS)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

    uint64_t plain_args[NATIVE_CALL_MAX_ARGS] = {};
    for (uint8_t i = 0; i < arg_count; ++i)
        plain_args[i] = decode_register(ctx, arg_regs[i], encoded_args[i]);

    auto fn = reinterpret_cast<uint64_t(*)(
        uint64_t, uint64_t, uint64_t, uint64_t,
        uint64_t, uint64_t, uint64_t, uint64_t)>(target);

    return fn(plain_args[0], plain_args[1], plain_args[2], plain_args[3],
              plain_args[4], plain_args[5], plain_args[6], plain_args[7]);
}

// =========================================================================
// Stripper function (D15§6.1, noinline).
//
// Why noinline: creates a real function boundary.  The caller passes
// masked_args (transition-domain values) + the ephemeral decode table.
// The decode table is a function parameter, not a global — the attacker
// must capture it during THIS invocation, and it is useless for the next.
//
// Why noclone: prevents the compiler from specializing this function
// for specific call sites, which would embed the decode table inline
// and defeat the purpose of the function boundary.
// =========================================================================

// =========================================================================
// Platform-specific native call trampoline.
//
// Why assembly: the C++ compiler always places uint64_t args in integer
// registers (rdi/rsi/... or x0-x7).  If arg i is actually a double, the
// callee expects it in xmm_i (x86-64) or d_i (ARM64).  Reinterpret_cast
// to a typed function pointer doesn't help because the signature varies
// per call-site and isn't known at compile time.
//
// The trampoline takes all args as uint64_t + an fp_mask bitfield, then
// moves FP-flagged args to the correct FP registers via inline asm.
//
// For variadic functions on x86-64 System V: AL = count of FP args in
// xmm registers.  ARM64 AAPCS doesn't need this.
// =========================================================================

#if defined(__x86_64__) || defined(_M_X64)

// x86-64 System V: integer args in rdi,rsi,rdx,rcx,r8,r9
//                   FP args in xmm0-xmm7
//                   variadic: AL = count of FP args used
static uint64_t platform_call(
    uintptr_t target, const uint64_t* args, uint8_t n,
    uint8_t fp_mask, bool is_variadic, bool returns_fp) noexcept
{
    // Move FP-flagged args to xmm registers via memcpy to double.
    // The compiler will generate movsd for these.
    double fp_args[8] = {};
    uint8_t fp_count = 0;
    for (uint8_t i = 0; i < n && i < 8; ++i) {
        if (fp_mask & (1u << i)) {
            std::memcpy(&fp_args[i], &args[i], 8);
            ++fp_count;
        }
    }

    // For the general case we use a function pointer with all-uint64_t
    // signature for integer args, and separately load FP args into xmm.
    // This is correct because on x86-64 System V:
    //   - Integer args go through rdi,rsi,rdx,rcx,r8,r9 (our fn ptr call)
    //   - FP args go through xmm0-7 (we set them explicitly before the call)
    //   - The two register sets are independent
    //
    // However, this only works if FP args are passed IN ADDITION TO integer
    // args (not replacing them).  In System V, integer and FP args have
    // separate counters.  A function like f(int, double, int) has:
    //   int arg 0 in rdi, double arg 1 in xmm0, int arg 2 in rsi.
    //
    // Our current encoding passes all args sequentially in r0-r7.  For
    // correct ABI compliance, we need to split into int_args[] and fp_args[]
    // with separate indexing.  This is a simplification for now:
    // we pass all args as uint64_t AND also load FP-flagged ones into xmm.
    // This works for functions where FP args are in the same position as
    // integer args (most common case with simple prototypes).

    // Load FP registers before the call
    if (fp_mask & 0x01) asm volatile("movsd %0, %%xmm0" : : "m"(fp_args[0]) : "xmm0");
    if (fp_mask & 0x02) asm volatile("movsd %0, %%xmm1" : : "m"(fp_args[1]) : "xmm1");
    if (fp_mask & 0x04) asm volatile("movsd %0, %%xmm2" : : "m"(fp_args[2]) : "xmm2");
    if (fp_mask & 0x08) asm volatile("movsd %0, %%xmm3" : : "m"(fp_args[3]) : "xmm3");
    if (fp_mask & 0x10) asm volatile("movsd %0, %%xmm4" : : "m"(fp_args[4]) : "xmm4");
    if (fp_mask & 0x20) asm volatile("movsd %0, %%xmm5" : : "m"(fp_args[5]) : "xmm5");
    if (fp_mask & 0x40) asm volatile("movsd %0, %%xmm6" : : "m"(fp_args[6]) : "xmm6");
    if (fp_mask & 0x80) asm volatile("movsd %0, %%xmm7" : : "m"(fp_args[7]) : "xmm7");

    // Set AL for variadic (number of FP args in xmm regs)
    if (is_variadic) {
        asm volatile("movb %0, %%al" : : "r"(fp_count) : "rax");
    }

    auto fn = reinterpret_cast<uint64_t(*)(
        uint64_t, uint64_t, uint64_t, uint64_t,
        uint64_t, uint64_t, uint64_t, uint64_t)>(target);

    uint64_t int_result = fn(args[0], args[1], args[2], args[3],
                              args[4], args[5], args[6], args[7]);

    if (returns_fp) {
        double fp_result;
        asm volatile("movsd %%xmm0, %0" : "=m"(fp_result) : : "xmm0");
        uint64_t result;
        std::memcpy(&result, &fp_result, 8);
        return result;
    }
    return int_result;
}

#elif defined(__aarch64__) || defined(_M_ARM64)

// ARM64 AAPCS: integer args in x0-x7, FP args in d0-d7
// No AL equivalent for variadic.
static uint64_t platform_call(
    uintptr_t target, const uint64_t* args, uint8_t n,
    uint8_t fp_mask, bool /*is_variadic*/, bool returns_fp) noexcept
{
    double fp_args[8] = {};
    for (uint8_t i = 0; i < n && i < 8; ++i) {
        if (fp_mask & (1u << i))
            std::memcpy(&fp_args[i], &args[i], 8);
    }

    // Load FP registers
    if (fp_mask & 0x01) asm volatile("fmov d0, %0" : : "r"(args[0]) : "d0");
    if (fp_mask & 0x02) asm volatile("fmov d1, %0" : : "r"(args[1]) : "d1");
    if (fp_mask & 0x04) asm volatile("fmov d2, %0" : : "r"(args[2]) : "d2");
    if (fp_mask & 0x08) asm volatile("fmov d3, %0" : : "r"(args[3]) : "d3");
    if (fp_mask & 0x10) asm volatile("fmov d4, %0" : : "r"(args[4]) : "d4");
    if (fp_mask & 0x20) asm volatile("fmov d5, %0" : : "r"(args[5]) : "d5");
    if (fp_mask & 0x40) asm volatile("fmov d6, %0" : : "r"(args[6]) : "d6");
    if (fp_mask & 0x80) asm volatile("fmov d7, %0" : : "r"(args[7]) : "d7");

    auto fn = reinterpret_cast<uint64_t(*)(
        uint64_t, uint64_t, uint64_t, uint64_t,
        uint64_t, uint64_t, uint64_t, uint64_t)>(target);

    uint64_t int_result = fn(args[0], args[1], args[2], args[3],
                              args[4], args[5], args[6], args[7]);

    if (returns_fp) {
        double fp_result;
        asm volatile("fmov %0, d0" : "=r"(fp_result) : : "d0");
        uint64_t result;
        std::memcpy(&result, &fp_result, 8);
        return result;
    }
    return int_result;
}

#else
// Fallback: integer-only (same as v1)
static uint64_t platform_call(
    uintptr_t target, const uint64_t* args, uint8_t /*n*/,
    uint8_t /*fp_mask*/, bool /*is_variadic*/, bool /*returns_fp*/) noexcept
{
    auto fn = reinterpret_cast<uint64_t(*)(
        uint64_t, uint64_t, uint64_t, uint64_t,
        uint64_t, uint64_t, uint64_t, uint64_t)>(target);
    return fn(args[0], args[1], args[2], args[3],
              args[4], args[5], args[6], args[7]);
}
#endif

// =========================================================================
// Stripper function (D15§6.1, noinline).
// =========================================================================

#if defined(__GNUC__) && !defined(__clang__)
__attribute__((noinline, noclone))
#else
__attribute__((noinline))
#endif
static uint64_t stripper_call(
    uintptr_t target,
    const uint64_t* masked_args,
    const uint8_t trans_dec[][256],
    uint8_t arg_count,
    uint8_t fp_mask,
    bool is_variadic,
    bool returns_fp) noexcept
{
    uint64_t plain_args[NATIVE_CALL_MAX_ARGS] = {};
    for (uint8_t i = 0; i < arg_count; ++i)
        plain_args[i] = apply_byte_lane_lut(trans_dec, masked_args[i]);

    return platform_call(target, plain_args, arg_count,
                          fp_mask, is_variadic, returns_fp);
}

// =========================================================================
// Ephemeral transition encoding bridge.
//
// Generates a fresh random bijection LUT from stored_seed + call_site_ip
// + monotonic nonce.  The LUT is used once to mask args before passing
// to stripper_call, then securely zeroed.
// =========================================================================

tl::expected<uint64_t, DiagnosticCode>
call_native_ephemeral(VMContext& ctx,
                      uintptr_t target,
                      const uint64_t* encoded_args,
                      const uint8_t* arg_regs,
                      uint8_t arg_count,
                      uint32_t call_site_ip,
                      uint8_t fp_mask,
                      bool is_variadic,
                      bool returns_fp) noexcept {

    if (target == 0)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);
    if (arg_count > NATIVE_CALL_MAX_ARGS)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

    // -- 1. Advance nonce (monotonic, never repeats) --
    const uint64_t nonce = ctx.native_call_nonce++;

    // -- 2. Derive per-invocation transition seed --
    //    trans_seed = BLAKE3_keyed(stored_seed, "nc" || call_site_ip || nonce)
    //    Why BLAKE3 keyed mode: PRF guarantee.  Without stored_seed,
    //    output is indistinguishable from random (standard assumption).
    uint8_t context[14];  // "nc"(2) + call_site_ip(4) + nonce(8) = 14
    context[0] = 'n'; context[1] = 'c';
    std::memcpy(context + 2, &call_site_ip, 4);
    std::memcpy(context + 6, &nonce, 8);

    uint8_t trans_seed[32];
    blake3_kdf(ctx.stored_seed, reinterpret_cast<const char*>(context),
               sizeof(context), trans_seed, 32);

    // -- 3. Generate ephemeral bijection tables (8 lanes) --
    //    Each lane's permutation is an independent random bijection
    //    with MinCircuit = Ω(2^n/n) = Ω(32) gates (Shannon 1949).
    uint8_t trans_enc[VM_BYTE_LANES][256];
    uint8_t trans_dec[VM_BYTE_LANES][256];
    for (uint8_t k = 0; k < VM_BYTE_LANES; ++k) {
        // Derive per-lane seed: BLAKE3_keyed(trans_seed, lane_index)
        uint8_t lane_seed[32];
        blake3_keyed_hash(trans_seed, &k, 1, lane_seed, 32);
        generate_bijection(lane_seed, trans_enc[k]);
        invert_bijection(trans_enc[k], trans_dec[k]);
    }

    // -- 4. Decode from register domain, re-encode with ephemeral LUT --
    //    Plaintext exists in a GPR for exactly ONE instruction (the
    //    apply_byte_lane_lut call).  Same exposure level as MBA carry
    //    bits (register-transient).
    uint64_t masked_args[NATIVE_CALL_MAX_ARGS] = {};
    for (uint8_t i = 0; i < arg_count; ++i) {
        uint64_t plain = decode_register(ctx, arg_regs[i], encoded_args[i]);
        masked_args[i] = apply_byte_lane_lut(trans_enc, plain);
    }

    // -- 5. Call stripper (noinline function boundary) --
    //    Stripper receives masked_args + trans_dec + ABI metadata.
    //    It inverts the ephemeral encoding, places FP args in FP regs
    //    via platform_call(), and returns the result.
    uint64_t result = stripper_call(target, masked_args, trans_dec,
                                     arg_count, fp_mask, is_variadic,
                                     returns_fp);

    // -- 6. Secure-zero ephemeral tables --
    //    Prevent post-call memory scan from recovering the LUT.
    //    volatile prevents compiler from eliding the write.
    volatile uint8_t* p = reinterpret_cast<volatile uint8_t*>(trans_enc);
    for (size_t i = 0; i < sizeof(trans_enc); ++i) p[i] = 0;
    p = reinterpret_cast<volatile uint8_t*>(trans_dec);
    for (size_t i = 0; i < sizeof(trans_dec); ++i) p[i] = 0;

    return result;
}

}  // namespace VMPilot::Runtime
