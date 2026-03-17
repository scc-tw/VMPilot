#ifndef __COMMON_DIAGNOSTIC_COLLECTOR_HPP__
#define __COMMON_DIAGNOSTIC_COLLECTOR_HPP__

#include <diagnostic.hpp>

#include <mutex>
#include <string>
#include <vector>

namespace VMPilot::Common {

/// Thread-safe collector for diagnostic messages across all pipeline stages.
class DiagnosticCollector {
public:
    DiagnosticCollector() = default;

    /// Singleton no-op collector — add() does nothing, thread-safe by design.
    static DiagnosticCollector& noop();

    /// Thread-safe: add a diagnostic.
    void add(Diagnostic diag);

    /// Convenience: emit a Note.
    void note(const std::string& source, DiagnosticCode code,
              const std::string& message,
              const std::string& unit_name = {},
              uint64_t addr = 0);

    /// Convenience: emit a Warning.
    void warn(const std::string& source, DiagnosticCode code,
              const std::string& message,
              const std::string& unit_name = {},
              uint64_t addr = 0);

    /// Convenience: emit an Error.
    void error(const std::string& source, DiagnosticCode code,
               const std::string& message,
               const std::string& unit_name = {},
               uint64_t addr = 0);

    bool has_errors() const;
    bool has_warnings() const;
    size_t error_count() const;
    size_t warning_count() const;
    size_t size() const;

    const std::vector<Diagnostic>& all() const;

    /// Human-readable report grouped by unit, errors first.
    std::string summary() const;

private:
    explicit DiagnosticCollector(bool is_noop) : is_noop_(is_noop) {}

    mutable std::mutex mtx_;
    std::vector<Diagnostic> diagnostics_;
    bool is_noop_ = false;
};

}  // namespace VMPilot::Common

#endif  // __COMMON_DIAGNOSTIC_COLLECTOR_HPP__
