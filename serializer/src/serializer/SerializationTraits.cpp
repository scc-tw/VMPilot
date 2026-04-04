#include <SerializationTraits.hpp>

#include <DataReference.hpp>
#include <Section.hpp>
#include <VMPilot_crypto.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Simple length-prefixed binary serialization helpers.
//
// Wire format primitives:
//   write_u8/u32/u64/i64  — fixed-width little-endian
//   write_bytes            — u32 length prefix + raw bytes
//   write_string           — u32 length prefix + UTF-8 bytes (no NUL)
//   read_*                 — corresponding readers, advancing a cursor
//
// All multi-byte integers are stored in the host byte order (little-endian
// on all supported platforms: x86, x86-64, AArch64).
// ---------------------------------------------------------------------------

namespace {

// ---- Writers ----

void write_u8(std::string& out, uint8_t v) {
    out.push_back(static_cast<char>(v));
}

void write_u32(std::string& out, uint32_t v) {
    const char* p = reinterpret_cast<const char*>(&v);
    out.append(p, sizeof(v));
}

void write_u64(std::string& out, uint64_t v) {
    const char* p = reinterpret_cast<const char*>(&v);
    out.append(p, sizeof(v));
}

void write_i64(std::string& out, int64_t v) {
    const char* p = reinterpret_cast<const char*>(&v);
    out.append(p, sizeof(v));
}

void write_bool(std::string& out, bool v) {
    write_u8(out, v ? 1 : 0);
}

void write_string(std::string& out, const std::string& s) {
    write_u32(out, static_cast<uint32_t>(s.size()));
    out.append(s);
}

void write_bytes(std::string& out, const uint8_t* data, size_t len) {
    write_u32(out, static_cast<uint32_t>(len));
    out.append(reinterpret_cast<const char*>(data), len);
}

void write_bytes(std::string& out, const std::vector<uint8_t>& v) {
    write_bytes(out, v.data(), v.size());
}

// ---- Readers ----

struct Reader {
    const char* data;
    size_t size;
    size_t pos = 0;

    bool has(size_t n) const { return pos + n <= size; }

    bool read_u8(uint8_t& v) {
        if (!has(1)) return false;
        v = static_cast<uint8_t>(data[pos++]);
        return true;
    }

    bool read_u32(uint32_t& v) {
        if (!has(sizeof(v))) return false;
        std::memcpy(&v, data + pos, sizeof(v));
        pos += sizeof(v);
        return true;
    }

    bool read_u64(uint64_t& v) {
        if (!has(sizeof(v))) return false;
        std::memcpy(&v, data + pos, sizeof(v));
        pos += sizeof(v);
        return true;
    }

    bool read_i64(int64_t& v) {
        if (!has(sizeof(v))) return false;
        std::memcpy(&v, data + pos, sizeof(v));
        pos += sizeof(v);
        return true;
    }

    bool read_bool(bool& v) {
        uint8_t u;
        if (!read_u8(u)) return false;
        v = (u != 0);
        return true;
    }

    bool read_string(std::string& s) {
        uint32_t len;
        if (!read_u32(len)) return false;
        if (!has(len)) return false;
        s.assign(data + pos, len);
        pos += len;
        return true;
    }

    bool read_bytes(std::vector<uint8_t>& v) {
        uint32_t len;
        if (!read_u32(len)) return false;
        if (!has(len)) return false;
        v.assign(reinterpret_cast<const uint8_t*>(data + pos),
                 reinterpret_cast<const uint8_t*>(data + pos + len));
        pos += len;
        return true;
    }

    // Read raw bytes into a pre-sized output (for hash fields)
    bool read_raw_bytes(std::vector<uint8_t>& v, size_t count) {
        if (!has(count)) return false;
        v.assign(reinterpret_cast<const uint8_t*>(data + pos),
                 reinterpret_cast<const uint8_t*>(data + pos + count));
        pos += count;
        return true;
    }
};

// ---- Relocation serialization ----

void writeRelocation(std::string& out,
                     const VMPilot::SDK::Core::RelocationEntry& r) {
    write_u64(out, r.offset);
    write_u32(out, r.type);
    write_u32(out, r.symbol_index);
    write_i64(out, r.addend);
    write_string(out, r.symbol_name);
}

bool readRelocation(Reader& rd, VMPilot::SDK::Core::RelocationEntry& r) {
    return rd.read_u64(r.offset) &&
           rd.read_u32(r.type) &&
           rd.read_u32(r.symbol_index) &&
           rd.read_i64(r.addend) &&
           rd.read_string(r.symbol_name);
}

}  // namespace

