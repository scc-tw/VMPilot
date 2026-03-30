#include <StubGenerator.hpp>

#include <cstring>

namespace VMPilot::Loader {

// ============================================================================
// Encoding helpers
// ============================================================================

static void emit_u32_le(std::vector<uint8_t>& v, uint32_t val) {
    v.push_back(static_cast<uint8_t>(val));
    v.push_back(static_cast<uint8_t>(val >> 8));
    v.push_back(static_cast<uint8_t>(val >> 16));
    v.push_back(static_cast<uint8_t>(val >> 24));
}

static void emit_placeholder32(std::vector<uint8_t>& v) {
    v.insert(v.end(), {0, 0, 0, 0});
}

static void emit_u64_le(std::vector<uint8_t>& v, uint64_t val) {
    for (int i = 0; i < 8; ++i)
        v.push_back(static_cast<uint8_t>(val >> (i * 8)));
}

static uint32_t arm64_b(int64_t byte_off) {
    return 0x14000000u | (static_cast<uint32_t>(byte_off >> 2) & 0x03FFFFFF);
}
static uint32_t arm64_bl(int64_t byte_off) {
    return 0x94000000u | (static_cast<uint32_t>(byte_off >> 2) & 0x03FFFFFF);
}
static uint32_t arm64_adr(uint8_t rd, int64_t byte_off) {
    const auto u = static_cast<uint32_t>(static_cast<int32_t>(byte_off));
    return 0x10000000 | (((u >> 2) & 0x7FFFF) << 5) | ((u & 0x3) << 29) | rd;
}
static uint32_t arm64_movz_x(uint8_t rd, uint16_t imm16) {
    return 0xD2800000 | (static_cast<uint32_t>(imm16) << 5) | rd;
}
static uint32_t arm64_movz_w(uint8_t rd, uint16_t imm16) {
    return 0x52800000 | (static_cast<uint32_t>(imm16) << 5) | rd;
}
static uint32_t arm64_movk_x(uint8_t rd, uint16_t imm16, uint8_t hw) {
    // MOVK Xd, #imm16, LSL #(hw*16)
    return 0xF2800000 | (static_cast<uint32_t>(hw) << 21) |
           (static_cast<uint32_t>(imm16) << 5) | rd;
}

static uint32_t arm64_sub_reg(uint8_t rd, uint8_t rn, uint8_t rm) {
    // SUB Xd, Xn, Xm (64-bit register subtract, no shift)
    return 0xCB000000 | (static_cast<uint32_t>(rm) << 16) |
           (static_cast<uint32_t>(rn) << 5) | rd;
}
static uint32_t arm64_str_x_sp_uoff(uint8_t rt, uint16_t byte_offset) {
    // STR Xt, [SP, #uoff] — unsigned offset, scaled by 8
    uint32_t imm12 = static_cast<uint32_t>(byte_offset / 8) & 0xFFF;
    return 0xF9000000 | (imm12 << 10) | (0x1F << 5) | rt;
}
constexpr uint32_t ARM64_NOP = 0xD503201F;
constexpr uint32_t ARM64_RET = 0xD65F03C0;

// ============================================================================
// Shared fixup helper
// ============================================================================

static tl::expected<void, Common::DiagnosticCode>
fixup_disp(std::vector<uint8_t>& code, size_t offset,
           int64_t displacement, Common::FileArch arch) noexcept {
    using DC = Common::DiagnosticCode;
    if (arch == Common::FileArch::ARM64) {
        constexpr int64_t ADR_RANGE = 1 << 20;
        if (displacement < -ADR_RANGE || displacement >= ADR_RANGE)
            return tl::unexpected(DC::PatchStubGenerationFailed);
        const uint8_t rd = code[offset] & 0x1F;
        const uint32_t insn = arm64_adr(rd, displacement);
        code[offset+0] = static_cast<uint8_t>(insn);
        code[offset+1] = static_cast<uint8_t>(insn >> 8);
        code[offset+2] = static_cast<uint8_t>(insn >> 16);
        code[offset+3] = static_cast<uint8_t>(insn >> 24);
    } else {
        if (displacement < INT32_MIN || displacement > INT32_MAX)
            return tl::unexpected(DC::PatchStubGenerationFailed);
        const auto d32 = static_cast<int32_t>(displacement);
        std::memcpy(code.data() + offset, &d32, 4);
    }
    return {};
}

static tl::expected<void, Common::DiagnosticCode>
fixup_branch(std::vector<uint8_t>& code, size_t offset,
             int64_t displacement, Common::FileArch arch) noexcept {
    using DC = Common::DiagnosticCode;
    if (arch == Common::FileArch::ARM64) {
        constexpr int64_t B_RANGE = 128LL * 1024 * 1024;
        if (displacement < -B_RANGE || displacement >= B_RANGE)
            return tl::unexpected(DC::PatchStubGenerationFailed);
        const uint32_t insn = arm64_b(displacement);
        code[offset+0] = static_cast<uint8_t>(insn);
        code[offset+1] = static_cast<uint8_t>(insn >> 8);
        code[offset+2] = static_cast<uint8_t>(insn >> 16);
        code[offset+3] = static_cast<uint8_t>(insn >> 24);
    } else {
        if (displacement < INT32_MIN || displacement > INT32_MAX)
            return tl::unexpected(DC::PatchStubGenerationFailed);
        const auto d32 = static_cast<int32_t>(displacement);
        std::memcpy(code.data() + offset, &d32, 4);
    }
    return {};
}

// ============================================================================
// x86-64 entry stub
//
// Mid-function handover.  Calls:
//   vm_execute_with_args(rdi=blob, rsi=size, rdx=seed,
//                        rcx=saved_regs, r8=16, r9=nullptr)
// ============================================================================

static tl::expected<Stub, Common::DiagnosticCode>
make_x86_64_entry_stub(uint32_t /*region_idx*/) {
    Stub s;
    auto& c = s.code;

    // --- save ALL 16 GPRs (rax-r15) + rflags ---
    // sub rsp, 128+8  (16 regs × 8 bytes + 8 for rflags, aligned to 16)
    c.push_back(0x48); c.push_back(0x81); c.push_back(0xEC);
    emit_u32_le(c, 136);  // sub rsp, 136

    // pushfq alternative: save rflags
    // Actually, do individual movs to the stack frame.
    // Layout: [rsp+0]=rax, [rsp+8]=rcx, [rsp+16]=rdx, ...
    // mov [rsp+0], rax
    auto mov_to_stack = [&](uint8_t reg_idx, uint32_t offset) {
        // REX.W MOV [rsp+disp8/32], reg
        uint8_t rex = 0x48;
        uint8_t modrm_reg = reg_idx & 0x7;
        if (reg_idx >= 8) rex |= 0x04;  // REX.R
        c.push_back(rex);
        c.push_back(0x89);  // MOV r/m64, r64
        if (offset < 128) {
            c.push_back(0x44 | (modrm_reg << 3));  // [rsp+disp8], mod=01
            c.push_back(0x24);  // SIB: base=rsp
            c.push_back(static_cast<uint8_t>(offset));
        } else {
            c.push_back(0x84 | (modrm_reg << 3));  // [rsp+disp32], mod=10
            c.push_back(0x24);
            emit_u32_le(c, offset);
        }
    };
    // rax=0, rcx=1, rdx=2, rbx=3, rsp=4(skip), rbp=5, rsi=6, rdi=7,
    // r8=8..r15=15
    // Save order: rax,rcx,rdx,rbx,rbp,rsi,rdi,r8-r15 (skip rsp — we need it)
    static constexpr uint8_t save_regs[] = {0,1,2,3,5,6,7,8,9,10,11,12,13,14,15};
    for (size_t i = 0; i < sizeof(save_regs); ++i)
        mov_to_stack(save_regs[i], static_cast<uint32_t>(i * 8));

    // --- compute load_base_delta ---
    // lea rax, [rip + 0]  ; rax = actual VA of the *next* instruction
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x05);
    emit_placeholder32(c);  // rip-relative disp = 0
    // delta_ref_offset = byte after LEA = what rax receives
    s.delta_ref_offset = c.size();  // = delta_lea_offset + 7

