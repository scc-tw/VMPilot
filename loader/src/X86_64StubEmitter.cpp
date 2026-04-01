#include <ArchTraits.hpp>
#include <StubArgsLayout.hpp>
#include <StubEmitter.hpp>

#include <vm/vm_stub_args.hpp>

#include <climits>
#include <cstring>

namespace VMPilot::Loader {

using Traits = ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_64>;
using Layout = StubArgsLayout<Common::FileArch::X86, Common::FileMode::MODE_64>;
using DC     = Common::DiagnosticCode;

// -----------------------------------------------------------------------
// x86-64 machine-code helpers
// -----------------------------------------------------------------------

/// PUSH r64 — either single-byte (for r0-r7) or two-byte REX.B (for r8-r15).
static void emit_push(std::vector<uint8_t>& c, uint8_t reg) {
    if (reg >= 8)
        c.push_back(0x41);              // REX.B
    c.push_back(0x50 + (reg & 7));
}

/// POP r64 — mirror of emit_push.
static void emit_pop(std::vector<uint8_t>& c, uint8_t reg) {
    if (reg >= 8)
        c.push_back(0x41);              // REX.B
    c.push_back(0x58 + (reg & 7));
}

/// MOV [rsp + disp32], r64  — REX.W 89 /r ModRM(10, reg, 100) SIB(00,100,100)
static void emit_mov_to_rsp_disp(std::vector<uint8_t>& c,
                                  int32_t disp, uint8_t reg) {
    uint8_t rex = 0x48;
    if (reg >= 8) rex |= 0x04;          // REX.R
    c.push_back(rex);
    c.push_back(0x89);
    c.push_back(static_cast<uint8_t>(0x84 | ((reg & 7) << 3)));   // ModRM(10, reg&7, 100)
    c.push_back(0x24);                       // SIB(00, 100=rsp, 100=rsp)
    // disp32 little-endian
    c.push_back(static_cast<uint8_t>(disp));
    c.push_back(static_cast<uint8_t>(disp >> 8));
    c.push_back(static_cast<uint8_t>(disp >> 16));
    c.push_back(static_cast<uint8_t>(disp >> 24));
}

/// LEA r64, [rip + disp32] — REX.W 8D /r ModRM(00, reg, 101)
/// Returns offset of disp32 within `c`.
static size_t emit_lea_rip(std::vector<uint8_t>& c, uint8_t reg) {
    uint8_t rex = 0x48;
    if (reg >= 8) rex |= 0x04;          // REX.R
    c.push_back(rex);
    c.push_back(0x8D);
    c.push_back(static_cast<uint8_t>(0x05 | ((reg & 7) << 3)));   // ModRM(00, reg&7, 101=rip)
    size_t off = c.size();
    c.insert(c.end(), 4, 0x00);              // placeholder disp32
    return off;
}

/// MOV r64, imm64 — REX.W B8+rd  (10-byte movabs)
/// Returns offset of imm64 within `c`.
static size_t emit_mov_imm64(std::vector<uint8_t>& c, uint8_t reg) {
    uint8_t rex = 0x48;
    if (reg >= 8) rex |= 0x01;          // REX.B
    c.push_back(rex);
    c.push_back(0xB8 + (reg & 7));
    size_t off = c.size();
    c.insert(c.end(), 8, 0x00);              // placeholder imm64
    return off;
}

/// SUB rax, r10  — REX.W 4C 29 D0  (actually REX.WR 29 /r)
static void emit_sub_rax_r10(std::vector<uint8_t>& c) {
    c.push_back(0x4C);                  // REX.WR
    c.push_back(0x29);
    c.push_back(0xD0);                  // ModRM(11, r10&7=2, rax=0)
}

/// SUB rsp, imm32
static void emit_sub_rsp_imm32(std::vector<uint8_t>& c, int32_t imm) {
    c.push_back(0x48);                  // REX.W
    c.push_back(0x81);
    c.push_back(0xEC);                  // ModRM(11, /5, rsp)
    c.push_back(static_cast<uint8_t>(imm));
    c.push_back(static_cast<uint8_t>(imm >> 8));
    c.push_back(static_cast<uint8_t>(imm >> 16));
    c.push_back(static_cast<uint8_t>(imm >> 24));
}

/// ADD rsp, imm32
static void emit_add_rsp_imm32(std::vector<uint8_t>& c, int32_t imm) {
    c.push_back(0x48);                  // REX.W
    c.push_back(0x81);
    c.push_back(0xC4);                  // ModRM(11, /0, rsp)
    c.push_back(static_cast<uint8_t>(imm));
    c.push_back(static_cast<uint8_t>(imm >> 8));
    c.push_back(static_cast<uint8_t>(imm >> 16));
    c.push_back(static_cast<uint8_t>(imm >> 24));
}

/// CALL [rax] — FF /2 (call indirect through memory at [rax]).
static void emit_call_rax_indirect(std::vector<uint8_t>& c) {
    c.push_back(0xFF);
    c.push_back(0x10);                  // ModRM(00, /2, rax)
}

// -----------------------------------------------------------------------
// X86_64StubEmitter
// -----------------------------------------------------------------------

class X86_64StubEmitter final : public StubEmitter {
public:
    [[nodiscard]] tl::expected<Stub, DC>
    emit_entry_stub() noexcept override;

