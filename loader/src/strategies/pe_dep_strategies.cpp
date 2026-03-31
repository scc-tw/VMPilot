/// @file strategies/pe_dep_strategies.cpp
/// @brief PE import injection strategy implementation.
///
/// Uses COFFI import_section_accessor to rebuild the import directory
/// with a new entry for the runtime DLL.

#include <strategies/pe_dep_strategies.hpp>

#include <coffi/coffi.hpp>

#include <string>

namespace VMPilot::Loader::strategies {

using DC = Common::DiagnosticCode;

tl::expected<void, DC>
CoffiImportInject::try_execute(
    Common::DiagnosticCollector& diag,
    COFFI::coffi& reader,
    std::string_view dll_name) noexcept
{
    COFFI::import_section_accessor imports(reader);

    if (!imports.add_import(std::string(dll_name), "vm_stub_entry")) {
        diag.note("CoffiImportInject", DC::PatchRuntimeDepFailed,
                  std::string("failed to inject PE import for '")
                  + std::string(dll_name) + "'");
        return tl::make_unexpected(DC::PatchRuntimeDepFailed);
    }

    return {};
}

}  // namespace VMPilot::Loader::strategies
