#ifndef VMPILOT_RUNTIME_PERSISTENT_FILE_HPP
#define VMPILOT_RUNTIME_PERSISTENT_FILE_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

// Shared I/O toolkit for CRC-framed binary persistent-state files.
// FileBackedStateProvider's on-disk schema reuses:
//
//   1. Prefix on-disk payload with a 10-byte magic + 4-byte version.
//   2. Suffix with a CRC-32 over {version, inner payload}.
//   3. Atomically publish writes by writing to `<path>.tmp` and
//      renaming onto `<path>`.
//   4. Treat a missing file as fresh state (not an error).
//
// This header lives under runtime/include/ (not common/) because the
// framing is internal to the runtime's persistent-state layer; no
// producer or common-side code should touch it.

namespace VMPilot::Runtime::PersistentFile {

// Shared I/O-level error enum. Each caller maps it onto its own
// richer error type (StoreError carries IoError / Corrupt /
// EpochRollbackDenied / NonceAlreadyPresent), so this is the thin
// surface the helpers themselves surface.
enum class IoError : std::uint8_t {
    OpenFailed = 1,
    ReadFailed,
    WriteFailed,
    RenameFailed,
    ShortRead,
    MagicMismatch,
    VersionMismatch,
    CrcMismatch,
    TruncatedPayload,
};

// ─── Little-endian primitives ───────────────────────────────────────────

inline void put_u32_le(std::vector<std::uint8_t>& out,
                       std::uint32_t v) noexcept {
    for (int i = 0; i < 4; ++i) {
        out.push_back(static_cast<std::uint8_t>((v >> (8 * i)) & 0xFFu));
    }
}
inline void put_u64_le(std::vector<std::uint8_t>& out,
                       std::uint64_t v) noexcept {
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<std::uint8_t>((v >> (8 * i)) & 0xFFu));
    }
}
[[nodiscard]] inline bool get_u32_le(const std::uint8_t*& p,
                                     const std::uint8_t* end,
                                     std::uint32_t& out) noexcept {
    if (end - p < 4) return false;
    out = static_cast<std::uint32_t>(p[0]) |
          (static_cast<std::uint32_t>(p[1]) << 8) |
          (static_cast<std::uint32_t>(p[2]) << 16) |
          (static_cast<std::uint32_t>(p[3]) << 24);
    p += 4;
    return true;
}
[[nodiscard]] inline bool get_u64_le(const std::uint8_t*& p,
                                     const std::uint8_t* end,
                                     std::uint64_t& out) noexcept {
    if (end - p < 8) return false;
    out = 0;
    for (int i = 0; i < 8; ++i) {
        out |= static_cast<std::uint64_t>(p[i]) << (8 * i);
    }
    p += 8;
    return true;
}

// Compact CRC-32/IEEE (polynomial 0xEDB88320), table-less. Fine for
// the small payloads (< 100 KB) persistent stores write.
[[nodiscard]] inline std::uint32_t crc32(const std::uint8_t* data,
                                         std::size_t size) noexcept {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            crc = (crc >> 1) ^ (0xEDB88320u &
                                -static_cast<std::uint32_t>(crc & 1));
        }
    }
    return ~crc;
}

// ─── Atomic file I/O ────────────────────────────────────────────────────

// Write `payload` to `<path>.tmp`, then rename onto `<path>`. On
// Windows, std::filesystem::rename is atomic for same-volume replace
// provided the destination isn't locked. The caller is responsible
// for serializing concurrent callers (std::mutex in each store).
[[nodiscard]] inline tl::expected<void, IoError>
atomic_write(const std::string& path,
             const std::vector<std::uint8_t>& payload) noexcept {
    try {
        const std::string tmp = path + ".tmp";
        {
            std::ofstream os(tmp, std::ios::binary | std::ios::trunc);
            if (!os.is_open()) return tl::make_unexpected(IoError::OpenFailed);
            os.write(reinterpret_cast<const char*>(payload.data()),
                     static_cast<std::streamsize>(payload.size()));
            if (!os.good()) return tl::make_unexpected(IoError::WriteFailed);
        }
        std::error_code ec;
        std::filesystem::rename(tmp, path, ec);
        if (ec) return tl::make_unexpected(IoError::RenameFailed);
        return {};
    } catch (...) {
        return tl::make_unexpected(IoError::WriteFailed);
    }
}

