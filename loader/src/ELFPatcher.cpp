#include <ELFPatcher.hpp>
#include <BlobSerializer.hpp>
#include <StubGenerator.hpp>

#include <elfio/elfio.hpp>

#include <algorithm>
#include <cstring>

// Pull ELF constants into scope
using ELFIO::PT_LOAD;
using ELFIO::SHT_PROGBITS;
using ELFIO::SHF_ALLOC;
using ELFIO::SHF_EXECINSTR;
using ELFIO::PF_R;
using ELFIO::PF_X;

namespace VMPilot::Loader {

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------

/// Find the .text section in an ELFIO reader.
static ELFIO::section* find_text_section(ELFIO::elfio& reader) {
    for (auto& sec : reader.sections) {
        if (sec->get_name() == ".text")
            return sec.get();
    }
    return nullptr;
}

/// Compute the next page-aligned VA after all existing loadable segments.
static uint64_t next_segment_va(const ELFIO::elfio& reader, uint64_t page_size) {
    uint64_t highest = 0;
    for (const auto& seg : reader.segments) {
        if (seg->get_type() == PT_LOAD) {
            const uint64_t end = seg->get_virtual_address() + seg->get_memory_size();
            if (end > highest) highest = end;
        }
    }
    // Align up to next page
    return (highest + page_size - 1) & ~(page_size - 1);
}

/// Overwrite bytes in a section at a given VA.
/// Returns false if the VA is outside the section.
static bool overwrite_section_bytes(ELFIO::section* sec,
                                    uint64_t target_va,
                                    const uint8_t* data, size_t len) {
    const uint64_t sec_addr = sec->get_address();
    const uint64_t sec_size = sec->get_size();
    if (target_va < sec_addr || target_va + len > sec_addr + sec_size)
        return false;

    const size_t offset = static_cast<size_t>(target_va - sec_addr);

    // ELFIO sections: get_data() returns const pointer.
    // To modify, we copy, patch, and set_data.
    std::vector<uint8_t> sec_data(sec->get_data(),
                                   sec->get_data() + sec->get_size());
    std::memcpy(sec_data.data() + offset, data, len);
    sec->set_data(reinterpret_cast<const char*>(sec_data.data()),
                  sec_data.size());
    return true;
}

// ---------------------------------------------------------------------------
// ELFPatcher::patch
// ---------------------------------------------------------------------------

tl::expected<PatchResult, Common::DiagnosticCode>
ELFPatcher::patch(const PatchRequest& request,
                  Common::DiagnosticCollector& diag) noexcept {

    // --- Validation ---
    if (request.input_path.empty() || request.output_path.empty()) {
        diag.error("loader", Common::DiagnosticCode::PatchInputInvalid,
                   "input or output path is empty");
        return tl::unexpected(Common::DiagnosticCode::PatchInputInvalid);
    }
    if (request.regions.empty()) {
        diag.error("loader", Common::DiagnosticCode::PatchInputInvalid,
                   "no regions to patch");
        return tl::unexpected(Common::DiagnosticCode::PatchInputInvalid);
    }
    if (request.compiled_outputs.empty()) {
        diag.error("loader", Common::DiagnosticCode::PatchInputInvalid,
                   "no compiled outputs");
        return tl::unexpected(Common::DiagnosticCode::PatchInputInvalid);
    }

    // --- Load ELF ---
    ELFIO::elfio reader;
    if (!reader.load(request.input_path)) {
        diag.error("loader", Common::DiagnosticCode::PatchBinaryReadFailed,
                   "failed to load ELF: " + request.input_path);
        return tl::unexpected(Common::DiagnosticCode::PatchBinaryReadFailed);
    }

    ELFIO::section* text_sec = find_text_section(reader);
    if (!text_sec) {
        diag.error("loader", Common::DiagnosticCode::PatchBinaryReadFailed,
                   "no .text section found");
        return tl::unexpected(Common::DiagnosticCode::PatchBinaryReadFailed);
    }

    // --- Serialize bytecodes into blob ---
    auto blob_result = BlobSerializer::serialize(request.compiled_outputs, diag);
    if (!blob_result) {
        return tl::unexpected(blob_result.error());
    }
    auto& blob_data = blob_result.value();

    // --- Generate entry stubs for each region ---
    struct StubInfo {
        size_t region_idx;
        Stub entry_stub;
    };
    std::vector<StubInfo> stubs;
    stubs.reserve(request.regions.size());

    for (size_t i = 0; i < request.regions.size(); ++i) {
        // blob_rel_offset is a placeholder — will be fixed up after we know
        // the layout within the new segment
        auto stub_result = StubGenerator::generate_entry_stub(
            request.arch, request.mode,
            static_cast<uint32_t>(i),
            0 /* placeholder, patched below */);

        if (!stub_result) {
            diag.error("loader", Common::DiagnosticCode::PatchStubGenerationFailed,
                       "failed to generate entry stub for region " +
                       request.regions[i].name);
            return tl::unexpected(stub_result.error());
        }
        stubs.push_back({i, std::move(stub_result.value())});
    }

    // --- Compute payload layout ---
    //
    //   [blob_data] [stub_0] [stub_1] ... [stub_n-1]
    //   ^--- blob_offset = 0
    //                ^--- stub_offsets[0]
    //
    const size_t blob_size = blob_data.size();
    size_t payload_size = blob_size;

    std::vector<size_t> stub_offsets;
    stub_offsets.reserve(stubs.size());
    for (const auto& si : stubs) {
        stub_offsets.push_back(payload_size);
        payload_size += si.entry_stub.code.size();
    }

    // --- Fix up blob-pointer displacements now that layout is known ---
    //
    // For x86-64: LEA rdi, [rip + disp32]
    //   disp32 = blob_va - (lea_insn_va + 3 + 4)
    //          = (seg_va + 0) - (seg_va + stub_offset + blob_fixup_offset + blob_insn_size)
    //          = -(stub_offset + blob_fixup_offset + blob_insn_size)
    //   (blob is at offset 0 in the segment)
    //
    // For ARM64: ADR x0, #offset  — offset = blob_va - adr_va
    //
    for (size_t i = 0; i < stubs.size(); ++i) {
        auto& s = stubs[i].entry_stub;
        const int64_t blob_disp =
            -static_cast<int64_t>(stub_offsets[i] + s.blob_fixup_offset + s.blob_insn_size);

        if (request.arch == Common::FileArch::ARM64) {
            // Re-encode ADR x0, #blob_disp
            const uint32_t immlo = (static_cast<uint32_t>(blob_disp) & 0x3) << 29;
            const uint32_t immhi = (static_cast<uint32_t>(blob_disp >> 2) & 0x7FFFF) << 5;
            const uint32_t adr = 0x10000000 | immhi | immlo;
            s.code[s.blob_fixup_offset + 0] = static_cast<uint8_t>(adr);
            s.code[s.blob_fixup_offset + 1] = static_cast<uint8_t>(adr >> 8);
            s.code[s.blob_fixup_offset + 2] = static_cast<uint8_t>(adr >> 16);
            s.code[s.blob_fixup_offset + 3] = static_cast<uint8_t>(adr >> 24);
        } else {
            // x86: patch disp32 in LEA or ADD
            const auto d32 = static_cast<int32_t>(blob_disp);
            std::memcpy(s.code.data() + s.blob_fixup_offset, &d32, 4);
        }
    }

    // --- Assemble payload ---
    std::vector<uint8_t> payload(payload_size);
    std::memcpy(payload.data(), blob_data.data(), blob_size);

    for (size_t i = 0; i < stubs.size(); ++i) {
        std::memcpy(payload.data() + stub_offsets[i],
                    stubs[i].entry_stub.code.data(),
                    stubs[i].entry_stub.code.size());
    }

    // --- Add new segment/section ---
    constexpr uint64_t PAGE_SIZE = 0x1000;
    const uint64_t new_seg_va = next_segment_va(reader, PAGE_SIZE);

    auto* new_sec = reader.sections.add(".text.vm");
    new_sec->set_type(SHT_PROGBITS);
    new_sec->set_flags(SHF_ALLOC | SHF_EXECINSTR);
    new_sec->set_addr_align(16);
    new_sec->set_address(new_seg_va);
    new_sec->set_data(reinterpret_cast<const char*>(payload.data()),
                      payload.size());

    auto* new_seg = reader.segments.add();
    new_seg->set_type(PT_LOAD);
    new_seg->set_flags(PF_R | PF_X);
    new_seg->set_align(PAGE_SIZE);
    new_seg->set_virtual_address(new_seg_va);
    new_seg->set_physical_address(new_seg_va);
    new_seg->add_section_index(new_sec->get_index(),
                               new_sec->get_addr_align());

    // --- Patch each protected region with JMP/B to its entry stub ---
    size_t patched_count = 0;
    for (size_t i = 0; i < request.regions.size(); ++i) {
        const auto& region = request.regions[i];

        // Check minimum size
        const size_t min_sz = StubGenerator::min_region_size(
            request.arch, request.mode);
        if (region.size < min_sz) {
            diag.warn("loader", Common::DiagnosticCode::PatchRegionTooSmall,
                      "region '" + region.name + "' is too small to patch ("
                      + std::to_string(region.size) + " < "
                      + std::to_string(min_sz) + " bytes), skipping",
                      region.name, region.addr);
            continue;
        }

        // Compute entry stub VA
        const uint64_t stub_va = new_seg_va + stub_offsets[i];

        auto jmp_result = StubGenerator::generate_region_patch(
            request.arch, request.mode,
            region.size, region.addr, stub_va);

        if (!jmp_result) {
            diag.error("loader", Common::DiagnosticCode::PatchStubGenerationFailed,
                       "failed to generate region patch for " + region.name,
                       region.name, region.addr);
            return tl::unexpected(jmp_result.error());
        }

        const auto& jmp_bytes = jmp_result.value();
        if (!overwrite_section_bytes(text_sec, region.addr,
                                     jmp_bytes.data(), jmp_bytes.size())) {
            diag.error("loader", Common::DiagnosticCode::PatchSegmentCreationFailed,
                       "region VA 0x" + std::to_string(region.addr) +
                       " falls outside .text section",
                       region.name, region.addr);
            return tl::unexpected(Common::DiagnosticCode::PatchSegmentCreationFailed);
        }

        ++patched_count;
        diag.note("loader", Common::DiagnosticCode::None,
                  "patched region '" + region.name + "' at 0x" +
                  std::to_string(region.addr) + " -> stub at 0x" +
                  std::to_string(stub_va),
                  region.name, region.addr);
    }

    // --- Write output ---
    if (!reader.save(request.output_path)) {
        diag.error("loader", Common::DiagnosticCode::PatchBinaryWriteFailed,
                   "failed to write patched ELF: " + request.output_path);
        return tl::unexpected(Common::DiagnosticCode::PatchBinaryWriteFailed);
    }

    PatchResult result;
    result.output_path = request.output_path;
    result.regions_patched = patched_count;
    result.blob_bytes_injected = blob_size;

    diag.note("loader", Common::DiagnosticCode::None,
              "patched " + std::to_string(patched_count) + " region(s), "
              "injected " + std::to_string(blob_size) + " bytes of bytecodes");

    return result;
}

}  // namespace VMPilot::Loader
