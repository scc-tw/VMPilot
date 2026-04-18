#include "cbor/strict.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "VMPilot_crypto.hpp"

namespace VMPilot::Cbor {

namespace {

// CBOR major type encoded in the top 3 bits of the initial byte.
enum MajorType : std::uint8_t {
    Major_Uint   = 0,
    Major_NegInt = 1,
    Major_Bytes  = 2,
    Major_Text   = 3,
    Major_Array  = 4,
    Major_Map    = 5,
    Major_Tag    = 6,
    Major_FloatSimple = 7,
};

struct Cursor {
    const std::uint8_t* base;
    std::size_t size;
    std::size_t pos;

    std::size_t remaining() const noexcept { return size - pos; }
    bool at_end() const noexcept { return pos >= size; }
};

// Compact error construction.
inline tl::unexpected<ParseError> err(ErrorCode code, std::size_t offset) noexcept {
    return tl::make_unexpected(ParseError{code, offset});
}

// Read a big-endian integer of width `width` bytes (1, 2, 4, or 8) at the
// current cursor position. Does not advance. Caller must have range-checked.
std::uint64_t read_be(const std::uint8_t* p, std::size_t width) noexcept {
    std::uint64_t v = 0;
    for (std::size_t i = 0; i < width; ++i) {
        v = (v << 8) | p[i];
    }
    return v;
}

// Decode the additional-info field and the argument, enforcing shortest-form
// encoding. Advances `c.pos` past the initial byte and any follow-up bytes.
// On success writes the decoded unsigned argument to `out`.
tl::expected<std::uint64_t, ParseError>
decode_argument(Cursor& c, std::uint8_t info) noexcept {
    // Initial byte has already been consumed by the caller; we only read
    // follow-up bytes here. Offsets reported for canonicality errors point
    // at the initial byte (one before c.pos), which is where a strict
    // validator would flag the problem.
    const std::size_t head_offset = c.pos - 1;

    if (info < 24) {
        return static_cast<std::uint64_t>(info);
    }

    std::size_t width = 0;
    switch (info) {
        case 24: width = 1; break;
        case 25: width = 2; break;
        case 26: width = 4; break;
        case 27: width = 8; break;
        case 28:
        case 29:
        case 30:
            return err(ErrorCode::ReservedAdditionalInfo, head_offset);
        case 31:
            return err(ErrorCode::IndefiniteLength, head_offset);
        default:
            return err(ErrorCode::ReservedAdditionalInfo, head_offset);
    }

    if (c.remaining() < width) {
        return err(ErrorCode::TruncatedInput, head_offset);
    }

    const std::uint64_t v = read_be(c.base + c.pos, width);
    c.pos += width;

    // Shortest-form check: value must not fit in a narrower encoding.
    bool ok;
    switch (info) {
        case 24: ok = v >= 24; break;
        case 25: ok = v > 0xffULL; break;
        case 26: ok = v > 0xffffULL; break;
        case 27: ok = v > 0xffffffffULL; break;
        default: ok = false; break;  // unreachable
    }
    if (!ok) {
        return err(ErrorCode::NonCanonicalInteger, head_offset);
    }
    return v;
}

tl::expected<Value, ParseError>
parse_value(Cursor& c, std::size_t depth) noexcept {
    if (depth >= kMaxDepth) {
        return err(ErrorCode::DepthLimitExceeded, c.pos);
    }
    if (c.at_end()) {
        return err(ErrorCode::TruncatedInput, c.pos);
    }

    const std::size_t head_offset = c.pos;
    const std::uint8_t head = c.base[c.pos++];
    const std::uint8_t major = static_cast<std::uint8_t>(head >> 5);
    const std::uint8_t info  = head & 0x1f;

    // Gatekeeper: reject majors the strict profile forbids before touching
    // the argument. This catches "negative integer" encoded like a uint.
    switch (major) {
        case Major_NegInt:
        case Major_Tag:
        case Major_FloatSimple:
            return err(ErrorCode::DisallowedMajorType, head_offset);
        default: break;
    }

    auto arg = decode_argument(c, info);
    if (!arg) return tl::make_unexpected(arg.error());
    const std::uint64_t a = *arg;

    switch (major) {
        case Major_Uint:
            return Value::make_uint(a);

        case Major_Bytes: {
            if (c.remaining() < a) return err(ErrorCode::TruncatedInput, head_offset);
            std::vector<std::uint8_t> buf(c.base + c.pos, c.base + c.pos + a);
            c.pos += a;
            return Value::make_bytes(std::move(buf));
        }

        case Major_Text: {
            if (c.remaining() < a) return err(ErrorCode::TruncatedInput, head_offset);
            std::string s(reinterpret_cast<const char*>(c.base + c.pos), a);
            c.pos += a;
            return Value::make_text(std::move(s));
        }

        case Major_Array: {
            std::vector<Value> children;
            children.reserve(a);
            for (std::uint64_t i = 0; i < a; ++i) {
                auto child = parse_value(c, depth + 1);
                if (!child) return child;
                children.push_back(std::move(*child));
            }
            return Value::make_array(std::move(children));
        }

        case Major_Map: {
            // Parse `a` (key, value) pairs. Between pairs, enforce that each
            // new key's encoded byte sequence is strictly greater than the
            // previous key's encoded bytes. Strict inequality catches
            // duplicates without a second scan.
            std::vector<Value> flat;
            flat.reserve(2 * a);
            std::size_t last_key_start = 0;
            std::size_t last_key_end   = 0;
            for (std::uint64_t i = 0; i < a; ++i) {
                const std::size_t key_start = c.pos;
                auto key = parse_value(c, depth + 1);
                if (!key) return key;
                const std::size_t key_end = c.pos;

                if (i > 0) {
                    const int cmp = std::memcmp(
                        c.base + last_key_start, c.base + key_start,
                        std::min(last_key_end - last_key_start,
                                 key_end - key_start));
                    const std::size_t lhs_len = last_key_end - last_key_start;
                    const std::size_t rhs_len = key_end - key_start;
                    bool in_order;
                    if (cmp < 0) {
                        in_order = true;
                    } else if (cmp > 0) {
                        in_order = false;
                    } else {
                        // Common prefix identical — decide by length; equal
                        // length + equal content means duplicate key.
                        if (lhs_len < rhs_len) in_order = true;
                        else if (lhs_len > rhs_len) in_order = false;
                        else {
                            return err(ErrorCode::DuplicateMapKey, key_start);
                        }
                    }
                    if (!in_order) {
                        return err(ErrorCode::MapKeyOutOfOrder, key_start);
                    }
                }

                auto val = parse_value(c, depth + 1);
                if (!val) return val;

                flat.push_back(std::move(*key));
                flat.push_back(std::move(*val));
                last_key_start = key_start;
                last_key_end   = key_end;
            }
            return Value::make_map(std::move(flat));
        }

        default:
            // Unreachable: disallowed majors rejected above, remaining values
            // 0..5 covered by the switch.
            return err(ErrorCode::DisallowedMajorType, head_offset);
    }
}

}  // namespace

// ─── Value factories ────────────────────────────────────────────────────

Value Value::make_uint(std::uint64_t v) {
    Value out;
    out.kind_ = Kind::Uint;
    out.uint_ = v;
    return out;
}
Value Value::make_bytes(std::vector<std::uint8_t> b) {
    Value out;
    out.kind_ = Kind::Bytes;
    out.bytes_ = std::move(b);
    return out;
}
Value Value::make_text(std::string s) {
    Value out;
    out.kind_ = Kind::Text;
    out.text_ = std::move(s);
    return out;
}
Value Value::make_array(std::vector<Value> children) {
    Value out;
    out.kind_ = Kind::Array;
    out.children_ = std::move(children);
    return out;
}
Value Value::make_map(std::vector<Value> flat_kv) {
    Value out;
    out.kind_ = Kind::Map;
    out.children_ = std::move(flat_kv);
    return out;
}

const Value* Value::find_by_uint_key(std::uint64_t key) const noexcept {
    if (kind_ != Kind::Map) return nullptr;
    for (std::size_t i = 0; i < map_size(); ++i) {
        const auto& k = map_key_at(i);
        if (k.kind() == Kind::Uint && k.as_uint() == key) {
            return &map_value_at(i);
        }
    }
    return nullptr;
}

const Value* Value::find_by_text_key(std::string_view key) const noexcept {
    if (kind_ != Kind::Map) return nullptr;
    for (std::size_t i = 0; i < map_size(); ++i) {
        const auto& k = map_key_at(i);
        if (k.kind() == Kind::Text && k.as_text() == key) {
            return &map_value_at(i);
        }
    }
    return nullptr;
}

// ─── Public parse entry ─────────────────────────────────────────────────

tl::expected<Value, ParseError>
parse_strict(const std::uint8_t* data, std::size_t size) noexcept {
    Cursor c{data, size, 0};
    auto v = parse_value(c, 0);
    if (!v) return v;
    if (c.pos != c.size) {
        return err(ErrorCode::TrailingBytes, c.pos);
    }
    return v;
}

// ─── Domain-separated hash ──────────────────────────────────────────────

std::array<std::uint8_t, 32>
domain_hash_sha256(std::string_view domain_label,
                   const std::uint8_t* canonical_bytes,
                   std::size_t size) noexcept {
    if (domain_label.empty() || domain_label.size() > 0xff) {
        std::abort();
    }
    std::vector<std::uint8_t> buf;
    buf.reserve(1 + domain_label.size() + size);
    buf.push_back(static_cast<std::uint8_t>(domain_label.size()));
    buf.insert(buf.end(),
               reinterpret_cast<const std::uint8_t*>(domain_label.data()),
               reinterpret_cast<const std::uint8_t*>(domain_label.data()) + domain_label.size());
    if (size > 0) {
        buf.insert(buf.end(), canonical_bytes, canonical_bytes + size);
    }

    const auto digest = VMPilot::Crypto::SHA256(buf, /*salt=*/{});
    std::array<std::uint8_t, 32> out{};
    if (digest.size() == 32) {
        std::memcpy(out.data(), digest.data(), 32);
    }
    return out;
}

}  // namespace VMPilot::Cbor
