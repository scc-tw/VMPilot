#include <Serializer.hpp>
#include <SerializationTraits.hpp>

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>
#include <VMPilot_crypto.hpp>

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

constexpr size_t kMaxComponent = 60;
constexpr size_t kMaxFilename = 120;

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

tl::expected<std::string, std::string> readFile(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in)
        return tl::unexpected(std::string("failed to open: ") + path.string());
    std::ostringstream ss;
    ss << in.rdbuf();
    if (!in.good() && !in.eof())
        return tl::unexpected(std::string("failed to read: ") + path.string());
    return ss.str();
}

// ---------------------------------------------------------------------------
// Manifest helpers
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
    const std::vector<Core::CompilationUnit>& units,
    const std::string& ctx_hash_hex,
    const std::vector<GroupManifestEntry>& groups) {
    toml::table manifest;

    // Derive arch/mode from the first unit's context
    std::string arch_str = "UNKNOWN";
    std::string mode_str = "UNKNOWN";
    std::string binary_path;
    if (!units.empty() && units[0].context) {
        arch_str = Segmentator::to_string(units[0].context->arch);
        mode_str = Segmentator::to_string(units[0].context->mode);
    }

    manifest.insert("context", toml::table{
        {"file", "context.pb"},
        {"sha256", ctx_hash_hex},
        {"arch", arch_str},
        {"mode", mode_str},
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
                {"context_hash", ctx_hash_hex},
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
// Public API: build_units
// ---------------------------------------------------------------------------

std::vector<Core::CompilationUnit> Serializer::build_units(
    const Segmentator::SegmentationResult& result) {
    std::vector<Core::CompilationUnit> units;

    // Shared context — one shared_ptr for all units from this binary
    auto ctx = std::make_shared<const Segmentator::CompilationContext>(
        result.context);

    for (const auto& group : result.groups) {
        for (const auto& site : group.sites) {
            // Find the matching NativeFunctionBase by address + name
            const Segmentator::NativeFunctionBase* found = nullptr;
            for (const auto& region : result.refined_regions) {
                if (region.getAddr() == site.addr &&
                    region.getName() == site.source_name) {
                    found = &region;
                    break;
                }
            }
            if (!found)
                continue;

            Core::CompilationUnit unit;
            unit.name = site.source_name;
            unit.addr = site.addr;
            unit.size = site.size;
            unit.code = found->getCode();
            unit.enclosing_symbol =
                site.enclosing_symbol.value_or(std::string{});
            unit.is_canonical = site.is_canonical;
            unit.context = ctx;
            units.push_back(std::move(unit));
        }
    }

    return units;
}

// ---------------------------------------------------------------------------
// Public API: dump
// ---------------------------------------------------------------------------

tl::expected<void, std::string> Serializer::dump(
    const std::vector<Core::CompilationUnit>& units,
    const std::string& output_dir) {

    if (units.empty())
        return tl::unexpected(std::string("no units to dump"));

    // 1. Create output directory
    std::error_code ec;
    fs::create_directories(output_dir, ec);
    if (ec)
        return tl::unexpected(std::string("failed to create output directory: ") +
                              ec.message());

    const fs::path out_root(output_dir);

    // 2. Serialize and write context
    // All units share the same context (guaranteed by build_units)
    if (!units[0].context)
        return tl::unexpected(std::string("units have no context"));

    auto ctx_bytes = serialize(*units[0].context);
    if (!ctx_bytes)
        return tl::unexpected(ctx_bytes.error());

    if (!writeFile(out_root / "context.pb", *ctx_bytes))
        return tl::unexpected(std::string("failed to write context.pb"));

    // Compute context hash for manifest
    std::vector<uint8_t> ctx_raw(ctx_bytes->begin(), ctx_bytes->end());
    auto ctx_hash = VMPilot::Crypto::SHA256(ctx_raw, /*salt=*/{});
    auto ctx_hash_hex = toHexString(ctx_hash);

    // 3. Group units by name for directory structure
    std::unordered_map<std::string, std::vector<size_t>> name_to_indices;
    for (size_t i = 0; i < units.size(); ++i)
        name_to_indices[units[i].name].push_back(i);

    // 4. Serialize each group's units
    std::vector<GroupManifestEntry> manifest_groups;

    for (const auto& [group_name, indices] : name_to_indices) {
        std::string dir_name = sanitize(group_name) + ".group";
        fs::path group_dir = out_root / dir_name;
        fs::create_directories(group_dir, ec);
        if (ec)
            return tl::unexpected(std::string("failed to create group directory: ") +
                                  ec.message());

        GroupManifestEntry manifest_group{group_name, dir_name, {}};

        for (size_t idx : indices) {
            const auto& unit = units[idx];

            auto filename = buildUnitFilename(
                unit.name, unit.is_canonical,
                unit.enclosing_symbol.empty() ? "unknown" : unit.enclosing_symbol,
                unit.addr);

            auto unit_bytes = serialize(unit);
            if (!unit_bytes)
                return tl::unexpected(unit_bytes.error());

            if (!writeFile(group_dir / filename, *unit_bytes))
                return tl::unexpected(std::string("failed to write ") + filename);

            manifest_group.units.push_back({
                dir_name + "/" + filename,
                toHexAddr(unit.addr),
                static_cast<int64_t>(unit.size),
                unit.is_canonical,
                unit.enclosing_symbol.empty() ? "unknown" : unit.enclosing_symbol,
            });
        }

        manifest_groups.push_back(std::move(manifest_group));
    }

    // 5. Write manifest
    auto manifest_str = buildManifest(units, ctx_hash_hex, manifest_groups);
    if (!writeFile(out_root / "manifest.toml", manifest_str))
        return tl::unexpected(std::string("failed to write manifest.toml"));

    return {};
}

// ---------------------------------------------------------------------------
// Public API: load
// ---------------------------------------------------------------------------

tl::expected<std::vector<Core::CompilationUnit>, std::string>
Serializer::load(const std::string& input_dir) {
    const fs::path root(input_dir);

    // 1. Read and deserialize context
    auto ctx_bytes = readFile(root / "context.pb");
    if (!ctx_bytes)
        return tl::unexpected(ctx_bytes.error());

    auto ctx_result = deserialize<Segmentator::CompilationContext>(*ctx_bytes);
    if (!ctx_result)
        return tl::unexpected(ctx_result.error());

    auto ctx = std::make_shared<const Segmentator::CompilationContext>(
        std::move(*ctx_result));

    // 2. Read manifest to find unit files
    auto manifest_bytes = readFile(root / "manifest.toml");
    if (!manifest_bytes)
        return tl::unexpected(manifest_bytes.error());

    toml::table manifest;
    try {
        manifest = toml::parse(*manifest_bytes);
    } catch (const toml::parse_error& e) {
        return tl::unexpected(
            std::string("failed to parse manifest.toml: ") + e.what());
    }

    // 3. Iterate groups and load each unit
    std::vector<Core::CompilationUnit> units;

    auto* groups = manifest["groups"].as_array();
    if (!groups)
        return tl::unexpected(std::string("manifest missing 'groups' array"));

    for (const auto& group_node : *groups) {
        auto* group = group_node.as_table();
        if (!group) continue;

        auto* units_arr = (*group)["units"].as_array();
        if (!units_arr) continue;

        for (const auto& unit_node : *units_arr) {
            auto* unit_tbl = unit_node.as_table();
            if (!unit_tbl) continue;

            auto file_val = (*unit_tbl)["file"].value<std::string>();
            if (!file_val) continue;

            auto unit_file_bytes = readFile(root / *file_val);
            if (!unit_file_bytes)
                return tl::unexpected(unit_file_bytes.error());

            auto unit = deserialize<Core::CompilationUnit>(*unit_file_bytes, ctx);
            if (!unit)
                return tl::unexpected(unit.error());

            units.push_back(std::move(*unit));
        }
    }

    return units;
}

// ---------------------------------------------------------------------------
// Public API: load_unit
// ---------------------------------------------------------------------------

tl::expected<Core::CompilationUnit, std::string>
Serializer::load_unit(const std::string& unit_pb_path,
                      const std::string& context_pb_path) {
    // 1. Read and deserialize context
    auto ctx_bytes = readFile(context_pb_path);
    if (!ctx_bytes)
        return tl::unexpected(ctx_bytes.error());

    auto ctx_result = deserialize<Segmentator::CompilationContext>(*ctx_bytes);
    if (!ctx_result)
        return tl::unexpected(ctx_result.error());

    auto ctx = std::make_shared<const Segmentator::CompilationContext>(
        std::move(*ctx_result));

    // 2. Read and deserialize unit
    auto unit_bytes = readFile(unit_pb_path);
    if (!unit_bytes)
        return tl::unexpected(unit_bytes.error());

    return deserialize<Core::CompilationUnit>(*unit_bytes, ctx);
}
