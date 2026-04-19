#ifndef VMPILOT_RUNTIME_TEST_HELPERS_SUBPROCESS_RUN_HPP
#define VMPILOT_RUNTIME_TEST_HELPERS_SUBPROCESS_RUN_HPP

#include <string>
#include <vector>

// Cross-platform test-only subprocess spawn. Synchronous — the caller
// waits for the child and receives its exit code. Suitable for
// concurrency testing when invoked from multiple threads, where each
// thread owns one child process.
//
// Returns -1 on spawn / wait failure; otherwise the child's exit code
// (0..255 on POSIX, full DWORD on Windows).

namespace VMPilot::RuntimeTest {

int run_subprocess(const std::string& exe,
                   const std::vector<std::string>& args) noexcept;

}  // namespace VMPilot::RuntimeTest

#endif  // VMPILOT_RUNTIME_TEST_HELPERS_SUBPROCESS_RUN_HPP