    [[nodiscard]] tl::expected<std::vector<uint8_t>, DC>
    emit_region_patch(uint64_t region_size,
                      uint64_t from_addr, uint64_t to_addr) noexcept override;

    [[nodiscard]] tl::expected<void, DC>
    fixup_ptr(std::vector<uint8_t>& code, std::size_t offset,
              uint64_t fixup_va, uint64_t target_va) noexcept override;

    [[nodiscard]] tl::expected<void, DC>
    fixup_branch(std::vector<uint8_t>& code, std::size_t offset,
                 uint64_t fixup_va, uint64_t target_va) noexcept override;

    void fixup_immediate(std::vector<uint8_t>& code,
                         std::size_t offset, uint64_t value) noexcept override;

    void fixup_static_va(std::vector<uint8_t>& code,
                         std::size_t offset, std::size_t size,
                         uint64_t va) noexcept override;

    [[nodiscard]] std::size_t min_region_size()    const noexcept override;
    [[nodiscard]] int64_t     max_branch_distance() const noexcept override;
};

// -----------------------------------------------------------------------
// emit_entry_stub  (D10§GAP7)
// -----------------------------------------------------------------------

tl::expected<Stub, DC>
X86_64StubEmitter::emit_entry_stub() noexcept {
    Stub s;
    auto& c = s.code;
    c.reserve(256);

    // ---- 0. CET landing pad (Intel CET indirect-call target) ----
    // ENDBR64: required on CET-enforced systems for indirect call targets.
    // NOP on older CPUs (decoded as a no-op by pre-CET microarchitectures).
    c.push_back(0xF3); c.push_back(0x0F);
    c.push_back(0x1E); c.push_back(0xFA);

    // ---- 1. Save callee-saved regs (push rbx, rbp, r12-r15) ----
    for (auto reg : Traits::callee_saved)
        emit_push(c, reg);

    // ---- 2. Compute ASLR delta into rax (keep in rax) ----
    //   lea rax, [rip + 0]        ; rax = runtime VA of next insn
    //   mov r10, <imm64>          ; r10 = static VA (placeholder)
    //   sub rax, r10              ; rax = load_base_delta

    size_t lea_disp = emit_lea_rip(c, 0 /* rax */);
    s.delta_ref_offset = lea_disp + 4;

    s.delta_fixup_offset = emit_mov_imm64(c, 10 /* r10 */);
    s.delta_fixup_size   = 8;

    emit_sub_rax_r10(c);
    // Delta stays in rax — do NOT push.

    // ---- 3. Build initial_regs[16] array on stack (128 bytes) ----
    constexpr int32_t regs_frame = Traits::gpr_count * 8;  // 128
    emit_sub_rsp_imm32(c, regs_frame);

    // Store all 16 GPRs into [rsp+i*8].
    // Register numbering: 0=rax,1=rcx,2=rdx,3=rbx,4=rsp,5=rbp,
    //   6=rsi,7=rdi,8=r8,...,15=r15
    // rsp (reg 4) is not meaningful here; skip it.
    for (uint8_t i = 0; i < Traits::gpr_count; ++i) {
        if (i == 4) continue;           // skip rsp
        emit_mov_to_rsp_disp(c, static_cast<int32_t>(i * 8), i);
    }

    // Save pointer to initial_regs array (current rsp) into r12.
    // mov r12, rsp  — 49 89 E4
    c.push_back(0x49);
    c.push_back(0x89);
    c.push_back(0xE4);

    // ---- 4. Allocate VmStubArgs on the stack ----
    constexpr int32_t args_frame = static_cast<int32_t>(Layout::total_size);  // 64
    emit_sub_rsp_imm32(c, args_frame);

    // ---- 5. Fill VmStubArgs fields ----

    // mov dword [rsp + off_version], VM_STUB_ABI_VERSION (=1)
    // off_version == 0, so: C7 04 24 <imm32>  (7 bytes)
    {
        c.push_back(0xC7);
        c.push_back(0x04);
        c.push_back(0x24);
        uint32_t ver = Common::VM::VM_STUB_ABI_VERSION;
        c.push_back(static_cast<uint8_t>(ver));
        c.push_back(static_cast<uint8_t>(ver >> 8));
        c.push_back(static_cast<uint8_t>(ver >> 16));
        c.push_back(static_cast<uint8_t>(ver >> 24));
    }

    // mov byte [rsp + off_num_regs], 16
    // C6 44 24 <disp8> <imm8>  (5 bytes)
    {
        c.push_back(0xC6);
        c.push_back(0x44);
        c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_num_regs));
        c.push_back(16);
    }

    // mov [rsp + off_load_base_delta], rax  (8-byte, delta from step 2)
    emit_mov_to_rsp_disp(c, static_cast<int32_t>(Layout::off_load_base_delta),
                          0 /* rax */);

    // lea rcx, [rip + blob_offset]; mov [rsp + off_blob_data], rcx
    s.blob_fixup_offset = emit_lea_rip(c, 1 /* rcx */);
    emit_mov_to_rsp_disp(c, static_cast<int32_t>(Layout::off_blob_data),
                          1 /* rcx */);

    // lea rcx, [rip + seed_offset]; mov [rsp + off_stored_seed], rcx
    s.seed_fixup_offset = emit_lea_rip(c, 1 /* rcx */);
    emit_mov_to_rsp_disp(c, static_cast<int32_t>(Layout::off_stored_seed),
                          1 /* rcx */);

    // mov [rsp + off_initial_regs], r12  (pointer from step 3)
    emit_mov_to_rsp_disp(c, static_cast<int32_t>(Layout::off_initial_regs),
                          12 /* r12 */);

    // mov dword [rsp + off_blob_size], <imm32>  (fixup placeholder)
    // C7 44 24 <disp8> <imm32>  (8 bytes)
    {
        c.push_back(0xC7);
        c.push_back(0x44);
        c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_blob_size));
        s.size_fixup_offset = c.size();
        c.insert(c.end(), 4, 0x00);     // placeholder imm32
    }

    // ---- 6. Set arg0 = pointer to VmStubArgs ----
    // lea rdi, [rsp]  — 48 8D 3C 24  (4 bytes)
    c.push_back(0x48);
    c.push_back(0x8D);
    c.push_back(0x3C);
    c.push_back(0x24);

    // ---- 7. Indirect call: lea rax, [rip + slot]; call [rax] ----
    s.call_slot_fixup_offset = emit_lea_rip(c, 0 /* rax */);
    emit_call_rax_indirect(c);

    // ---- 7b. Zero the 32-byte seed in the payload section (doc 16 E1) ----
    //
    // WHY: Forward secrecy requires that stored_seed not persist after use.
    // The runtime copied it into VmImmutable::stored_seed; the payload copy
    // must be destroyed.  This prevents a post-execution memory dump from
    // recovering the root secret from the injected payload section.
    //
    // HOW: reload the seed pointer from VmStubArgs (still on stack at rsp+24),
    // then rep stosb 32 zero bytes through it.
    //
    //   mov rdi, [rsp + 24]   ; stored_seed pointer
    //   xor eax, eax          ; zero byte
    //   mov ecx, 32           ; count
    //   rep stosb             ; zero 32 bytes
    c.push_back(0x48); c.push_back(0x8B); c.push_back(0x7C);
    c.push_back(0x24); c.push_back(Layout::off_stored_seed);  // mov rdi, [rsp+24]
    c.push_back(0x31); c.push_back(0xC0);                     // xor eax, eax
    c.push_back(0xB9); c.push_back(32); c.push_back(0);
    c.push_back(0); c.push_back(0);                            // mov ecx, 32
    c.push_back(0xF3); c.push_back(0xAA);                     // rep stosb

    // ---- 8. Deallocate VmStubArgs ----
    emit_add_rsp_imm32(c, args_frame);

    // ---- 9. Deallocate initial_regs ----
    emit_add_rsp_imm32(c, regs_frame);

    // ---- 10. Restore callee-saved (reverse order) ----
    for (auto it = Traits::callee_saved.rbegin();
         it != Traits::callee_saved.rend(); ++it)
        emit_pop(c, *it);

    // ---- 11. JMP <resume_addr> (E9 rel32 placeholder) ----
    c.push_back(0xE9);
    s.resume_fixup_offset = c.size();
    c.insert(c.end(), 4, 0x00);         // placeholder rel32

    return s;
}

