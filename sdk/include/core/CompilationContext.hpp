#ifndef __SDK_SEGMENTATOR_COMPILATION_CONTEXT_HPP__
#define __SDK_SEGMENTATOR_COMPILATION_CONTEXT_HPP__
#pragma once

#include <ArchEnum.hpp>
#include <ModeEnum.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>
#include <SectionInfo.hpp>

#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Shared context for all regions extracted from the same binary.
/// Populated by Segmentator, consumed by:
///   - ArchHandler (__FUNCTION__ name resolution)
///   - ReferenceAnalyzer (data/TLS reference detection)
///   - BytecodeCompiler (jump table + constant data, future)
struct CompilationContext {
    NativeSymbolTable symbols;
    std::vector<ReadOnlySection> rodata_sections;
    std::vector<Core::SectionInfo> all_sections;
    Arch arch;
    Mode mode;
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_SEGMENTATOR_COMPILATION_CONTEXT_HPP__
