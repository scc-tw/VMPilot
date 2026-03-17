#ifndef __SDK_SEGMENTATOR_HANDLER_REGISTRY_HPP__
#define __SDK_SEGMENTATOR_HANDLER_REGISTRY_HPP__
#pragma once

#include <ArchHandlerStrategy.hpp>
#include <FileHandlerStrategy.hpp>

#include <unordered_map>

namespace VMPilot::SDK::Segmentator {

class HandlerRegistry {
   public:
    static HandlerRegistry& instance();

    void registerFileHandler(VMPilot::Common::FileFormat format,
                             FileHandlerFactory factory);
    void registerArchHandler(VMPilot::Common::FileArch arch,
                             ArchHandlerFactory factory);

    std::unique_ptr<FileHandlerStrategy> createFileHandler(
        VMPilot::Common::FileFormat format, const std::string& filename) const;
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

#endif  // __SDK_SEGMENTATOR_HANDLER_REGISTRY_HPP__
