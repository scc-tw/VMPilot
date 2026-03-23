#include <StubGenerator.hpp>

#include <cstring>

namespace VMPilot::Loader {

// ---------------------------------------------------------------------------
// x86 / x86-64 helpers
// ---------------------------------------------------------------------------

/// Build a JMP rel32 (E9 xx xx xx xx) + NOP fill.
static std::vector<uint8_t>
make_x86_jmp_patch(uint64_t region_size, uint64_t from_addr, uint64_t to_addr) {
    std::vector<uint8_t> patch(region_size, 0x90);  // fill with NOP

    // E9 <rel32>: target = from_addr + 5 + rel32
    const int64_t rel = static_cast<int64_t>(to_addr) -
                        static_cast<int64_t>(from_addr + 5);
    const auto rel32 = static_cast<int32_t>(rel);

    patch[0] = 0xE9;
    std::memcpy(patch.data() + 1, &rel32, 4);
    return patch;
}

/// Build an x86-64 entry stub.
///
/// Layout:
///   push rbp               55
///   mov rbp, rsp           48 89 E5
///   push rbx               53
///   push r12               41 54
///   push r13               41 55
///   push r14               41 56
///   push r15               41 57
///   sub rsp, 8             48 83 EC 08       ; align stack to 16
///   lea rdi, [rip+disp32]  48 8D 3D xx xx xx xx  ; arg0 = blob ptr
///   mov esi, imm32         BE xx xx xx xx    ; arg1 = region_idx
///   call rel32             E8 xx xx xx xx    ; _vmpilot_vm_entry (fixup)
///   add rsp, 8             48 83 C4 08
///   pop r15                41 5F
///   pop r14                41 5E
///   pop r13                41 5D
///   pop r12                41 5C
///   pop rbx                5B
///   pop rbp                5D
///   ret                    C3
static tl::expected<Stub, Common::DiagnosticCode>
make_x86_64_entry_stub(uint32_t region_idx, int64_t blob_rel_offset) {
    Stub stub;
    auto& c = stub.code;

    // push rbp
    c.push_back(0x55);
    // mov rbp, rsp
    c.push_back(0x48); c.push_back(0x89); c.push_back(0xE5);
    // push rbx
    c.push_back(0x53);
    // push r12
    c.push_back(0x41); c.push_back(0x54);
    // push r13
    c.push_back(0x41); c.push_back(0x55);
    // push r14
    c.push_back(0x41); c.push_back(0x56);
    // push r15
    c.push_back(0x41); c.push_back(0x57);
    // sub rsp, 8
    c.push_back(0x48); c.push_back(0x83); c.push_back(0xEC); c.push_back(0x08);

    // lea rdi, [rip + disp32]  — arg0 = blob pointer
    // RIP-relative: target = RIP_after_insn + disp32
    // instruction = 48 8D 3D xx xx xx xx  (7 bytes)
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x3D);
    stub.blob_fixup_offset = c.size();  // offset of disp32 within code
    stub.blob_insn_size = 4;            // remaining bytes of this instruction
    // Placeholder disp32 — patched by ELFPatcher after layout
    const auto disp32 = static_cast<int32_t>(blob_rel_offset);
    uint8_t disp_bytes[4];
    std::memcpy(disp_bytes, &disp32, 4);
    c.insert(c.end(), disp_bytes, disp_bytes + 4);

    // mov esi, region_idx
    c.push_back(0xBE);
    uint8_t idx_bytes[4];
    std::memcpy(idx_bytes, &region_idx, 4);
    c.insert(c.end(), idx_bytes, idx_bytes + 4);

    // call rel32 — placeholder 0x00000000 (needs relocation to _vmpilot_vm_entry)
    c.push_back(0xE8);
    stub.call_fixup_offset = c.size();  // offset of call rel32
    c.push_back(0x00); c.push_back(0x00); c.push_back(0x00); c.push_back(0x00);

    // add rsp, 8
    c.push_back(0x48); c.push_back(0x83); c.push_back(0xC4); c.push_back(0x08);
    // pop r15
    c.push_back(0x41); c.push_back(0x5F);
    // pop r14
    c.push_back(0x41); c.push_back(0x5E);
    // pop r13
    c.push_back(0x41); c.push_back(0x5D);
    // pop r12
    c.push_back(0x41); c.push_back(0x5C);
    // pop rbx
    c.push_back(0x5B);
    // pop rbp
    c.push_back(0x5D);
    // ret
    c.push_back(0xC3);

    return stub;
}

// ---------------------------------------------------------------------------
// x86 (32-bit) helpers
// ---------------------------------------------------------------------------

