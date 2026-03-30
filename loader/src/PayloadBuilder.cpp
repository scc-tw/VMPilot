#include <PayloadBuilder.hpp>

#include <cstring>

namespace VMPilot::Loader {

tl::expected<PatchPayload, Common::DiagnosticCode>
build_payload(const std::vector<RegionPatchInfo>& regions,
              const std::vector<uint8_t>& blob_data,
              const std::array<uint8_t, SEED_SIZE>& stored_seed,
              Common::FileArch arch, Common::FileMode mode,
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

    // --- Generate entry stubs (placeholders, fixed up below) ---
    std::vector<Stub> stubs;
    stubs.reserve(regions.size());
    for (size_t i = 0; i < regions.size(); ++i) {
        auto s = StubGenerator::generate_entry_stub(arch, mode,
                     static_cast<uint32_t>(i));
        if (!s) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "stub generation failed for '" + regions[i].name + "'");
            return tl::unexpected(s.error());
        }
        stubs.push_back(std::move(*s));
    }

    // --- Layout: [ blob (N) | seed (32) | stub_0 | stub_1 | ... ] ---
    const size_t blob_size = blob_data.size();
    const size_t seed_offset = blob_size;
    size_t payload_size = blob_size + SEED_SIZE;

    std::vector<RegionLayout> layouts;
    layouts.reserve(regions.size());
    for (size_t i = 0; i < stubs.size(); ++i) {
        RegionLayout rl;
        rl.name = regions[i].name;
        rl.stub_offset = payload_size;
        rl.stub_size = stubs[i].code.size();
        // Resume fixup: payload_offset = stub_offset + resume_fixup within stub
        rl.resume_fixup_payload_offset = payload_size + stubs[i].resume_fixup_offset;
        rl.resume_insn_size = stubs[i].resume_insn_size;
        layouts.push_back(std::move(rl));
        payload_size += stubs[i].code.size();
    }

    // --- Fix up all displacements ---
    for (size_t i = 0; i < stubs.size(); ++i) {
        auto& s = stubs[i];
        const auto stub_base = static_cast<int64_t>(layouts[i].stub_offset);

        // blob_ptr: blob is at offset 0
        const int64_t blob_pc = stub_base +
            static_cast<int64_t>(s.blob_fixup_offset + s.blob_insn_size);
        if (auto fx = StubGenerator::fixup_blob_displacement(s, -blob_pc, arch); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "blob displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // seed_ptr: seed is at offset seed_offset
        const int64_t seed_pc = stub_base +
            static_cast<int64_t>(s.seed_fixup_offset + s.seed_insn_size);
        const int64_t seed_disp = static_cast<int64_t>(seed_offset) - seed_pc;
        if (auto fx = StubGenerator::fixup_seed_displacement(s, seed_disp, arch); !fx) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "seed displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fx.error());
        }

        // blob_size
        StubGenerator::fixup_blob_size(s, blob_size);
    }

    // --- Assemble ---
    std::vector<uint8_t> payload(payload_size);
    std::memcpy(payload.data(), blob_data.data(), blob_size);
    std::memcpy(payload.data() + seed_offset, stored_seed.data(), SEED_SIZE);
    for (size_t i = 0; i < stubs.size(); ++i) {
        std::memcpy(payload.data() + layouts[i].stub_offset,
                    stubs[i].code.data(), stubs[i].code.size());
    }

    return PatchPayload{std::move(payload), std::move(layouts), blob_size, seed_offset};
}

}  // namespace VMPilot::Loader
