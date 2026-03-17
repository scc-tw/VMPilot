#ifndef __SDK_ELF_HANDLER_HPP__
#define __SDK_ELF_HANDLER_HPP__
#pragma once

#include <FileHandlerStrategy.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::Segmentator {

class ELFFileHandlerStrategy : public FileHandlerStrategy {
   private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    friend std::unique_ptr<Impl> make_elf_impl(const std::string& filename);

   public:
    explicit ELFFileHandlerStrategy(const std::string& filename);
    virtual ~ELFFileHandlerStrategy();

   protected:
    virtual std::vector<uint8_t> doGetTextSection() noexcept override;
    virtual uint64_t doGetTextBaseAddr() noexcept override;
    virtual std::vector<ReadOnlySection> doGetReadOnlySections() noexcept
        override;

    virtual NativeSymbolTable doGetSymbols() noexcept override;
    virtual std::vector<CallTarget> doGetStubCallTargets() noexcept override;
    virtual std::vector<CallTarget> doGetPointerTableTargets() noexcept
        override;
    virtual std::string doGetCompilerInfo() noexcept override;

   private:
    /// dynsym index -> (rela.plt index, GOT address)
    struct RelaInfo {
        uint64_t rela_idx;
        uint64_t got_addr;
    };

    /// Build map from .rela.plt: dynsym_index -> RelaInfo
    std::unordered_map<uint64_t, RelaInfo> buildRelocationMap() noexcept;

    std::vector<uint8_t> doGetTextSectionIntl() noexcept;
};

std::unique_ptr<ELFFileHandlerStrategy::Impl> make_elf_impl(
    const std::string& filename);

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ELF_HANDLER_HPP__
