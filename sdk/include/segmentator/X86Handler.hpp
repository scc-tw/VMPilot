#ifndef __SDK_X86_HANDLER_HPP__
#define __SDK_X86_HANDLER_HPP__
#pragma once

#include <Strategy.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace VMPilot::SDK::Segmentator {
class X86Handler : public ArchHandlerStrategy {
   public:
    X86Handler(Mode mode, const NativeSymbolTable& symbols);
    virtual ~X86Handler();

   private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    friend std::unique_ptr<Impl> make_x86_handler_impl(
        Mode mode, const NativeSymbolTable& symbols);

   protected:
    virtual bool doLoad(const std::vector<uint8_t>& code,
                        const uint64_t base_addr) noexcept override;

    virtual std::vector<NativeFunctionBase>
    doGetNativeFunctions() noexcept override;
};

std::unique_ptr<X86Handler::Impl> make_x86_handler_impl(
    Mode mode, const NativeSymbolTable& symbols);

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_X86_HANDLER_HPP__
