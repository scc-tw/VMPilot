#include <CompilationOrchestrator.hpp>
#include <thread_pool.hpp>

#include <future>
#include <vector>

namespace VMPilot::SDK::BytecodeCompiler {

using Common::DiagnosticCode;

CompilationOrchestrator::CompilationOrchestrator(
    std::unique_ptr<CompilerBackend> backend,
    CompileConfig config,
    size_t num_threads)
    : backend_(std::move(backend)),
      config_(std::move(config)),
      num_threads_(num_threads) {}

tl::expected<CompilationResult, DiagnosticCode>
CompilationOrchestrator::compile(
    const std::vector<Core::CompilationUnit>& units,
    Common::DiagnosticCollector& diag) noexcept {
    if (!backend_) {
        diag.error("compiler", DiagnosticCode::NullBackend,
                   "no compiler backend provided");
        return tl::unexpected(DiagnosticCode::NullBackend);
    }

    CompilationResult comp_result;
    comp_result.total_units = units.size();

    if (units.empty())
        return comp_result;

    Common::ThreadPool pool(num_threads_);
    using FutureResult = std::future<tl::expected<CompilationOutput, DiagnosticCode>>;
    std::vector<FutureResult> futures;
    futures.reserve(units.size());

    for (const auto& unit : units) {
        futures.push_back(pool.submit(
            [this, &unit, &diag]()
                -> tl::expected<CompilationOutput, DiagnosticCode> {
                return backend_->compile_unit(unit, config_, diag);
            }));
    }

    pool.wait_all();

    for (auto& fut : futures) {
        try {
            auto res = fut.get();
            if (res.has_value()) {
                comp_result.outputs.push_back(std::move(res.value()));
            } else {
                comp_result.failed_units++;
                // Error already recorded in DiagnosticCollector by the backend
            }
        } catch (const std::exception& e) {
            comp_result.failed_units++;
            diag.error("compiler", DiagnosticCode::CompilerInternalError,
                       std::string("exception during compilation: ") + e.what());
        }
    }

    return comp_result;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
