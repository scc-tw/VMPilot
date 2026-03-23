#include <PayloadBuilder.hpp>
#include <BlobSerializer.hpp>

#include <cstring>
#include <unordered_map>

namespace VMPilot::Loader {

tl::expected<PatchPayload, Common::DiagnosticCode>
build_payload(const std::vector<RegionPatchInfo>& regions,
              const std::vector<SDK::BytecodeCompiler::CompilationOutput>& compiled_outputs,
              Common::FileArch arch, Common::FileMode mode,
              Common::DiagnosticCollector& diag) noexcept {
    using DC = Common::DiagnosticCode;

    if (regions.empty())
        return tl::unexpected(DC::PatchInputInvalid);

    // --- Match regions to compiled outputs by name ---
    std::unordered_map<std::string, size_t> output_idx;
    for (size_t i = 0; i < compiled_outputs.size(); ++i)
        output_idx[compiled_outputs[i].name] = i;

    for (const auto& r : regions) {
        if (output_idx.find(r.name) == output_idx.end()) {
            diag.error("loader", DC::PatchInputInvalid,
                       "region '" + r.name + "' has no matching compiled output");
            return tl::unexpected(DC::PatchInputInvalid);
        }
    }

    // --- Serialize bytecodes → blob ---
    auto blob_result = BlobSerializer::serialize(compiled_outputs, diag);
    if (!blob_result) return tl::unexpected(blob_result.error());
    const auto& blob_data = blob_result.value();
    const size_t blob_size = blob_data.size();

    // --- Generate entry stubs (with placeholder blob offset) ---
    std::vector<Stub> stubs;
    stubs.reserve(regions.size());
    for (size_t i = 0; i < regions.size(); ++i) {
        auto stub_result = StubGenerator::generate_entry_stub(
            arch, mode, static_cast<uint32_t>(i));
        if (!stub_result) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "entry stub generation failed for '" + regions[i].name + "'");
            return tl::unexpected(stub_result.error());
        }
        stubs.push_back(std::move(stub_result.value()));
    }

    // --- Compute layout: [blob | stub_0 | stub_1 | ...] ---
    std::vector<RegionLayout> layouts;
    layouts.reserve(regions.size());
    size_t payload_size = blob_size;

    for (size_t i = 0; i < stubs.size(); ++i) {
        layouts.push_back({regions[i].name, payload_size, stubs[i].code.size()});
        payload_size += stubs[i].code.size();
    }

    // --- Fix up blob-pointer displacements ---
    // Blob starts at payload offset 0.
    // Stub i is at layouts[i].stub_offset.
    // The fixup instruction PC (after instruction) is at:
    //   layouts[i].stub_offset + stub.blob_fixup_offset + stub.blob_insn_size
    // Displacement = 0 - PC = -(layouts[i].stub_offset + blob_fixup_offset + blob_insn_size)
    for (size_t i = 0; i < stubs.size(); ++i) {
        const auto pc = static_cast<int64_t>(
            layouts[i].stub_offset + stubs[i].blob_fixup_offset + stubs[i].blob_insn_size);
        const int64_t disp = -pc;

        auto fixup = StubGenerator::fixup_blob_displacement(stubs[i], disp, arch);
        if (!fixup) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "blob displacement out of range for '" + regions[i].name + "'");
            return tl::unexpected(fixup.error());
        }
    }

    // --- Assemble payload bytes ---
    std::vector<uint8_t> payload(payload_size);
    std::memcpy(payload.data(), blob_data.data(), blob_size);
    for (size_t i = 0; i < stubs.size(); ++i) {
        std::memcpy(payload.data() + layouts[i].stub_offset,
                    stubs[i].code.data(), stubs[i].code.size());
    }

    return PatchPayload{std::move(payload), std::move(layouts), blob_size};
}

}  // namespace VMPilot::Loader
