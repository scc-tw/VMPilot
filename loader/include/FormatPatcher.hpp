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
template <typename Editor>
class FormatPatcher : public BinaryPatcher {
public:
    explicit FormatPatcher(std::string fmt_name) : name_(std::move(fmt_name)) {}

    [[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept override {
        using DC = Common::DiagnosticCode;

        // --- 1. Build payload (blob/seed/size fixups done, resume is placeholder) ---
        auto payload = build_payload(request.regions, request.blob_data,
                                     request.stored_seed,
                                     request.arch, request.mode, diag);
        if (!payload) return tl::unexpected(payload.error());

        // --- 2. Open binary ---
        auto editor = Editor::open(request.input_path, diag);
        if (!editor) return tl::unexpected(editor.error());

        // --- 3. Validate regions within .text ---
        auto text = editor->text_section();
        for (const auto& r : request.regions) {
            if (r.addr < text.base_addr ||
                r.addr + r.size > text.base_addr + text.size) {
                diag.error("loader", DC::PatchInputInvalid,
                           "region '" + r.name + "' outside .text section");
                return tl::unexpected(DC::PatchInputInvalid);
            }
        }

        // --- 4. Add segment (gets the VA) ---
        constexpr uint64_t PAGE_ALIGN = 0x1000;
        auto seg = editor->add_segment(".text.vm", payload->data, PAGE_ALIGN, diag);
        if (!seg) return tl::unexpected(seg.error());
        const uint64_t seg_va = seg->va;

        // --- 5. Patch resume displacements into the injected segment ---
        // Now we know seg_va, so we can compute each stub's resume target.
        // We patch the segment bytes in-place via overwrite on the new segment.
        // But the new segment isn't accessible through overwrite_text (which
        // only writes to .text). Instead, we re-compute and write via the
        // editor's raw segment data if possible.
        //
        // Simpler approach: patch payload->data, then overwrite the segment.
        // But add_segment already copied the data...
        //
        // Most practical: compute resume fixups and write them into the
        // segment via byte-level overwrites. For ELFIO the section data is
        // mutable after add. For Mach-O the buffer is mutable.
        //
        // Actually, the cleanest approach: compute resume fixups NOW and
        // write them back using overwrite_text's "cousin" on the new segment.
        // Both ELF and Mach-O editors can overwrite any VA after add_segment.
        // Let's extend overwrite_text to work on any writable section.
        //
        // For now: use a dedicated overwrite call on the __VMPILOT segment.
        for (size_t i = 0; i < request.regions.size(); ++i) {
            const auto& region = request.regions[i];
            const auto& layout = payload->layouts[i];

            const uint64_t resume_target = region.addr + region.size;
            const uint64_t resume_insn_va =
                seg_va + layout.resume_fixup_payload_offset;
            const int64_t resume_disp = static_cast<int64_t>(resume_target) -
                static_cast<int64_t>(resume_insn_va + layout.resume_insn_size);

            // Patch the resume displacement into the payload data
            // (we still have a mutable copy)
            if (request.arch == Common::FileArch::ARM64) {
                const uint32_t insn = 0x14000000u |
                    (static_cast<uint32_t>(resume_disp >> 2) & 0x03FFFFFF);
                const auto off = layout.resume_fixup_payload_offset;
                payload->data[off+0] = static_cast<uint8_t>(insn);
                payload->data[off+1] = static_cast<uint8_t>(insn >> 8);
                payload->data[off+2] = static_cast<uint8_t>(insn >> 16);
                payload->data[off+3] = static_cast<uint8_t>(insn >> 24);
            } else {
                const auto d32 = static_cast<int32_t>(resume_disp);
                std::memcpy(payload->data.data() + layout.resume_fixup_payload_offset,
                            &d32, 4);
            }

            // Overwrite the bytes in the already-injected segment
            const uint64_t fixup_va = seg_va + layout.resume_fixup_payload_offset;
            const uint8_t* fixup_data =
                payload->data.data() + layout.resume_fixup_payload_offset;
            auto ow = editor->overwrite_segment(fixup_va, fixup_data, 4, diag);
            if (!ow) return tl::unexpected(ow.error());
        }

        // --- 5b. Patch load_base_delta static VAs (Phase 2: PIE/ASLR) ---
        for (size_t i = 0; i < request.regions.size(); ++i) {
            const auto& layout = payload->layouts[i];
            if (layout.delta_fixup_size == 0) continue;

            // static VA of the delta reference point =
            //   seg_va + stub_offset + delta_ref_stub_offset
            const uint64_t static_va =
                seg_va + layout.stub_offset + layout.delta_ref_stub_offset;

            // Build a temporary Stub to use fixup_delta_static_va.
            // We operate directly on payload->data at the fixup offset.
            // Copy the fixup region, patch it, and write it back.
            const auto off = layout.delta_fixup_payload_offset;
            const auto sz = layout.delta_fixup_size;
            if (off + sz > payload->data.size()) continue;

            // Create a minimal Stub wrapper over the fixup bytes
            Stub tmp;
            tmp.code.assign(payload->data.begin() + static_cast<ptrdiff_t>(off),
                            payload->data.begin() + static_cast<ptrdiff_t>(off + sz));
            tmp.delta_static_va_fixup_offset = 0;  // offset within tmp.code
            // Temporarily set to a non-zero sentinel so fixup doesn't bail
            // (the real implementation checks off == 0 as "not set")
            // Actually we need off != 0 — but our offset IS 0 in the tmp.
            // So instead, patch directly in payload->data.

            // Direct patch: for ARM64, write MOVZ/MOVK sequence;
            // for x86-64, write raw imm64.
            if (request.arch == Common::FileArch::ARM64 && sz == 16) {
                // ARM64: 4 instructions at payload->data[off..off+16)
                auto write_insn = [&](size_t insn_off, uint32_t insn) {
                    payload->data[insn_off+0] = static_cast<uint8_t>(insn);
                    payload->data[insn_off+1] = static_cast<uint8_t>(insn >> 8);
                    payload->data[insn_off+2] = static_cast<uint8_t>(insn >> 16);
                    payload->data[insn_off+3] = static_cast<uint8_t>(insn >> 24);
                };
                // Preserve rd from existing MOVZ instruction
                const uint8_t rd = payload->data[off] & 0x1F;
                write_insn(off,      0xD2800000 | (static_cast<uint32_t>(
                    static_cast<uint16_t>(static_va)) << 5) | rd);
                write_insn(off + 4,  0xF2A00000 | (static_cast<uint32_t>(
                    static_cast<uint16_t>(static_va >> 16)) << 5) | rd);
                write_insn(off + 8,  0xF2C00000 | (static_cast<uint32_t>(
                    static_cast<uint16_t>(static_va >> 32)) << 5) | rd);
                write_insn(off + 12, 0xF2E00000 | (static_cast<uint32_t>(
                    static_cast<uint16_t>(static_va >> 48)) << 5) | rd);
            } else if (sz == 8) {
                // x86-64: raw 8-byte immediate
                std::memcpy(payload->data.data() + off, &static_va, 8);
            }

            // Overwrite in already-injected segment
            const uint64_t fixup_va = seg_va + off;
            auto ow = editor->overwrite_segment(
                fixup_va, payload->data.data() + off, sz, diag);
            if (!ow) return tl::unexpected(ow.error());
        }

        // --- 6. Overwrite protected regions with JMP/B ---
        size_t patched = 0;
        for (size_t i = 0; i < request.regions.size(); ++i) {
            const auto& region = request.regions[i];
            const size_t min_sz = StubGenerator::min_region_size(
                request.arch, request.mode);
            if (region.size < min_sz) {
                diag.warn("loader", DC::PatchRegionTooSmall,
                          "'" + region.name + "' too small, skipping",
                          region.name, region.addr);
                continue;
            }

            const uint64_t stub_va = seg_va + payload->layouts[i].stub_offset;
            const int64_t dist = static_cast<int64_t>(stub_va) -
                                 static_cast<int64_t>(region.addr);
            if (std::abs(dist) > StubGenerator::max_branch_distance(request.arch)) {
                diag.error("loader", DC::PatchStubGenerationFailed,
                           "branch distance exceeds range for '" + region.name + "'");
                return tl::unexpected(DC::PatchStubGenerationFailed);
            }

            auto jmp = StubGenerator::generate_region_patch(
                request.arch, request.mode, region.size, region.addr, stub_va);
            if (!jmp) return tl::unexpected(jmp.error());

            auto ow = editor->overwrite_text(
                region.addr, jmp->data(), jmp->size(), diag);
            if (!ow) return tl::unexpected(ow.error());

            ++patched;
        }

        // --- 7. Save ---
        auto sv = editor->save(request.output_path, diag);
        if (!sv) return tl::unexpected(sv.error());

        diag.note("loader", DC::None,
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
