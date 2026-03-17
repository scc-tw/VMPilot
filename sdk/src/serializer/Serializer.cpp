#include <Serializer.hpp>

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>
#include <VMPilot_crypto.hpp>
#include <vmpilot.pb.h>

#include <toml++/toml.hpp>

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string_view>
#include <unordered_map>

namespace fs = std::filesystem;

using namespace VMPilot::SDK;

// ---------------------------------------------------------------------------
// Filename helpers
// ---------------------------------------------------------------------------

namespace {

/// Max length for a single sanitized name component.
/// Kept short so that directory + filename stay under both
/// Windows MAX_PATH (260) and Linux NAME_MAX (255).
/// Budget: output_dir (~70) + "/" + dir (~66) + "/" + filename (~120) < 260.
constexpr size_t kMaxComponent = 60;
constexpr size_t kMaxFilename = 120;

/// Replace filesystem-unsafe characters and truncate.
/// When truncation occurs, append a short FNV-1a hash of the original
/// to preserve uniqueness (e.g. nested-template names that share
/// a long common prefix).
std::string sanitize(std::string_view input,
                     size_t max_len = kMaxComponent) {
    std::string result(input);
    for (char& c : result) {
        if (c == '?' || c == '<' || c == '>' || c == ':' || c == '"' ||
            c == '|' || c == '*' || c == '\\' || c == '/')
            c = '_';
    }
    if (result.size() > max_len) {
        uint32_t h = 0x811c9dc5u;
        for (unsigned char c : input) {
            h ^= c;
            h *= 0x01000193u;
        }
        char suffix[10];
        std::snprintf(suffix, sizeof(suffix), "_%08x", h);
        result.resize(max_len - 9);
        result += suffix;
    }
    return result;
}

std::string toHexAddr(uint64_t addr) {
    std::ostringstream ss;
    ss << "0x" << std::hex << addr;
    return ss.str();
}

std::string toHexString(const std::vector<uint8_t>& bytes) {
    std::ostringstream ss;
    for (auto b : bytes)
        ss << std::hex << std::setfill('0') << std::setw(2)
           << static_cast<int>(b);
    return ss.str();
}

/// Build unit filename with truncation safety.
/// The address suffix is the true unique key and is never truncated.
std::string buildUnitFilename(std::string_view source_name,
                              bool is_canonical,
                              std::string_view enclosing,
                              uint64_t addr) {
    auto addr_str = toHexAddr(addr);
    std::string filename = sanitize(source_name) + "__" +
                           (is_canonical ? "canonical" : "inline") + "__" +
                           sanitize(enclosing) + "__" +
                           addr_str + ".unit.pb";

    if (filename.size() > kMaxFilename) {
        std::string tail = "__" + addr_str + ".unit.pb";
        filename = filename.substr(0, kMaxFilename - tail.size()) + tail;
    }
    return filename;
}

// ---------------------------------------------------------------------------
// I/O helpers
// ---------------------------------------------------------------------------

bool writeFile(const fs::path& path, std::string_view data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return out.good();
}

std::string entryTypeString(
    const Segmentator::NativeSymbolTableEntry& sym) {
    return sym.getAttribute<std::string>("entry_type", std::string());
}

// ---------------------------------------------------------------------------
// Context serialization
// ---------------------------------------------------------------------------

struct ContextBlob {
    std::string serialized;       // raw protobuf bytes
    std::vector<uint8_t> hash;    // SHA-256
    std::string hash_hex;
};

tl::expected<ContextBlob, std::string>
serializeContext(const Segmentator::CompilationContext& ctx) {
    vmpilot::CompilationContext pb;
    pb.set_arch(static_cast<uint32_t>(ctx.arch));
    pb.set_mode(static_cast<uint32_t>(ctx.mode));

    for (const auto& sym : ctx.symbols) {
        auto* s = pb.add_symbols();
        s->set_name(sym.name);
        s->set_address(sym.address);
        s->set_size(sym.size);
        s->set_entry_type(entryTypeString(sym));
        s->set_is_global(sym.isGlobal);
    }

    for (const auto& sec : ctx.rodata_sections) {
        auto* r = pb.add_rodata_sections();
        r->set_base_addr(sec.base_addr);
        r->set_data(sec.data.data(), sec.data.size());
    }

    ContextBlob blob;
    if (!pb.SerializeToString(&blob.serialized))
        return tl::make_unexpected("failed to serialize CompilationContext");

    std::vector<uint8_t> bytes(blob.serialized.begin(),
                               blob.serialized.end());
    blob.hash = VMPilot::Crypto::SHA256(bytes, /*salt=*/{});
    blob.hash_hex = toHexString(blob.hash);
    return blob;
}

// ---------------------------------------------------------------------------
// Unit serialization
// ---------------------------------------------------------------------------

tl::expected<std::string, std::string>
serializeUnit(const Segmentator::NativeFunctionBase& region,
              std::string_view enclosing,
              bool is_canonical,
              const std::vector<uint8_t>& context_hash) {
    vmpilot::CompilationUnit pb;
    pb.set_name(region.getName());
    pb.set_addr(region.getAddr());
    pb.set_size(region.getSize());
    auto code = region.getCode();
    pb.set_code(code.data(), code.size());
    pb.set_enclosing_symbol(std::string(enclosing));
    pb.set_is_canonical(is_canonical);
    pb.set_context_file("context.pb");
    pb.set_context_hash(context_hash.data(), context_hash.size());

    std::string out;
    if (!pb.SerializeToString(&out))
        return tl::make_unexpected("failed to serialize CompilationUnit for " +
                                   region.getName());
    return out;
}

// ---------------------------------------------------------------------------
// Manifest builder
// ---------------------------------------------------------------------------

struct UnitManifestEntry {
    std::string file;
    std::string addr;
    int64_t size;
    bool is_canonical;
    std::string enclosing_symbol;
};

struct GroupManifestEntry {
    std::string name;
    std::string directory;
    std::vector<UnitManifestEntry> units;
};

std::string buildManifest(
    const Segmentator::SegmentationResult& result,
    const ContextBlob& ctx_blob,
    const std::vector<GroupManifestEntry>& groups) {
    toml::table manifest;

    manifest.insert("binary", toml::table{
        {"path", result.binary_path},
        {"compiler_info", result.compiler_info},
    });

    manifest.insert("context", toml::table{
        {"file", "context.pb"},
        {"sha256", ctx_blob.hash_hex},
        {"arch", std::string(Segmentator::to_string(result.context.arch))},
        {"mode", std::string(Segmentator::to_string(result.context.mode))},
    });

    toml::array groups_arr;
    for (const auto& g : groups) {
        toml::table group_tbl;
        group_tbl.insert("name", g.name);
        group_tbl.insert("directory", g.directory);

        toml::array units_arr;
        for (const auto& u : g.units) {
            units_arr.push_back(toml::table{
                {"file", u.file},
                {"context_file", "context.pb"},
                {"context_hash", ctx_blob.hash_hex},
                {"addr", u.addr},
                {"size", u.size},
                {"is_canonical", u.is_canonical},
                {"enclosing_symbol", u.enclosing_symbol},
            });
        }

        group_tbl.insert("units", std::move(units_arr));
        groups_arr.push_back(std::move(group_tbl));
    }

    manifest.insert("groups", std::move(groups_arr));

    std::ostringstream ss;
    ss << manifest;
    return ss.str();
}

}  // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

