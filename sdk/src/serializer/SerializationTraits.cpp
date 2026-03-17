#include <SerializationTraits.hpp>

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

    for (const auto& sec : ctx.rodata_sections) {
        auto* r = pb.add_rodata_sections();
        r->set_base_addr(sec.base_addr);
        r->set_data(sec.data.data(), sec.data.size());
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

    for (int i = 0; i < pb.rodata_sections_size(); ++i) {
        const auto& r = pb.rodata_sections(i);
        Segmentator::ReadOnlySection sec;
        sec.base_addr = r.base_addr();
        sec.data.assign(r.data().begin(), r.data().end());
        ctx.rodata_sections.push_back(std::move(sec));
    }

    return ctx;
}

// ---------------------------------------------------------------------------
// CompilationUnit
// ---------------------------------------------------------------------------

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

    return unit;
}

}  // namespace VMPilot::SDK::Serializer
