#include <CompilationOrchestrator.hpp>
#include <thread_pool.hpp>

#include <future>
#include <vector>

namespace VMPilot::SDK::BytecodeCompiler {

CompilationOrchestrator::CompilationOrchestrator(
    std::unique_ptr<CompilerBackend> backend,
    CompileConfig config,
    size_t num_threads)
    : backend_(std::move(backend)),
      config_(std::move(config)),
      num_threads_(num_threads) {}

tl::expected<CompilationResult, std::string>
CompilationOrchestrator::compile(
    const std::vector<Core::CompilationUnit>& units) noexcept {
    if (!backend_)
        return tl::unexpected(std::string("No compiler backend provided"));

    CompilationResult comp_result;
    comp_result.total_units = units.size();

    if (units.empty())
        return comp_result;

    // Dispatch to thread pool
    Common::ThreadPool pool(num_threads_);
    using FutureResult = std::future<tl::expected<CompilationOutput, CompileError>>;
    std::vector<FutureResult> futures;
    futures.reserve(units.size());

    for (const auto& unit : units) {
        futures.push_back(pool.submit(
            [this, &unit]() -> tl::expected<CompilationOutput, CompileError> {
                return backend_->compile_unit(unit, config_);
            }));
    }

    pool.wait_all();

    // Collect results
    for (auto& fut : futures) {
        try {
            auto res = fut.get();
            if (res.has_value()) {
                comp_result.outputs.push_back(std::move(res.value()));
            } else {
                comp_result.errors.push_back(std::move(res.error()));
            }
        } catch (const std::exception& e) {
            comp_result.errors.push_back(CompileError{
                CompileErrorCode::InternalError,
                std::string("Exception during compilation: ") + e.what(),
                {},
                0,
            });
        }
    }

    return comp_result;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