tl::expected<void, std::string> Serializer::dump(
    const Segmentator::SegmentationResult& result,
    const std::string& output_dir) {

    // 1. Create output directory
    std::error_code ec;
    fs::create_directories(output_dir, ec);
    if (ec)
        return tl::make_unexpected("failed to create output directory: " +
                                   ec.message());

    const fs::path out_root(output_dir);

    // 2. Serialize context and write to disk
    auto ctx_blob = serializeContext(result.context);
    if (!ctx_blob)
        return tl::make_unexpected(ctx_blob.error());

    if (!writeFile(out_root / "context.pb", ctx_blob->serialized))
        return tl::make_unexpected("failed to write context.pb");

    // 3. Build addr → index lookup for refined regions
    std::unordered_map<uint64_t, size_t> addr_to_idx;
    for (size_t i = 0; i < result.refined_regions.size(); ++i)
        addr_to_idx[result.refined_regions[i].getAddr()] = i;

    // 4. Serialize each group's units and collect manifest entries
    std::vector<GroupManifestEntry> manifest_groups;

    for (const auto& group : result.groups) {
        std::string dir_name = sanitize(group.source_name) + ".group";
        fs::path group_dir = out_root / dir_name;
        fs::create_directories(group_dir, ec);
        if (ec)
            return tl::make_unexpected("failed to create group directory: " +
                                       ec.message());

        GroupManifestEntry manifest_group{group.source_name, dir_name, {}};

        for (const auto& site : group.sites) {
            auto it = addr_to_idx.find(site.addr);
            if (it == addr_to_idx.end()) continue;

            const auto& region = result.refined_regions[it->second];
            std::string enclosing =
                site.enclosing_symbol.value_or("unknown");

            // Build filename and serialize unit
            auto filename = buildUnitFilename(group.source_name,
                                              site.is_canonical,
                                              enclosing, site.addr);

            auto unit_bytes = serializeUnit(region, enclosing,
                                            site.is_canonical,
                                            ctx_blob->hash);
            if (!unit_bytes)
                return tl::make_unexpected(unit_bytes.error());

            if (!writeFile(group_dir / filename, *unit_bytes))
                return tl::make_unexpected("failed to write " + filename);

            manifest_group.units.push_back({
                dir_name + "/" + filename,
                toHexAddr(site.addr),
                static_cast<int64_t>(region.getSize()),
                site.is_canonical,
                enclosing,
            });
        }

        manifest_groups.push_back(std::move(manifest_group));
    }

    // 5. Write manifest
    auto manifest_str = buildManifest(result, *ctx_blob, manifest_groups);
    if (!writeFile(out_root / "manifest.toml", manifest_str))
        return tl::make_unexpected("failed to write manifest.toml");

    return {};
}