namespace VMPilot::SDK::Serializer {

// ---------------------------------------------------------------------------
// CompilationContext
// ---------------------------------------------------------------------------

tl::expected<std::string, std::string>
SerializationTraits<Segmentator::CompilationContext>::to_bytes(
    const Segmentator::CompilationContext& ctx) {
    std::string out;

    write_u32(out, static_cast<uint32_t>(ctx.arch));
    write_u32(out, static_cast<uint32_t>(ctx.mode));

    // Symbols
    write_u32(out, static_cast<uint32_t>(ctx.symbols.size()));
    for (const auto& sym : ctx.symbols) {
        write_string(out, sym.name);
        write_u64(out, sym.address);
        write_u64(out, sym.size);
        write_string(out, sym.getAttribute<std::string>(
            "entry_type", std::string()));
        write_bool(out, sym.isGlobal);
    }

    // Sections
    write_u32(out, static_cast<uint32_t>(ctx.sections.size()));
    for (const auto& sec : ctx.sections) {
        write_u64(out, sec.base_addr);
        write_u64(out, sec.size);
        write_u32(out, static_cast<uint32_t>(sec.kind));
        write_string(out, sec.name);
        if (sec.has_data()) {
            write_bytes(out, sec.data);
        } else {
            write_u32(out, 0);  // zero-length data
        }
    }

    return out;
}

tl::expected<Segmentator::CompilationContext, std::string>
SerializationTraits<Segmentator::CompilationContext>::from_bytes(
    const std::string& bytes) {
    Reader rd{bytes.data(), bytes.size()};

    uint32_t arch_u32, mode_u32;
    if (!rd.read_u32(arch_u32) || !rd.read_u32(mode_u32))
        return tl::unexpected(
            std::string("failed to parse CompilationContext header"));

    Segmentator::CompilationContext ctx;
    ctx.arch = static_cast<Segmentator::Arch>(arch_u32);
    ctx.mode = static_cast<Segmentator::Mode>(mode_u32);

    uint32_t sym_count;
    if (!rd.read_u32(sym_count))
        return tl::unexpected(
            std::string("failed to parse CompilationContext symbol count"));

    for (uint32_t i = 0; i < sym_count; ++i) {
        Segmentator::NativeSymbolTableEntry entry;
        std::string entry_type;
        if (!rd.read_string(entry.name) ||
            !rd.read_u64(entry.address) ||
            !rd.read_u64(entry.size) ||
            !rd.read_string(entry_type) ||
            !rd.read_bool(entry.isGlobal))
            return tl::unexpected(
                std::string("failed to parse symbol entry ") +
                std::to_string(i));
        if (!entry_type.empty())
            entry.setAttribute("entry_type", entry_type);
        ctx.symbols.push_back(std::move(entry));
    }

    uint32_t sec_count;
    if (!rd.read_u32(sec_count))
        return tl::unexpected(
            std::string("failed to parse CompilationContext section count"));

    for (uint32_t i = 0; i < sec_count; ++i) {
        Core::Section sec;
        uint32_t kind_u32;
        if (!rd.read_u64(sec.base_addr) ||
            !rd.read_u64(sec.size) ||
            !rd.read_u32(kind_u32) ||
            !rd.read_string(sec.name) ||
            !rd.read_bytes(sec.data))
            return tl::unexpected(
                std::string("failed to parse section entry ") +
                std::to_string(i));
        sec.kind = static_cast<Core::SectionKind>(kind_u32);
        ctx.sections.push_back(std::move(sec));
    }

    return ctx;
}

// ---------------------------------------------------------------------------
// CompilationUnit
// ---------------------------------------------------------------------------

tl::expected<std::string, std::string>
SerializationTraits<Core::CompilationUnit>::to_bytes(
    const Core::CompilationUnit& unit) {
    std::string out;

    write_string(out, unit.name);
    write_u64(out, unit.addr);
    write_u64(out, unit.size);
    write_bytes(out, unit.code);
    write_string(out, unit.enclosing_symbol);
    write_bool(out, unit.is_canonical);

    // context_file — always "context.pb" (filesystem convention)
    write_string(out, std::string("context.pb"));

    // context_hash — SHA-256 of serialized context (32 bytes, fixed length)
    std::vector<uint8_t> ctx_hash(32, 0);
    if (unit.context) {
        auto ctx_bytes = SerializationTraits<Segmentator::CompilationContext>
                             ::to_bytes(*unit.context);
        if (ctx_bytes.has_value()) {
            ctx_hash = VMPilot::Crypto::SHA256(
                std::vector<uint8_t>(ctx_bytes->begin(), ctx_bytes->end()),
                /*salt=*/{});
        }
    }
    // Write fixed 32 bytes (no length prefix — always SHA-256)
    out.append(reinterpret_cast<const char*>(ctx_hash.data()), 32);

    // Data references
    write_u32(out, static_cast<uint32_t>(unit.data_references.size()));
    for (const auto& ref : unit.data_references) {
        write_u64(out, ref.insn_offset);
        write_u64(out, ref.target_va);
        write_string(out, ref.target_symbol);
        write_u32(out, static_cast<uint32_t>(ref.kind));
        write_u32(out, static_cast<uint32_t>(ref.tls_model));
        write_u32(out, static_cast<uint32_t>(ref.source));
        write_u32(out, ref.access_size);
        write_bool(out, ref.is_write);
        write_bool(out, ref.is_pc_relative);
        write_u32(out, static_cast<uint32_t>(ref.atomic_width));
        write_u32(out, static_cast<uint32_t>(ref.atomic_ordering));
        write_u32(out, static_cast<uint32_t>(ref.atomic_op));

        writeRelocation(out, ref.relocation);

        // Jump table (optional)
        write_bool(out, ref.jump_table.has_value());
        if (ref.jump_table.has_value()) {
            const auto& jt = *ref.jump_table;
            write_u64(out, jt.table_base);
            write_u32(out, jt.entry_size);
            write_u32(out, jt.entry_count);
            write_bool(out, jt.relative_entries);
            write_u32(out, static_cast<uint32_t>(jt.targets.size()));
            for (size_t i = 0; i < jt.targets.size(); ++i)
                write_u64(out, jt.targets[i]);
        }
    }

    return out;
}

tl::expected<Core::CompilationUnit, std::string>
SerializationTraits<Core::CompilationUnit>::from_bytes(
    const std::string& bytes,
    std::shared_ptr<const Segmentator::CompilationContext> ctx) {
    Reader rd{bytes.data(), bytes.size()};

    Core::CompilationUnit unit;

    std::string context_file;
    if (!rd.read_string(unit.name) ||
        !rd.read_u64(unit.addr) ||
        !rd.read_u64(unit.size) ||
        !rd.read_bytes(unit.code) ||
        !rd.read_string(unit.enclosing_symbol) ||
        !rd.read_bool(unit.is_canonical) ||
        !rd.read_string(context_file))
        return tl::unexpected(
            std::string("failed to parse CompilationUnit header"));

    // context_hash — fixed 32 bytes
    std::vector<uint8_t> ctx_hash;
    if (!rd.read_raw_bytes(ctx_hash, 32))
        return tl::unexpected(
            std::string("failed to parse CompilationUnit context_hash"));

    unit.context = std::move(ctx);

    // Data references
    uint32_t ref_count;
    if (!rd.read_u32(ref_count))
        return tl::unexpected(
            std::string("failed to parse CompilationUnit data_references count"));

    for (uint32_t i = 0; i < ref_count; ++i) {
        Core::DataReference ref;
        uint32_t kind_u32, tls_u32, source_u32;
        uint32_t aw_u32, ao_u32, aop_u32;
        bool has_jt;

        if (!rd.read_u64(ref.insn_offset) ||
            !rd.read_u64(ref.target_va) ||
            !rd.read_string(ref.target_symbol) ||
            !rd.read_u32(kind_u32) ||
            !rd.read_u32(tls_u32) ||
            !rd.read_u32(source_u32) ||
            !rd.read_u32(ref.access_size) ||
            !rd.read_bool(ref.is_write) ||
            !rd.read_bool(ref.is_pc_relative) ||
            !rd.read_u32(aw_u32) ||
            !rd.read_u32(ao_u32) ||
            !rd.read_u32(aop_u32) ||
            !readRelocation(rd, ref.relocation) ||
            !rd.read_bool(has_jt))
            return tl::unexpected(
                std::string("failed to parse data reference ") +
                std::to_string(i));

        ref.kind = static_cast<Core::DataRefKind>(kind_u32);
        ref.tls_model = static_cast<Core::TlsModel>(tls_u32);
        ref.source = static_cast<Core::DataRefSource>(source_u32);
        ref.atomic_width = static_cast<Core::AtomicWidth>(aw_u32);
        ref.atomic_ordering = static_cast<Core::AtomicOrdering>(ao_u32);
        ref.atomic_op = static_cast<Core::AtomicOp>(aop_u32);

        if (has_jt) {
            Core::JumpTableRef jt;
            uint32_t target_count;
            if (!rd.read_u64(jt.table_base) ||
                !rd.read_u32(jt.entry_size) ||
                !rd.read_u32(jt.entry_count) ||
                !rd.read_bool(jt.relative_entries) ||
                !rd.read_u32(target_count))
                return tl::unexpected(
                    std::string("failed to parse jump table in ref ") +
                    std::to_string(i));
            for (uint32_t j = 0; j < target_count; ++j) {
                uint64_t t;
                if (!rd.read_u64(t))
                    return tl::unexpected(
                        std::string("failed to parse jump table target"));
                jt.targets.push_back(t);
            }
            ref.jump_table = std::move(jt);
        }

        unit.data_references.push_back(std::move(ref));
    }

    return unit;
}

}  // namespace VMPilot::SDK::Serializer