static tl::expected<Stub, Common::DiagnosticCode>
make_x86_32_entry_stub(uint32_t region_idx, int64_t blob_rel_offset) {
    Stub stub;
    auto& c = stub.code;

    // push ebp
    c.push_back(0x55);
    // mov ebp, esp
    c.push_back(0x89); c.push_back(0xE5);
    // push ebx
    c.push_back(0x53);
    // push esi
    c.push_back(0x56);
    // push edi
    c.push_back(0x57);

    // For 32-bit, use call/pop to get EIP (PIC-style)
    // call next
    c.push_back(0xE8);
    c.push_back(0x00); c.push_back(0x00); c.push_back(0x00); c.push_back(0x00);
    // pop eax — eax = current EIP
    c.push_back(0x58);

    // add eax, imm32  ; compute blob address (EIP + blob_rel_offset)
    c.push_back(0x05);
    stub.blob_fixup_offset = c.size();
    stub.blob_insn_size = 4;  // imm32 within ADD
    const auto adj = static_cast<int32_t>(blob_rel_offset);
    uint8_t adj_bytes[4];
    std::memcpy(adj_bytes, &adj, 4);
    c.insert(c.end(), adj_bytes, adj_bytes + 4);

    // push region_idx  (arg1 — cdecl, right-to-left)
    c.push_back(0x68);
    uint8_t idx_bytes[4];
    std::memcpy(idx_bytes, &region_idx, 4);
    c.insert(c.end(), idx_bytes, idx_bytes + 4);

    // push eax  (arg0 — blob pointer)
    c.push_back(0x50);

    // call rel32 — placeholder (needs relocation)
    c.push_back(0xE8);
    stub.call_fixup_offset = c.size();
    c.push_back(0x00); c.push_back(0x00); c.push_back(0x00); c.push_back(0x00);

    // add esp, 8  (clean up cdecl args)
    c.push_back(0x83); c.push_back(0xC4); c.push_back(0x08);

    // pop edi
    c.push_back(0x5F);
    // pop esi
    c.push_back(0x5E);
    // pop ebx
    c.push_back(0x5B);
    // pop ebp
    c.push_back(0x5D);
    // ret
    c.push_back(0xC3);

    return stub;
}

// ---------------------------------------------------------------------------
// ARM64 helpers
// ---------------------------------------------------------------------------

static constexpr uint32_t ARM64_NOP = 0xD503201F;

/// Encode ARM64 B (unconditional branch) instruction.
/// B imm26: offset = imm26 << 2, range ±128MB
static uint32_t arm64_encode_b(int64_t offset) {
    const int32_t imm26 = static_cast<int32_t>(offset >> 2) & 0x03FFFFFF;
    return 0x14000000u | static_cast<uint32_t>(imm26);
}

/// Encode ARM64 BL (branch with link) instruction.
static uint32_t arm64_encode_bl(int64_t offset) {
    const int32_t imm26 = static_cast<int32_t>(offset >> 2) & 0x03FFFFFF;
    return 0x94000000u | static_cast<uint32_t>(imm26);
}

static void push_u32_le(std::vector<uint8_t>& v, uint32_t val) {
    v.push_back(static_cast<uint8_t>(val));
    v.push_back(static_cast<uint8_t>(val >> 8));
    v.push_back(static_cast<uint8_t>(val >> 16));
    v.push_back(static_cast<uint8_t>(val >> 24));
}

/// Build an ARM64 B + NOP fill patch.
static std::vector<uint8_t>
make_arm64_branch_patch(uint64_t region_size, uint64_t from_addr, uint64_t to_addr) {
    std::vector<uint8_t> patch;
    patch.reserve(region_size);

    const int64_t offset = static_cast<int64_t>(to_addr - from_addr);
    push_u32_le(patch, arm64_encode_b(offset));

    // Fill remaining with NOP (4-byte aligned)
    while (patch.size() < region_size)
        push_u32_le(patch, ARM64_NOP);

    patch.resize(region_size);
    return patch;
}

