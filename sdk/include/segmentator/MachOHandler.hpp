#ifndef __SDK_MACHO_HANDLER_HPP__
#define __SDK_MACHO_HANDLER_HPP__
#pragma once

#include <Strategy.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

class MachOFileHandlerStrategy : public FileHandlerStrategy {
   private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    friend std::unique_ptr<Impl> make_macho_impl(const std::string& filename);

   public:
    explicit MachOFileHandlerStrategy(const std::string& filename);
    virtual ~MachOFileHandlerStrategy();

   protected:
    virtual std::vector<uint8_t> doGetTextSection() noexcept override;
    virtual uint64_t doGetTextBaseAddr() noexcept override;

    virtual NativeSymbolTable doGetSymbols() noexcept override;
    virtual std::vector<CallTarget> doGetStubCallTargets() noexcept override;
    virtual std::vector<CallTarget>
    doGetPointerTableTargets() noexcept override;
};

std::unique_ptr<MachOFileHandlerStrategy::Impl> make_macho_impl(
    const std::string& filename);

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_MACHO_HANDLER_HPP__
