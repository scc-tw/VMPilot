#pragma once
/// @file temp_file.hpp
/// @brief Cross-platform temporary file creation for loader tests.

#include <string>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <cstdlib>
#endif

namespace VMPilot::Test {

/// Create a temporary file and return its path.
/// The file exists on disk (empty) when this returns.
/// Caller is responsible for cleanup (std::remove).
inline std::string make_temp_file(const char* prefix = "vmp") {
#ifdef _WIN32
    char temp_dir[MAX_PATH];
    DWORD dir_len = GetTempPathA(MAX_PATH, temp_dir);
    if (dir_len == 0 || dir_len > MAX_PATH)
        throw std::runtime_error("GetTempPathA failed");
    char temp_file[MAX_PATH];
    if (GetTempFileNameA(temp_dir, prefix, 0, temp_file) == 0)
        throw std::runtime_error("GetTempFileNameA failed");
    return std::string(temp_file);
#else
    std::string tmpl = std::string("/tmp/") + prefix + "_XXXXXX";
    // mkstemp requires a mutable char array
    std::vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    int fd = mkstemp(buf.data());
    if (fd < 0)
        throw std::runtime_error("mkstemp failed");
    close(fd);
    return std::string(buf.data());
#endif
}

}  // namespace VMPilot::Test
