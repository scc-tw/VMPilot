#ifndef __LOADER_BLOB_SERIALIZER_HPP__
#define __LOADER_BLOB_SERIALIZER_HPP__
#pragma once

#include <LoaderTypes.hpp>
#include <CompilationOutput.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Loader {

/// Serializes compiled bytecodes into a single binary blob suitable for
/// injection into the target binary.
///
/// Blob layout: BlobHeader | BlobEntry[n] | raw bytecode data
class BlobSerializer {
public:
    /// Serialize one or more CompilationOutputs into a binary blob.
    ///
    /// Each output's bytecodes are flattened via Instruction::flatten() and
    /// concatenated.  A BlobHeader + BlobEntry[] directory is prepended.
    [[nodiscard]] static tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
    serialize(const std::vector<SDK::BytecodeCompiler::CompilationOutput>& outputs,
             Common::DiagnosticCollector& diag =
                 Common::DiagnosticCollector::noop()) noexcept;

    /// Validate a blob's header magic and structure.
    [[nodiscard]] static bool
    validate(const std::vector<uint8_t>& blob) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_BLOB_SERIALIZER_HPP__
