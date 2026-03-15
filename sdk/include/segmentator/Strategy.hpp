#ifndef __SDK_SEGMENTATOR_STRATEGY_HPP__
#define __SDK_SEGMENTATOR_STRATEGY_HPP__
#pragma once

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>
#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace VMPilot::SDK::Segmentator {

// Strategy for file handling
class FileHandlerStrategy {
   protected:
    virtual std::pair<uint64_t, uint64_t> doGetBeginEndAddr() noexcept;
    virtual std::vector<uint8_t> doGetTextSection() noexcept;
    virtual uint64_t doGetTextBaseAddr() noexcept;
    virtual NativeSymbolTable doGetNativeSymbolTable() noexcept;

   public:
    virtual ~FileHandlerStrategy() = default;
    std::pair<uint64_t, uint64_t> getBeginEndAddr();
    std::vector<uint8_t> getTextSection();
    uint64_t getTextBaseAddr();
    NativeSymbolTable getNativeSymbolTable();
};

// Strategy for architecture handling
class ArchHandlerStrategy {
   protected:
    Arch m_arch;
    Mode m_mode;

    virtual bool doLoad(const std::vector<uint8_t>& code,
                        const uint64_t base_addr);
    virtual std::vector<std::unique_ptr<NativeFunctionBase>>
    doGetNativeFunctions();

   public:
    virtual ~ArchHandlerStrategy() = default;
    ArchHandlerStrategy();
    ArchHandlerStrategy(Arch arch, Mode mode);

    bool Load(const std::vector<uint8_t>& code, const uint64_t base_addr);
    std::vector<std::unique_ptr<NativeFunctionBase>> getNativeFunctions();
};


using FileHandlerFactory =
    std::function<std::unique_ptr<FileHandlerStrategy>(const std::string&)>;
using ArchHandlerFactory =
    std::function<std::unique_ptr<ArchHandlerStrategy>(
        VMPilot::Common::FileMode, const NativeSymbolTable&)>;

class HandlerRegistry {
   public:
    static HandlerRegistry& instance();

    void registerFileHandler(VMPilot::Common::FileFormat format,
                             FileHandlerFactory factory);
    void registerArchHandler(VMPilot::Common::FileArch arch,
                             ArchHandlerFactory factory);

    std::unique_ptr<FileHandlerStrategy> createFileHandler(
        VMPilot::Common::FileFormat format,
        const std::string& filename) const;
    std::unique_ptr<ArchHandlerStrategy> createArchHandler(
        VMPilot::Common::FileArch arch, VMPilot::Common::FileMode mode,
        const NativeSymbolTable& symbols) const;

   private:
    HandlerRegistry() = default;
    std::unordered_map<VMPilot::Common::FileFormat, FileHandlerFactory>
        file_handlers_;
    std::unordered_map<VMPilot::Common::FileArch, ArchHandlerFactory>
        arch_handlers_;
};

// Helper for self-registration from handler .cpp files
struct FileHandlerRegistrar {
    FileHandlerRegistrar(VMPilot::Common::FileFormat format,
                         FileHandlerFactory factory);
};

struct ArchHandlerRegistrar {
    ArchHandlerRegistrar(VMPilot::Common::FileArch arch,
                         ArchHandlerFactory factory);
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_SEGMENTATOR_STRATEGY_HPP__
