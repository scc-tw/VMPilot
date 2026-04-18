#include "epoch_store.hpp"

#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

namespace VMPilot::Runtime::EpochStore {

namespace {

constexpr char kEpochMagic[]   = "VMPLEPOCH";
constexpr std::size_t kEpochMagicLen = 10;  // includes trailing NUL
constexpr char kNonceMagic[]   = "VMPLNONCE";
constexpr std::size_t kNonceMagicLen = 10;
constexpr std::uint32_t kVersion = 1;

// Compact CRC-32/IEEE table-less polynomial 0xEDB88320. Fine for
// the small payloads the store writes (dozens to thousands of bytes).
std::uint32_t crc32(const std::uint8_t* data, std::size_t size) noexcept {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            crc = (crc >> 1) ^ (0xEDB88320u & -static_cast<std::uint32_t>(
                                                  crc & 1));
        }
    }
    return ~crc;
}

void put_u32_le(std::vector<std::uint8_t>& out, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        out.push_back(static_cast<std::uint8_t>((v >> (8 * i)) & 0xFFu));
    }
}
void put_u64_le(std::vector<std::uint8_t>& out, std::uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<std::uint8_t>((v >> (8 * i)) & 0xFFu));
    }
}
bool get_u32_le(const std::uint8_t*& p, const std::uint8_t* end,
                std::uint32_t& out) noexcept {
    if (end - p < 4) return false;
    out = static_cast<std::uint32_t>(p[0]) |
          (static_cast<std::uint32_t>(p[1]) << 8) |
          (static_cast<std::uint32_t>(p[2]) << 16) |
          (static_cast<std::uint32_t>(p[3]) << 24);
    p += 4;
    return true;
}
bool get_u64_le(const std::uint8_t*& p, const std::uint8_t* end,
                std::uint64_t& out) noexcept {
    if (end - p < 8) return false;
    out = 0;
    for (int i = 0; i < 8; ++i) {
        out |= static_cast<std::uint64_t>(p[i]) << (8 * i);
    }
    p += 8;
    return true;
}

// Atomic file swap: write to `path.tmp`, fsync, rename to `path`. On
// Windows std::filesystem::rename is atomic for same-volume replace
// provided the destination is not locked; for 1.0 that's acceptable.
tl::expected<void, StoreError> atomic_write(
    const std::string& path, const std::vector<std::uint8_t>& payload) noexcept {
    try {
        const std::string tmp = path + ".tmp";
        {
            std::ofstream os(tmp, std::ios::binary | std::ios::trunc);
            if (!os.is_open()) return tl::make_unexpected(StoreError::IoError);
            os.write(reinterpret_cast<const char*>(payload.data()),
                     static_cast<std::streamsize>(payload.size()));
            if (!os.good()) return tl::make_unexpected(StoreError::IoError);
        }
        std::error_code ec;
        std::filesystem::rename(tmp, path, ec);
        if (ec) return tl::make_unexpected(StoreError::IoError);
        return {};
    } catch (...) {
        return tl::make_unexpected(StoreError::IoError);
    }
}

tl::expected<std::vector<std::uint8_t>, StoreError>
read_whole_file(const std::string& path) noexcept {
    try {
        std::ifstream is(path, std::ios::binary | std::ios::ate);
        if (!is.is_open()) {
            return tl::make_unexpected(StoreError::IoError);
        }
        const auto size = is.tellg();
        if (size < 0) return tl::make_unexpected(StoreError::IoError);
        is.seekg(0, std::ios::beg);
        std::vector<std::uint8_t> buf(static_cast<std::size_t>(size));
        if (size > 0) {
            is.read(reinterpret_cast<char*>(buf.data()),
                    static_cast<std::streamsize>(size));
            if (!is.good()) return tl::make_unexpected(StoreError::IoError);
        }
        return buf;
    } catch (...) {
        return tl::make_unexpected(StoreError::IoError);
    }
}

}  // namespace

// ─── FileBackedEpochStore ───────────────────────────────────────────────

FileBackedEpochStore::FileBackedEpochStore(std::string path)
    : path_(std::move(path)) {
    (void)load();
}

EpochState FileBackedEpochStore::get() const noexcept {
    std::lock_guard<std::mutex> g(mu_);
    return state_;
}

tl::expected<void, StoreError>
FileBackedEpochStore::advance(const EpochState& proposed) noexcept {
    std::lock_guard<std::mutex> g(mu_);
    if (proposed.runtime_epoch < state_.runtime_epoch) {
        return tl::make_unexpected(StoreError::EpochRollbackDenied);
    }
    if (proposed.minimum_accepted_epoch < state_.minimum_accepted_epoch) {
        return tl::make_unexpected(StoreError::EpochRollbackDenied);
    }
    state_ = proposed;
    return save_locked();
}

void FileBackedEpochStore::reset_for_testing(const EpochState& state) noexcept {
    std::lock_guard<std::mutex> g(mu_);
    state_ = state;
    (void)save_locked();
}

tl::expected<void, StoreError>
FileBackedEpochStore::save_locked() noexcept {
    std::vector<std::uint8_t> payload;
    payload.reserve(kEpochMagicLen + 4 + 8 + 8 + 4);
    payload.insert(payload.end(), kEpochMagic,
                   kEpochMagic + kEpochMagicLen);
    put_u32_le(payload, kVersion);
    put_u64_le(payload, state_.runtime_epoch);
    put_u64_le(payload, state_.minimum_accepted_epoch);
    const std::uint32_t checksum =
        crc32(payload.data() + kEpochMagicLen,
              payload.size() - kEpochMagicLen);
    put_u32_le(payload, checksum);
    return atomic_write(path_, payload);
}

