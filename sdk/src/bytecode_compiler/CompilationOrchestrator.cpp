#include <CompilationOrchestrator.hpp>
#include <CompilationUnit.hpp>
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
    const Segmentator::SegmentationResult& result) noexcept {
    if (!backend_)
        return tl::unexpected(std::string("No compiler backend provided"));

    // Build CompilationUnits from SegmentationResult groups
    std::vector<Core::CompilationUnit> units;

    for (const auto& group : result.groups) {
        for (const auto& site : group.sites) {
            // Find the matching NativeFunctionBase by address
            const Segmentator::NativeFunctionBase* found = nullptr;
            for (const auto& region : result.refined_regions) {
                if (region.getAddr() == site.addr &&
                    region.getName() == site.source_name) {
                    found = &region;
                    break;
                }
            }
            if (!found)
                continue;

            Core::CompilationUnit unit;
            unit.name = site.source_name;
            unit.addr = site.addr;
            unit.size = site.size;
            unit.code = found->getCode();
            unit.enclosing_symbol =
                site.enclosing_symbol.value_or(std::string{});
            unit.is_canonical = site.is_canonical;
            unit.context = &result.context;
            units.push_back(std::move(unit));
        }
    }

    CompilationResult comp_result;
    comp_result.total_units = units.size();

    if (units.empty()) {
        return comp_result;
    }

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
