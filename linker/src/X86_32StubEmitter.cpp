#include <StubEmitter.hpp>
#include <StubArgsLayout.hpp>
#include <ArchTraits.hpp>
#include <vm/vm_stub_args.hpp>

#include <cstring>
#include <vector>

namespace VMPilot::Loader {

using Traits = ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_32>;
using Layout = StubArgsLayout<Common::FileArch::X86, Common::FileMode::MODE_32>;
using DC = Common::DiagnosticCode;

static void emit32(std::vector<uint8_t>& c, uint32_t v) {
    c.push_back(static_cast<uint8_t>(v));
    c.push_back(static_cast<uint8_t>(v >> 8));
    c.push_back(static_cast<uint8_t>(v >> 16));
    c.push_back(static_cast<uint8_t>(v >> 24));
}

static void placeholder32(std::vector<uint8_t>& c) { emit32(c, 0); }

class X86_32StubEmitter final : public StubEmitter {
public:
    tl::expected<Stub, DC> emit_entry_stub() noexcept override {
        Stub s;
        auto& c = s.code;

        // ---- 0. CET landing pad (Intel CET indirect-call target) ----
        // ENDBR32: required on CET-enforced systems for indirect call targets.
        // NOP on older CPUs.
        c.push_back(0xF3); c.push_back(0x0F);
        c.push_back(0x1E); c.push_back(0xFB);

        // ---- 1. Save callee-saved: push ebx(3), ebp(5), esi(6), edi(7) ----
        for (auto reg : Traits::callee_saved) {
            if (reg < 8) c.push_back(0x50 + reg);
        }

        // ---- 2. ASLR delta via call/pop PIC trick ----
        // call .Lbase (E8 00000000) — pushes return address
        c.push_back(0xE8); emit32(c, 0);
        // pop ebx — ebx = EIP of this pop instruction
        c.push_back(0x5B);
        s.delta_ref_offset = c.size() - 1;

        // mov eax, <static_va> (placeholder); sub ebx, eax → ebx = delta
        c.push_back(0xB8);  // mov eax, imm32
        s.delta_fixup_offset = c.size();
        s.delta_fixup_size = 4;
        placeholder32(c);
        c.push_back(0x29); c.push_back(0xC3);  // sub ebx, eax
        // ebx = load_base_delta (kept in register, NOT pushed)

        // ---- 3. Build initial_regs[8] on stack (32 bytes) ----
        c.push_back(0x83); c.push_back(0xEC); c.push_back(0x20);  // sub esp, 32
        for (int i = 0; i < 8; ++i) {
            c.push_back(0x89);
            c.push_back(static_cast<uint8_t>(0x44 | ((i & 7) << 3)));  // ModRM [esp+disp8], reg
            c.push_back(0x24);                    // SIB base=esp
            c.push_back(static_cast<uint8_t>(i * 4));
        }
        // Save initial_regs pointer in esi (callee-saved, already pushed)
        // mov esi, esp  (89 E6)
        c.push_back(0x89); c.push_back(0xE6);

        // ---- 4. Allocate VmStubArgs (64 bytes) ----
        c.push_back(0x83); c.push_back(0xEC);
        c.push_back(static_cast<uint8_t>(Layout::total_size));  // sub esp, 64

        // Zero the struct: push 16 dwords of 0 would be expensive.
        // Instead we rely on writing all meaningful fields; pad_ is
        // don't-care (runtime only reads defined fields).

        // ---- 5. Fill VmStubArgs fields ----

        // version: mov dword [esp+0], VM_STUB_ABI_VERSION
        // C7 04 24 <imm32>
        c.push_back(0xC7); c.push_back(0x04); c.push_back(0x24);
        emit32(c, Common::VM::VM_STUB_ABI_VERSION);

        // num_regs: mov byte [esp+4], 8
        // C6 44 24 04 08
        c.push_back(0xC6); c.push_back(0x44); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_num_regs));
        c.push_back(Traits::gpr_count);  // 8

