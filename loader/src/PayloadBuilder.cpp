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

    // --- Layout: [ call_slot (8) | blob (N) | seed (32) | stubs... ] ---
    //
    // call_slot at offset 0: the runtime init constructor writes
    // &vm_stub_entry to the first 8 bytes of the section — no need to
    // know blob_size or any payload internal.
    const size_t call_slot_offset = 0;
    const size_t blob_offset      = CALL_SLOT_SIZE;
    const size_t blob_size        = blob_data.size();
    const size_t seed_offset      = blob_offset + blob_size;
    size_t payload_size           = seed_offset + SEED_SIZE;

    std::vector<RegionLayout> layouts;
    layouts.reserve(regions.size());
    for (size_t i = 0; i < stubs.size(); ++i) {
        layouts.push_back({regions[i].name, payload_size, stubs[i].code.size()});
        payload_size += stubs[i].code.size();
    }

    // --- Fix up all displacements (one pass, segment_va known) ---
    //
    // PayloadBuilder provides (fixup_va, target_va) pairs only.
    // Each emitter internally computes the correct encoded displacement
    // for its architecture (x86: target - fixup - 4; ARM64: target - fixup).
    // No bias, no arch knowledge here.

    const uint64_t blob_va = segment_va + blob_offset;
    const uint64_t seed_va = segment_va + seed_offset;
    const uint64_t slot_va = segment_va + call_slot_offset;

    for (size_t i = 0; i < stubs.size(); ++i) {
        auto& s = stubs[i];
        const uint64_t stub_va = segment_va + layouts[i].stub_offset;

        // blob_ptr
        if (auto fx = emitter.fixup_ptr(s.code, s.blob_fixup_offset,
                stub_va + s.blob_fixup_offset, blob_va); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "blob ptr fixup failed for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // seed_ptr
        if (auto fx = emitter.fixup_ptr(s.code, s.seed_fixup_offset,
                stub_va + s.seed_fixup_offset, seed_va); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "seed ptr fixup failed for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // call_slot ptr
        if (auto fx = emitter.fixup_ptr(s.code, s.call_slot_fixup_offset,
                stub_va + s.call_slot_fixup_offset, slot_va); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "call slot fixup failed for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // blob_size immediate (not PC-relative)
        emitter.fixup_immediate(s.code, s.size_fixup_offset, blob_size);

        // resume: branch to region.addr + region.size
        if (auto fx = emitter.fixup_branch(s.code, s.resume_fixup_offset,
                stub_va + s.resume_fixup_offset,
                regions[i].addr + regions[i].size); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "resume fixup failed for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // ASLR delta static VA (absolute, not PC-relative)
        if (s.delta_fixup_size > 0) {
            emitter.fixup_static_va(s.code, s.delta_fixup_offset,
                                    s.delta_fixup_size,
                                    stub_va + s.delta_ref_offset);
        }
    }

    // --- Assemble ---
    std::vector<uint8_t> payload(payload_size, 0);
    // call_slot at offset 0 — zero-initialized (runtime constructor fills it)
    std::memcpy(payload.data() + blob_offset, blob_data.data(), blob_size);
    std::memcpy(payload.data() + seed_offset, stored_seed.data(), SEED_SIZE);
    for (size_t i = 0; i < stubs.size(); ++i) {
        std::memcpy(payload.data() + layouts[i].stub_offset,
                    stubs[i].code.data(), stubs[i].code.size());
    }

    return PatchPayload{std::move(payload), std::move(layouts),
                        blob_size, seed_offset, call_slot_offset};
}

}  // namespace VMPilot::Loader
