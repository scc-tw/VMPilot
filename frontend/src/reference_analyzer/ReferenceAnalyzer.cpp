#include <ReferenceAnalyzer.hpp>

#include "RefMerger.hpp"
#include "SectionLookup.hpp"
#include "layers/InsnLayer.hpp"
#include "layers/PatternLayer.hpp"
#include "layers/RelocationLayer.hpp"
#include "traits/RelocationTraits.hpp"

namespace VMPilot::SDK::ReferenceAnalyzer {

// ---- Primary implementation ----

std::vector<Core::DataReference> analyze(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const AnalysisContext& ctx) noexcept {
    using FileArch = VMPilot::Common::FileArch;
    SectionLookup lookup(ctx.sections);

    if (ctx.arch == FileArch::X86) {
        auto l1 = analyzeRelocations<X86RelocTraits>(
            ctx.text_relocations, region_addr, region_size);
        auto l2 = analyzeInsns<X86InsnTraits>(
            insns, region_addr, region_size, lookup, ctx.symbols);
        auto l3 = analyzePatterns<Segmentator::X86ArchTraits>(
            insns, region_addr, region_size,
            ctx.symbols, lookup, ctx.sections, ctx.mode);
        return RefMerger::merge(
            std::move(l1), std::move(l2), std::move(l3));
    }

    if (ctx.arch == FileArch::ARM64) {
        auto l1 = analyzeRelocations<ARM64RelocTraits>(
            ctx.text_relocations, region_addr, region_size);
        auto l2 = analyzeInsns<ARM64InsnTraits>(
            insns, region_addr, region_size, lookup, ctx.symbols);
        auto l3 = analyzePatterns<Segmentator::ARM64ArchTraits>(
            insns, region_addr, region_size,
            ctx.symbols, lookup, ctx.sections, ctx.mode);
        return RefMerger::merge(
            std::move(l1), std::move(l2), std::move(l3));
    }

    return {};
}

// ---- Compatibility wrapper ----

std::vector<Core::DataReference> analyze(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const std::vector<Core::Section>& sections,
    const std::vector<Core::RelocationEntry>& text_relocations,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept {
    auto ctx = AnalysisContext::build(
        sections, text_relocations, symbols,
        arch, mode);
    return analyze(insns, region_addr, region_size, ctx);
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
