#include "epoch_store.hpp"

#include <array>
#include <cstring>
#include <vector>

#include "persistent_file.hpp"

namespace VMPilot::Runtime::EpochStore {

namespace {

namespace PF = VMPilot::Runtime::PersistentFile;

constexpr char kEpochMagic[] = "VMPLEPOCH";  // 10 bytes incl. trailing NUL
constexpr char kNonceMagic[] = "VMPLNONCE";
constexpr std::uint32_t kVersion = 1;

// Map the toolkit's IoError into the store's richer StoreError. The
// one important distinction is OpenFailed → "missing file, fresh
// install", which both callers treat as a success path (not
// Corrupt).
[[nodiscard]] inline StoreError map_io_error(PF::IoError e) noexcept {
    switch (e) {
        case PF::IoError::OpenFailed:
        case PF::IoError::ReadFailed:
        case PF::IoError::WriteFailed:
        case PF::IoError::RenameFailed:
        case PF::IoError::ShortRead:
            return StoreError::IoError;
        case PF::IoError::MagicMismatch:
        case PF::IoError::VersionMismatch:
        case PF::IoError::CrcMismatch:
        case PF::IoError::TruncatedPayload:
            return StoreError::Corrupt;
    }
    return StoreError::IoError;
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
    // Body: u64 runtime_epoch + u64 minimum_accepted_epoch.
    std::vector<std::uint8_t> body;
    body.reserve(16);
    PF::put_u64_le(body, state_.runtime_epoch);
    PF::put_u64_le(body, state_.minimum_accepted_epoch);
    auto r = PF::write_framed(path_, kEpochMagic, kVersion, body);
    if (!r) return tl::make_unexpected(map_io_error(r.error()));
    return {};
}

tl::expected<void, StoreError> FileBackedEpochStore::load() noexcept {
    std::lock_guard<std::mutex> g(mu_);
    auto body_or = PF::read_framed(path_, kEpochMagic, kVersion);
    if (!body_or) {
        if (body_or.error() == PF::IoError::OpenFailed) {
            // Missing file: keep zeros and treat as fresh start.
            state_ = EpochState{};
            return {};
        }
        return tl::make_unexpected(map_io_error(body_or.error()));
    }
    const auto& body = *body_or;
    if (body.size() != 16) return tl::make_unexpected(StoreError::Corrupt);
    const std::uint8_t* p = body.data();
    const std::uint8_t* end = p + body.size();
    EpochState s{};
    if (!PF::get_u64_le(p, end, s.runtime_epoch) ||
        !PF::get_u64_le(p, end, s.minimum_accepted_epoch)) {
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
    // Body: u32 count + count * 32-byte nonces.
    std::vector<std::uint8_t> body;
    body.reserve(4 + consumed_.size() * 32);
    PF::put_u32_le(body, static_cast<std::uint32_t>(consumed_.size()));
    for (const auto& n : consumed_) {
        body.insert(body.end(), n.begin(), n.end());
    }
    auto r = PF::write_framed(path_, kNonceMagic, kVersion, body);
    if (!r) return tl::make_unexpected(map_io_error(r.error()));
    return {};
}

tl::expected<void, StoreError> FileBackedNonceStore::load() noexcept {
    std::lock_guard<std::mutex> g(mu_);
    auto body_or = PF::read_framed(path_, kNonceMagic, kVersion);
    if (!body_or) {
        if (body_or.error() == PF::IoError::OpenFailed) {
            consumed_.clear();
            return {};
        }
        return tl::make_unexpected(map_io_error(body_or.error()));
    }
    const auto& body = *body_or;
    const std::uint8_t* p = body.data();
    const std::uint8_t* end = p + body.size();
    std::uint32_t count = 0;
    if (!PF::get_u32_le(p, end, count)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    const std::size_t needed = static_cast<std::size_t>(count) * 32;
    if (static_cast<std::size_t>(end - p) != needed) {
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
    consumed_ = std::move(fresh);
    return {};
}

}  // namespace VMPilot::Runtime::EpochStore
