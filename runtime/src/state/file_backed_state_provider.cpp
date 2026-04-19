#include "state/file_backed_state_provider.hpp"

#include <algorithm>
#include <cstring>
#include <unordered_set>
#include <utility>
#include <vector>

#include "persistent_file.hpp"       // crc32, put/get primitives, read_whole_file
#include "platform_file_ops.hpp"     // ScopedFileLock, durable_atomic_write

namespace VMPilot::Runtime::State {

namespace {

namespace PF = VMPilot::Runtime::PersistentFile;
namespace D  = VMPilot::Runtime::State::detail;

// ─── On-disk schema (doc 17 §5.3 v1) ────────────────────────────────────
//
//   magic                  : "VMPLSTATE\0"  (10 bytes)
//   schema_version         : u32 LE       (= 1)
//   body:
//     provider_kind        : u8           (= 1, file_backed)
//     pad                  : 3 bytes      (zero)
//     local_generation     : u64 LE
//     runtime_epoch        : u64 LE
//     minimum_accepted_ep. : u64 LE
//     previous_record_hash : 32 bytes     (reserved; zero for file-backed)
//     payload_hash         : 32 bytes     (reserved; zero for file-backed)
//     nonce_count          : u32 LE
//     nonces               : count × 32 bytes (sorted for determinism)
//   crc32                  : u32 LE over (schema_version || body)

constexpr char         kMagic[]                   = "VMPLSTATE";
static_assert(sizeof(kMagic) == 10, "magic must be exactly 10 bytes");
constexpr std::uint32_t kSchemaVersion            = 1;
constexpr std::uint8_t  kProviderKindFileBacked   = 1;
constexpr std::size_t   kFixedBodyPrefix          =
    1 /*provider_kind*/ + 3 /*pad*/ + 8 /*gen*/ + 8 /*rt_ep*/ +
    8 /*min_ep*/ + 32 /*prev_hash*/ + 32 /*payload_hash*/ +
    4 /*nonce_count*/;

// FNV-1a over 32 bytes. Nonces are already random, so this avoids
// pulling in a heavier hash while giving the set well-distributed
// buckets.
struct ArrayHash {
    std::size_t operator()(
        const std::array<std::uint8_t, 32>& a) const noexcept {
        std::size_t h = 0xCBF29CE484222325ull;
        for (auto b : a) {
            h ^= b;
            h *= 0x100000001B3ull;
        }
        return h;
    }
};

using NonceSet = std::unordered_set<std::array<std::uint8_t, 32>, ArrayHash>;

struct StateSnapshot {
    std::uint64_t local_generation{0};
    EpochState    epoch{};
    NonceSet      consumed;
};

[[nodiscard]] std::vector<std::uint8_t>
encode(const StateSnapshot& s) noexcept {
    std::vector<std::uint8_t> body;
    body.reserve(kFixedBodyPrefix + s.consumed.size() * 32);

    body.push_back(kProviderKindFileBacked);
    body.push_back(0); body.push_back(0); body.push_back(0);
    PF::put_u64_le(body, s.local_generation);
    PF::put_u64_le(body, s.epoch.runtime_epoch);
    PF::put_u64_le(body, s.epoch.minimum_accepted_epoch);
    body.insert(body.end(), 32, 0);  // previous_record_hash (reserved)
    body.insert(body.end(), 32, 0);  // payload_hash         (reserved)
    PF::put_u32_le(body, static_cast<std::uint32_t>(s.consumed.size()));

    // Sort nonces so the encoded bytes (and therefore the CRC) are
    // deterministic regardless of unordered_set iteration order.
    std::vector<std::array<std::uint8_t, 32>> sorted(s.consumed.begin(),
                                                     s.consumed.end());
    std::sort(sorted.begin(), sorted.end());
    for (const auto& n : sorted) {
        body.insert(body.end(), n.begin(), n.end());
    }

    std::vector<std::uint8_t> payload;
    payload.reserve(10 + 4 + body.size() + 4);
    payload.insert(payload.end(), kMagic, kMagic + 10);
    PF::put_u32_le(payload, kSchemaVersion);
    payload.insert(payload.end(), body.begin(), body.end());
    const std::uint32_t crc =
        PF::crc32(payload.data() + 10, 4 + body.size());
    PF::put_u32_le(payload, crc);
    return payload;
}

[[nodiscard]] tl::expected<StateSnapshot, StoreError>
decode(const std::vector<std::uint8_t>& buf) noexcept {
    if (buf.size() < 10 + 4 + kFixedBodyPrefix + 4) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    if (std::memcmp(buf.data(), kMagic, 10) != 0) {
        return tl::make_unexpected(StoreError::Corrupt);
    }

    const std::uint8_t* p   = buf.data() + 10;
    const std::uint8_t* end = buf.data() + buf.size();

    std::uint32_t version = 0;
    if (!PF::get_u32_le(p, end, version) || version != kSchemaVersion) {
        return tl::make_unexpected(StoreError::Corrupt);
    }

    if (end - p < 4) return tl::make_unexpected(StoreError::Corrupt);
    const std::uint8_t* body      = p;
    const std::uint8_t* body_end  = end - 4;

    std::uint32_t stored_crc = 0;
    const std::uint8_t* crc_ptr = body_end;
    if (!PF::get_u32_le(crc_ptr, end, stored_crc)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }

    // CRC covers schema_version || body.
    std::vector<std::uint8_t> crc_input;
    crc_input.reserve(4 + static_cast<std::size_t>(body_end - body));
    PF::put_u32_le(crc_input, version);
    crc_input.insert(crc_input.end(), body, body_end);
    if (PF::crc32(crc_input.data(), crc_input.size()) != stored_crc) {
        return tl::make_unexpected(StoreError::Corrupt);
    }

    if (static_cast<std::size_t>(body_end - body) < kFixedBodyPrefix) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    if (body[0] != kProviderKindFileBacked) {
        return tl::make_unexpected(StoreError::Corrupt);
    }

    p = body + 1 + 3;  // skip provider_kind + pad

    StateSnapshot s{};
    if (!PF::get_u64_le(p, body_end, s.local_generation) ||
        !PF::get_u64_le(p, body_end, s.epoch.runtime_epoch) ||
        !PF::get_u64_le(p, body_end, s.epoch.minimum_accepted_epoch)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    p += 32;  // previous_record_hash (reserved)
    p += 32;  // payload_hash         (reserved)

    std::uint32_t count = 0;
    if (!PF::get_u32_le(p, body_end, count)) {
        return tl::make_unexpected(StoreError::Corrupt);
    }
    if (static_cast<std::size_t>(body_end - p) !=
        static_cast<std::size_t>(count) * 32) {
        return tl::make_unexpected(StoreError::Corrupt);
    }

    s.consumed.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::array<std::uint8_t, 32> n{};
        std::memcpy(n.data(), p, 32);
        p += 32;
        s.consumed.insert(n);
    }
    return s;
}

[[nodiscard]] tl::expected<StateSnapshot, StoreError>
load_from_disk(const std::string& path) noexcept {
    auto buf_or = PF::read_whole_file(path);
    if (!buf_or) {
        // Missing file is the fresh-install path — default-construct
        // rather than surfacing it as an error.
        if (buf_or.error() == PF::IoError::OpenFailed) {
            return StateSnapshot{};
        }
        return tl::make_unexpected(StoreError::IoError);
    }
    return decode(*buf_or);
}

[[nodiscard]] StoreError map_file_op_error(D::FileOpError) noexcept {
    // Every FileOpError collapses onto the same StoreError bucket
    // here; richer mapping is a Stage A PR-2 follow-up once the error
    // enum grows (e.g. RecoveryRequired).
    return StoreError::IoError;
}

}  // namespace

// ────────────────────────────────────────────────────────────────────────

FileBackedStateProvider::FileBackedStateProvider(
    std::string state_path) noexcept
    : state_path_(std::move(state_path)),
      lock_path_(state_path_ + ".lock") {}

tl::expected<void, StoreError>
FileBackedStateProvider::reserve_nonce(
    const std::array<std::uint8_t, 32>& nonce) noexcept {
    D::ScopedFileLock lock(lock_path_);
    if (!lock.is_locked()) {
        return tl::make_unexpected(StoreError::IoError);
    }

    auto snap_or = load_from_disk(state_path_);
    if (!snap_or) return tl::make_unexpected(snap_or.error());
    auto& snap = *snap_or;

    if (snap.consumed.count(nonce) != 0) {
        return tl::make_unexpected(StoreError::NonceAlreadyPresent);
    }
    snap.consumed.insert(nonce);
    snap.local_generation += 1;

    auto w = D::durable_atomic_write(state_path_, encode(snap));
    if (!w) return tl::make_unexpected(map_file_op_error(w.error()));
    return {};
}

tl::expected<EpochState, StoreError>
FileBackedStateProvider::current_epoch_state() const noexcept {
    D::ScopedFileLock lock(lock_path_);
    if (!lock.is_locked()) {
        return tl::make_unexpected(StoreError::IoError);
    }

    auto snap_or = load_from_disk(state_path_);
    if (!snap_or) return tl::make_unexpected(snap_or.error());
    return snap_or->epoch;
}

tl::expected<void, StoreError>
FileBackedStateProvider::advance_epoch_state(
    const EpochState& proposed) noexcept {
    D::ScopedFileLock lock(lock_path_);
    if (!lock.is_locked()) {
        return tl::make_unexpected(StoreError::IoError);
    }

    auto snap_or = load_from_disk(state_path_);
    if (!snap_or) return tl::make_unexpected(snap_or.error());
    auto& snap = *snap_or;

    // Disk-truth monotonicity: compare the proposal against what's
    // actually committed, not against any cached in-memory copy.
    if (proposed.runtime_epoch          < snap.epoch.runtime_epoch ||
        proposed.minimum_accepted_epoch < snap.epoch.minimum_accepted_epoch) {
        return tl::make_unexpected(StoreError::EpochRollbackDenied);
    }
    snap.epoch = proposed;
    snap.local_generation += 1;

    auto w = D::durable_atomic_write(state_path_, encode(snap));
    if (!w) return tl::make_unexpected(map_file_op_error(w.error()));
    return {};
}

PersistenceCapability
FileBackedStateProvider::capabilities() const noexcept {
    PersistenceCapability c{};
    c.cross_process_atomic     = true;
    c.crash_consistent         = true;
    c.rollback_resistant       = false;
    c.tamper_evident           = false;
    c.hardware_bound           = false;
    c.policy_bound             = false;
    c.signed_recovery_required = false;
    // File-backed is always Available the moment it is constructed;
    // a missing state file is a fresh-install path (see load_from_disk
    // above), not a provisioning / recovery condition.
    c.operational_state        = ProviderOperationalState::Available;
    return c;
}

}  // namespace VMPilot::Runtime::State
