// Stage 12 — persistent epoch + nonce replay store.
//
// Every test scribbles into a unique temp path so the file-backed
// stores can exercise load/save without polluting a shared fixture
// directory.

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#if defined(_WIN32)
#include <process.h>  // _getpid
#define VMPILOT_GETPID() ::_getpid()
#else
#include <unistd.h>   // getpid
#define VMPILOT_GETPID() ::getpid()
#endif

#include "epoch_store.hpp"

namespace {

namespace fs = std::filesystem;

using VMPilot::Runtime::EpochStore::EpochState;
using VMPilot::Runtime::EpochStore::FileBackedEpochStore;
using VMPilot::Runtime::EpochStore::FileBackedNonceStore;
using VMPilot::Runtime::EpochStore::StoreError;

class TempFile {
public:
    explicit TempFile(std::string suffix) {
        static int counter = 0;
        ++counter;
        path_ = (fs::temp_directory_path() /
                 ("vmpilot_stage12_" + std::to_string(VMPILOT_GETPID()) + "_" +
                  std::to_string(counter) + "_" + suffix))
                    .string();
        std::error_code ec;
        fs::remove(path_, ec);
    }
    ~TempFile() {
        std::error_code ec;
        fs::remove(path_, ec);
        fs::remove(path_ + ".tmp", ec);
    }
    const std::string& path() const noexcept { return path_; }

private:
    std::string path_;
};

std::array<std::uint8_t, 32> nonce_from_byte(std::uint8_t b) {
    std::array<std::uint8_t, 32> n{};
    n.fill(b);
    return n;
}

}  // namespace

// ─── Epoch ──────────────────────────────────────────────────────────────

TEST(EpochStore, FreshStoreReadsZeros) {
    TempFile tf("epoch");
    FileBackedEpochStore store(tf.path());
    const auto state = store.get();
    EXPECT_EQ(state.runtime_epoch, 0u);
    EXPECT_EQ(state.minimum_accepted_epoch, 0u);
}

TEST(EpochStore, AdvanceThenReloadRoundtripsExactValues) {
    TempFile tf("epoch");
    {
        FileBackedEpochStore store(tf.path());
        auto r = store.advance({5, 3});
        ASSERT_TRUE(r.has_value()) << static_cast<int>(r.error());
    }
    FileBackedEpochStore reopened(tf.path());
    const auto state = reopened.get();
    EXPECT_EQ(state.runtime_epoch, 5u);
    EXPECT_EQ(state.minimum_accepted_epoch, 3u);
}

TEST(EpochStore, MonotonicAdvanceRejectsRollback) {
    TempFile tf("epoch");
    FileBackedEpochStore store(tf.path());
    ASSERT_TRUE(store.advance({10, 10}).has_value());

    // Either field moving backwards must be rejected, and the
    // persisted state must remain at the higher value.
    auto r = store.advance({9, 10});
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::EpochRollbackDenied);
    EXPECT_EQ(store.get().runtime_epoch, 10u);

    r = store.advance({10, 9});
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::EpochRollbackDenied);
    EXPECT_EQ(store.get().minimum_accepted_epoch, 10u);
}

TEST(EpochStore, AdvanceToEqualValuesIsAllowed) {
    TempFile tf("epoch");
    FileBackedEpochStore store(tf.path());
    ASSERT_TRUE(store.advance({7, 7}).has_value());
    // No-op advance at the same epoch should succeed and leave state
    // untouched; the saved file stays valid.
    ASSERT_TRUE(store.advance({7, 7}).has_value());
    EXPECT_EQ(store.get().runtime_epoch, 7u);
}

TEST(EpochStore, CorruptMagicFailsLoad) {
    TempFile tf("epoch");
    {
        FileBackedEpochStore store(tf.path());
        ASSERT_TRUE(store.advance({4, 2}).has_value());
    }
    // Clobber the first byte of the magic so the next load fails.
    {
        std::fstream fs(tf.path(), std::ios::binary | std::ios::in |
                                       std::ios::out);
        ASSERT_TRUE(fs.is_open());
        fs.put('X');
    }
    FileBackedEpochStore store(tf.path());
    auto r = store.load();
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::Corrupt);
}

