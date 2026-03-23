#ifndef __LOADER_FORMAT_PATCHER_HPP__
#define __LOADER_FORMAT_PATCHER_HPP__
#pragma once

#include <BinaryPatcher.hpp>
#include <BlobSerializer.hpp>
#include <PatcherTraits.hpp>
#include <StubGenerator.hpp>

#include <cstring>

namespace VMPilot::Loader {

/// Generic binary patcher parameterized by format-specific Traits.
///
/// The patching algorithm is identical for all formats:
///   1. Open binary  (Traits::open)
///   2. Serialize bytecodes into blob
///   3. Generate entry stubs per region
///   4. Fix up blob-pointer displacements
///   5. Add segment with payload  (Traits::add_segment)
///   6. Overwrite protected regions with JMP/B  (Traits::overwrite_text)
///   7. Save  (Traits::save)
///
/// Only the format-specific operations differ — those come from Traits.
template <typename Traits>
class FormatPatcher : public BinaryPatcher {
public:
    explicit FormatPatcher(std::string name) : name_(std::move(name)) {}

    [[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept override {

        // --- Validation ---
        if (request.input_path.empty() || request.output_path.empty()) {
            diag.error("loader", Common::DiagnosticCode::PatchInputInvalid,
                       "input or output path is empty");
            return tl::unexpected(Common::DiagnosticCode::PatchInputInvalid);
        }
        if (request.regions.empty() || request.compiled_outputs.empty()) {
            diag.error("loader", Common::DiagnosticCode::PatchInputInvalid,
                       "no regions or compiled outputs");
            return tl::unexpected(Common::DiagnosticCode::PatchInputInvalid);
        }

        // --- Open binary ---
        auto ctx_result = Traits::open(request.input_path, diag);
        if (!ctx_result) return tl::unexpected(ctx_result.error());
        auto& ctx = *ctx_result.value();

        // --- Get .text section info ---
        auto text_result = Traits::get_text_section(ctx, diag);
        if (!text_result) return tl::unexpected(text_result.error());

        // --- Serialize bytecodes into blob ---
        auto blob_result = BlobSerializer::serialize(request.compiled_outputs, diag);
        if (!blob_result) return tl::unexpected(blob_result.error());
        auto& blob_data = blob_result.value();

        // --- Generate entry stubs ---
        std::vector<Stub> stubs;
        stubs.reserve(request.regions.size());

        for (size_t i = 0; i < request.regions.size(); ++i) {
            auto stub_result = StubGenerator::generate_entry_stub(
                request.arch, request.mode,
                static_cast<uint32_t>(i), 0 /* patched below */);
            if (!stub_result) {
                diag.error("loader", Common::DiagnosticCode::PatchStubGenerationFailed,
                           "failed to generate entry stub for " + request.regions[i].name);
                return tl::unexpected(stub_result.error());
            }
            stubs.push_back(std::move(stub_result.value()));
        }

        // --- Compute payload layout: [blob] [stub_0] [stub_1] ... ---
        const size_t blob_size = blob_data.size();
        size_t payload_size = blob_size;
        std::vector<size_t> stub_offsets;
        stub_offsets.reserve(stubs.size());
        for (const auto& s : stubs) {
            stub_offsets.push_back(payload_size);
            payload_size += s.code.size();
        }

        // --- Fix up blob-pointer displacements ---
        for (size_t i = 0; i < stubs.size(); ++i) {
            auto& s = stubs[i];
            // blob is at offset 0, stub is at stub_offsets[i]
            // For x86-64 LEA [rip+disp32]: target = rip_after_insn + disp32
            //   rip_after_insn is at stub_offsets[i] + blob_fixup_offset + blob_insn_size
            //   target is at offset 0 (blob start)
            //   disp32 = 0 - (stub_offsets[i] + blob_fixup_offset + blob_insn_size)
            const int64_t blob_disp =
                -static_cast<int64_t>(stub_offsets[i] + s.blob_fixup_offset + s.blob_insn_size);

            if (request.arch == Common::FileArch::ARM64) {
                const uint32_t immlo = (static_cast<uint32_t>(blob_disp) & 0x3) << 29;
                const uint32_t immhi = (static_cast<uint32_t>(blob_disp >> 2) & 0x7FFFF) << 5;
                const uint32_t adr = 0x10000000 | immhi | immlo;
                s.code[s.blob_fixup_offset + 0] = static_cast<uint8_t>(adr);
                s.code[s.blob_fixup_offset + 1] = static_cast<uint8_t>(adr >> 8);
                s.code[s.blob_fixup_offset + 2] = static_cast<uint8_t>(adr >> 16);
                s.code[s.blob_fixup_offset + 3] = static_cast<uint8_t>(adr >> 24);
            } else {
                const auto d32 = static_cast<int32_t>(blob_disp);
                std::memcpy(s.code.data() + s.blob_fixup_offset, &d32, 4);
            }
        }

        // --- Assemble payload ---
        std::vector<uint8_t> payload(payload_size);
        std::memcpy(payload.data(), blob_data.data(), blob_size);
        for (size_t i = 0; i < stubs.size(); ++i) {
            std::memcpy(payload.data() + stub_offsets[i],
                        stubs[i].code.data(), stubs[i].code.size());
        }

        // --- Add segment to binary ---
        constexpr uint64_t PAGE_ALIGN = 0x1000;
        auto seg_result = Traits::add_segment(ctx, ".text.vm", payload, PAGE_ALIGN, diag);
        if (!seg_result) return tl::unexpected(seg_result.error());
        const uint64_t seg_va = seg_result.value().va;

        // --- Patch each protected region ---
        size_t patched = 0;
        for (size_t i = 0; i < request.regions.size(); ++i) {
            const auto& region = request.regions[i];
            const size_t min_sz = StubGenerator::min_region_size(request.arch, request.mode);
            if (region.size < min_sz) {
                diag.warn("loader", Common::DiagnosticCode::PatchRegionTooSmall,
                          "region '" + region.name + "' too small, skipping",
                          region.name, region.addr);
                continue;
            }

            const uint64_t stub_va = seg_va + stub_offsets[i];
            auto jmp_result = StubGenerator::generate_region_patch(
                request.arch, request.mode, region.size, region.addr, stub_va);
            if (!jmp_result) {
                diag.error("loader", Common::DiagnosticCode::PatchStubGenerationFailed,
                           "failed to generate patch for " + region.name,
                           region.name, region.addr);
                return tl::unexpected(jmp_result.error());
            }

            auto& jmp = jmp_result.value();
            auto ow = Traits::overwrite_text(ctx, region.addr, jmp.data(), jmp.size(), diag);
            if (!ow) return tl::unexpected(ow.error());

            ++patched;
            diag.note("loader", Common::DiagnosticCode::None,
                      "patched '" + region.name + "' -> stub at 0x" +
                      std::to_string(stub_va), region.name, region.addr);
        }

        // --- Save ---
        auto save_result = Traits::save(ctx, request.output_path, diag);
        if (!save_result) return tl::unexpected(save_result.error());

        return PatchResult{request.output_path, patched, blob_size};
    }

    [[nodiscard]] std::string name() const noexcept override { return name_; }

private:
    std::string name_;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_FORMAT_PATCHER_HPP__
