#ifndef VMPILOT_RUNTIME_TEST_FIXTURES_CBOR_TEST_ENCODER_HPP
#define VMPILOT_RUNTIME_TEST_FIXTURES_CBOR_TEST_ENCODER_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Minimal, deterministic CBOR encoder for test fixture assembly.
//
// Intentionally smaller than libcbor and shaped to produce bytes that the
// Stage 1 strict decoder accepts without complaint. Test-only — this encoder
// is never linked into production runtime.
//
// Rules enforced:
//   • shortest-form unsigned integers (major 0)
//   • definite-length byte / text strings, arrays, maps
//   • map keys sorted by encoded CBOR bytes (RFC 8949 §4.2.1)
//   • no floats, no indefinite length, no tags

namespace VMPilot::Fixtures::Cbor {

// Output buffer holding the serialized CBOR bytes.
class Writer {
public:
    const std::vector<std::uint8_t>& bytes() const noexcept { return buf_; }
    std::vector<std::uint8_t> take() noexcept { return std::move(buf_); }
    std::size_t size() const noexcept { return buf_.size(); }

    void uint_(std::uint64_t v) { write_head(0, v); }

    void bytes_(const std::uint8_t* data, std::size_t n) {
        write_head(2, n);
        buf_.insert(buf_.end(), data, data + n);
    }
    void bytes_(const std::vector<std::uint8_t>& data) {
        bytes_(data.data(), data.size());
    }

    void text_(std::string_view s) {
        write_head(3, s.size());
        buf_.insert(buf_.end(),
                    reinterpret_cast<const std::uint8_t*>(s.data()),
                    reinterpret_cast<const std::uint8_t*>(s.data()) + s.size());
    }

    void array_begin(std::size_t n) { write_head(4, n); }
    void map_begin(std::size_t n)   { write_head(5, n); }

private:
    std::vector<std::uint8_t> buf_;

    void write_head(std::uint8_t major, std::uint64_t v) {
        const std::uint8_t m = static_cast<std::uint8_t>(major << 5);
        if (v < 24) {
            buf_.push_back(m | static_cast<std::uint8_t>(v));
        } else if (v <= 0xffULL) {
            buf_.push_back(m | 24);
            buf_.push_back(static_cast<std::uint8_t>(v));
        } else if (v <= 0xffffULL) {
            buf_.push_back(m | 25);
            buf_.push_back(static_cast<std::uint8_t>((v >> 8) & 0xff));
            buf_.push_back(static_cast<std::uint8_t>(v & 0xff));
        } else if (v <= 0xffffffffULL) {
            buf_.push_back(m | 26);
            for (int i = 3; i >= 0; --i)
                buf_.push_back(static_cast<std::uint8_t>((v >> (i * 8)) & 0xff));
        } else {
            buf_.push_back(m | 27);
            for (int i = 7; i >= 0; --i)
                buf_.push_back(static_cast<std::uint8_t>((v >> (i * 8)) & 0xff));
        }
    }
};

// Encode a single primitive into a standalone byte vector.
inline std::vector<std::uint8_t> encode_uint(std::uint64_t v) {
    Writer w;
    w.uint_(v);
    return w.take();
}
inline std::vector<std::uint8_t> encode_text(std::string_view s) {
    Writer w;
    w.text_(s);
    return w.take();
}
inline std::vector<std::uint8_t> encode_bytes(const std::vector<std::uint8_t>& b) {
    Writer w;
    w.bytes_(b);
    return w.take();
}

// Build a canonical map from (key_bytes, value_bytes) pairs. Caller provides
// each key already encoded; the encoder sorts by encoded key bytes and emits
// the map header + entries. Duplicate keys throw — canonical CBOR forbids
// them (RFC 8949 §4.2.1).
class MapBuilder {
public:
    void put(std::vector<std::uint8_t> key, std::vector<std::uint8_t> value) {
        entries_.emplace_back(std::move(key), std::move(value));
    }
    void put_text(std::string_view key, std::vector<std::uint8_t> value) {
        put(encode_text(key), std::move(value));
    }
    void put_uint(std::uint64_t key, std::vector<std::uint8_t> value) {
        put(encode_uint(key), std::move(value));
    }

    std::vector<std::uint8_t> build() {
        std::sort(entries_.begin(), entries_.end(),
                  [](const Entry& a, const Entry& b) {
                      return std::lexicographical_compare(
                          a.first.begin(), a.first.end(),
                          b.first.begin(), b.first.end());
                  });
        for (std::size_t i = 1; i < entries_.size(); ++i) {
            if (entries_[i - 1].first == entries_[i].first)
                throw std::invalid_argument("cbor_test_encoder: duplicate map key");
        }
        Writer w;
        w.map_begin(entries_.size());
        auto buf = w.take();
        for (auto& [k, v] : entries_) {
            buf.insert(buf.end(), k.begin(), k.end());
            buf.insert(buf.end(), v.begin(), v.end());
        }
        return buf;
    }

private:
    using Entry = std::pair<std::vector<std::uint8_t>, std::vector<std::uint8_t>>;
    std::vector<Entry> entries_;
};

// Build a definite-length array from already-encoded element bytes.
inline std::vector<std::uint8_t>
encode_array(const std::vector<std::vector<std::uint8_t>>& elems) {
    Writer w;
    w.array_begin(elems.size());
    auto buf = w.take();
    for (const auto& e : elems) buf.insert(buf.end(), e.begin(), e.end());
    return buf;
}

}  // namespace VMPilot::Fixtures::Cbor

#endif  // VMPILOT_RUNTIME_TEST_FIXTURES_CBOR_TEST_ENCODER_HPP
