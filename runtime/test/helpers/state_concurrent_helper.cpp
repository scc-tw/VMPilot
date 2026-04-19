// Cross-process driver for FileBackedStateProvider concurrency tests —
// doc 17 §10 Stage A tests #1-#3.
//
// The integration test spawns N copies of this binary concurrently
// against the same state file and tallies exit codes to verify the
// provider's cross-process invariants:
//
//   #1 two processes same nonce            -> exactly one Ok, rest Replay
//   #2 two processes distinct nonces       -> all Ok, all persist
//   #3 advance_epoch_state race             -> disk_final == max(proposals)
//
// CLI (all arguments required for the chosen op):
//
//   helper --state-path <path> --op reserve-nonce
//          --nonce <64 lowercase-hex chars>
//
//   helper --state-path <path> --op advance-epoch
//          --runtime-epoch <u64>
//          --minimum-epoch <u64>
//
// Exit codes are stable — the test asserts on them directly. See the
// `Exit` enum below.

#include "state/file_backed_state_provider.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>

namespace {

enum Exit : int {
    Ok             = 0,
    NonceReplay    = 10,
    EpochRollback  = 11,
    IoError        = 12,
    Corrupt        = 13,
    UnknownError   = 14,
    BadArgs        = 20,
};

int nibble(char c) noexcept {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Decode a hex string of exactly 2*N chars into an N-byte array.
// Returns false on any format error.
bool hex_to_bytes(std::string_view hex,
                  std::uint8_t* out, std::size_t n) noexcept {
    if (hex.size() != n * 2) return false;
    for (std::size_t i = 0; i < n; ++i) {
        const int hi = nibble(hex[i * 2]);
        const int lo = nibble(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) return false;
        out[i] = static_cast<std::uint8_t>((hi << 4) | lo);
    }
    return true;
}

int map_store_error(VMPilot::Runtime::State::StoreError e) noexcept {
    using E = VMPilot::Runtime::State::StoreError;
    switch (e) {
        case E::NonceAlreadyPresent: return Exit::NonceReplay;
        case E::EpochRollbackDenied: return Exit::EpochRollback;
        case E::IoError:             return Exit::IoError;
        case E::Corrupt:             return Exit::Corrupt;
    }
    return Exit::UnknownError;
}

}  // namespace

int main(int argc, char** argv) {
    std::string   state_path;
    std::string   op;
    std::string   nonce_hex;
    std::uint64_t runtime_epoch = 0;
    std::uint64_t minimum_epoch = 0;

    // Tiny flag parser — no third-party dependency wanted in a test
    // helper. Unknown flags are a fail-closed error so typos can't
    // masquerade as valid runs.
    for (int i = 1; i < argc; ++i) {
        const std::string_view a = argv[i];
        auto take = [&]() -> const char* {
            return (i + 1 < argc) ? argv[++i] : nullptr;
        };
        if      (a == "--state-path")     { auto v = take(); if (!v) return Exit::BadArgs; state_path    = v; }
        else if (a == "--op")             { auto v = take(); if (!v) return Exit::BadArgs; op            = v; }
        else if (a == "--nonce")          { auto v = take(); if (!v) return Exit::BadArgs; nonce_hex     = v; }
        else if (a == "--runtime-epoch")  { auto v = take(); if (!v) return Exit::BadArgs; runtime_epoch = std::strtoull(v, nullptr, 10); }
        else if (a == "--minimum-epoch")  { auto v = take(); if (!v) return Exit::BadArgs; minimum_epoch = std::strtoull(v, nullptr, 10); }
        else                              { return Exit::BadArgs; }
    }
    if (state_path.empty() || op.empty()) return Exit::BadArgs;

    VMPilot::Runtime::State::FileBackedStateProvider provider(state_path);

    if (op == "reserve-nonce") {
        std::array<std::uint8_t, 32> nonce{};
        if (!hex_to_bytes(nonce_hex, nonce.data(), nonce.size())) {
            return Exit::BadArgs;
        }
        auto r = provider.reserve_nonce(nonce);
        if (r.has_value()) return Exit::Ok;
        return map_store_error(r.error());
    }

    if (op == "advance-epoch") {
        VMPilot::Runtime::State::EpochState s{};
        s.runtime_epoch          = runtime_epoch;
        s.minimum_accepted_epoch = minimum_epoch;
        auto r = provider.advance_epoch_state(s);
        if (r.has_value()) return Exit::Ok;
        return map_store_error(r.error());
    }

    return Exit::BadArgs;
}
