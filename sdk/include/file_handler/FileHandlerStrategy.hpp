#ifndef __SDK_FILE_HANDLER_STRATEGY_HPP__
#define __SDK_FILE_HANDLER_STRATEGY_HPP__
#pragma once

#include <DataReference.hpp>
#include <NativeFunctionBase.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>
#include <SectionInfo.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
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
/// never throw --- they return empty/sentinel values on failure.
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

    /// Return compiler/linker identification string from the binary.
    /// (ELF: .comment section, PE: linker version, MachO: CPU type fallback)
    virtual std::string doGetCompilerInfo() noexcept;

    /// Return all sections with classified kinds for reference analysis.
    virtual std::vector<Core::SectionInfo> doGetAllSections() noexcept;

    /// Return .rela.text / .rel.text relocation entries for reference analysis.
    virtual std::vector<Core::RelocationEntry> doGetTextRelocations() noexcept;

   public:
    virtual ~FileHandlerStrategy() = default;
    std::vector<uint8_t> getTextSection();
    uint64_t getTextBaseAddr();
    std::vector<ReadOnlySection> getReadOnlySections();

    /// Assemble a complete symbol table from the three sources above.
    /// Not virtual --- subclasses override doGetSymbols/Direct/Indirect instead.
    NativeSymbolTable getNativeSymbolTable();

    /// Return compiler/linker identification string.
    std::string getCompilerInfo();

    /// Return all sections with classified kinds.
    std::vector<Core::SectionInfo> getAllSections();

    /// Return text relocation entries.
    std::vector<Core::RelocationEntry> getTextRelocations();
};

using FileHandlerFactory =
    std::function<std::unique_ptr<FileHandlerStrategy>(const std::string&)>;

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_FILE_HANDLER_STRATEGY_HPP__