        // load_base_delta: ebx holds delta (32-bit value, sign-extended to 64)
        // mov [esp+8], ebx  (low 32 bits)
        // 89 5C 24 08
        c.push_back(0x89); c.push_back(0x5C); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_load_base_delta));
        // mov [esp+12], edx (high 32 bits: sign-extend ebx into edx via CDQ)
        // First: mov eax, ebx (89 D8); cdq (99) — edx:eax = sign-extend(ebx)
        c.push_back(0x89); c.push_back(0xD8);  // mov eax, ebx
        c.push_back(0x99);                      // cdq → edx = sign-extend of eax
        c.push_back(0x89); c.push_back(0x54); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_load_base_delta + 4));

        // blob_data: lea eax, [ebx + disp32]; mov [esp+16], eax
        c.push_back(0x8D); c.push_back(0x83);  // lea eax, [ebx + disp32]
        s.blob_fixup_offset = c.size();
        placeholder32(c);
        c.push_back(0x89); c.push_back(0x44); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_blob_data));

        // stored_seed: lea eax, [ebx + disp32]; mov [esp+20], eax
        c.push_back(0x8D); c.push_back(0x83);
        s.seed_fixup_offset = c.size();
        placeholder32(c);
        c.push_back(0x89); c.push_back(0x44); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_stored_seed));

        // initial_regs: mov [esp+24], esi
        c.push_back(0x89); c.push_back(0x74); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_initial_regs));

        // blob_size: mov dword [esp+28], imm32
        // C7 44 24 1C <imm32>
        c.push_back(0xC7); c.push_back(0x44); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_blob_size));
        s.size_fixup_offset = c.size();
        placeholder32(c);

        // ---- 6. Push arg0 = pointer to VmStubArgs (cdecl: on stack) ----
        // lea eax, [esp]; push eax — but push changes esp!
        // Actually: push esp works (pushes current esp value)
        // No — push esp pushes the value AFTER decrement on some x86.
        // Safe way: mov eax, esp; push eax
        c.push_back(0x89); c.push_back(0xE0);  // mov eax, esp
        c.push_back(0x50);                      // push eax

        // ---- 7. Indirect call: lea eax, [ebx + slot_disp]; call [eax] ----
        c.push_back(0x8D); c.push_back(0x83);
        s.call_slot_fixup_offset = c.size();
        placeholder32(c);
        c.push_back(0xFF); c.push_back(0x10);  // call [eax]

        // ---- 8. Clean up cdecl arg: add esp, 4 ----
        c.push_back(0x83); c.push_back(0xC4); c.push_back(0x04);

        // ---- 8b. Zero the 32-byte seed in payload (doc 16 E1) ----
        //   mov edi, [esp + off_stored_seed]  ; seed pointer from VmStubArgs
        //   xor eax, eax
        //   mov ecx, 32
        //   rep stosb
        c.push_back(0x8B); c.push_back(0x7C); c.push_back(0x24);
        c.push_back(static_cast<uint8_t>(Layout::off_stored_seed));
        c.push_back(0x31); c.push_back(0xC0);                     // xor eax, eax
        c.push_back(0xB9); c.push_back(32); c.push_back(0);
        c.push_back(0); c.push_back(0);                            // mov ecx, 32
        c.push_back(0xF3); c.push_back(0xAA);                     // rep stosb

        // ---- 9. Deallocate VmStubArgs: add esp, 64 ----
        c.push_back(0x83); c.push_back(0xC4);
        c.push_back(static_cast<uint8_t>(Layout::total_size));

        // ---- 10. Deallocate initial_regs: add esp, 32 ----
        c.push_back(0x83); c.push_back(0xC4); c.push_back(0x20);

        // ---- 11. Restore callee-saved (reverse): pop edi, esi, ebp, ebx ----
        for (int i = static_cast<int>(Traits::callee_saved.size()) - 1; i >= 0; --i) {
            auto reg = Traits::callee_saved[static_cast<size_t>(i)];
            if (reg < 8) c.push_back(0x58 + reg);
        }

        // ---- 12. JMP resume (E9 rel32 placeholder) ----
        c.push_back(0xE9);
        s.resume_fixup_offset = c.size();
        placeholder32(c);

        return s;
    }

    tl::expected<std::vector<uint8_t>, DC>
    emit_region_patch(uint64_t region_size, uint64_t from, uint64_t to) noexcept override {
        if (region_size < Traits::min_region_size)
            return tl::unexpected(DC::PatchRegionTooSmall);
        auto rel = static_cast<int32_t>(static_cast<int64_t>(to) - static_cast<int64_t>(from + 5));
        if (static_cast<int64_t>(to) - static_cast<int64_t>(from + 5) > INT32_MAX ||
            static_cast<int64_t>(to) - static_cast<int64_t>(from + 5) < INT32_MIN)
            return tl::unexpected(DC::PatchRegionTooSmall);
        std::vector<uint8_t> p(static_cast<size_t>(region_size), 0x90);
        p[0] = 0xE9;
        std::memcpy(p.data() + 1, &rel, 4);
        return p;
    }

    tl::expected<void, DC>
    fixup_ptr(std::vector<uint8_t>& code, std::size_t offset,
              uint64_t fixup_va, uint64_t target_va) noexcept override {
        if (offset + 4 > code.size())
            return tl::unexpected(DC::PatchStubGenerationFailed);
        // x86_32 EIP-relative: EIP = fixup_addr + 4 after reading disp32
        auto disp = static_cast<int64_t>(target_va - (fixup_va + 4));
        auto d = static_cast<int32_t>(disp);
        std::memcpy(code.data() + offset, &d, 4);
        return {};
    }

    tl::expected<void, DC>
    fixup_branch(std::vector<uint8_t>& code, std::size_t offset,
                 uint64_t fixup_va, uint64_t target_va) noexcept override {
        return fixup_ptr(code, offset, fixup_va, target_va);
    }

    void fixup_immediate(std::vector<uint8_t>& code, std::size_t offset,
                         uint64_t val) noexcept override {
        auto v = static_cast<uint32_t>(val);
        std::memcpy(code.data() + offset, &v, 4);
    }

    void fixup_static_va(std::vector<uint8_t>& code, std::size_t offset,
                         std::size_t /*size*/, uint64_t va) noexcept override {
        auto v = static_cast<uint32_t>(va);
        std::memcpy(code.data() + offset, &v, 4);
    }

    std::size_t min_region_size() const noexcept override { return Traits::min_region_size; }
    int64_t max_branch_distance() const noexcept override { return Traits::max_branch_dist; }
};

std::unique_ptr<StubEmitter> make_x86_32_emitter() {
    return std::make_unique<X86_32StubEmitter>();
}

}  // namespace VMPilot::Loader
