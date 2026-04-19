#ifndef VMPILOT_RUNTIME_TEST_FIXTURES_CBOR_TEST_ENCODER_HPP
#define VMPILOT_RUNTIME_TEST_FIXTURES_CBOR_TEST_ENCODER_HPP

#include "cbor/encode.hpp"

// Test-only convenience namespace that forwards to the shared
// strict-CBOR encoder in common/include/cbor/encode.hpp. The fixture
// generator used to carry a parallel copy here; keeping two encoders
// in sync courts drift (a fix in one never propagates to the other),
// so this header now aliases the single canonical implementation.
//
// The namespace is retained so existing `VMPilot::Fixtures::Cbor::foo`
// references at test-site call points keep compiling.

namespace VMPilot::Fixtures::Cbor {

using VMPilot::Cbor::Encode::Writer;
using VMPilot::Cbor::Encode::MapBuilder;
using VMPilot::Cbor::Encode::encode_uint;
using VMPilot::Cbor::Encode::encode_text;
using VMPilot::Cbor::Encode::encode_bytes;
using VMPilot::Cbor::Encode::encode_array;

}  // namespace VMPilot::Fixtures::Cbor

#endif  // VMPILOT_RUNTIME_TEST_FIXTURES_CBOR_TEST_ENCODER_HPP