    // movabs r10, imm64   ; r10 = static VA (patched by fixup_delta_static_va)
    c.push_back(0x49); c.push_back(0xBA);
    s.delta_static_va_fixup_offset = c.size();
    emit_u64_le(c, 0);  // placeholder: 8 bytes of static VA
    s.delta_fixup_size = 8;

    // sub rax, r10        ; rax = actual - static = load_base_delta
    c.push_back(0x4C); c.push_back(0x29); c.push_back(0xD0); // sub rax, r10

    // mov [rsp + 120], rax  ; store delta at known offset (after 15 saved GPRs)
    c.push_back(0x48); c.push_back(0x89); c.push_back(0x44);
    c.push_back(0x24); c.push_back(0x78);  // [rsp+0x78] = [rsp+120]

    // --- arg0: rdi = blob_ptr ---
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x3D);
    s.blob_fixup_offset = c.size();
    s.blob_insn_size = 4;
    emit_placeholder32(c);

    // --- arg1: rsi = blob_size ---
    c.push_back(0xBE);
    s.size_fixup_offset = c.size();
    emit_placeholder32(c);

    // --- arg2: rdx = seed_ptr ---
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x15);
    s.seed_fixup_offset = c.size();
    s.seed_insn_size = 4;
    emit_placeholder32(c);

    // --- arg3: rcx = pointer to saved regs on stack ---
    // lea rcx, [rsp]
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x0C);
    c.push_back(0x24);  // SIB: [rsp]

    // --- arg4: r8 = 16 (num_regs) ---
    c.push_back(0x41); c.push_back(0xB8);
    emit_u32_le(c, 16);

    // --- arg5: r9 = nullptr (config) ---
    c.push_back(0x4D); c.push_back(0x31); c.push_back(0xC9); // xor r9, r9

    // --- call vm_execute_with_args ---
    c.push_back(0xE8);
    s.call_fixup_offset = c.size();
    emit_placeholder32(c);

    // --- write VM return value (rax) back to saved rax slot ---
    // mov [rsp+0], rax
    c.push_back(0x48); c.push_back(0x89); c.push_back(0x04); c.push_back(0x24);

    // --- restore ALL GPRs ---
    auto mov_from_stack = [&](uint8_t reg_idx, uint32_t offset) {
        uint8_t rex = 0x48;
        uint8_t modrm_reg = reg_idx & 0x7;
        if (reg_idx >= 8) rex |= 0x04;
        c.push_back(rex);
        c.push_back(0x8B);  // MOV r64, r/m64
        if (offset < 128) {
            c.push_back(0x44 | (modrm_reg << 3));
            c.push_back(0x24);
            c.push_back(static_cast<uint8_t>(offset));
        } else {
            c.push_back(0x84 | (modrm_reg << 3));
            c.push_back(0x24);
            emit_u32_le(c, offset);
        }
    };
    for (size_t i = 0; i < sizeof(save_regs); ++i)
        mov_from_stack(save_regs[i], static_cast<uint32_t>(i * 8));

    // add rsp, 136
    c.push_back(0x48); c.push_back(0x81); c.push_back(0xC4);
    emit_u32_le(c, 136);

    // --- resume: jmp rel32 to region.addr + region.size ---
    c.push_back(0xE9);
    s.resume_fixup_offset = c.size();
    s.resume_insn_size = 4;  // RIP = addr after JMP = addr + 5; disp is rel to that
    emit_placeholder32(c);

    return s;
}

