#ifndef VMPILOT_RUNTIME_TEST_FIXTURES_SIGN_ED25519_TESTONLY_HPP
#define VMPILOT_RUNTIME_TEST_FIXTURES_SIGN_ED25519_TESTONLY_HPP

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Test-only Ed25519 signing helper.
//
// Directly calls the active crypto backend's signer (Botan PK_Signer today)
// bypassing the VMPilot::Crypto adaptor interface so that `Sign_Ed25519`
// never leaks into the production crypto public header. Only linked into
// test fixture library `vmpilot_runtime_test_fixtures`.
//
// The signed message is `length_prefix(covered_domain) || message`, matching
// the verifier rule in common/crypto (Stage 2). Keeping this rule identical
// on both sides is the whole point of the helper.

namespace VMPilot::Fixtures::Sign {

// 64-byte Ed25519 signature over length-prefixed domain + message, using the
// 32-byte Ed25519 seed as the private key. Throws `std::runtime_error` on
// backend failure (tests should treat that as a hard failure, not a soft
// assertion).
std::array<std::uint8_t, 64>
sign_ed25519(const std::array<std::uint8_t, 32>& private_seed,
             std::string_view covered_domain,
             const std::vector<std::uint8_t>& message);

// Same length-prefixed domain scheme used by the production verifier.
// Exposed so tests can independently compute the pre-sign message and check
// the verifier agrees byte-for-byte.
std::vector<std::uint8_t>
domain_prefixed_message(std::string_view covered_domain,
                        const std::vector<std::uint8_t>& message);

}  // namespace VMPilot::Fixtures::Sign

#endif  // VMPILOT_RUNTIME_TEST_FIXTURES_SIGN_ED25519_TESTONLY_HPP