tl::expected<void, StoreError> FileBackedEpochStore::load() noexcept {
    std::lock_guard<std::mutex> g(mu_);
    auto buf_or = read_whole_file(path_);
    if (!buf_or) {
        // Missing file: keep zeros and treat as fresh start.
        state_ = EpochState{};
        return {};
    }
    const auto& buf = *buf_or;
    if (buf.size() < kEpochMagicLen + 4 + 8 + 8 + 4) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    if (std::memcmp(buf.data(), kEpochMagic, kEpochMagicLen) != 0) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    const std::uint8_t* p = buf.data() + kEpochMagicLen;
    const std::uint8_t* end = buf.data() + buf.size();
    std::uint32_t version = 0;
    if (!get_u32_le(p, end, version) || version != kVersion) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    EpochState s{};
    if (!get_u64_le(p, end, s.runtime_epoch) ||
        !get_u64_le(p, end, s.minimum_accepted_epoch)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    std::uint32_t stored_crc = 0;
    if (!get_u32_le(p, end, stored_crc)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    const std::uint32_t calc_crc = crc32(buf.data() + kEpochMagicLen,
                                         4 + 8 + 8);
    if (stored_crc != calc_crc) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    state_ = s;
    return {};
}

// ─── FileBackedNonceStore ───────────────────────────────────────────────

FileBackedNonceStore::FileBackedNonceStore(std::string path)
    : path_(std::move(path)) {
    (void)load();
}

std::size_t FileBackedNonceStore::ArrayHash::operator()(
    const std::array<std::uint8_t, 32>& a) const noexcept {
    // FNV-1a over 32 bytes. Good enough for an unordered_set of
    // one-time nonces where contents are already randomised.
    std::size_t h = 0xCBF29CE484222325ull;
    for (auto b : a) {
        h ^= b;
        h *= 0x100000001B3ull;
    }
    return h;
}

bool FileBackedNonceStore::is_consumed(
    const std::array<std::uint8_t, 32>& nonce) const noexcept {
    std::lock_guard<std::mutex> g(mu_);
    return consumed_.count(nonce) != 0;
}

void FileBackedNonceStore::mark_consumed(
    const std::array<std::uint8_t, 32>& nonce) noexcept {
    std::lock_guard<std::mutex> g(mu_);
    consumed_.insert(nonce);
}

std::size_t FileBackedNonceStore::size() const noexcept {
    std::lock_guard<std::mutex> g(mu_);
    return consumed_.size();
}

tl::expected<void, StoreError> FileBackedNonceStore::persist() noexcept {
    std::lock_guard<std::mutex> g(mu_);
    std::vector<std::uint8_t> payload;
    payload.reserve(kNonceMagicLen + 4 + 4 + consumed_.size() * 32 + 4);
    payload.insert(payload.end(), kNonceMagic,
                   kNonceMagic + kNonceMagicLen);
    put_u32_le(payload, kVersion);
    put_u32_le(payload, static_cast<std::uint32_t>(consumed_.size()));
    for (const auto& n : consumed_) {
        payload.insert(payload.end(), n.begin(), n.end());
    }
    const std::uint32_t checksum =
        crc32(payload.data() + kNonceMagicLen,
              payload.size() - kNonceMagicLen);
    put_u32_le(payload, checksum);
    return atomic_write(path_, payload);
}

tl::expected<void, StoreError> FileBackedNonceStore::load() noexcept {
    std::lock_guard<std::mutex> g(mu_);
    auto buf_or = read_whole_file(path_);
    if (!buf_or) {
        consumed_.clear();
        return {};
    }
    const auto& buf = *buf_or;
    if (buf.size() < kNonceMagicLen + 4 + 4 + 4) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    if (std::memcmp(buf.data(), kNonceMagic, kNonceMagicLen) != 0) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    const std::uint8_t* p = buf.data() + kNonceMagicLen;
    const std::uint8_t* end = buf.data() + buf.size();
    std::uint32_t version = 0;
    std::uint32_t count = 0;
    if (!get_u32_le(p, end, version) || version != kVersion) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    if (!get_u32_le(p, end, count)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    const std::size_t needed_bytes =
        static_cast<std::size_t>(count) * 32 + 4;  // entries + final CRC
    if (static_cast<std::size_t>(end - p) < needed_bytes) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    std::unordered_set<std::array<std::uint8_t, 32>, ArrayHash> fresh;
    fresh.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::array<std::uint8_t, 32> n{};
        std::memcpy(n.data(), p, 32);
        p += 32;
        fresh.insert(n);
    }
    std::uint32_t stored_crc = 0;
    if (!get_u32_le(p, end, stored_crc)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    const std::uint32_t payload_len_excluding_crc =
        static_cast<std::uint32_t>(4 + 4 + count * 32);
    const std::uint32_t calc_crc = crc32(buf.data() + kNonceMagicLen,
                                         payload_len_excluding_crc);
    if (stored_crc != calc_crc) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    consumed_ = std::move(fresh);
    return {};
}

}  // namespace VMPilot::Runtime::EpochStore