// ============================================================================
// ARM64 entry stub
//
// Mid-function handover.  Calls:
//   vm_execute_with_args(x0=blob, x1=size, x2=seed,
//                        x3=saved_regs, w4=16, x5=nullptr)
// ============================================================================

static tl::expected<Stub, Common::DiagnosticCode>
make_arm64_entry_stub(uint32_t /*region_idx*/) {
    Stub s;
    auto& c = s.code;

    // --- save ALL GPRs (x0-x30) + LR ---
    // sub sp, sp, #256     (32 regs × 8 bytes)
    emit_u32_le(c, 0xD10403FF);  // sub sp, sp, #256

    // stp pairs: x0-x29 (15 pairs)
    for (int i = 0; i < 30; i += 2) {
        // stp xi, xi+1, [sp, #i*8]
        uint32_t rt  = static_cast<uint32_t>(i);
        uint32_t rt2 = static_cast<uint32_t>(i + 1);
        int32_t  imm = i * 8;
        uint32_t imm7 = (static_cast<uint32_t>(imm / 8) & 0x7F) << 15;
        emit_u32_le(c, 0xA9000000 | imm7 | (rt2 << 10) | (0x1F << 5) | rt);
        // encoding: STP Xt1, Xt2, [SP, #imm7*8] — unsigned offset form
    }
    // str x30, [sp, #240]
    emit_u32_le(c, 0xF9007BFE);  // str x30, [sp, #240] (offset=240/8=30)

    // --- compute load_base_delta ---
    // adr x9, .  (x9 = actual VA of this ADR instruction)
    s.delta_ref_offset = c.size();  // ADR's own address = the reference point
    emit_u32_le(c, arm64_adr(9, 0));  // adr x9, #0

    // movz x10, #static_va[15:0]        (patched by fixup_delta_static_va)
    s.delta_static_va_fixup_offset = c.size();
    emit_u32_le(c, arm64_movz_x(10, 0));
    // movk x10, #static_va[31:16], lsl #16
    emit_u32_le(c, arm64_movk_x(10, 0, 1));
    // movk x10, #static_va[47:32], lsl #32
    emit_u32_le(c, arm64_movk_x(10, 0, 2));
    // movk x10, #static_va[63:48], lsl #48
    emit_u32_le(c, arm64_movk_x(10, 0, 3));
    s.delta_fixup_size = 16;  // 4 instructions × 4 bytes

    // sub x9, x9, x10    (x9 = load_base_delta)
    emit_u32_le(c, arm64_sub_reg(9, 9, 10));

    // str x9, [sp, #248]  (store delta at known offset for runtime)
    emit_u32_le(c, arm64_str_x_sp_uoff(9, 248));

    // --- arg0: x0 = blob_ptr (ADR x0, #disp) ---
    s.blob_fixup_offset = c.size();
    s.blob_insn_size = 0;  // ARM64 ADR is PC-relative (not PC+4)
    emit_u32_le(c, arm64_adr(0, 0));

    // --- arg1: x1 = blob_size ---
    s.size_fixup_offset = c.size();
    emit_u32_le(c, arm64_movz_x(1, 0));

    // --- arg2: x2 = seed_ptr (ADR x2, #disp) ---
    s.seed_fixup_offset = c.size();
    s.seed_insn_size = 0;
    emit_u32_le(c, arm64_adr(2, 0));

    // --- arg3: x3 = saved_regs (SP) ---
    emit_u32_le(c, 0x910003E3);  // mov x3, sp

    // --- arg4: w4 = 16 ---
    emit_u32_le(c, arm64_movz_w(4, 16));

    // --- arg5: x5 = nullptr ---
    emit_u32_le(c, arm64_movz_x(5, 0));

    // --- call vm_execute_with_args ---
    s.call_fixup_offset = c.size();
    emit_u32_le(c, arm64_bl(0));

    // --- write VM return value (x0) back to saved x0 slot ---
    // str x0, [sp, #0]
    emit_u32_le(c, 0xF90003E0);

    // --- restore ALL GPRs ---
    for (int i = 0; i < 30; i += 2) {
        uint32_t rt  = static_cast<uint32_t>(i);
        uint32_t rt2 = static_cast<uint32_t>(i + 1);
        int32_t  imm = i * 8;
        uint32_t imm7 = (static_cast<uint32_t>(imm / 8) & 0x7F) << 15;
        emit_u32_le(c, 0xA9400000 | imm7 | (rt2 << 10) | (0x1F << 5) | rt);
        // LDP Xt1, Xt2, [SP, #imm7*8]
    }
    emit_u32_le(c, 0xF9407BFE);  // ldr x30, [sp, #240]

    // add sp, sp, #256
    emit_u32_le(c, 0x910403FF);

    // --- resume: b resume_addr ---
    s.resume_fixup_offset = c.size();
    s.resume_insn_size = 0;  // ARM64 B is PC-relative
    emit_u32_le(c, arm64_b(0));  // placeholder

    return s;
}