// Read entire file contents. Missing file → IoError::OpenFailed so
// callers can distinguish "fresh install" (OpenFailed → use defaults)
// from "corrupt file" (MagicMismatch / CrcMismatch → fail-closed).
[[nodiscard]] inline tl::expected<std::vector<std::uint8_t>, IoError>
read_whole_file(const std::string& path) noexcept {
    try {
        std::ifstream is(path, std::ios::binary | std::ios::ate);
        if (!is.is_open()) return tl::make_unexpected(IoError::OpenFailed);
        const auto size = is.tellg();
        if (size < 0) return tl::make_unexpected(IoError::ReadFailed);
        is.seekg(0, std::ios::beg);
        std::vector<std::uint8_t> buf(static_cast<std::size_t>(size));
        if (size > 0) {
            is.read(reinterpret_cast<char*>(buf.data()),
                    static_cast<std::streamsize>(size));
            if (!is.good()) return tl::make_unexpected(IoError::ReadFailed);
        }
        return buf;
    } catch (...) {
        return tl::make_unexpected(IoError::ReadFailed);
    }
}

// ─── Magic + version + crc framing ──────────────────────────────────────
//
// On-disk layout:
//
//   magic[magic_len] || u32 version || <body> || u32 crc32(version || body)
//
// `body` is opaque: the caller builds it however it wants (the
// store-specific schema). `magic_len` and `version` are ints to let
// callers bump schema without touching the framing helpers.

// Write `body` into `<path>` with the given framing. CRC covers
// version + body (the magic is fixed, so including it in the CRC
// adds no detection power).
template <std::size_t MagicLen>
[[nodiscard]] inline tl::expected<void, IoError>
write_framed(const std::string& path,
             const char (&magic)[MagicLen],
             std::uint32_t version,
             const std::vector<std::uint8_t>& body) noexcept {
    std::vector<std::uint8_t> payload;
    payload.reserve(MagicLen + 4 + body.size() + 4);
    payload.insert(payload.end(), magic, magic + MagicLen);
    put_u32_le(payload, version);
    payload.insert(payload.end(), body.begin(), body.end());
    const std::uint32_t checksum =
        crc32(payload.data() + MagicLen, 4 + body.size());
    put_u32_le(payload, checksum);
    return atomic_write(path, payload);
}

// Verify the frame and return the inner body bytes. Returns
// `IoError::OpenFailed` when the file doesn't exist so callers can
// treat that as fresh state; every other return is a genuine
// corruption + fail-closed signal.
template <std::size_t MagicLen>
[[nodiscard]] inline tl::expected<std::vector<std::uint8_t>, IoError>
read_framed(const std::string& path,
            const char (&magic)[MagicLen],
            std::uint32_t expected_version) noexcept {
    auto buf_or = read_whole_file(path);
    if (!buf_or) return tl::make_unexpected(buf_or.error());
    const auto& buf = *buf_or;
    if (buf.size() < MagicLen + 4 + 4) {
        return tl::make_unexpected(IoError::TruncatedPayload);
    }
    if (std::memcmp(buf.data(), magic, MagicLen) != 0) {
        return tl::make_unexpected(IoError::MagicMismatch);
    }
    const std::uint8_t* p = buf.data() + MagicLen;
    const std::uint8_t* end = buf.data() + buf.size();
    std::uint32_t version = 0;
    if (!get_u32_le(p, end, version)) {
        return tl::make_unexpected(IoError::TruncatedPayload);
    }
    if (version != expected_version) {
        return tl::make_unexpected(IoError::VersionMismatch);
    }
    if (end - p < 4) {
        return tl::make_unexpected(IoError::TruncatedPayload);
    }
    const std::size_t body_size = static_cast<std::size_t>(end - p) - 4;
    std::vector<std::uint8_t> body(p, p + body_size);
    p += body_size;
    std::uint32_t stored_crc = 0;
    if (!get_u32_le(p, end, stored_crc)) {
        return tl::make_unexpected(IoError::TruncatedPayload);
    }
    // CRC covers version + body.
    std::vector<std::uint8_t> checked;
    checked.reserve(4 + body_size);
    put_u32_le(checked, version);
    checked.insert(checked.end(), body.begin(), body.end());
    const std::uint32_t calc_crc = crc32(checked.data(), checked.size());
    if (stored_crc != calc_crc) {
        return tl::make_unexpected(IoError::CrcMismatch);
    }
    return body;
}

}  // namespace VMPilot::Runtime::PersistentFile

#endif  // VMPILOT_RUNTIME_PERSISTENT_FILE_HPP
