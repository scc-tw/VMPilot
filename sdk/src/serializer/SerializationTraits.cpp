#include <SerializationTraits.hpp>

#include <DataReference.hpp>
#include <Section.hpp>
#include <VMPilot_crypto.hpp>
#include <vmpilot.pb.h>

namespace VMPilot::SDK::Serializer {

// ---------------------------------------------------------------------------
// CompilationContext
// ---------------------------------------------------------------------------

tl::expected<std::string, std::string>
SerializationTraits<Segmentator::CompilationContext>::to_bytes(
    const Segmentator::CompilationContext& ctx) {
    vmpilot::CompilationContext pb;
    pb.set_arch(static_cast<uint32_t>(ctx.arch));
    pb.set_mode(static_cast<uint32_t>(ctx.mode));

    for (const auto& sym : ctx.symbols) {
        auto* s = pb.add_symbols();
        s->set_name(sym.name);
        s->set_address(sym.address);
        s->set_size(sym.size);
        s->set_entry_type(
            sym.getAttribute<std::string>("entry_type", std::string()));
        s->set_is_global(sym.isGlobal);
    }

    for (const auto& sec : ctx.sections) {
        auto* s = pb.add_all_sections();
        s->set_base_addr(sec.base_addr);
        s->set_size(sec.size);
        s->set_kind(static_cast<uint32_t>(sec.kind));
        s->set_name(sec.name);
        if (sec.has_data())
            s->set_data(sec.data.data(), sec.data.size());
    }

    std::string bytes;
    if (!pb.SerializeToString(&bytes))
        return tl::unexpected(
            std::string("failed to serialize CompilationContext"));
    return bytes;
}

tl::expected<Segmentator::CompilationContext, std::string>
SerializationTraits<Segmentator::CompilationContext>::from_bytes(
    const std::string& bytes) {
    vmpilot::CompilationContext pb;
    if (!pb.ParseFromString(bytes))
        return tl::unexpected(
            std::string("failed to parse CompilationContext"));

    Segmentator::CompilationContext ctx;
    ctx.arch = static_cast<Segmentator::Arch>(pb.arch());
    ctx.mode = static_cast<Segmentator::Mode>(pb.mode());

    for (int i = 0; i < pb.symbols_size(); ++i) {
        const auto& s = pb.symbols(i);
        Segmentator::NativeSymbolTableEntry entry;
        entry.name = s.name();
        entry.address = s.address();
        entry.size = s.size();
        entry.isGlobal = s.is_global();
        if (!s.entry_type().empty())
            entry.setAttribute("entry_type", s.entry_type());
        ctx.symbols.push_back(std::move(entry));
    }

    for (int i = 0; i < pb.all_sections_size(); ++i) {
        const auto& s = pb.all_sections(i);
        Core::Section sec;
        sec.base_addr = s.base_addr();
        sec.size = s.size();
        sec.kind = static_cast<Core::SectionKind>(s.kind());
        sec.name = s.name();
        if (!s.data().empty())
            sec.data.assign(s.data().begin(), s.data().end());
        ctx.sections.push_back(std::move(sec));
    }

    return ctx;
}

// ---------------------------------------------------------------------------
// CompilationUnit
// ---------------------------------------------------------------------------

namespace {

void serializeRelocationEntry(vmpilot::RelocationEntry* pb,
                               const Core::RelocationEntry& entry) {
    pb->set_offset(entry.offset);
    pb->set_type(entry.type);
    pb->set_symbol_index(entry.symbol_index);
    pb->set_addend(entry.addend);
    pb->set_symbol_name(entry.symbol_name);
}

Core::RelocationEntry deserializeRelocationEntry(
    const vmpilot::RelocationEntry& pb) {
    Core::RelocationEntry entry;
    entry.offset = pb.offset();
    entry.type = pb.type();
    entry.symbol_index = pb.symbol_index();
    entry.addend = pb.addend();
    entry.symbol_name = pb.symbol_name();
    return entry;
}

}  // namespace

tl::expected<std::string, std::string>
SerializationTraits<Core::CompilationUnit>::to_bytes(
    const Core::CompilationUnit& unit) {
    vmpilot::CompilationUnit pb;
    pb.set_name(unit.name);
    pb.set_addr(unit.addr);
    pb.set_size(unit.size);
    pb.set_code(unit.code.data(), unit.code.size());
    pb.set_enclosing_symbol(unit.enclosing_symbol);
    pb.set_is_canonical(unit.is_canonical);

    // context_file and context_hash are set by the dump() layer,
    // not here — they are filesystem-level metadata.
    pb.set_context_file("context.pb");

    // Compute context hash if context is available
    if (unit.context) {
        auto ctx_bytes = SerializationTraits<Segmentator::CompilationContext>
                             ::to_bytes(*unit.context);
        if (ctx_bytes.has_value()) {
            auto hash = VMPilot::Crypto::SHA256(
                std::vector<uint8_t>(ctx_bytes->begin(), ctx_bytes->end()),
                /*salt=*/{});
            pb.set_context_hash(hash.data(), hash.size());
        }
    }

    for (const auto& ref : unit.data_references) {
        auto* r = pb.add_data_references();
        r->set_insn_offset(ref.insn_offset);
        r->set_target_va(ref.target_va);
        r->set_target_symbol(ref.target_symbol);
        r->set_kind(static_cast<uint32_t>(ref.kind));
        r->set_tls_model(static_cast<uint32_t>(ref.tls_model));
        r->set_source(static_cast<uint32_t>(ref.source));
        r->set_access_size(ref.access_size);
        r->set_is_write(ref.is_write);
        r->set_is_pc_relative(ref.is_pc_relative);
        r->set_atomic_width(static_cast<uint32_t>(ref.atomic_width));
        r->set_atomic_ordering(static_cast<uint32_t>(ref.atomic_ordering));
        r->set_atomic_op(static_cast<uint32_t>(ref.atomic_op));
        serializeRelocationEntry(r->mutable_relocation(), ref.relocation);

        if (ref.jump_table.has_value()) {
            auto* jt = r->mutable_jump_table();
            jt->set_table_base(ref.jump_table->table_base);
            jt->set_entry_size(ref.jump_table->entry_size);
            jt->set_entry_count(ref.jump_table->entry_count);
            jt->set_relative_entries(ref.jump_table->relative_entries);
            for (uint64_t target : ref.jump_table->targets)
                jt->add_targets(target);
        }
    }

    std::string bytes;
    if (!pb.SerializeToString(&bytes))
        return tl::unexpected(
            std::string("failed to serialize CompilationUnit: ") + unit.name);
    return bytes;
}

tl::expected<Core::CompilationUnit, std::string>
SerializationTraits<Core::CompilationUnit>::from_bytes(
    const std::string& bytes,
    std::shared_ptr<const Segmentator::CompilationContext> ctx) {
    vmpilot::CompilationUnit pb;
    if (!pb.ParseFromString(bytes))
        return tl::unexpected(
            std::string("failed to parse CompilationUnit"));

    Core::CompilationUnit unit;
    unit.name = pb.name();
    unit.addr = pb.addr();
    unit.size = pb.size();
    unit.code.assign(pb.code().begin(), pb.code().end());
    unit.enclosing_symbol = pb.enclosing_symbol();
    unit.is_canonical = pb.is_canonical();
    unit.context = std::move(ctx);

    for (int i = 0; i < pb.data_references_size(); ++i) {
        const auto& r = pb.data_references(i);
        Core::DataReference ref;
        ref.insn_offset = r.insn_offset();
        ref.target_va = r.target_va();
        ref.target_symbol = r.target_symbol();
        ref.kind = static_cast<Core::DataRefKind>(r.kind());
        ref.tls_model = static_cast<Core::TlsModel>(r.tls_model());
        ref.source = static_cast<Core::DataRefSource>(r.source());
        ref.access_size = r.access_size();
        ref.is_write = r.is_write();
        ref.is_pc_relative = r.is_pc_relative();
        ref.atomic_width = static_cast<Core::AtomicWidth>(r.atomic_width());
        ref.atomic_ordering =
            static_cast<Core::AtomicOrdering>(r.atomic_ordering());
        ref.atomic_op = static_cast<Core::AtomicOp>(r.atomic_op());
        ref.relocation = deserializeRelocationEntry(r.relocation());

        if (r.has_jump_table()) {
            const auto& jt_pb = r.jump_table();
            Core::JumpTableRef jt;
            jt.table_base = jt_pb.table_base();
            jt.entry_size = jt_pb.entry_size();
            jt.entry_count = jt_pb.entry_count();
            jt.relative_entries = jt_pb.relative_entries();
            for (int j = 0; j < jt_pb.targets_size(); ++j)
                jt.targets.push_back(jt_pb.targets(j));
            ref.jump_table = std::move(jt);
        }

        unit.data_references.push_back(std::move(ref));
    }

    return unit;
}

}  // namespace VMPilot::SDK::Serializer