// ============================================================================
// Region patch (JMP / B)
// ============================================================================

static std::vector<uint8_t>
make_x86_jmp_patch(uint64_t region_size, uint64_t from, uint64_t to) {
    std::vector<uint8_t> p(region_size, 0x90);
    const auto rel = static_cast<int32_t>(
        static_cast<int64_t>(to) - static_cast<int64_t>(from + 5));
    p[0] = 0xE9;
    std::memcpy(p.data() + 1, &rel, 4);
    return p;
}

static std::vector<uint8_t>
make_arm64_branch_patch(uint64_t region_size, uint64_t from, uint64_t to) {
    std::vector<uint8_t> p;
    p.reserve(region_size);
    emit_u32_le(p, arm64_b(static_cast<int64_t>(to - from)));
    while (p.size() < region_size) emit_u32_le(p, ARM64_NOP);
    p.resize(region_size);
    return p;
}

// ============================================================================
// Public API
// ============================================================================

tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
StubGenerator::generate_region_patch(
    Common::FileArch arch, Common::FileMode mode,
    uint64_t region_size, uint64_t from, uint64_t to) noexcept {
    if (region_size < min_region_size(arch, mode))
        return tl::unexpected(Common::DiagnosticCode::PatchRegionTooSmall);
    switch (arch) {
        case Common::FileArch::X86:   return make_x86_jmp_patch(region_size, from, to);
        case Common::FileArch::ARM64: return make_arm64_branch_patch(region_size, from, to);
        default: return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
    }
}

