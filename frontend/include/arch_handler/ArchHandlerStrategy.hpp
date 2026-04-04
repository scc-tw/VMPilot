#ifndef __SDK_ARCH_HANDLER_STRATEGY_HPP__
#define __SDK_ARCH_HANDLER_STRATEGY_HPP__
#pragma once

#include <ArchEnum.hpp>
#include <CompilationContext.hpp>
#include <ModeEnum.hpp>
#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Strategy for architecture handling.
/// ArchHandlers never throw. Load() returns false on failure;
/// getNativeFunctions() returns empty on failure.
class ArchHandlerStrategy {
   protected:
    Arch m_arch;
    Mode m_mode;
    CompilationContext m_compilation_ctx;

    virtual bool doLoad(const std::vector<uint8_t>& code,
                        const uint64_t base_addr);
    virtual std::vector<NativeFunctionBase>
    doGetNativeFunctions();

   public:
    virtual ~ArchHandlerStrategy() = default;
    ArchHandlerStrategy();
    ArchHandlerStrategy(Arch arch, Mode mode);

    void setCompilationContext(CompilationContext ctx);
    const CompilationContext& getCompilationContext() const;

    [[nodiscard]] bool Load(const std::vector<uint8_t>& code,
                            const uint64_t base_addr);
    std::vector<NativeFunctionBase> getNativeFunctions();
};

using ArchHandlerFactory = std::function<std::unique_ptr<ArchHandlerStrategy>(
    VMPilot::Common::FileMode, const NativeSymbolTable&)>;

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARCH_HANDLER_STRATEGY_HPP__
