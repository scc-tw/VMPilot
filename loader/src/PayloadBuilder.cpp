#include <PayloadBuilder.hpp>

#include <cstring>

namespace VMPilot::Loader {

constexpr size_t CALL_SLOT_SIZE = 8;

tl::expected<PatchPayload, Common::DiagnosticCode>
build_payload(const std::vector<RegionPatchInfo>& regions,
              const std::vector<uint8_t>& blob_data,
              const std::array<uint8_t, SEED_SIZE>& stored_seed,
              uint64_t segment_va,
              StubEmitter& emitter,
              Common::DiagnosticCollector& diag) noexcept {
    using DC = Common::DiagnosticCode;

    if (regions.empty()) {
        diag.error("loader", DC::PatchInputInvalid, "no regions to patch");
        return tl::unexpected(DC::PatchInputInvalid);
    }
    if (blob_data.empty()) {
        diag.error("loader", DC::PatchInputInvalid, "empty blob data");
        return tl::unexpected(DC::PatchInputInvalid);
    }

    // --- Generate entry stubs (placeholders) ---
    std::vector<Stub> stubs;
    stubs.reserve(regions.size());
    for (size_t i = 0; i < regions.size(); ++i) {
        auto s = emitter.emit_entry_stub();
        if (!s) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "stub generation failed for '" + regions[i].name + "'");
            return tl::unexpected(s.error());
        }
        stubs.push_back(std::move(*s));
    }

    // --- Layout: [ blob | seed (32) | call_slot (8) | stubs... ] ---
    const size_t blob_size = blob_data.size();
    const size_t seed_offset = blob_size;
    const size_t call_slot_offset = seed_offset + SEED_SIZE;
    size_t payload_size = call_slot_offset + CALL_SLOT_SIZE;

    std::vector<RegionLayout> layouts;
    layouts.reserve(regions.size());
    for (size_t i = 0; i < stubs.size(); ++i) {
        layouts.push_back({regions[i].name, payload_size, stubs[i].code.size()});
        payload_size += stubs[i].code.size();
    }

    // --- Fix up all displacements (one pass, segment_va known) ---
    //
    // x86_64/x86_32: RIP/EIP = fixup_addr + 4 after reading disp32, so
    //   disp = target - (fixup_addr + 4) = target - fixup_addr - bias.
    // ARM64: PC = instruction_addr = fixup_addr, so bias = 0.
    const int64_t bias = emitter.pc_fixup_bias();

    for (size_t i = 0; i < stubs.size(); ++i) {
        auto& s = stubs[i];
        const auto stub_va = static_cast<int64_t>(segment_va + layouts[i].stub_offset);

        // blob_ptr displacement
        const int64_t blob_disp = static_cast<int64_t>(segment_va)
            - (stub_va + static_cast<int64_t>(s.blob_fixup_offset) + bias);
        if (auto fx = emitter.fixup_ptr_disp(s.code, s.blob_fixup_offset, blob_disp); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "blob displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // seed_ptr displacement
        const int64_t seed_disp = static_cast<int64_t>(segment_va + seed_offset)
            - (stub_va + static_cast<int64_t>(s.seed_fixup_offset) + bias);
        if (auto fx = emitter.fixup_ptr_disp(s.code, s.seed_fixup_offset, seed_disp); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "seed displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // call_slot displacement
        const int64_t slot_disp = static_cast<int64_t>(segment_va + call_slot_offset)
            - (stub_va + static_cast<int64_t>(s.call_slot_fixup_offset) + bias);
        if (auto fx = emitter.fixup_ptr_disp(s.code, s.call_slot_fixup_offset, slot_disp); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "call slot displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // blob_size immediate (not PC-relative — no bias)
        emitter.fixup_immediate(s.code, s.size_fixup_offset, blob_size);

        // resume: jump to region.addr + region.size
        const int64_t resume_disp = static_cast<int64_t>(regions[i].addr + regions[i].size)
            - (stub_va + static_cast<int64_t>(s.resume_fixup_offset) + bias);
        if (auto fx = emitter.fixup_branch_disp(s.code, s.resume_fixup_offset, resume_disp); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "resume displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // ASLR delta static VA (absolute — no bias)
        if (s.delta_fixup_size > 0) {
            emitter.fixup_static_va(s.code, s.delta_fixup_offset, s.delta_fixup_size,
                                    segment_va + layouts[i].stub_offset + s.delta_ref_offset);
        }
    }

    // --- Assemble ---
    std::vector<uint8_t> payload(payload_size, 0);
    std::memcpy(payload.data(), blob_data.data(), blob_size);
    std::memcpy(payload.data() + seed_offset, stored_seed.data(), SEED_SIZE);
    // call_slot = 0 initially (runtime fills it)
    for (size_t i = 0; i < stubs.size(); ++i) {
        std::memcpy(payload.data() + layouts[i].stub_offset,
                    stubs[i].code.data(), stubs[i].code.size());
    }

    return PatchPayload{std::move(payload), std::move(layouts),
                        blob_size, seed_offset, call_slot_offset};
}

}  // namespace VMPilot::Loader
