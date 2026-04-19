#ifndef VMPILOT_RUNTIME_STATE_PLATFORM_FILE_OPS_HPP
#define VMPILOT_RUNTIME_STATE_PLATFORM_FILE_OPS_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <tl/expected.hpp>

// Private platform helpers for FileBackedStateProvider — doc 17 §5.
//
// Kept out of runtime/include/ on purpose: only state/ TUs may depend
// on these primitives, and they carry OS-specific semantics that
// shouldn't leak into public headers.

namespace VMPilot::Runtime::State::detail {

enum class FileOpError : std::uint8_t {
    LockFailed = 1,
    OpenFailed,
    WriteFailed,
    FsyncFailed,
    RenameFailed,
};

// RAII exclusive lock on a sibling `<state>.lock` file.
//
//   POSIX   : flock(LOCK_EX) on an O_RDWR|O_CREAT fd. Advisory — only
//             cooperating processes observe it. Per-process; robust
//             across fork() in the sense that a locked fd stays locked
//             until explicit unlock or close.
//
//   Windows : LockFileEx(LOCKFILE_EXCLUSIVE_LOCK) on a held HANDLE.
//             Mandatory for cooperating CreateFile + LockFileEx paths.
//
// Doc 17 §3 declares this is NOT a security boundary — the backing
// file itself remains attacker-writable. `rollback_resistant=false`
// and `tamper_evident=false` stay honest on the capability sheet.
class ScopedFileLock {
public:
    explicit ScopedFileLock(const std::string& path) noexcept;
    ~ScopedFileLock() noexcept;

    ScopedFileLock(const ScopedFileLock&)            = delete;
    ScopedFileLock& operator=(const ScopedFileLock&) = delete;
    ScopedFileLock(ScopedFileLock&&)                 = delete;
    ScopedFileLock& operator=(ScopedFileLock&&)      = delete;

    bool is_locked() const noexcept { return locked_; }

private:
#if defined(_WIN32)
    void* handle_ = nullptr;   // Windows HANDLE; nullptr = no handle
#else
    int   fd_     = -1;
#endif
    bool locked_ = false;
};

// Atomically write `bytes` to `path` with a durability guarantee —
// after a successful return, either the previous valid contents are
// visible OR `bytes` are visible. Never a partial / empty / torn
// state. Doc 17 §5.1 (POSIX) / §5.2 (Windows) commit sequences:
//
//   POSIX   : open("<path>.tmp", O_WRONLY|O_CREAT|O_TRUNC)
//             write(); fsync(tmp_fd); close()
//             rename("<path>.tmp", "<path>")
//             fsync(parent_dir_fd)
//
//   Windows : CreateFile("<path>.tmp", CREATE_ALWAYS)
//             WriteFile(); FlushFileBuffers(); CloseHandle()
//             MoveFileExA("<path>.tmp", "<path>",
//                         MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)
//
// Caller holds `ScopedFileLock` across the call to serialise against
// other cooperating processes.
tl::expected<void, FileOpError>
durable_atomic_write(const std::string& path,
                     const std::vector<std::uint8_t>& bytes) noexcept;

}  // namespace VMPilot::Runtime::State::detail

#endif  // VMPILOT_RUNTIME_STATE_PLATFORM_FILE_OPS_HPP
