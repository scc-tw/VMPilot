#ifndef VMPILOT_COMMON_CBOR_ENCODE_HPP
#define VMPILOT_COMMON_CBOR_ENCODE_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Minimal, deterministic CBOR encoder.
//
// The production runtime stays primarily verify-only (doc 07), but
// some runtime-internal records — provider evidence is the concrete
// case — genuinely need to be hashed into stable bytes. Hand-rolled
// little-endian layouts for those introduce the same serializer-drift
// risk we resolved for PolicyRequirement (cross-producer sync).
//
// This encoder is a strict-CBOR subset matching the decoder in
// common/include/cbor/strict.hpp: shortest-form integers, definite
// lengths only, map keys sorted by encoded bytes (RFC 8949 §4.2.1),
// no floats, no tags, no indefinite-length containers.
//
// Shares its shape with the fixture encoder in
// runtime/test/fixtures/cbor_test_encoder.hpp; that header keeps its
// separate namespace for test-only convenience aliases.

namespace VMPilot::Cbor::Encode {

// Streaming writer. Append primitives (uint, bytes, text) or
// container headers (array/map) directly; containers track their
// declared element count via the CBOR head, not the writer.
class Writer {
public:
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept { return buf_; }
    std::vector<std::uint8_t> take() noexcept { return std::move(buf_); }
    [[nodiscard]] std::size_t size() const noexcept { return buf_.size(); }

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

// Stand-alone single-value encoders — primarily used to produce
// already-encoded elements to feed MapBuilder / encode_array.
[[nodiscard]] inline std::vector<std::uint8_t> encode_uint(std::uint64_t v) {
    Writer w;
    w.uint_(v);
    return w.take();
}
[[nodiscard]] inline std::vector<std::uint8_t> encode_text(std::string_view s) {
    Writer w;
    w.text_(s);
    return w.take();
}
[[nodiscard]] inline std::vector<std::uint8_t>
encode_bytes(const std::vector<std::uint8_t>& b) {
    Writer w;
    w.bytes_(b);
    return w.take();
}
[[nodiscard]] inline std::vector<std::uint8_t>
encode_bytes(const std::uint8_t* data, std::size_t n) {
    Writer w;
    w.bytes_(data, n);
    return w.take();
}

// Build a canonical map from (encoded_key_bytes, encoded_value_bytes)
// pairs. Sorts by encoded key bytes and rejects duplicates at build
// time (RFC 8949 §4.2.1); callers feed each key through encode_uint /
// encode_text first.
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

    [[nodiscard]] std::vector<std::uint8_t> build() {
        std::sort(entries_.begin(), entries_.end(),
                  [](const Entry& a, const Entry& b) {
                      return std::lexicographical_compare(
                          a.first.begin(), a.first.end(),
                          b.first.begin(), b.first.end());
                  });
        for (std::size_t i = 1; i < entries_.size(); ++i) {
            if (entries_[i - 1].first == entries_[i].first) {
                throw std::invalid_argument(
                    "cbor::encode: duplicate map key");
            }
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

// Definite-length array from already-encoded elements.
[[nodiscard]] inline std::vector<std::uint8_t>
encode_array(const std::vector<std::vector<std::uint8_t>>& elems) {
    Writer w;
    w.array_begin(elems.size());
    auto buf = w.take();
    for (const auto& e : elems) buf.insert(buf.end(), e.begin(), e.end());
    return buf;
}

}  // namespace VMPilot::Cbor::Encode

#endif  // VMPILOT_COMMON_CBOR_ENCODE_HPP
