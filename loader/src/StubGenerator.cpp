#include <StubGenerator.hpp>

#include <cstring>

namespace VMPilot::Loader {

// ============================================================================
// Helpers
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

/// Encode ARM64 B imm26.
static uint32_t arm64_b(int64_t byte_offset) {
    return 0x14000000u | (static_cast<uint32_t>(byte_offset >> 2) & 0x03FFFFFF);
}

/// Encode ARM64 BL imm26.
static uint32_t arm64_bl(int64_t byte_offset) {
    return 0x94000000u | (static_cast<uint32_t>(byte_offset >> 2) & 0x03FFFFFF);
}

/// Encode ARM64 ADR Xd, #imm21.
static uint32_t arm64_adr(uint8_t rd, int64_t byte_offset) {
    const auto u = static_cast<uint32_t>(static_cast<int32_t>(byte_offset));
    const uint32_t immlo = (u & 0x3) << 29;
    const uint32_t immhi = ((u >> 2) & 0x7FFFF) << 5;
    return 0x10000000 | immhi | immlo | rd;
}

/// Encode ARM64 MOVZ Wd, #imm16.
static uint32_t arm64_movz_w(uint8_t rd, uint16_t imm16) {
    return 0x52800000 | (static_cast<uint32_t>(imm16) << 5) | rd;
}

/// Encode ARM64 MOVZ Xd, #imm16.
static uint32_t arm64_movz_x(uint8_t rd, uint16_t imm16) {
    return 0xD2800000 | (static_cast<uint32_t>(imm16) << 5) | rd;
}

constexpr uint32_t ARM64_NOP = 0xD503201F;
constexpr uint32_t ARM64_RET = 0xD65F03C0;

// ============================================================================
// Patch displacement helpers (shared between blob and seed fixups)
// ============================================================================

static tl::expected<void, Common::DiagnosticCode>
fixup_disp(std::vector<uint8_t>& code, size_t offset, size_t insn_size,
           int64_t displacement, Common::FileArch arch) noexcept {
    using DC = Common::DiagnosticCode;
    if (arch == Common::FileArch::ARM64) {
        constexpr int64_t ADR_RANGE = 1 << 20;
        if (displacement < -ADR_RANGE || displacement >= ADR_RANGE)
            return tl::unexpected(DC::PatchStubGenerationFailed);
        const uint32_t insn = arm64_adr(
            code[offset] & 0x1F,  // preserve Rd from placeholder
            displacement);
        code[offset+0] = static_cast<uint8_t>(insn);
        code[offset+1] = static_cast<uint8_t>(insn >> 8);
        code[offset+2] = static_cast<uint8_t>(insn >> 16);
        code[offset+3] = static_cast<uint8_t>(insn >> 24);
    } else {
        (void)insn_size;
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
// ABI: vm_execute(rdi=blob_ptr, rsi=blob_size, rdx=seed_ptr, rcx=nullptr)
// ============================================================================

static tl::expected<Stub, Common::DiagnosticCode>
make_x86_64_entry_stub(uint32_t /*region_idx*/) {
    Stub s;
    auto& c = s.code;

    // --- prologue: save callee-saved regs ---
    c.push_back(0x55);                                      // push rbp
    c.push_back(0x48); c.push_back(0x89); c.push_back(0xE5); // mov rbp, rsp
    c.push_back(0x53);                                      // push rbx
    c.push_back(0x41); c.push_back(0x54);                   // push r12
    c.push_back(0x41); c.push_back(0x55);                   // push r13
    c.push_back(0x41); c.push_back(0x56);                   // push r14
    c.push_back(0x41); c.push_back(0x57);                   // push r15
    c.push_back(0x48); c.push_back(0x83); c.push_back(0xEC);
    c.push_back(0x08);                                      // sub rsp, 8 (align 16)

    // --- arg0: rdi = blob_ptr (LEA rdi, [rip + disp32]) ---
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x3D);
    s.blob_fixup_offset = c.size();
    s.blob_insn_size = 4;
    emit_placeholder32(c);

    // --- arg1: rsi = blob_size (MOV esi, imm32) ---
    c.push_back(0xBE);
    s.size_fixup_offset = c.size();
    emit_placeholder32(c);

    // --- arg2: rdx = seed_ptr (LEA rdx, [rip + disp32]) ---
    c.push_back(0x48); c.push_back(0x8D); c.push_back(0x15);
    s.seed_fixup_offset = c.size();
    s.seed_insn_size = 4;
    emit_placeholder32(c);

    // --- arg3: rcx = nullptr (default config) ---
    c.push_back(0x48); c.push_back(0x31); c.push_back(0xC9); // xor rcx, rcx

    // --- call vm_execute (rel32 placeholder) ---
    c.push_back(0xE8);
    s.call_fixup_offset = c.size();
    emit_placeholder32(c);

    // --- epilogue ---
    c.push_back(0x48); c.push_back(0x83); c.push_back(0xC4);
    c.push_back(0x08);                                      // add rsp, 8
    c.push_back(0x41); c.push_back(0x5F);                   // pop r15
    c.push_back(0x41); c.push_back(0x5E);                   // pop r14
    c.push_back(0x41); c.push_back(0x5D);                   // pop r13
    c.push_back(0x41); c.push_back(0x5C);                   // pop r12
    c.push_back(0x5B);                                      // pop rbx
    c.push_back(0x5D);                                      // pop rbp
    c.push_back(0xC3);                                      // ret
    return s;
}

// ============================================================================
// ARM64 entry stub
//
// ABI: vm_execute(x0=blob_ptr, x1=blob_size, x2=seed_ptr, x3=nullptr)
// ============================================================================

static tl::expected<Stub, Common::DiagnosticCode>
make_arm64_entry_stub(uint32_t /*region_idx*/) {
    Stub s;
    auto& c = s.code;

    // --- prologue ---
    emit_u32_le(c, 0xA9BF7BFD);   // stp x29, x30, [sp, #-16]!
    emit_u32_le(c, 0xA9BF53F3);   // stp x19, x20, [sp, #-16]!
    emit_u32_le(c, 0xA9BF5BF5);   // stp x21, x22, [sp, #-16]!
    emit_u32_le(c, 0xA9BF63F7);   // stp x23, x24, [sp, #-16]!

    // --- arg0: x0 = blob_ptr (ADR x0, #disp) ---
    s.blob_fixup_offset = c.size();
    s.blob_insn_size = 4;
    emit_u32_le(c, arm64_adr(0, 0));  // placeholder

    // --- arg1: x1 = blob_size (MOVZ x1, #imm16 — patched later) ---
    s.size_fixup_offset = c.size();
    emit_u32_le(c, arm64_movz_x(1, 0));  // placeholder

    // --- arg2: x2 = seed_ptr (ADR x2, #disp) ---
    s.seed_fixup_offset = c.size();
    s.seed_insn_size = 4;
    emit_u32_le(c, arm64_adr(2, 0));  // placeholder

    // --- arg3: x3 = nullptr ---
    emit_u32_le(c, arm64_movz_x(3, 0));

    // --- call vm_execute (BL, placeholder) ---
    s.call_fixup_offset = c.size();
    emit_u32_le(c, arm64_bl(0));

    // --- epilogue ---
    emit_u32_le(c, 0xA8C163F7);   // ldp x23, x24, [sp], #16
    emit_u32_le(c, 0xA8C15BF5);   // ldp x21, x22, [sp], #16
    emit_u32_le(c, 0xA8C153F3);   // ldp x19, x20, [sp], #16
    emit_u32_le(c, 0xA8C17BFD);   // ldp x29, x30, [sp], #16
    emit_u32_le(c, ARM64_RET);

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
            // 32-bit not supported for new ABI (vm_execute is 64-bit only)
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
        }
        case Common::FileArch::ARM64:
            return make_arm64_entry_stub(region_idx);
        default:
            return tl::unexpected(Common::DiagnosticCode::PatchArchUnsupported);
    }
}