tl::expected<Stub, Common::DiagnosticCode>
StubGenerator::generate_entry_stub(
    Common::FileArch arch, Common::FileMode mode, uint32_t region_idx) noexcept {
    switch (arch) {
        case Common::FileArch::X86: {
            const auto m = static_cast<uint32_t>(mode);
            if (m & static_cast<uint32_t>(Common::FileMode::MODE_64))
                return make_x86_64_entry_stub(region_idx);
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
        }
        case Common::FileArch::ARM64:
            return make_arm64_entry_stub(region_idx);
        default:
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
    }
}

tl::expected<void, Common::DiagnosticCode>
StubGenerator::fixup_blob_displacement(Stub& s, int64_t disp,
                                       Common::FileArch arch) noexcept {
    return fixup_disp(s.code, s.blob_fixup_offset, disp, arch);
}

tl::expected<void, Common::DiagnosticCode>
StubGenerator::fixup_seed_displacement(Stub& s, int64_t disp,
                                       Common::FileArch arch) noexcept {
    return fixup_disp(s.code, s.seed_fixup_offset, disp, arch);
}

tl::expected<void, Common::DiagnosticCode>
StubGenerator::fixup_resume_displacement(Stub& s, int64_t disp,
                                         Common::FileArch arch) noexcept {
    return fixup_branch(s.code, s.resume_fixup_offset, disp, arch);
}

