#include <Serializer.hpp>

#include <VMPilot_crypto.hpp>
#include <vmpilot.pb.h>

#include <toml++/toml.hpp>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;

using namespace VMPilot::SDK;

namespace {

/// Max length for a single sanitized name component.
/// Kept short so that directory + filename stay under both
/// Windows MAX_PATH (260) and Linux NAME_MAX (255).
/// Budget: output_dir (~70) + "/" + dir (~66) + "/" + filename (~120) < 260.
static constexpr size_t kMaxComponent = 60;
static constexpr size_t kMaxFilename = 120;

/// Replace filesystem-unsafe characters and truncate.
/// When truncation occurs, append a short hash of the original
/// to preserve uniqueness (e.g. nested-template names that share
/// a long common prefix).
static std::string sanitize(const std::string& input,
                            size_t max_len = kMaxComponent) {
    std::string result = input;
    for (char& c : result) {
        if (c == '?' || c == '<' || c == '>' || c == ':' || c == '"' ||
            c == '|' || c == '*' || c == '\\' || c == '/')
            c = '_';
    }
    if (result.size() > max_len) {
        // Hash the full original so truncated names stay unique.
        // Use a simple FNV-1a 32-bit hash — no crypto needed here.
        uint32_t h = 0x811c9dc5u;
        for (unsigned char c : input) {
            h ^= c;
            h *= 0x01000193u;
        }
        char suffix[10];
        std::snprintf(suffix, sizeof(suffix), "_%08x", h);
        // Reserve room for the hash suffix (9 chars: _ + 8 hex)
        result.resize(max_len - 9);
        result += suffix;
    }
    return result;
}

static std::string hexString(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (auto b : bytes) oss << std::hex << std::setfill('0') << std::setw(2)
                             << static_cast<int>(b);
    return oss.str();
}

static std::string archToString(Segmentator::Arch arch) {
    switch (arch) {
        case Segmentator::Arch::X86: return "X86";
        case Segmentator::Arch::ARM64: return "ARM64";
        case Segmentator::Arch::ARM: return "ARM";
        case Segmentator::Arch::MIPS: return "MIPS";
        case Segmentator::Arch::PPC: return "PPC";
        case Segmentator::Arch::RISCV: return "RISCV";
        default: return "UNKNOWN";
    }
}

static std::string modeToString(Segmentator::Mode mode) {
    auto m = static_cast<uint32_t>(mode);
    if (m & static_cast<uint32_t>(Segmentator::Mode::MODE_64))
        return "MODE_64";
    if (m & static_cast<uint32_t>(Segmentator::Mode::MODE_32))
        return "MODE_32";
    if (m & static_cast<uint32_t>(Segmentator::Mode::MODE_16))
        return "MODE_16";
    return "MODE_LITTLE_ENDIAN";
}

static bool writeFile(const fs::path& path,
                      const std::vector<uint8_t>& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
    return out.good();
}

static bool writeFile(const fs::path& path, const std::string& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return out.good();
}

static std::string entryTypeString(
    const Segmentator::NativeSymbolTableEntry& sym) {
    return sym.getAttribute<std::string>("entry_type", std::string(""));
}

}  // namespace

