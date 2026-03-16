#include <ARM64Handler.hpp>
#include <utilities.hpp>

#include <algorithm>
#include <unordered_map>

#include <capstone.hpp>
#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

// Self-register ARM64 arch handler
static ArchHandlerRegistrar arm64_registrar(
    VMPilot::Common::FileArch::ARM64,
    [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
        return std::make_unique<ARM64Handler>(mode, symbols);
    });

using AddrToSymbol = std::unordered_map<uint64_t, std::string>;

static AddrToSymbol buildAddrLookup(const NativeSymbolTable& symbols) {
    AddrToSymbol lookup;
    for (const auto& entry : symbols) {
        lookup[entry.address] = entry.name;
    }
    return lookup;
}

struct ARM64Handler::Impl {
    Capstone::Capstone cs;
    AddrToSymbol addr_lookup;
    uint64_t base_addr = static_cast<uint64_t>(-1);
    std::vector<Capstone::Instruction> instructions;
    std::vector<std::unique_ptr<NativeFunctionBase>> native_functions;

    Impl(Capstone::Capstone&& cs, AddrToSymbol&& lookup)
        : cs(std::move(cs)), addr_lookup(std::move(lookup)) {}
};

ARM64Handler::ARM64Handler(Mode mode, const NativeSymbolTable& symbols)
    : ArchHandlerStrategy(Arch::ARM64, mode),
      pImpl(make_arm64_handler_impl(mode, symbols)) {}

std::unique_ptr<ARM64Handler::Impl>
VMPilot::SDK::Segmentator::make_arm64_handler_impl(
    Mode mode, const NativeSymbolTable& symbols) {
    return std::make_unique<ARM64Handler::Impl>(
        Capstone::Capstone(Capstone::Arch::ARM64,
                           static_cast<Capstone::Mode>(mode)),
        buildAddrLookup(symbols));
}

ARM64Handler::~ARM64Handler() = default;

bool ARM64Handler::doLoad(const std::vector<uint8_t>& code,
                          const uint64_t base_addr) noexcept {
    auto& impl = this->pImpl;
    impl->base_addr = base_addr;
    impl->instructions = impl->cs.disasm(code, base_addr);
    return !impl->instructions.empty();
}

static const std::string* resolveCallTarget(
    const Capstone::Instruction& insn, const AddrToSymbol& lookup) {
    // ARM64 bl: direct call with immediate target
    uint64_t target = insn.getDirectTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<NativeFunctionBase>>
ARM64Handler::doGetNativeFunctions() noexcept {
    auto& impl = this->pImpl;
    auto& native_functions = impl->native_functions;

    if (!native_functions.empty()) {
        std::vector<std::unique_ptr<NativeFunctionBase>> result;
        result.reserve(native_functions.size());
        for (const auto& nf : native_functions) {
            result.push_back(std::make_unique<NativeFunctionBase>(
                nf->getAddr(), nf->getSize(), nf->getName(), nf->getCode()));
        }
        return result;
    }

    const auto& begin_sig = VMPilot::Common::BEGIN_VMPILOT_SIGNATURE;
    const auto& end_sig = VMPilot::Common::END_VMPILOT_SIGNATURE;
    const auto& instructions = impl->instructions;
    const auto& lookup = impl->addr_lookup;

    struct Region {
        size_t begin_idx;
        size_t end_idx;
    };
    std::vector<Region> regions;
    size_t pending_begin = static_cast<size_t>(-1);

    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto& insn = instructions[i];
        if (!insn.isCall()) continue;

        const auto* sym = resolveCallTarget(insn, lookup);
        if (!sym) continue;

        if (*sym == begin_sig) {
            if (pending_begin != static_cast<size_t>(-1)) {
                spdlog::warn(
                    "Nested VMPilot_Begin at 0x{:x}, previous begin at 0x{:x}",
                    insn.address, instructions[pending_begin].address);
            }
            pending_begin = i;
        } else if (*sym == end_sig) {
            if (pending_begin == static_cast<size_t>(-1)) {
                spdlog::warn("VMPilot_End at 0x{:x} without matching Begin",
                             insn.address);
                continue;
            }
            regions.push_back({pending_begin, i});
            pending_begin = static_cast<size_t>(-1);
        }
    }

    if (pending_begin != static_cast<size_t>(-1)) {
        spdlog::warn("Unmatched VMPilot_Begin at 0x{:x}",
                     instructions[pending_begin].address);
    }

    for (const auto& region : regions) {
        const auto& begin_insn = instructions[region.begin_idx];
        const auto& end_insn = instructions[region.end_idx];

        uint64_t start_addr = begin_insn.address;
        uint64_t end_addr = end_insn.address + end_insn.size;
        uint64_t size = end_addr - start_addr;

        std::vector<uint8_t> code;
        code.reserve(size);
        for (size_t i = region.begin_idx; i <= region.end_idx; ++i) {
            const auto& insn = instructions[i];
            code.insert(code.end(), insn.bytes.begin(), insn.bytes.end());
        }

        std::string name = "vmpilot_region_0x" + ([&] {
                               char buf[17];
                               snprintf(buf, sizeof(buf), "%lx", start_addr);
                               return std::string(buf);
                           })();

        native_functions.push_back(std::make_unique<NativeFunctionBase>(
            start_addr, size, std::move(name), std::move(code)));

        spdlog::info("Found protected region: 0x{:x} - 0x{:x} ({} bytes)",
                     start_addr, end_addr, size);
    }

    std::vector<std::unique_ptr<NativeFunctionBase>> result;
    result.reserve(native_functions.size());
    for (const auto& nf : native_functions) {
        result.push_back(std::make_unique<NativeFunctionBase>(
            nf->getAddr(), nf->getSize(), nf->getName(), nf->getCode()));
    }
    return result;
}
