#include <BinaryEditor.hpp>
#include <PayloadBuilder.hpp>
#include <PlatformTraits.hpp>
#include <StubEmitter.hpp>
#include <diagnostic_collector.hpp>

#include <cmath>
#include <string>

namespace VMPilot::Loader {

tl::expected<PatchResult, Common::DiagnosticCode>
patch(const PatchRequest& request,
      Common::DiagnosticCollector& diag) noexcept {
    using DC = Common::DiagnosticCode;
    auto cfg = FormatConfig::for_format(request.format);

    // --- 1. Open binary ---
    auto editor_result = open_binary(request.input_path, request.format, diag);
    if (!editor_result) return tl::unexpected(editor_result.error());
    auto& editor = *editor_result.value();

    // --- 2. Create arch-specific emitter ---
    auto emitter = create_emitter(request.arch, request.mode);
    if (!emitter) {
        diag.error("loader", DC::PatchArchUnsupported,
                   "unsupported architecture for stub generation");
        return tl::unexpected(DC::PatchArchUnsupported);
    }

    // --- 3. Validate regions within .text ---
    auto text = editor.text_section();
    for (const auto& r : request.regions) {
        if (r.addr < text.base_addr ||
            r.addr + r.size > text.base_addr + text.size) {
            diag.error("loader", DC::PatchInputInvalid,
                       "region '" + r.name + "' outside .text section");
            return tl::unexpected(DC::PatchInputInvalid);
        }
    }

    // --- 4. Query segment VA (read-only, no mutation) ---
    const uint64_t seg_va = editor.next_segment_va(cfg.page_align);

    // --- 5. Build payload (ALL fixups resolved in one pass) ---
    auto payload = build_payload(request.regions, request.blob_data,
                                  request.stored_seed, seg_va,
                                  *emitter, diag);
    if (!payload) return tl::unexpected(payload.error());

    // --- 6. Inject segment (final data, one call) ---
    auto seg = editor.add_segment(cfg.section_name, payload->data,
                                   cfg.page_align, diag);
    if (!seg) return tl::unexpected(seg.error());

    // --- 7. Overwrite protected regions with JMP/B ---
    size_t patched = 0;
    for (size_t i = 0; i < request.regions.size(); ++i) {
        const auto& region = request.regions[i];
        if (region.size < emitter->min_region_size()) {
            diag.warn("loader", DC::PatchRegionTooSmall,
                      "'" + region.name + "' too small ("
                      + std::to_string(region.size) + "B) — Segmentator bug?",
                      region.name, region.addr);
            continue;
        }

        const uint64_t stub_va = seg_va + payload->layouts[i].stub_offset;
        const int64_t dist = static_cast<int64_t>(stub_va) -
                             static_cast<int64_t>(region.addr);
        if (std::abs(dist) > emitter->max_branch_distance()) {
            diag.error("loader", DC::PatchStubGenerationFailed,
                       "branch distance exceeds range for '" + region.name + "'");
            return tl::unexpected(DC::PatchStubGenerationFailed);
        }

        auto jmp = emitter->emit_region_patch(region.size, region.addr, stub_va);
        if (!jmp) return tl::unexpected(jmp.error());

        auto ow = editor.overwrite_text(region.addr, jmp->data(), jmp->size(), diag);
        if (!ow) return tl::unexpected(ow.error());
        ++patched;
    }

    // --- 8. Add runtime library dependency ---
    auto dep = editor.add_runtime_dep(cfg.runtime_lib, diag);
    if (!dep) return tl::unexpected(dep.error());

    // --- 9. Invalidate old code signature ---
    editor.invalidate_signature();

    // --- 10. Save ---
    auto sv = editor.save(request.output_path, diag);
    if (!sv) return tl::unexpected(sv.error());

    diag.note("loader", DC::None,
              "patched " + std::to_string(patched) + " region(s), "
              + std::to_string(payload->blob_size) + " bytes injected");

    return PatchResult{request.output_path, patched, payload->blob_size};
}

}  // namespace VMPilot::Loader