tl::expected<void, std::string> Serializer::dump(
    const Segmentator::SegmentationResult& result,
    const std::string& output_dir) {
    // 1. Create output directory
    std::error_code ec;
    fs::create_directories(output_dir, ec);
    if (ec) {
        return tl::make_unexpected("failed to create output directory: " +
                                   ec.message());
    }

    const fs::path out_root(output_dir);

    // 2. Serialize CompilationContext -> context.pb
    vmpilot::CompilationContext ctx_pb;
    ctx_pb.set_arch(static_cast<uint32_t>(result.context.arch));
    ctx_pb.set_mode(static_cast<uint32_t>(result.context.mode));

    for (const auto& sym : result.context.symbols) {
        auto* s = ctx_pb.add_symbols();
        s->set_name(sym.name);
        s->set_address(sym.address);
        s->set_size(sym.size);
        s->set_entry_type(entryTypeString(sym));
        s->set_is_global(sym.isGlobal);
    }

    for (const auto& sec : result.context.rodata_sections) {
        auto* r = ctx_pb.add_rodata_sections();
        r->set_base_addr(sec.base_addr);
        r->set_data(sec.data.data(), sec.data.size());
    }

    std::string ctx_serialized;
    if (!ctx_pb.SerializeToString(&ctx_serialized)) {
        return tl::make_unexpected("failed to serialize CompilationContext");
    }

    const fs::path ctx_path = out_root / "context.pb";
    if (!writeFile(ctx_path, ctx_serialized)) {
        return tl::make_unexpected("failed to write context.pb");
    }

    // 3. Compute SHA-256 of context.pb
    std::vector<uint8_t> ctx_bytes(ctx_serialized.begin(),
                                   ctx_serialized.end());
    auto context_hash = VMPilot::Crypto::SHA256(ctx_bytes, /*salt=*/{});
    std::string context_hash_hex = hexString(context_hash);

    // 4. Build addr -> index lookup for refined_regions
    std::unordered_map<uint64_t, size_t> addr_to_idx;
    for (size_t i = 0; i < result.refined_regions.size(); ++i)
        addr_to_idx[result.refined_regions[i].getAddr()] = i;

    // 5. For each group, create directory and write units
    toml::table manifest;

    // Binary metadata
    toml::table binary_tbl;
    binary_tbl.insert("path", result.binary_path);
    binary_tbl.insert("compiler_info", result.compiler_info);
    manifest.insert("binary", std::move(binary_tbl));

    // Context metadata
    toml::table context_tbl;
    context_tbl.insert("file", "context.pb");
    context_tbl.insert("sha256", context_hash_hex);
    context_tbl.insert("arch", archToString(result.context.arch));
    context_tbl.insert("mode", modeToString(result.context.mode));
    manifest.insert("context", std::move(context_tbl));

    // Groups array
    toml::array groups_arr;

    for (const auto& group : result.groups) {
        std::string dir_name = sanitize(group.source_name) + ".group";
        fs::path group_dir = out_root / dir_name;
        fs::create_directories(group_dir, ec);
        if (ec) {
            return tl::make_unexpected("failed to create group directory: " +
                                       ec.message());
        }

        toml::table group_tbl;
        group_tbl.insert("name", group.source_name);
        group_tbl.insert("directory", dir_name);
        toml::array units_arr;

        for (const auto& site : group.sites) {
            auto it = addr_to_idx.find(site.addr);
            if (it == addr_to_idx.end()) continue;

            const auto& region = result.refined_regions[it->second];

            // Build filename.
            // Format: {source}__{canonical|inline}__{enclosing}__{addr}.unit.pb
            // The addr suffix is the true unique key and must never be truncated.
            // Individual name components are capped by sanitize() to kMaxComponent.
            std::string enclosing = site.enclosing_symbol.value_or("unknown");
            std::string canon_str =
                site.is_canonical ? "canonical" : "inline";
            std::ostringstream unit_addr_ss;
            unit_addr_ss << "0x" << std::hex << site.addr;
            std::string addr_str = unit_addr_ss.str();

            std::string filename = sanitize(group.source_name) + "__" +
                                   canon_str + "__" +
                                   sanitize(enclosing) + "__" +
                                   addr_str + ".unit.pb";

            // Final safety net: cap total filename at kMaxFilename chars.
            // Keep the addr + suffix intact (they're at the end and short).
            if (filename.size() > kMaxFilename) {
                std::string tail = "__" + addr_str + ".unit.pb";
                size_t budget = kMaxFilename - tail.size();
                filename = filename.substr(0, budget) + tail;
            }

            // Build CompilationUnit protobuf
            vmpilot::CompilationUnit unit_pb;
            unit_pb.set_name(region.getName());
            unit_pb.set_addr(region.getAddr());
            unit_pb.set_size(region.getSize());
            auto code = region.getCode();
            unit_pb.set_code(code.data(), code.size());
            unit_pb.set_enclosing_symbol(enclosing);
            unit_pb.set_is_canonical(site.is_canonical);
            unit_pb.set_context_file("context.pb");
            unit_pb.set_context_hash(context_hash.data(),
                                     context_hash.size());

            std::string unit_serialized;
            if (!unit_pb.SerializeToString(&unit_serialized)) {
                return tl::make_unexpected(
                    "failed to serialize CompilationUnit for " +
                    region.getName());
            }

            fs::path unit_path = group_dir / filename;
            if (!writeFile(unit_path, unit_serialized)) {
                return tl::make_unexpected("failed to write " + filename);
            }

            // Add to manifest
            toml::table unit_tbl;
            std::string rel_path = dir_name + "/" + filename;
            unit_tbl.insert("file", rel_path);
            unit_tbl.insert("context_file", "context.pb");
            unit_tbl.insert("context_hash", context_hash_hex);
            std::ostringstream addr_ss;
            addr_ss << "0x" << std::hex << site.addr;
            unit_tbl.insert("addr", addr_ss.str());
            unit_tbl.insert("size",
                            static_cast<int64_t>(region.getSize()));
            unit_tbl.insert("is_canonical", site.is_canonical);
            unit_tbl.insert("enclosing_symbol", enclosing);
            units_arr.push_back(std::move(unit_tbl));
        }

        group_tbl.insert("units", std::move(units_arr));
        groups_arr.push_back(std::move(group_tbl));
    }

    manifest.insert("groups", std::move(groups_arr));

    // 6. Write manifest.toml
    std::ostringstream toml_ss;
    toml_ss << manifest;
    if (!writeFile(out_root / "manifest.toml", toml_ss.str())) {
        return tl::make_unexpected("failed to write manifest.toml");
    }

    return {};
}