tl::expected<void, Common::DiagnosticCode>
StubGenerator::fixup_blob_displacement(Stub& stub, int64_t disp,
                                       Common::FileArch arch) noexcept {
    return fixup_disp(stub.code, stub.blob_fixup_offset, stub.blob_insn_size,
                      disp, arch);
}

tl::expected<void, Common::DiagnosticCode>
StubGenerator::fixup_seed_displacement(Stub& stub, int64_t disp,
                                       Common::FileArch arch) noexcept {
    return fixup_disp(stub.code, stub.seed_fixup_offset, stub.seed_insn_size,
                      disp, arch);
}

void StubGenerator::fixup_blob_size(Stub& stub, uint64_t blob_size) noexcept {
    // x86-64: MOV esi, imm32 at size_fixup_offset
    // ARM64:  MOVZ x1, #imm16 at size_fixup_offset
    const auto off = stub.size_fixup_offset;
    if (off + 4 <= stub.code.size()) {
        // Detect arch from instruction: ARM64 instructions are 4-byte aligned
        // and the MOVZ has specific encoding. Simpler: check if it's 4-byte
        // boundary (ARM64) or not (x86).
        // For x86: just write 32-bit little-endian
        // For ARM64: re-encode MOVZ x1, #imm16
        const uint32_t existing = static_cast<uint32_t>(stub.code[off]) |
                                  (static_cast<uint32_t>(stub.code[off+1]) << 8) |
                                  (static_cast<uint32_t>(stub.code[off+2]) << 16) |
                                  (static_cast<uint32_t>(stub.code[off+3]) << 24);
        if ((existing & 0x7F800000) == 0x52800000 ||
            (existing & 0x7F800000) == 0xD2800000) {
            // ARM64 MOVZ — re-encode with new imm16
            const uint8_t rd = existing & 0x1F;
            const bool is_64 = (existing >> 31) & 1;
            const uint32_t insn = is_64
                ? arm64_movz_x(rd, static_cast<uint16_t>(blob_size))
                : arm64_movz_w(rd, static_cast<uint16_t>(blob_size));
            stub.code[off+0] = static_cast<uint8_t>(insn);
            stub.code[off+1] = static_cast<uint8_t>(insn >> 8);
            stub.code[off+2] = static_cast<uint8_t>(insn >> 16);
            stub.code[off+3] = static_cast<uint8_t>(insn >> 24);
        } else {
            // x86: write imm32
            const auto sz32 = static_cast<uint32_t>(blob_size);
            std::memcpy(stub.code.data() + off, &sz32, 4);
        }
    }
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