TEST(EpochStore, CorruptCrcFailsLoad) {
    TempFile tf("epoch");
    {
        FileBackedEpochStore store(tf.path());
        ASSERT_TRUE(store.advance({4, 2}).has_value());
    }
    // Flip the very last byte — that's the stored CRC tail.
    {
        std::fstream fs(tf.path(),
                        std::ios::binary | std::ios::in | std::ios::out);
        ASSERT_TRUE(fs.is_open());
        fs.seekg(0, std::ios::end);
        const auto end = fs.tellg();
        fs.seekp(end - std::streamoff(1));
        fs.put(static_cast<char>(0xFF));
    }
    FileBackedEpochStore store(tf.path());
    auto r = store.load();
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::Corrupt);
}

// ─── Nonce store ────────────────────────────────────────────────────────

TEST(NonceStore, MarkThenQueryReports_IsConsumed) {
    TempFile tf("nonce");
    FileBackedNonceStore store(tf.path());
    const auto n = nonce_from_byte(0x42);
    EXPECT_FALSE(store.is_consumed(n));
    store.mark_consumed(n);
    EXPECT_TRUE(store.is_consumed(n));
    EXPECT_FALSE(store.is_consumed(nonce_from_byte(0x43)));
}

TEST(NonceStore, PersistThenReloadSurvivesEntries) {
    TempFile tf("nonce");
    const auto n1 = nonce_from_byte(0x11);
    const auto n2 = nonce_from_byte(0x22);
    const auto n3 = nonce_from_byte(0x33);
    {
        FileBackedNonceStore store(tf.path());
        store.mark_consumed(n1);
        store.mark_consumed(n2);
        auto r = store.persist();
        ASSERT_TRUE(r.has_value());
    }
    FileBackedNonceStore reopened(tf.path());
    EXPECT_TRUE(reopened.is_consumed(n1));
    EXPECT_TRUE(reopened.is_consumed(n2));
    EXPECT_FALSE(reopened.is_consumed(n3));
    EXPECT_EQ(reopened.size(), 2u);
}

TEST(NonceStore, CorruptFileFailsLoad) {
    TempFile tf("nonce");
    {
        FileBackedNonceStore store(tf.path());
        store.mark_consumed(nonce_from_byte(0x55));
        ASSERT_TRUE(store.persist().has_value());
    }
    {
        std::fstream fs(tf.path(),
                        std::ios::binary | std::ios::in | std::ios::out);
        ASSERT_TRUE(fs.is_open());
        fs.put('X');  // break magic
    }
    FileBackedNonceStore store(tf.path());
    auto r = store.load();
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::Corrupt);
}

TEST(NonceStore, ReloadOverEmptyFilePreservesInMemoryState) {
    // An unrelated file that just isn't ours shouldn't wipe the
    // in-memory set under us: load() only replaces `consumed_` on
    // success. A failed load leaves the caller's runtime state intact.
    TempFile tf("nonce");
    FileBackedNonceStore store(tf.path());
    store.mark_consumed(nonce_from_byte(0x77));
    // Write garbage into the path so load() fails.
    {
        std::ofstream os(tf.path(), std::ios::binary | std::ios::trunc);
        os << "not a nonce file";
    }
    auto r = store.load();
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), StoreError::Corrupt);
    // In-memory state still reports the nonce as consumed.
    EXPECT_TRUE(store.is_consumed(nonce_from_byte(0x77)));
}

TEST(NonceStore, PersistIsAtomicAcrossMultipleCalls) {
    // Exercise multiple persist cycles to ensure the atomic rename
    // path doesn't leave stale `.tmp` files behind that later load
    // rounds could mistake for the primary.
    TempFile tf("nonce");
    FileBackedNonceStore store(tf.path());
    for (std::uint8_t i = 0; i < 32; ++i) {
        store.mark_consumed(nonce_from_byte(i));
        ASSERT_TRUE(store.persist().has_value());
    }
    FileBackedNonceStore reopened(tf.path());
    EXPECT_EQ(reopened.size(), 32u);
    for (std::uint8_t i = 0; i < 32; ++i) {
        EXPECT_TRUE(reopened.is_consumed(nonce_from_byte(i)));
    }
    // .tmp should no longer exist after a clean persist.
    EXPECT_FALSE(fs::exists(tf.path() + ".tmp"));
}
