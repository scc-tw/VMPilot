#include <ArchTraits.hpp>
#include <StubEmitter.hpp>

#include <climits>
#include <cstring>

namespace VMPilot::Loader {

using Traits = ArchTraits<Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN>;
using DC     = Common::DiagnosticCode;

// -----------------------------------------------------------------------
// ARM64 instruction encoding helpers
// -----------------------------------------------------------------------

/// B imm26 — unconditional branch, PC-relative, +/-128 MB.
static uint32_t arm64_b(int64_t off) {
    return 0x14000000u | (static_cast<uint32_t>(off >> 2) & 0x03FFFFFFu);
}

/// BL imm26 — branch-with-link.
[[maybe_unused]]
static uint32_t arm64_bl(int64_t off) {
    return 0x94000000u | (static_cast<uint32_t>(off >> 2) & 0x03FFFFFFu);
}

/// ADR Xd, #imm21 — PC-relative address (+/-1 MB).
static uint32_t arm64_adr(uint8_t rd, int64_t off) {
    auto u = static_cast<uint32_t>(static_cast<int32_t>(off));
    return 0x10000000u
         | (((u >> 2) & 0x7FFFFu) << 5)
         | ((u & 3u) << 29)
         | rd;
}

/// MOVZ Xd, #imm16, LSL #(hw*16)
static uint32_t arm64_movz_x(uint8_t rd, uint16_t imm16, uint8_t hw = 0) {
    return 0xD2800000u
         | (static_cast<uint32_t>(hw) << 21)
         | (static_cast<uint32_t>(imm16) << 5)
         | rd;
}

/// MOVK Xd, #imm16, LSL #(hw*16)
static uint32_t arm64_movk_x(uint8_t rd, uint16_t imm16, uint8_t hw) {
    return 0xF2800000u
         | (static_cast<uint32_t>(hw) << 21)
         | (static_cast<uint32_t>(imm16) << 5)
         | rd;
}

/// STP Xt1, Xt2, [Xn, #simm7]!   (pre-index, writeback)
static uint32_t arm64_stp_pre(uint8_t rt1, uint8_t rt2,
                               uint8_t rn, int16_t simm7) {
    // C6.2.276  STP (pre-index): 1 01 0 1001 11 imm7 Rt2 Rn Rt
    uint32_t imm7_field = static_cast<uint32_t>(simm7 / 8) & 0x7Fu;
    return 0xA9800000u
         | (imm7_field << 15)
         | (static_cast<uint32_t>(rt2) << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rt1;
}

/// LDP Xt1, Xt2, [Xn], #simm7   (post-index)
static uint32_t arm64_ldp_post(uint8_t rt1, uint8_t rt2,
                                uint8_t rn, int16_t simm7) {
    // C6.2.131  LDP (post-index): 1 01 0 1000 11 imm7 Rt2 Rn Rt
    uint32_t imm7_field = static_cast<uint32_t>(simm7 / 8) & 0x7Fu;
    return 0xA8C00000u
         | (imm7_field << 15)
         | (static_cast<uint32_t>(rt2) << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rt1;
}

/// STP Xt1, Xt2, [Xn, #uimm7]   (signed offset, no writeback)
static uint32_t arm64_stp_offset(uint8_t rt1, uint8_t rt2,
                                  uint8_t rn, int16_t simm7) {
    // C6.2.276  STP (signed offset): 1 01 0 1001 00 imm7 Rt2 Rn Rt
    uint32_t imm7_field = static_cast<uint32_t>(simm7 / 8) & 0x7Fu;
    return 0xA9000000u
         | (imm7_field << 15)
         | (static_cast<uint32_t>(rt2) << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rt1;
}

/// STR Xt, [Xn, #simm9]!   (pre-index)
static uint32_t arm64_str_pre(uint8_t rt, uint8_t rn, int16_t simm9) {
    // F8 00 0C 00 : STR Xt, [Xn, #simm9]!
    uint32_t imm9_field = static_cast<uint32_t>(simm9) & 0x1FFu;
    return 0xF8000C00u
         | (imm9_field << 12)
         | (static_cast<uint32_t>(rn) << 5)
         | rt;
}

/// STR Xt, [Xn, #uimm12]   (unsigned offset, scaled by 8)
static uint32_t arm64_str_uoff(uint8_t rt, uint8_t rn, uint16_t byte_off) {
    // F9 00 00 00 : STR Xt, [Xn, #uimm12<<3]
    uint32_t imm12 = static_cast<uint32_t>(byte_off / 8) & 0xFFFu;
    return 0xF9000000u
         | (imm12 << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rt;
}

/// SUB Xd, Xn, Xm
static uint32_t arm64_sub_reg(uint8_t rd, uint8_t rn, uint8_t rm) {
    return 0xCB000000u
         | (static_cast<uint32_t>(rm) << 16)
         | (static_cast<uint32_t>(rn) << 5)
         | rd;
}

/// SUB Xd, Xn, #imm12
static uint32_t arm64_sub_imm(uint8_t rd, uint8_t rn, uint16_t imm12) {
    return 0xD1000000u
         | (static_cast<uint32_t>(imm12 & 0xFFFu) << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rd;
}

/// ADD Xd, Xn, #imm12
static uint32_t arm64_add_imm(uint8_t rd, uint8_t rn, uint16_t imm12) {
    return 0x91000000u
         | (static_cast<uint32_t>(imm12 & 0xFFFu) << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rd;
}

/// MOV Xd, Xm  (alias for ORR Xd, XZR, Xm)
static uint32_t arm64_mov_reg(uint8_t rd, uint8_t rm) {
    return 0xAA0003E0u
         | (static_cast<uint32_t>(rm) << 16)
         | rd;
}

/// MOV Wd, #imm16  (MOVZ Wd — 32-bit)
static uint32_t arm64_movz_w(uint8_t rd, uint16_t imm16) {
    return 0x52800000u
         | (static_cast<uint32_t>(imm16) << 5)
         | rd;
}

/// MOV Xd, #0  (MOVZ Xd, #0)
static uint32_t arm64_mov_imm0(uint8_t rd) {
    return arm64_movz_x(rd, 0, 0);
}

/// LDR Xd, [Xn]  (unsigned offset 0)
static uint32_t arm64_ldr_uoff(uint8_t rt, uint8_t rn, uint16_t byte_off = 0) {
    uint32_t imm12 = static_cast<uint32_t>(byte_off / 8) & 0xFFFu;
    return 0xF9400000u
         | (imm12 << 10)
         | (static_cast<uint32_t>(rn) << 5)
         | rt;
}

/// BLR Xn — branch-with-link to register.
static uint32_t arm64_blr(uint8_t rn) {
    return 0xD63F0000u | (static_cast<uint32_t>(rn) << 5);
}

/// NOP
static constexpr uint32_t arm64_nop() { return 0xD503201Fu; }

/// Append a 32-bit instruction in little-endian.
static void emit32(std::vector<uint8_t>& c, uint32_t insn) {
    c.push_back(static_cast<uint8_t>(insn));
    c.push_back(static_cast<uint8_t>(insn >> 8));
    c.push_back(static_cast<uint8_t>(insn >> 16));
    c.push_back(static_cast<uint8_t>(insn >> 24));
}

/// Write a 32-bit instruction at a given byte offset.
static void write32(std::vector<uint8_t>& c, size_t off, uint32_t insn) {
    c[off + 0] = static_cast<uint8_t>(insn);
    c[off + 1] = static_cast<uint8_t>(insn >> 8);
    c[off + 2] = static_cast<uint8_t>(insn >> 16);
    c[off + 3] = static_cast<uint8_t>(insn >> 24);
}

/// Read a 32-bit instruction from a given byte offset.
static uint32_t read32(const std::vector<uint8_t>& c, size_t off) {
    uint32_t v;
    std::memcpy(&v, &c[off], 4);
    return v;
}

// -----------------------------------------------------------------------
// ARM64StubEmitter
// -----------------------------------------------------------------------

class ARM64StubEmitter final : public StubEmitter {
public:
    [[nodiscard]] tl::expected<Stub, DC>
    emit_entry_stub() noexcept override;

    [[nodiscard]] tl::expected<std::vector<uint8_t>, DC>
    emit_region_patch(uint64_t region_size,
                      uint64_t from_addr, uint64_t to_addr) noexcept override;

    [[nodiscard]] tl::expected<void, DC>
    fixup_ptr_disp(std::vector<uint8_t>& code,
                   size_t offset, int64_t disp) noexcept override;

    [[nodiscard]] tl::expected<void, DC>
    fixup_branch_disp(std::vector<uint8_t>& code,
                      size_t offset, int64_t disp) noexcept override;

    void fixup_immediate(std::vector<uint8_t>& code,
                         size_t offset, uint64_t value) noexcept override;

    void fixup_static_va(std::vector<uint8_t>& code,
                         size_t offset, size_t size,
                         uint64_t va) noexcept override;

    [[nodiscard]] size_t  min_region_size()    const noexcept override;
    [[nodiscard]] int64_t max_branch_distance() const noexcept override;
    [[nodiscard]] int64_t pc_fixup_bias()       const noexcept override;
};

// -----------------------------------------------------------------------
// emit_entry_stub
// -----------------------------------------------------------------------

tl::expected<Stub, DC>
ARM64StubEmitter::emit_entry_stub() noexcept {
    Stub s;
    auto& c = s.code;
    c.reserve(512);

    constexpr uint8_t sp = 31;  // stack pointer encoding

    // ---- 1. Save callee-saved regs (6 STP pairs, pre-index) ----
    // Pairs: (x19,x20), (x21,x22), (x23,x24), (x25,x26), (x27,x28), (x29,x30)
    static constexpr uint8_t pairs[][2] = {
        {19, 20}, {21, 22}, {23, 24}, {25, 26}, {27, 28}, {29, 30}
    };
    for (auto& p : pairs)
        emit32(c, arm64_stp_pre(p[0], p[1], sp, -16));

    // ---- 2. ASLR delta ----
    //   adr x9, .                           ; x9 = runtime VA of this insn
    //   movz x10, #0                        ; placeholder imm64 (4 insns)
    //   movk x10, #0, lsl #16
    //   movk x10, #0, lsl #32
    //   movk x10, #0, lsl #48
    //   sub x9, x9, x10                     ; x9 = delta
    //   str x9, [sp, #-8]!                  ; push delta

    s.delta_ref_offset = c.size();              // ADR instruction offset
    emit32(c, arm64_adr(9, 0));                 // adr x9, .

    s.delta_fixup_offset = c.size();
    s.delta_fixup_size   = 16;                  // 4 instructions x 4 bytes
    emit32(c, arm64_movz_x(10, 0, 0));         // movz x10, #0
    emit32(c, arm64_movk_x(10, 0, 1));         // movk x10, #0, lsl #16
    emit32(c, arm64_movk_x(10, 0, 2));         // movk x10, #0, lsl #32
    emit32(c, arm64_movk_x(10, 0, 3));         // movk x10, #0, lsl #48

    emit32(c, arm64_sub_reg(9, 9, 10));         // sub x9, x9, x10
    emit32(c, arm64_str_pre(9, sp, -8));        // str x9, [sp, #-8]!

    // ---- 3. Build initial_regs[16] on stack ----
    // We store 16 x 64-bit GPRs = 256 bytes (even though gpr_count is 31,
    // the vm_execute_with_args interface uses num_regs=16 for the first 16).
    constexpr int16_t regs_frame = 256;         // 32 * 8 (space for pairs up to x30)
    emit32(c, arm64_sub_imm(sp, sp, regs_frame));

    // STP pairs: x0/x1 .. x28/x29 at [sp, #offset]
    for (uint8_t i = 0; i < 30; i += 2)
        emit32(c, arm64_stp_offset(i, i + 1, sp, static_cast<int16_t>(i * 8)));

    // STR x30 (LR) at [sp, #240]
    emit32(c, arm64_str_uoff(30, sp, 240));

    // ---- 4. Set up arguments ----
    //   adr x0, blob                        ; arg0: blob_ptr
    //   movz w1, #size                      ; arg1: blob_size (imm16 placeholder)
    //   adr x2, seed                        ; arg2: seed_ptr
    //   mov x3, sp                          ; arg3: initial_regs
    //   mov w4, #16                         ; arg4: num_regs
    //   mov x5, #0                          ; arg5: config = nullptr

    s.blob_fixup_offset = c.size();
    emit32(c, arm64_adr(0, 0));                 // adr x0, blob (placeholder)

    s.size_fixup_offset = c.size();
    emit32(c, arm64_movz_w(1, 0));              // movz w1, #0 (placeholder)

    s.seed_fixup_offset = c.size();
    emit32(c, arm64_adr(2, 0));                 // adr x2, seed (placeholder)

    emit32(c, arm64_mov_reg(3, sp));            // mov x3, sp

    emit32(c, arm64_movz_w(4, 16));             // mov w4, #16

    emit32(c, arm64_mov_imm0(5));               // mov x5, #0

    // ---- 5. Indirect call: adr x9, slot; ldr x9, [x9]; blr x9 ----
    s.call_slot_fixup_offset = c.size();
    emit32(c, arm64_adr(9, 0));                 // adr x9, slot (placeholder)
    emit32(c, arm64_ldr_uoff(9, 9, 0));        // ldr x9, [x9]
    emit32(c, arm64_blr(9));                    // blr x9

    // ---- 6. Pop initial_regs frame ----
    emit32(c, arm64_add_imm(sp, sp, regs_frame));

    // ---- 7. Pop delta (discard) ----
    emit32(c, arm64_add_imm(sp, sp, 8));

    // ---- 8. Restore callee-saved (reverse order) ----
    for (int i = 5; i >= 0; --i)
        emit32(c, arm64_ldp_post(pairs[i][0], pairs[i][1], sp, 16));

    // ---- 9. B <resume> (placeholder) ----
    s.resume_fixup_offset = c.size();
    emit32(c, arm64_b(0));                      // b . (placeholder)

    return s;
}

// -----------------------------------------------------------------------
// emit_region_patch
// -----------------------------------------------------------------------

tl::expected<std::vector<uint8_t>, DC>
ARM64StubEmitter::emit_region_patch(uint64_t region_size,
                                     uint64_t from_addr,
                                     uint64_t to_addr) noexcept {
    if (region_size < Traits::min_region_size)
        return tl::unexpected(DC::PatchRegionTooSmall);

    int64_t off = static_cast<int64_t>(to_addr - from_addr);
    constexpr int64_t limit = 128LL * 1024 * 1024;
    if (off > limit || off < -limit)
        return tl::unexpected(DC::PatchRegionTooSmall);

    // region_size must be a multiple of 4 (ARM64 instruction alignment)
    auto insn_count = static_cast<size_t>(region_size / 4);
    if (insn_count == 0)
        return tl::unexpected(DC::PatchRegionTooSmall);

    std::vector<uint8_t> patch;
    patch.reserve(static_cast<size_t>(region_size));

    // First instruction: B to stub
    emit32(patch, arm64_b(off));

    // Remaining: NOP fill
    for (size_t i = 1; i < insn_count; ++i)
        emit32(patch, arm64_nop());

    return patch;
}

// -----------------------------------------------------------------------
// Fixup methods
// -----------------------------------------------------------------------

tl::expected<void, DC>
ARM64StubEmitter::fixup_ptr_disp(std::vector<uint8_t>& code,
                                  size_t offset, int64_t disp) noexcept {
    if (offset + 4 > code.size())
        return tl::unexpected(DC::PatchStubGenerationFailed);

    // Re-encode ADR Xd, #imm21.
    // Preserve existing Rd field; replace immhi:immlo with new displacement.
    uint32_t insn = read32(code, offset);
    uint8_t rd = insn & 0x1Fu;
    write32(code, offset, arm64_adr(rd, disp));
    return {};
}

tl::expected<void, DC>
ARM64StubEmitter::fixup_branch_disp(std::vector<uint8_t>& code,
                                     size_t offset, int64_t disp) noexcept {
    if (offset + 4 > code.size())
        return tl::unexpected(DC::PatchStubGenerationFailed);

    // Re-encode B imm26.
    write32(code, offset, arm64_b(disp));
    return {};
}

void ARM64StubEmitter::fixup_immediate(std::vector<uint8_t>& code,
                                        size_t offset,
                                        uint64_t value) noexcept {
    // Re-encode MOVZ with the given 16-bit value.
    // Detect the register from the existing instruction.
    uint32_t insn = read32(code, offset);
    uint8_t rd = insn & 0x1Fu;

    // Check if this is a 32-bit MOVZ (W-form) or 64-bit (X-form)
    bool is_w = (insn & 0x80000000u) == 0;  // sf bit = bit 31
    if (is_w) {
        write32(code, offset, arm64_movz_w(rd, static_cast<uint16_t>(value)));
    } else {
        write32(code, offset, arm64_movz_x(rd, static_cast<uint16_t>(value), 0));
    }
}

void ARM64StubEmitter::fixup_static_va(std::vector<uint8_t>& code,
                                        size_t offset, size_t /*size*/,
                                        uint64_t va) noexcept {
    // Re-encode MOVZ + 3x MOVK (16 bytes, 4 instructions).
    // Preserve Rd from existing first instruction.
    uint32_t insn0 = read32(code, offset);
    uint8_t rd = insn0 & 0x1Fu;

    write32(code, offset +  0, arm64_movz_x(rd, static_cast<uint16_t>(va >>  0), 0));
    write32(code, offset +  4, arm64_movk_x(rd, static_cast<uint16_t>(va >> 16), 1));
    write32(code, offset +  8, arm64_movk_x(rd, static_cast<uint16_t>(va >> 32), 2));
    write32(code, offset + 12, arm64_movk_x(rd, static_cast<uint16_t>(va >> 48), 3));
}

size_t  ARM64StubEmitter::min_region_size()     const noexcept { return Traits::min_region_size; }
int64_t ARM64StubEmitter::max_branch_distance() const noexcept { return Traits::max_branch_dist; }
int64_t ARM64StubEmitter::pc_fixup_bias()       const noexcept { return 0; }

// Factory — called from StubEmitter.cpp via create_emitter().
std::unique_ptr<StubEmitter> make_arm64_emitter() {
    return std::make_unique<ARM64StubEmitter>();
}

}  // namespace VMPilot::Loader