/// Build an ARM64 entry stub.
///
/// Layout:
///   stp x29, x30, [sp, #-16]!
///   stp x19, x20, [sp, #-16]!
///   stp x21, x22, [sp, #-16]!
///   stp x23, x24, [sp, #-16]!
///   adr x0, blob_data        ; arg0 = blob ptr (placeholder, ±1MB)
///   mov w1, #region_idx      ; arg1 = region index
///   bl _vmpilot_vm_entry     ; (fixup)
///   ldp x23, x24, [sp], #16
///   ldp x21, x22, [sp], #16
///   ldp x19, x20, [sp], #16
///   ldp x29, x30, [sp], #16
///   ret
static tl::expected<Stub, Common::DiagnosticCode>
make_arm64_entry_stub(uint32_t region_idx, int64_t blob_rel_offset) {
    Stub stub;
    auto& c = stub.code;

    // stp x29, x30, [sp, #-16]!
    push_u32_le(c, 0xA9BF7BFD);
    // stp x19, x20, [sp, #-16]!
    push_u32_le(c, 0xA9BF53F3);
    // stp x21, x22, [sp, #-16]!
    push_u32_le(c, 0xA9BF5BF5);
    // stp x23, x24, [sp, #-16]!
    push_u32_le(c, 0xA9BF63F7);

    // adr x0, blob_data — ADR: PC-relative, ±1MB
    // Placeholder — patched by ELFPatcher after layout
    stub.blob_fixup_offset = c.size();
    stub.blob_insn_size = 4;  // ARM64 instruction is always 4 bytes
    {
        const int64_t pc_offset = blob_rel_offset;
        const uint32_t immlo = (static_cast<uint32_t>(pc_offset) & 0x3) << 29;
        const uint32_t immhi = (static_cast<uint32_t>(pc_offset >> 2) & 0x7FFFF) << 5;
        push_u32_le(c, 0x10000000 | immhi | immlo);  // adr x0, #offset
    }

    // mov w1, #region_idx — MOVZ w1, #imm16
    push_u32_le(c, 0x52800001 | (static_cast<uint32_t>(region_idx & 0xFFFF) << 5));

    // bl _vmpilot_vm_entry — placeholder offset 0
    stub.call_fixup_offset = c.size();
    push_u32_le(c, arm64_encode_bl(0));  // needs relocation

    // ldp x23, x24, [sp], #16
    push_u32_le(c, 0xA8C163F7);
    // ldp x21, x22, [sp], #16
    push_u32_le(c, 0xA8C15BF5);
    // ldp x19, x20, [sp], #16
    push_u32_le(c, 0xA8C153F3);
    // ldp x29, x30, [sp], #16
    push_u32_le(c, 0xA8C17BFD);
    // ret
    push_u32_le(c, 0xD65F03C0);

    return stub;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
StubGenerator::generate_region_patch(
    Common::FileArch arch, Common::FileMode mode,
    uint64_t region_size,
    uint64_t from_addr, uint64_t to_addr) noexcept {

    const size_t min_sz = min_region_size(arch, mode);
    if (region_size < min_sz)
        return tl::unexpected(Common::DiagnosticCode::PatchRegionTooSmall);

    switch (arch) {
        case Common::FileArch::X86:
            return make_x86_jmp_patch(region_size, from_addr, to_addr);
        case Common::FileArch::ARM64:
            return make_arm64_branch_patch(region_size, from_addr, to_addr);
        default:
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
    }
}

tl::expected<Stub, Common::DiagnosticCode>
StubGenerator::generate_entry_stub(
    Common::FileArch arch, Common::FileMode mode,
    uint32_t region_idx, int64_t blob_rel_offset) noexcept {

    switch (arch) {
        case Common::FileArch::X86: {
            const auto m = static_cast<uint32_t>(mode);
            if (m & static_cast<uint32_t>(Common::FileMode::MODE_64))
                return make_x86_64_entry_stub(region_idx, blob_rel_offset);
            if (m & static_cast<uint32_t>(Common::FileMode::MODE_32))
                return make_x86_32_entry_stub(region_idx, blob_rel_offset);
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
        }
        case Common::FileArch::ARM64:
            return make_arm64_entry_stub(region_idx, blob_rel_offset);
        default:
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
    }
}

size_t StubGenerator::min_region_size(
    Common::FileArch arch, Common::FileMode /*mode*/) noexcept {
    switch (arch) {
        case Common::FileArch::X86:   return 5;    // E9 rel32
        case Common::FileArch::ARM64: return 4;    // B imm26
        default:                      return 0;
    }
}

int64_t StubGenerator::max_branch_distance(Common::FileArch arch) noexcept {
    switch (arch) {
        case Common::FileArch::X86:   return INT32_MAX;          // ±2GB
        case Common::FileArch::ARM64: return 128LL * 1024 * 1024; // ±128MB
        default:                      return 0;
    }
}

tl::expected<void, Common::DiagnosticCode>
StubGenerator::fixup_blob_displacement(Stub& stub, int64_t displacement,
                                       Common::FileArch arch) noexcept {
    using DC = Common::DiagnosticCode;

    if (arch == Common::FileArch::ARM64) {
        // ARM64 ADR: 21-bit signed, byte granularity → ±1MB
        constexpr int64_t ADR_RANGE = 1 << 20;  // 1048576
        if (displacement < -ADR_RANGE || displacement >= ADR_RANGE)
            return tl::unexpected(DC::PatchStubGenerationFailed);

        const auto ud = static_cast<uint32_t>(static_cast<int32_t>(displacement));
        const uint32_t immlo = (ud & 0x3) << 29;
        const uint32_t immhi = ((ud >> 2) & 0x7FFFF) << 5;
        const uint32_t adr = 0x10000000 | immhi | immlo;

        stub.code[stub.blob_fixup_offset + 0] = static_cast<uint8_t>(adr);
        stub.code[stub.blob_fixup_offset + 1] = static_cast<uint8_t>(adr >> 8);
        stub.code[stub.blob_fixup_offset + 2] = static_cast<uint8_t>(adr >> 16);
        stub.code[stub.blob_fixup_offset + 3] = static_cast<uint8_t>(adr >> 24);
    } else {
        // x86: disp32 → ±2GB
        if (displacement < INT32_MIN || displacement > INT32_MAX)
            return tl::unexpected(DC::PatchStubGenerationFailed);

        const auto d32 = static_cast<int32_t>(displacement);
        std::memcpy(stub.code.data() + stub.blob_fixup_offset, &d32, 4);
    }
    return {};
}

}  // namespace VMPilot::Loader
