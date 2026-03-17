#ifndef __SDK_ARM64_HANDLER_HPP__
#define __SDK_ARM64_HANDLER_HPP__
#pragma once

#include <Strategy.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace VMPilot::SDK::Segmentator {

class ARM64Handler : public ArchHandlerStrategy {
   public:
    ARM64Handler(Mode mode, const NativeSymbolTable& symbols);
    virtual ~ARM64Handler();

   private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    friend std::unique_ptr<Impl> make_arm64_handler_impl(
        Mode mode, const NativeSymbolTable& symbols);

   protected:
    virtual bool doLoad(const std::vector<uint8_t>& code,
                        const uint64_t base_addr) noexcept override;

    virtual std::vector<NativeFunctionBase>
    doGetNativeFunctions() noexcept override;
};

std::unique_ptr<ARM64Handler::Impl> make_arm64_handler_impl(
    Mode mode, const NativeSymbolTable& symbols);

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARM64_HANDLER_HPP__
