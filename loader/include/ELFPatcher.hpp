#ifndef __LOADER_ELF_PATCHER_HPP__
#define __LOADER_ELF_PATCHER_HPP__
#pragma once

#include <BinaryPatcher.hpp>

namespace VMPilot::Loader {

/// ELF-specific binary patcher using the ELFIO library.
///
/// Strategy:
///   1. Load the original ELF with ELFIO.
///   2. Serialize compiled bytecodes into a BlobHeader + entries + data.
///   3. Generate per-region entry stubs.
///   4. Append a new PT_LOAD segment containing the blob + stubs.
///   5. Overwrite each protected region's first bytes with JMP rel32
///      (x86) or B imm26 (ARM64) to the corresponding entry stub.
///   6. Save the modified ELF.
class ELFPatcher : public BinaryPatcher {
public:
    [[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] std::string name() const noexcept override { return "ELF"; }
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_ELF_PATCHER_HPP__
