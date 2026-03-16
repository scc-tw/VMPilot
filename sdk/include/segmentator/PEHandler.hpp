#ifndef __SDK_PE_HANDLER_HPP__
#define __SDK_PE_HANDLER_HPP__
#pragma once

#include <Strategy.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// PE file handler.
///
/// Unlike ELF (PLT section) and Mach-O (__stubs section), PE/COFF has no
/// dedicated stub section. MSVC links import thunks (`jmp [IAT]`, encoded
/// as `FF 25 <disp32>`) inline in the `.text` section. Therefore
/// doGetStubCallTargets() scans `.text` for `FF 25` byte patterns to
/// discover them, rather than reading a separate section.
class PEFileHandlerStrategy : public FileHandlerStrategy {
   private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    friend std::unique_ptr<Impl> make_pe_impl(const std::string& filename);

   public:
    explicit PEFileHandlerStrategy(const std::string& filename);
    virtual ~PEFileHandlerStrategy();

   protected:
    virtual std::vector<uint8_t> doGetTextSection() noexcept override;
    virtual uint64_t doGetTextBaseAddr() noexcept override;

    virtual NativeSymbolTable doGetSymbols() noexcept override;
    virtual std::vector<CallTarget> doGetStubCallTargets() noexcept override;
    virtual std::vector<CallTarget> doGetPointerTableTargets() noexcept
        override;

   private:
    void parseImports() noexcept;
    const char* rvaToPtr(uint32_t rva) const noexcept;
};

std::unique_ptr<PEFileHandlerStrategy::Impl> make_pe_impl(
    const std::string& filename);

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_PE_HANDLER_HPP__
