// Test-only Ed25519 signer. See sign_ed25519_testonly.hpp.
//
// Directly invokes Botan 3's Ed25519 primitives. This TU exists only so the
// production `VMPilot::Crypto` adaptor never exposes signing. Included solely
// in the `vmpilot_runtime_test_fixtures` static library.

#include "sign_ed25519_testonly.hpp"

#include <cstdint>
#include <span>
#include <stdexcept>

#include <botan/auto_rng.h>
#include <botan/ed25519.h>
#include <botan/pubkey.h>

namespace VMPilot::Fixtures::Sign {

namespace {

// Length-prefixed domain scheme: 1 byte length (must fit in 255) followed by
// the UTF-8 domain bytes, followed by the caller-supplied message. Matches
// the verifier rule in common/crypto (Stage 2). Keep in lockstep.
inline void append_length_prefixed_domain(std::vector<std::uint8_t>& out,
                                          std::string_view covered_domain) {
    if (covered_domain.size() > 0xff) {
        throw std::invalid_argument("sign_ed25519_testonly: covered_domain too long");
    }
    out.push_back(static_cast<std::uint8_t>(covered_domain.size()));
    out.insert(out.end(),
               reinterpret_cast<const std::uint8_t*>(covered_domain.data()),
               reinterpret_cast<const std::uint8_t*>(covered_domain.data()) +
                   covered_domain.size());
}

}  // namespace

std::vector<std::uint8_t>
domain_prefixed_message(std::string_view covered_domain,
                        const std::vector<std::uint8_t>& message) {
    std::vector<std::uint8_t> out;
    out.reserve(1 + covered_domain.size() + message.size());
    append_length_prefixed_domain(out, covered_domain);
    out.insert(out.end(), message.begin(), message.end());
    return out;
}

std::array<std::uint8_t, 64>
sign_ed25519(const std::array<std::uint8_t, 32>& private_seed,
             std::string_view covered_domain,
             const std::vector<std::uint8_t>& message) {
    const std::span<const std::uint8_t> seed_span{private_seed.data(), private_seed.size()};
    Botan::Ed25519_PrivateKey priv = Botan::Ed25519_PrivateKey::from_seed(seed_span);

    Botan::AutoSeeded_RNG rng;
    Botan::PK_Signer signer{priv, rng, "Pure"};

    const auto prefixed = domain_prefixed_message(covered_domain, message);
    signer.update(prefixed.data(), prefixed.size());
    const auto sig = signer.signature(rng);

    if (sig.size() != 64) {
        throw std::runtime_error("sign_ed25519_testonly: unexpected signature size");
    }
    std::array<std::uint8_t, 64> out{};
    for (std::size_t i = 0; i < 64; ++i) out[i] = sig[i];
    return out;
}

}  // namespace VMPilot::Fixtures::Sign
