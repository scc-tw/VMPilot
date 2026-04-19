#include "subprocess_run.hpp"

#include <cstdlib>
#include <string>
#include <vector>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#else
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif

namespace VMPilot::RuntimeTest {

namespace {

#if defined(_WIN32)

// Windows CreateProcess command-line quoting (MSDN CommandLineToArgvW
// rules): quote any arg containing space/tab/quote, escape embedded
// quotes with backslashes, and double trailing backslashes before a
// closing quote. Without this, a helper invoked with a temp path that
// contains spaces (common on Windows) would be split into multiple
// args and the helper would fail its own arg check.
std::string quote_arg(const std::string& arg) {
    if (arg.empty()) return "\"\"";
    const bool needs_quotes =
        arg.find_first_of(" \t\"") != std::string::npos;
    if (!needs_quotes) return arg;

    std::string out;
    out += '"';
    std::size_t backslashes = 0;
    for (char c : arg) {
        if (c == '\\') {
            ++backslashes;
        } else if (c == '"') {
            out.append(2 * backslashes + 1, '\\');
            backslashes = 0;
            out += '"';
        } else {
            out.append(backslashes, '\\');
            backslashes = 0;
            out += c;
        }
    }
    out.append(2 * backslashes, '\\');
    out += '"';
    return out;
}

#endif

}  // namespace

int run_subprocess(const std::string& exe,
                   const std::vector<std::string>& args) noexcept {
#if defined(_WIN32)
    std::string cmdline = quote_arg(exe);
    for (const auto& a : args) {
        cmdline += ' ';
        cmdline += quote_arg(a);
    }

    // CreateProcessA takes a mutable buffer for lpCommandLine.
    std::vector<char> mutable_cmd(cmdline.begin(), cmdline.end());
    mutable_cmd.push_back('\0');

    STARTUPINFOA        si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    if (!::CreateProcessA(nullptr, mutable_cmd.data(),
                          nullptr, nullptr, FALSE, 0,
                          nullptr, nullptr, &si, &pi)) {
        return -1;
    }

    ::WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code = 0;
    if (!::GetExitCodeProcess(pi.hProcess, &code)) {
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
        return -1;
    }
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
    return static_cast<int>(code);

#else
    // Build argv[]. Own the strings locally so argv[] data is stable
    // until after execv (which only fails) returns.
    std::vector<std::string> owned;
    owned.reserve(args.size() + 1);
    owned.push_back(exe);
    for (const auto& a : args) owned.push_back(a);

    std::vector<char*> argv;
    argv.reserve(owned.size() + 1);
    for (auto& s : owned) argv.push_back(s.data());
    argv.push_back(nullptr);

    const pid_t pid = ::fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        ::execv(exe.c_str(), argv.data());
        // execv only returns on failure.
        ::_exit(127);
    }

    int status = 0;
    if (::waitpid(pid, &status, 0) < 0) return -1;
    if (WIFEXITED(status))   return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
    return -1;
#endif
}

}  // namespace VMPilot::RuntimeTest
