/// @file strategies/pe_dep_strategies.cpp
/// @brief PE import injection strategy implementation.
///
/// Uses coffi-modern coff_editor::imports() builder to add a new
/// import entry for the runtime DLL.

#include <strategies/pe_dep_strategies.hpp>

#include <coffi/coffi.hpp>

#include <string>

namespace VMPilot::Loader::strategies {

using DC = Common::DiagnosticCode;

tl::expected<void, DC>
CoffiImportInject::try_execute(
    Common::DiagnosticCollector& /*diag*/,
    coffi::coff_editor<coffi::pe32_traits>& editor,
    std::string_view dll_name) noexcept
{
    editor.imports().add_symbol(
        std::string(dll_name), "vm_stub_entry", 0);

    return {};
}

}  // namespace VMPilot::Loader::strategies
