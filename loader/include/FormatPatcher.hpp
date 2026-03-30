#ifndef __LOADER_FORMAT_PATCHER_HPP__
#define __LOADER_FORMAT_PATCHER_HPP__
#pragma once

#include <BinaryPatcher.hpp>
#include <PayloadBuilder.hpp>
#include <StubGenerator.hpp>

#include <cstring>
#include <string>

namespace VMPilot::Loader {

/// Generic binary patcher parameterized by a concrete Editor type.
///
/// Editor must satisfy:
///   static expected<Editor, DC> open(path, diag)
///   TextSectionInfo text_section() const
///   expected<void, DC> overwrite_text(va, data, len, diag)
///   expected<NewSegmentInfo, DC> add_segment(name, payload, align, diag)
///   expected<void, DC> save(path, diag)
///
/// All complex logic lives in PayloadBuilder (pure) and StubGenerator.
/// FormatPatcher only orchestrates I/O.
template <typename Editor>
class FormatPatcher : public BinaryPatcher {
public:
    explicit FormatPatcher(std::string fmt_name) : name_(std::move(fmt_name)) {}

    [[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept override {
        using DC = Common::DiagnosticCode;

        // --- 1. Build payload (pure, no I/O) ---
        auto payload = build_payload(request.regions, request.blob_data,
                                     request.stored_seed,
                                     request.arch, request.mode, diag);
        if (!payload) return tl::unexpected(payload.error());

        // --- 2. Open binary ---
        auto editor = Editor::open(request.input_path, diag);
        if (!editor) return tl::unexpected(editor.error());

        // --- 3. Validate regions are within .text ---
        auto text = editor->text_section();
        for (const auto& r : request.regions) {
            if (r.addr < text.base_addr ||
                r.addr + r.size > text.base_addr + text.size) {
                diag.error("loader", DC::PatchInputInvalid,
                           "region '" + r.name + "' outside .text section");
                return tl::unexpected(DC::PatchInputInvalid);
            }
        }

        // --- 4. Add segment ---
        constexpr uint64_t PAGE_ALIGN = 0x1000;
        auto seg = editor->add_segment(".text.vm", payload->data, PAGE_ALIGN, diag);
        if (!seg) return tl::unexpected(seg.error());
        const uint64_t seg_va = seg->va;

        // --- 5. Overwrite protected regions with JMP/B ---
        size_t patched = 0;
        for (size_t i = 0; i < request.regions.size(); ++i) {
            const auto& region = request.regions[i];
            const size_t min_sz = StubGenerator::min_region_size(request.arch, request.mode);
            if (region.size < min_sz) {
                diag.warn("loader", DC::PatchRegionTooSmall,
                          "'" + region.name + "' too small (" +
                          std::to_string(region.size) + "B), skipping",
                          region.name, region.addr);
                continue;
            }

            const uint64_t stub_va = seg_va + payload->layouts[i].stub_offset;

            // Validate branch range
            const int64_t dist = static_cast<int64_t>(stub_va) -
                                 static_cast<int64_t>(region.addr);
            const int64_t max_dist = StubGenerator::max_branch_distance(request.arch);
            if (std::abs(dist) > max_dist) {
                diag.error("loader", DC::PatchStubGenerationFailed,
                           "branch distance exceeds range for '" + region.name + "'");
                return tl::unexpected(DC::PatchStubGenerationFailed);
            }

            auto jmp = StubGenerator::generate_region_patch(
                request.arch, request.mode, region.size, region.addr, stub_va);
            if (!jmp) return tl::unexpected(jmp.error());

            auto ow = editor->overwrite_text(region.addr, jmp->data(), jmp->size(), diag);
            if (!ow) return tl::unexpected(ow.error());

            ++patched;
        }

        // --- 6. Save ---
        auto sv = editor->save(request.output_path, diag);
        if (!sv) return tl::unexpected(sv.error());

        diag.note("loader", Common::DiagnosticCode::None,
                  "patched " + std::to_string(patched) + " region(s), " +
                  std::to_string(payload->blob_size) + " bytes injected");

        return PatchResult{request.output_path, patched, payload->blob_size};
    }

    [[nodiscard]] std::string name() const noexcept override { return name_; }

private:
    std::string name_;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_FORMAT_PATCHER_HPP__