// -----------------------------------------------------------------------
// emit_region_patch  (D13§D2)
// -----------------------------------------------------------------------

tl::expected<std::vector<uint8_t>, DC>
X86_64StubEmitter::emit_region_patch(uint64_t region_size,
                                      uint64_t from_addr,
                                      uint64_t to_addr) noexcept {
    if (region_size < Traits::min_region_size)
        return tl::unexpected(DC::PatchRegionTooSmall);

    int64_t disp = static_cast<int64_t>(to_addr - (from_addr + 5));
    if (disp > INT32_MAX || disp < INT32_MIN)
        return tl::unexpected(DC::PatchRegionTooSmall);

    std::vector<uint8_t> patch(static_cast<size_t>(region_size), 0x90);  // NOP fill
    patch[0] = 0xE9;                                                      // JMP rel32
    auto d32 = static_cast<int32_t>(disp);
    std::memcpy(&patch[1], &d32, 4);
    return patch;
}

// -----------------------------------------------------------------------
// Fixup methods
// -----------------------------------------------------------------------

tl::expected<void, DC>
X86_64StubEmitter::fixup_ptr(std::vector<uint8_t>& code, std::size_t offset,
                              uint64_t fixup_va, uint64_t target_va) noexcept {
    if (offset + 4 > code.size())
        return tl::unexpected(DC::PatchStubGenerationFailed);

    auto disp = static_cast<int64_t>(target_va - (fixup_va + 4));
    auto d32  = static_cast<int32_t>(disp);
    std::memcpy(&code[offset], &d32, 4);
    return {};
}

