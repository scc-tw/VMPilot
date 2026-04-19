#ifndef VMPILOT_RUNTIME_EPOCH_STORE_HPP
#define VMPILOT_RUNTIME_EPOCH_STORE_HPP

#include <array>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include <tl/expected.hpp>

#include "tokens.hpp"

// Stage 12 — persistent epoch + one-time nonce state.
//
// The runtime's anti-downgrade and replay protection both live outside
// artifact authenticity: they are local state the runtime mutates as
// it accepts new material. Without persistence this state is lost on
// every process restart, which collapses two guarantees:
//
//   1. minimum_accepted_epoch can never rewind (doc 06 §10). Losing
//      it means a freshly-launched runtime would re-accept a package
//      whose anti_downgrade_epoch the previous run had already
//      advanced past.
//
//   2. one-time nonces on reprovision / migration tokens (doc 10
//      §6.3, doc 15 §9 #9) must stay consumed forever. An in-memory
//      store would let the same token be replayed after a restart.
//
// Stage 12 therefore provides a persistent, atomic file-backed
// implementation plus an in-memory implementation retained for tests.

namespace VMPilot::Runtime::EpochStore {

// Persistent epoch state layout: fixed two u64 fields. On-disk format
// is `VMPLEPOCH\0` magic + u32 version + u64 runtime_epoch +
// u64 minimum_accepted_epoch + u32 crc32-of-payload, all
// little-endian. Rejects any corrupt file by returning LoadError.
struct EpochState {
    std::uint64_t runtime_epoch{0};
    std::uint64_t minimum_accepted_epoch{0};
};

enum class StoreError : std::uint8_t {
    IoError = 1,
    Corrupt,             // magic / version / crc mismatch
    EpochRollbackDenied, // attempt to set an epoch strictly less than
                         // the currently persisted value
    NonceAlreadyPresent, // mark_consumed called on a nonce already in
                         // the store (treated as replay)
};

// ─── Epoch ──────────────────────────────────────────────────────────────

class EpochStore {
public:
    virtual ~EpochStore() = default;

    virtual EpochState get() const noexcept = 0;

    // Monotonic updates only. Both fields may move forward freely;
    // either moving backwards returns EpochRollbackDenied.
    virtual tl::expected<void, StoreError>
    advance(const EpochState& proposed) noexcept = 0;

    // Force-install (tests only). Bypasses monotonic guard.
    virtual void reset_for_testing(const EpochState& state) noexcept = 0;
};

// ─── Nonce store ────────────────────────────────────────────────────────

// Extends Tokens::NonceStore with a persist() hook so the file-backed
// implementation can flush after a successful token acceptance. The
// caller invokes persist() AFTER mark_consumed() returns; if persist
// fails, the nonce has already been consumed in memory, which is the
// fail-closed side of the ledger.
class PersistentNonceStore : public VMPilot::Runtime::Tokens::NonceStore {
public:
    virtual tl::expected<void, StoreError> persist() noexcept = 0;
};

// ─── File-backed implementation ─────────────────────────────────────────

class FileBackedEpochStore final : public EpochStore {
public:
    explicit FileBackedEpochStore(std::string path);

    EpochState get() const noexcept override;
    [[nodiscard]] tl::expected<void, StoreError>
    advance(const EpochState& proposed) noexcept override;
    void reset_for_testing(const EpochState& state) noexcept override;

    // Reload state from disk. Missing file is not an error — returns
    // a zero-initialised EpochState. Caller retains the store.
    [[nodiscard]] tl::expected<void, StoreError> load() noexcept;

private:
    [[nodiscard]] tl::expected<void, StoreError> save_locked() noexcept;

    std::string path_;
    mutable std::mutex mu_;
    EpochState state_{};
};

// Structured file layout for nonces:
//   magic "VMPLNONCE\0" (10 bytes) + u32 version + u32 count +
//   (count × 32 bytes nonce bytes) + u32 crc32-of-payload.
class FileBackedNonceStore final : public PersistentNonceStore {
public:
    explicit FileBackedNonceStore(std::string path);

    bool is_consumed(
        const std::array<std::uint8_t, 32>& nonce) const noexcept override;
    void mark_consumed(
        const std::array<std::uint8_t, 32>& nonce) noexcept override;

    [[nodiscard]] tl::expected<void, StoreError> persist() noexcept override;
    [[nodiscard]] tl::expected<void, StoreError> load() noexcept;

    std::size_t size() const noexcept;

private:
    struct ArrayHash {
        std::size_t operator()(
            const std::array<std::uint8_t, 32>& a) const noexcept;
    };

    std::string path_;
    mutable std::mutex mu_;
    std::unordered_set<std::array<std::uint8_t, 32>, ArrayHash> consumed_;
};

}  // namespace VMPilot::Runtime::EpochStore

#endif  // VMPILOT_RUNTIME_EPOCH_STORE_HPP
