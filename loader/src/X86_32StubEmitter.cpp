#include <StubEmitter.hpp>
#include <ArchTraits.hpp>

#include <cstring>
#include <vector>

namespace VMPilot::Loader {

using Traits = ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_32>;
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

        // 1. Save callee-saved: push ebx, ebp, esi, edi
        for (auto reg : Traits::callee_saved) {
            if (reg < 8) c.push_back(0x50 + reg); // push reg
        }

        // 2. ASLR delta: call/pop to get EIP (PIC trick)
        // call .Lbase (E8 00 00 00 00)
        c.push_back(0xE8); emit32(c, 0);
        // pop ebx (5B) — ebx = EIP of pop instruction
        c.push_back(0x5B);
        s.delta_ref_offset = c.size() - 1; // address of pop = reference point

        // mov eax, <static_va>; sub ebx, eax; (ebx = delta)
        c.push_back(0xB8); // mov eax, imm32
        s.delta_fixup_offset = c.size();
        s.delta_fixup_size = 4;
        placeholder32(c);
        // sub ebx, eax
        c.push_back(0x29); c.push_back(0xC3);
        // Now ebx = load_base_delta. Save it.
        c.push_back(0x53); // push ebx (delta on stack)

        // 3. Build initial_regs[8] on stack
        // sub esp, 32 (8 regs * 4 bytes)
        c.push_back(0x83); c.push_back(0xEC); c.push_back(0x20);
        // mov [esp+i*4], reg for eax(0)..edi(7)
        for (int i = 0; i < 8; ++i) {
            // mov [esp+disp8], reg
            c.push_back(0x89);
            c.push_back(0x44 | ((i & 7) << 3)); // ModRM: [esp+disp8], reg
            c.push_back(0x24); // SIB: base=esp
            c.push_back(static_cast<uint8_t>(i * 4));
        }

        // 4. Push args for cdecl (right to left):
        //    config(nullptr), load_base_delta, num_regs(8),
        //    initial_regs, seed, size, blob

        // push 0 (config = nullptr)
        c.push_back(0x6A); c.push_back(0x00);

        // push ebx (load_base_delta — still in ebx)
        c.push_back(0x53);

        // push 8 (num_regs)
        c.push_back(0x6A); c.push_back(0x08);

        // lea eax, [esp+12]; push eax (initial_regs ptr)
        // After 3 pushes above, esp moved by 12. Array is at esp+12.
        c.push_back(0x8D); c.push_back(0x44); c.push_back(0x24); c.push_back(0x0C);
        c.push_back(0x50); // push eax

        // lea eax, [ebx + disp32]; push eax (seed_ptr)
        c.push_back(0x8D); c.push_back(0x83); // lea eax, [ebx + disp32]
        s.seed_fixup_offset = c.size();
        placeholder32(c);
        c.push_back(0x50); // push eax

        // push blob_size (imm32)
        c.push_back(0x68);
        s.size_fixup_offset = c.size();
        placeholder32(c);

        // lea eax, [ebx + disp32]; push eax (blob_ptr)
        c.push_back(0x8D); c.push_back(0x83);
        s.blob_fixup_offset = c.size();
        placeholder32(c);
        c.push_back(0x50); // push eax

        // 5. Indirect call: lea eax, [ebx + disp32]; call [eax]
        c.push_back(0x8D); c.push_back(0x83);
        s.call_slot_fixup_offset = c.size();
        placeholder32(c);
        c.push_back(0xFF); c.push_back(0x10); // call [eax]

        // 6. Clean up: add esp, 28 (7 pushed args * 4)
        c.push_back(0x83); c.push_back(0xC4); c.push_back(0x1C);

        // 7. Clean up initial_regs: add esp, 32
        c.push_back(0x83); c.push_back(0xC4); c.push_back(0x20);

        // 8. Pop delta: add esp, 4
        c.push_back(0x83); c.push_back(0xC4); c.push_back(0x04);

        // 9. Restore callee-saved (reverse order): pop edi, esi, ebp, ebx
        for (int i = static_cast<int>(Traits::callee_saved.size()) - 1; i >= 0; --i) {
            auto reg = Traits::callee_saved[static_cast<size_t>(i)];
            if (reg < 8) c.push_back(0x58 + reg); // pop reg
        }

        // 10. jmp resume (E9 rel32)
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
    fixup_ptr_disp(std::vector<uint8_t>& code, size_t offset, int64_t disp) noexcept override {
        if (offset + 4 > code.size())
            return tl::unexpected(DC::PatchStubGenerationFailed);
        auto d = static_cast<int32_t>(disp);
        std::memcpy(code.data() + offset, &d, 4);
        return {};
    }

    tl::expected<void, DC>
    fixup_branch_disp(std::vector<uint8_t>& code, size_t offset, int64_t disp) noexcept override {
        return fixup_ptr_disp(code, offset, disp);
    }

    void fixup_immediate(std::vector<uint8_t>& code, size_t offset, uint64_t val) noexcept override {
        auto v = static_cast<uint32_t>(val);
        std::memcpy(code.data() + offset, &v, 4);
    }

    void fixup_static_va(std::vector<uint8_t>& code, size_t offset,
                         size_t /*size*/, uint64_t va) noexcept override {
        auto v = static_cast<uint32_t>(va);
        std::memcpy(code.data() + offset, &v, 4);
    }

    size_t min_region_size() const noexcept override { return Traits::min_region_size; }
    int64_t max_branch_distance() const noexcept override { return Traits::max_branch_dist; }
    int64_t pc_fixup_bias() const noexcept override { return 4; }
};

std::unique_ptr<StubEmitter> make_x86_32_emitter() {
    return std::make_unique<X86_32StubEmitter>();
}

}  // namespace VMPilot::Loader