tl::expected<void, DC>
X86_64StubEmitter::fixup_branch(std::vector<uint8_t>& code, std::size_t offset,
                                 uint64_t fixup_va, uint64_t target_va) noexcept {
    if (offset + 4 > code.size())
        return tl::unexpected(DC::PatchStubGenerationFailed);

    auto disp = static_cast<int64_t>(target_va - (fixup_va + 4));
    auto d32  = static_cast<int32_t>(disp);
    std::memcpy(&code[offset], &d32, 4);
    return {};
}

void X86_64StubEmitter::fixup_immediate(std::vector<uint8_t>& code,
                                         std::size_t offset,
                                         uint64_t value) noexcept {
    auto v32 = static_cast<uint32_t>(value);
    std::memcpy(&code[offset], &v32, 4);
}

void X86_64StubEmitter::fixup_static_va(std::vector<uint8_t>& code,
                                         std::size_t offset, std::size_t /*size*/,
                                         uint64_t va) noexcept {
    std::memcpy(&code[offset], &va, 8);
}

std::size_t X86_64StubEmitter::min_region_size()    const noexcept { return Traits::min_region_size; }
int64_t     X86_64StubEmitter::max_branch_distance() const noexcept { return Traits::max_branch_dist; }

// Factory — called from StubEmitter.cpp via create_emitter().
std::unique_ptr<StubEmitter> make_x86_64_emitter() {
    return std::make_unique<X86_64StubEmitter>();
}

}  // namespace VMPilot::Loader
