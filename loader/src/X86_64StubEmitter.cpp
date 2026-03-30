#include <ArchTraits.hpp>
#include <StubEmitter.hpp>

#include <climits>
#include <cstring>

namespace VMPilot::Loader {

using Traits = ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_64>;
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
    c.push_back(0x84 | ((reg & 7) << 3));   // ModRM(10, reg&7, 100)
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
    c.push_back(0x05 | ((reg & 7) << 3));   // ModRM(00, reg&7, 101=rip)
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

/// MOV r32, imm32 — optional REX + B8+rd  (5 or 6 bytes)
/// Returns offset of imm32 within `c`.
static size_t emit_mov_imm32(std::vector<uint8_t>& c, uint8_t reg) {
    if (reg >= 8)
        c.push_back(0x41);              // REX.B
    c.push_back(0xB8 + (reg & 7));
    size_t off = c.size();
    c.insert(c.end(), 4, 0x00);              // placeholder imm32
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

/// XOR r32, r32 — zero a 32-bit register (implicit zero-extend to 64).
static void emit_xor_r32(std::vector<uint8_t>& c, uint8_t reg) {
    uint8_t rex = 0x00;
    if (reg >= 8) rex = 0x45;           // REX.RB
    if (rex) c.push_back(rex);
    c.push_back(0x31);
    c.push_back(0xC0 | ((reg & 7) << 3) | (reg & 7));
}

/// LEA rcx, [rsp] — load address of initial_regs array into rcx.
static void emit_lea_rcx_rsp(std::vector<uint8_t>& c) {
    c.push_back(0x48);                  // REX.W
    c.push_back(0x8D);
    c.push_back(0x0C);                  // ModRM(00, rcx=1, 100=SIB)
    c.push_back(0x24);                  // SIB(00, 100=none, 100=rsp)
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

    // ---- 1. Save callee-saved regs (push rbx, rbp, r12-r15) ----
    for (auto reg : Traits::callee_saved)
        emit_push(c, reg);

    // ---- 2. Compute ASLR delta ----
    //   lea rax, [rip + 0]        ; rax = runtime VA of next insn
    //   mov r10, <imm64>          ; r10 = static VA (placeholder)
    //   sub rax, r10              ; rax = delta
    //   push rax                  ; save delta on stack

    // The LEA [rip+0] loads the address of the instruction following
    // the LEA itself.  We record delta_ref_offset = end of LEA insn
    // (i.e. the address loaded is stub_base + delta_ref_offset).
    size_t lea_disp = emit_lea_rip(c, 0 /* rax */);
    // Write 0 displacement so rax = address right after this LEA.
    // (The disp32 is already 0x00000000 from the placeholder.)
    s.delta_ref_offset = lea_disp + 4;          // runtime VA = stub + this

    s.delta_fixup_offset = emit_mov_imm64(c, 10 /* r10 */);
    s.delta_fixup_size   = 8;

    emit_sub_rax_r10(c);
    emit_push(c, 0 /* rax */);

    // ---- 3. Build initial_regs[16] array on stack ----
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

    // ---- 4. Set up vm_execute_with_args arguments ----
    //   arg0 (rdi) : blob_ptr       — LEA rdi, [rip + blob_offset]
    //   arg1 (esi) : blob_size      — MOV esi, <imm32>
    //   arg2 (rdx) : seed_ptr       — LEA rdx, [rip + seed_offset]
    //   arg3 (rcx) : initial_regs   — LEA rcx, [rsp]
    //   arg4 (r8d) : num_regs = 16  — MOV r8d, 16
    //   arg5 (r9d) : config = 0     — XOR r9d, r9d

    s.blob_fixup_offset = emit_lea_rip(c, 7 /* rdi */);
    s.size_fixup_offset = emit_mov_imm32(c, 6 /* esi */);
    s.seed_fixup_offset = emit_lea_rip(c, 2 /* rdx */);

    emit_lea_rcx_rsp(c);

    // mov r8d, 16
    {
        size_t off = emit_mov_imm32(c, 8 /* r8 */);
        uint32_t val = 16;
        std::memcpy(&c[off], &val, 4);
    }

    emit_xor_r32(c, 9 /* r9 */);

    // ---- 5. Indirect call: lea rax, [rip + slot]; call [rax] ----
    s.call_slot_fixup_offset = emit_lea_rip(c, 0 /* rax */);
    emit_call_rax_indirect(c);

    // ---- 6. Return mapping ----
    // VmExecResult.return_value is already in rax — nothing to do.

    // ---- 7. Pop initial_regs frame ----
    emit_add_rsp_imm32(c, regs_frame);

    // ---- 8. Pop delta (discard) ----
    emit_add_rsp_imm32(c, 8);

    // ---- 9. Restore callee-saved (reverse order) ----
    for (auto it = Traits::callee_saved.rbegin();
         it != Traits::callee_saved.rend(); ++it)
        emit_pop(c, *it);

    // ---- 10. JMP <resume_addr> (E9 rel32 placeholder) ----
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
