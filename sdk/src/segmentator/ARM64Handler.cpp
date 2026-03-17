#include <ARM64Handler.hpp>

#include "ArchHandlerCommon.hpp"

#include <arm64.hpp>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

// Self-register ARM64 arch handler
static ArchHandlerRegistrar arm64_registrar(
    VMPilot::Common::FileArch::ARM64,
    [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
        return std::make_unique<ARM64Handler>(mode, symbols);
    });

struct ARM64Handler::Impl : ArchHandlerImplBase {
    using ArchHandlerImplBase::ArchHandlerImplBase;
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
    pImpl->base_addr = base_addr;
    pImpl->instructions = pImpl->cs.disasm(code, base_addr);
    return !pImpl->instructions.empty();
}

// ARM64-specific call resolver: handles direct bl only
static std::optional<std::string> arm64ResolveCallTarget(
    const Capstone::Instruction& insn, const AddrToSymbol& lookup) {
    uint64_t target = insn.getDirectTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end())
            return it->second;
    }
    return std::nullopt;
}

// ---------------------------------------------------------------------------
// Backward constant propagation: resolve the constant value held in `reg`
// by scanning instructions backwards and following data flow through:
//   - ADRP reg, #page             (page-aligned PC-relative)
//   - ADR  reg, #addr             (small PC-relative offset)
//   - ADD  reg, src, #imm         (page offset or arithmetic adjustment)
//   - SUB  reg, src, #imm         (arithmetic adjustment)
//   - MOV  reg, reg               (register forwarding, arbitrary chain)
// Respects BL/RET boundaries via callee-saved register awareness.
// ---------------------------------------------------------------------------
static uint64_t resolveRegValue(
    unsigned reg, size_t from_idx,
    const std::vector<Capstone::Instruction>& instructions, size_t min_idx,
    int depth = 0) {
    namespace CA = Capstone::ARM64;

    constexpr int kMaxDepth = 5;
    if (depth >= kMaxDepth || from_idx >= instructions.size())
        return 0;

    for (size_t i = from_idx + 1; i-- > min_idx;) {
        const auto& insn = instructions[i];

        // BL/RET clobber caller-saved registers (x0-x18)
        if (insn.isCall() || insn.isRet()) {
            if (!CA::isCalleeSaved(reg))
                return 0;
            continue;
        }

        // Skip instructions that don't write to our register
        if (!CA::writesToReg(insn, reg))
            continue;

        // --- Terminal patterns ---

        // adrp reg, #page
        if (CA::isADRP(insn))
            return static_cast<uint64_t>(insn.operands[1].imm);

        // adr reg, #addr
        if (CA::isADR(insn))
            return static_cast<uint64_t>(insn.operands[1].imm);

        // --- Forwarding patterns ---

        // mov reg, src_reg (handles MOV and ORR alias)
        if (CA::isRegToRegMov(insn))
            return resolveRegValue(CA::getMovSource(insn),
                                   i > 0 ? i - 1 : 0, instructions, min_idx,
                                   depth + 1);

        // add reg, src, #imm
        if (CA::isRegPlusImmADD(insn)) {
            unsigned src = insn.operands[1].reg;
            int64_t imm = insn.operands[2].imm;
            // src may be the same or different register
            size_t prev = i > 0 ? i - 1 : 0;
            uint64_t src_val =
                (src == reg)
                    ? resolveRegValue(reg, prev, instructions, min_idx,
                                      depth + 1)
                    : resolveRegValue(src, prev, instructions, min_idx,
                                      depth + 1);
            if (src_val != 0)
                return static_cast<uint64_t>(
                    static_cast<int64_t>(src_val) + imm);
            return 0;
        }

        // sub reg, src, #imm
        if (CA::isRegMinusImmSUB(insn)) {
            unsigned src = insn.operands[1].reg;
            int64_t imm = insn.operands[2].imm;
            size_t prev = i > 0 ? i - 1 : 0;
            uint64_t src_val =
                (src == reg)
                    ? resolveRegValue(reg, prev, instructions, min_idx,
                                      depth + 1)
                    : resolveRegValue(src, prev, instructions, min_idx,
                                      depth + 1);
            if (src_val != 0)
                return static_cast<uint64_t>(
                    static_cast<int64_t>(src_val) - imm);
            return 0;
        }

        // Unknown write to register → can't resolve
        return 0;
    }

    return 0;
}

// Extract the VA of the first string argument near a BL instruction.
// Uses backward constant propagation to handle arbitrary compiler code
// generation patterns including instruction scheduling, register spilling,
// ADRP+ADD pairs with intervening instructions, and cross-register variants.
static uint64_t arm64ExtractStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions) {
    if (call_idx == 0)
        return 0;

    constexpr size_t kMaxWindow = 20;
    const size_t start = (call_idx > kMaxWindow) ? call_idx - kMaxWindow : 0;

    // Resolve X0 (AAPCS64 first argument register) at the call site.
    return resolveRegValue(Capstone::ARM64::firstArgReg(), call_idx - 1,
                           instructions, start);
}

std::vector<std::unique_ptr<NativeFunctionBase>>
ARM64Handler::doGetNativeFunctions() noexcept {
    pImpl->compilation_ctx = &m_compilation_ctx;
    return extractNativeFunctions(*pImpl, arm64ResolveCallTarget,
                                  arm64ExtractStringArg);
}