void StubGenerator::fixup_blob_size(Stub& s, uint64_t blob_size) noexcept {
    const auto off = s.size_fixup_offset;
    if (off + 4 > s.code.size()) return;
    const uint32_t existing = static_cast<uint32_t>(s.code[off]) |
                              (static_cast<uint32_t>(s.code[off+1]) << 8) |
                              (static_cast<uint32_t>(s.code[off+2]) << 16) |
                              (static_cast<uint32_t>(s.code[off+3]) << 24);
    // Detect ARM64 MOVZ vs x86 imm32
    if ((existing & 0x7F800000) == 0x52800000 ||
        (existing & 0x7F800000) == 0xD2800000) {
        const uint8_t rd = existing & 0x1F;
        const bool is_64 = (existing >> 31) & 1;
        const uint32_t insn = is_64
            ? arm64_movz_x(rd, static_cast<uint16_t>(blob_size))
            : arm64_movz_w(rd, static_cast<uint16_t>(blob_size));
        s.code[off+0] = static_cast<uint8_t>(insn);
        s.code[off+1] = static_cast<uint8_t>(insn >> 8);
        s.code[off+2] = static_cast<uint8_t>(insn >> 16);
        s.code[off+3] = static_cast<uint8_t>(insn >> 24);
    } else {
        const auto sz = static_cast<uint32_t>(blob_size);
        std::memcpy(s.code.data() + off, &sz, 4);
    }
}

void StubGenerator::fixup_delta_static_va(Stub& s, uint64_t static_va) noexcept {
    const auto off = s.delta_static_va_fixup_offset;
    if (off == 0) return;

    // Detect architecture by inspecting the first instruction at fixup offset.
    // ARM64 MOVZ X10: opcode mask 0xFF800000 == 0xD2800000 (sf=1, opc=10, hw=00)
    // x86-64: raw imm64 (8 bytes after movabs prefix)
    if (off + 4 <= s.code.size()) {
        const uint32_t first = static_cast<uint32_t>(s.code[off]) |
                               (static_cast<uint32_t>(s.code[off+1]) << 8) |
                               (static_cast<uint32_t>(s.code[off+2]) << 16) |
                               (static_cast<uint32_t>(s.code[off+3]) << 24);

        if ((first & 0xFF800000) == 0xD2800000) {
            // ARM64: patch 4 instructions (MOVZ + 3 MOVK) at consecutive offsets
            if (off + 16 > s.code.size()) return;
            const uint8_t rd = first & 0x1F;

            auto write_insn = [&](size_t insn_off, uint32_t insn) {
                s.code[insn_off+0] = static_cast<uint8_t>(insn);
                s.code[insn_off+1] = static_cast<uint8_t>(insn >> 8);
                s.code[insn_off+2] = static_cast<uint8_t>(insn >> 16);
                s.code[insn_off+3] = static_cast<uint8_t>(insn >> 24);
            };

            write_insn(off,      arm64_movz_x(rd, static_cast<uint16_t>(static_va)));
            write_insn(off + 4,  arm64_movk_x(rd, static_cast<uint16_t>(static_va >> 16), 1));
            write_insn(off + 8,  arm64_movk_x(rd, static_cast<uint16_t>(static_va >> 32), 2));
            write_insn(off + 12, arm64_movk_x(rd, static_cast<uint16_t>(static_va >> 48), 3));
            return;
        }
    }

    // x86-64: raw 8-byte immediate (movabs r10, imm64)
    if (off + 8 > s.code.size()) return;
    std::memcpy(s.code.data() + off, &static_va, 8);
}

size_t StubGenerator::min_region_size(
    Common::FileArch arch, Common::FileMode /*mode*/) noexcept {
    switch (arch) {
        case Common::FileArch::X86:   return 5;
        case Common::FileArch::ARM64: return 4;
        default:                      return 0;
    }
}

int64_t StubGenerator::max_branch_distance(Common::FileArch arch) noexcept {
    switch (arch) {
        case Common::FileArch::X86:   return INT32_MAX;
        case Common::FileArch::ARM64: return 128LL * 1024 * 1024;
        default:                      return 0;
    }
}

}  // namespace VMPilot::Loader
