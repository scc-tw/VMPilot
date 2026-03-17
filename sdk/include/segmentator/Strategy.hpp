#ifndef __SDK_SEGMENTATOR_STRATEGY_HPP__
#define __SDK_SEGMENTATOR_STRATEGY_HPP__
#pragma once

#include <ArchEnum.hpp>
#include <CompilationContext.hpp>
#include <ModeEnum.hpp>
#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Format-neutral representation of a callable target address
/// (ELF PLT entry, PE IAT thunk, Mach-O __stub, or GOT/IAT pointer)
struct CallTarget {
    std::string name;
    uint64_t address;
    uint64_t size;
};

/// Strategy for file handling.
/// Subclass constructors throw on parse failure (e.g. file not found,
/// corrupt header). After successful construction, virtual methods
/// never throw — they return empty/sentinel values on failure.
class FileHandlerStrategy {
   protected:
    virtual std::vector<uint8_t> doGetTextSection() noexcept;
    virtual uint64_t doGetTextBaseAddr() noexcept;

    /// Read-only data sections for string/data resolution.
    /// (ELF: .rodata, PE: .rdata, Mach-O: __TEXT,__const + __TEXT,__cstring)
    /// Used by __FUNCTION__ extraction and BytecodeCompiler.
    virtual std::vector<ReadOnlySection> doGetReadOnlySections() noexcept;

    /// Collect function symbols from the binary
    /// (ELF: .dynsym, PE: export table, Mach-O: LC_SYMTAB)
    virtual NativeSymbolTable doGetSymbols() noexcept;

    /// Collect stub/thunk addresses that map to a symbol
    /// (ELF: PLT entries, PE: import thunks, Mach-O: __stubs)
    virtual std::vector<CallTarget> doGetStubCallTargets() noexcept;

    /// Collect pointer-table addresses that map to a symbol
    /// (ELF: GOT entries, PE: IAT entries, Mach-O: __got / __la_symbol_ptr)
    virtual std::vector<CallTarget> doGetPointerTableTargets() noexcept;

   public:
    virtual ~FileHandlerStrategy() = default;
    std::vector<uint8_t> getTextSection();
    uint64_t getTextBaseAddr();
    std::vector<ReadOnlySection> getReadOnlySections();

    /// Assemble a complete symbol table from the three sources above.
    /// Not virtual — subclasses override doGetSymbols/Direct/Indirect instead.
    NativeSymbolTable getNativeSymbolTable();
};

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
    virtual std::vector<std::unique_ptr<NativeFunctionBase>>
    doGetNativeFunctions();

   public:
    virtual ~ArchHandlerStrategy() = default;
    ArchHandlerStrategy();
    ArchHandlerStrategy(Arch arch, Mode mode);

    void setCompilationContext(CompilationContext ctx);
    const CompilationContext& getCompilationContext() const;

    [[nodiscard]] bool Load(const std::vector<uint8_t>& code,
                            const uint64_t base_addr);
    std::vector<std::unique_ptr<NativeFunctionBase>> getNativeFunctions();
};

using FileHandlerFactory =
    std::function<std::unique_ptr<FileHandlerStrategy>(const std::string&)>;
using ArchHandlerFactory = std::function<std::unique_ptr<ArchHandlerStrategy>(
    VMPilot::Common::FileMode, const NativeSymbolTable&)>;

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

#endif  // __SDK_SEGMENTATOR_STRATEGY_HPP__
