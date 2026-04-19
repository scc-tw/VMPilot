#include "platform_file_ops.hpp"

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#else
#  include <cerrno>
#  include <fcntl.h>
#  include <sys/file.h>
#  include <unistd.h>
#endif

namespace VMPilot::Runtime::State::detail {

// ─── ScopedFileLock ─────────────────────────────────────────────────────

#if defined(_WIN32)

ScopedFileLock::ScopedFileLock(const std::string& path) noexcept {
    HANDLE h = ::CreateFileA(path.c_str(),
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             nullptr,
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);
    if (h == INVALID_HANDLE_VALUE) return;

    OVERLAPPED ov{};
    if (!::LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0,
                      MAXDWORD, MAXDWORD, &ov)) {
        ::CloseHandle(h);
        return;
    }
    handle_ = h;
    locked_ = true;
}

ScopedFileLock::~ScopedFileLock() noexcept {
    if (locked_) {
        OVERLAPPED ov{};
        ::UnlockFileEx(static_cast<HANDLE>(handle_), 0,
                       MAXDWORD, MAXDWORD, &ov);
    }
    if (handle_ != nullptr) {
        ::CloseHandle(static_cast<HANDLE>(handle_));
    }
}

#else

ScopedFileLock::ScopedFileLock(const std::string& path) noexcept {
    int fd = ::open(path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0600);
    if (fd < 0) return;
    // Blocking exclusive lock. Advisory; robust enough for cooperating
    // processes. LOCK_NB would surface contention as a fail path — not
    // what callers want here.
    if (::flock(fd, LOCK_EX) != 0) {
        ::close(fd);
        return;
    }
    fd_     = fd;
    locked_ = true;
}

ScopedFileLock::~ScopedFileLock() noexcept {
    if (locked_) {
        ::flock(fd_, LOCK_UN);
    }
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

#endif

// ─── durable_atomic_write ───────────────────────────────────────────────

#if defined(_WIN32)

tl::expected<void, FileOpError>
durable_atomic_write(const std::string& path,
                     const std::vector<std::uint8_t>& bytes) noexcept {
    const std::string tmp = path + ".tmp";

    HANDLE h = ::CreateFileA(tmp.c_str(),
                             GENERIC_WRITE,
                             0,  // no sharing while writing
                             nullptr,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return tl::make_unexpected(FileOpError::OpenFailed);
    }

    // Cap WriteFile calls at 16 MiB to stay inside DWORD bounds on
    // oversized payloads — vanishingly unlikely for the state file but
    // cheaper to handle now than to rediscover during an incident.
    constexpr DWORD kChunk = 1u << 24;
    const std::uint8_t* p = bytes.data();
    std::size_t remaining = bytes.size();
    while (remaining > 0) {
        const DWORD chunk = static_cast<DWORD>(
            remaining < kChunk ? remaining : kChunk);
        DWORD written = 0;
        if (!::WriteFile(h, p, chunk, &written, nullptr) || written != chunk) {
            ::CloseHandle(h);
            ::DeleteFileA(tmp.c_str());
            return tl::make_unexpected(FileOpError::WriteFailed);
        }
        p += chunk;
        remaining -= chunk;
    }

    if (!::FlushFileBuffers(h)) {
        ::CloseHandle(h);
        ::DeleteFileA(tmp.c_str());
        return tl::make_unexpected(FileOpError::FsyncFailed);
    }
    ::CloseHandle(h);

    // MOVEFILE_WRITE_THROUGH flushes the rename's metadata; data was
    // already durable via FlushFileBuffers. REPLACE_EXISTING is safe
    // when the destination does not yet exist (first write).
    if (!::MoveFileExA(tmp.c_str(), path.c_str(),
                       MOVEFILE_REPLACE_EXISTING |
                       MOVEFILE_WRITE_THROUGH)) {
        ::DeleteFileA(tmp.c_str());
        return tl::make_unexpected(FileOpError::RenameFailed);
    }
    return {};
}

#else

tl::expected<void, FileOpError>
durable_atomic_write(const std::string& path,
                     const std::vector<std::uint8_t>& bytes) noexcept {
    const std::string tmp = path + ".tmp";

    int fd = ::open(tmp.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
                    0600);
    if (fd < 0) return tl::make_unexpected(FileOpError::OpenFailed);

    const std::uint8_t* p = bytes.data();
    std::size_t remaining = bytes.size();
    while (remaining > 0) {
        ssize_t n = ::write(fd, p, remaining);
        if (n < 0) {
            if (errno == EINTR) continue;
            ::close(fd);
            ::unlink(tmp.c_str());
            return tl::make_unexpected(FileOpError::WriteFailed);
        }
        p += n;
        remaining -= static_cast<std::size_t>(n);
    }

    if (::fsync(fd) != 0) {
        ::close(fd);
        ::unlink(tmp.c_str());
        return tl::make_unexpected(FileOpError::FsyncFailed);
    }
    ::close(fd);

    if (::rename(tmp.c_str(), path.c_str()) != 0) {
        ::unlink(tmp.c_str());
        return tl::make_unexpected(FileOpError::RenameFailed);
    }

    // fsync the parent directory so the rename itself is durable.
    // Best-effort: if we can't open the parent (e.g. path had no
    // leading directory component), we've still flushed the file
    // contents, which is the only durability guarantee we claim.
    std::string parent = path;
    const auto slash = parent.find_last_of('/');
    if (slash == std::string::npos)       parent = ".";
    else if (slash == 0)                  parent = "/";
    else                                  parent.resize(slash);

    int dfd = ::open(parent.c_str(), O_RDONLY | O_CLOEXEC);
    if (dfd >= 0) {
        (void)::fsync(dfd);
        ::close(dfd);
    }
    return {};
}

#endif

}  // namespace VMPilot::Runtime::State::detail
