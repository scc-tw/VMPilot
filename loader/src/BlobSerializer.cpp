#include <BlobSerializer.hpp>
#include <instruction_t.hpp>

#include <cstring>

namespace VMPilot::Loader {

tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
BlobSerializer::serialize(
    const std::vector<SDK::BytecodeCompiler::CompilationOutput>& outputs,
    Common::DiagnosticCollector& diag) noexcept {

    if (outputs.empty()) {
        diag.error("loader", Common::DiagnosticCode::PatchInputInvalid,
                   "no compilation outputs to serialize");
        return tl::unexpected(Common::DiagnosticCode::PatchInputInvalid);
    }

    // Phase 1: flatten all bytecodes into per-region byte buffers
    struct RegionData {
        uint64_t original_addr;
        std::vector<uint8_t> bytes;
    };
    std::vector<RegionData> region_data;
    region_data.reserve(outputs.size());

    Common::Instruction helper;
    for (const auto& output : outputs) {
        RegionData rd;
        rd.original_addr = output.addr;
        rd.bytes.reserve(output.bytecodes.size() * sizeof(Common::Instruction_t));

        for (const auto& insn : output.bytecodes) {
            auto flat = helper.flatten(insn);
            rd.bytes.insert(rd.bytes.end(), flat.begin(), flat.end());
        }
        region_data.push_back(std::move(rd));
    }

    // Phase 2: compute sizes
    const auto entry_count = static_cast<uint16_t>(region_data.size());
    const size_t header_size = sizeof(BlobHeader);
    const size_t directory_size = entry_count * sizeof(BlobEntry);
    size_t data_size = 0;
    for (const auto& rd : region_data)
        data_size += rd.bytes.size();

    const size_t total = header_size + directory_size + data_size;

    // Phase 3: assemble blob
    std::vector<uint8_t> blob(total);

    // Write header
    BlobHeader hdr;
    hdr.entry_count = entry_count;
    std::memcpy(blob.data(), &hdr, sizeof(hdr));

    // Write directory entries and data
    size_t data_offset = 0;
    for (size_t i = 0; i < region_data.size(); ++i) {
        BlobEntry entry;
        entry.original_addr = region_data[i].original_addr;
        entry.bytecode_offset = static_cast<uint32_t>(data_offset);
        entry.bytecode_size = static_cast<uint32_t>(region_data[i].bytes.size());

        const size_t entry_pos = header_size + i * sizeof(BlobEntry);
        std::memcpy(blob.data() + entry_pos, &entry, sizeof(entry));

        const size_t data_pos = header_size + directory_size + data_offset;
        std::memcpy(blob.data() + data_pos,
                    region_data[i].bytes.data(),
                    region_data[i].bytes.size());
        data_offset += region_data[i].bytes.size();
    }

    diag.note("loader", Common::DiagnosticCode::None,
              "serialized " + std::to_string(entry_count) + " region(s), "
              + std::to_string(total) + " bytes total");

    return blob;
}

bool BlobSerializer::validate(const std::vector<uint8_t>& blob) noexcept {
    if (blob.size() < sizeof(BlobHeader))
        return false;

    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));

    if (hdr.magic != 0x564D5031)
        return false;
    if (hdr.version != 1)
        return false;

    const size_t expected_min =
        sizeof(BlobHeader) + hdr.entry_count * sizeof(BlobEntry);
    return blob.size() >= expected_min;
}

}  // namespace VMPilot::Loader
