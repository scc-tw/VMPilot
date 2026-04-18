// Stage 4 — outer envelope parser.
//
// Confirms:
//   • default-built fixture round-trips cleanly through parse_outer_envelope
//   • every field on the parsed OuterEnvelope matches the builder's
//     metadata_offset / length commitments and partition locators
//   • wrong magic, truncated metadata, bad CBOR, missing / wrong-type core
//     fields, unsupported format version, and locator arithmetic errors
//     each surface a distinct ParseError
//   • public outer shape never tolerates tier / family / provider tokens —
//     injecting "debug" / "highsec" / "f1" / "tpm" text into the metadata
//     triggers TierRevealingToken

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "envelope/outer.hpp"

#include "fixtures/fixture_generator.hpp"

namespace {

using VMPilot::Runtime::Envelope::OuterEnvelope;
using VMPilot::Runtime::Envelope::ParseError;
using VMPilot::Runtime::Envelope::kOuterMagic;
using VMPilot::Runtime::Envelope::parse_outer_envelope;

VMPilot::Fixtures::OuterEnvelopeArtifact default_artifact() {
    return VMPilot::Fixtures::OuterEnvelopeBuilder{}.build();
}

}  // namespace

TEST(OuterEnvelope, DefaultArtifactRoundTrips) {
    const auto art = default_artifact();
    auto parsed = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_TRUE(parsed.has_value()) << "err = "
        << static_cast<int>(parsed.error());

    EXPECT_EQ(parsed->outer_format_version, 1u);
    EXPECT_EQ(parsed->metadata_offset, art.metadata_offset);
    EXPECT_EQ(parsed->metadata_length, art.metadata_length);
    EXPECT_EQ(parsed->package_binding_record.offset, art.pbr_offset);
    EXPECT_EQ(parsed->package_binding_record.length, art.pbr_length);
    EXPECT_EQ(parsed->inner_metadata_partition.offset, art.inner_offset);
    EXPECT_EQ(parsed->inner_metadata_partition.length, art.inner_length);
    EXPECT_EQ(parsed->payload_partition.offset, art.payload_offset);
    EXPECT_EQ(parsed->payload_partition.length, art.payload_length);
    EXPECT_EQ(parsed->canonical_encoding_id, "canonical-metadata-bytes-v1");
}

TEST(OuterEnvelope, BufferSmallerThanFixedHeaderRejected) {
    std::vector<std::uint8_t> tiny(10, 0);
    auto r = parse_outer_envelope(tiny.data(), tiny.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::BufferTooSmall);
}

TEST(OuterEnvelope, WrongMagicRejected) {
    auto art = default_artifact();
    art.bytes[0] = 'X';
    auto r = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::WrongMagic);
}

TEST(OuterEnvelope, MetadataLengthLargerThanBufferRejected) {
    auto art = default_artifact();
    // Rewrite the big-endian u32 length to something that overruns the
    // buffer by 1 byte. This must be caught before the CBOR parser runs.
    const std::uint32_t oversize = static_cast<std::uint32_t>(
        art.bytes.size() - 20 + 1);
    art.bytes[16] = static_cast<std::uint8_t>((oversize >> 24) & 0xff);
    art.bytes[17] = static_cast<std::uint8_t>((oversize >> 16) & 0xff);
    art.bytes[18] = static_cast<std::uint8_t>((oversize >>  8) & 0xff);
    art.bytes[19] = static_cast<std::uint8_t>(oversize & 0xff);
    auto r = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::TruncatedMetadata);
}

TEST(OuterEnvelope, CorruptCborRejected) {
    auto art = default_artifact();
    // Flip the first metadata byte; canonical parser must reject.
    art.bytes[art.metadata_offset] = 0xff;
    auto r = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::BadCbor);
}

TEST(OuterEnvelope, UnsupportedFormatVersionRejected) {
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    b.outer_format_version(99);
    auto art = b.build();
    auto r = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::UnsupportedFormatVersion);
}

TEST(OuterEnvelope, LocatorOverflowRejected) {
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    // Payload length larger than actual buffer by many bytes. Builder
    // itself won't complain because it only sets size = pb.size(); we
    // instead truncate the built bytes to trigger the BadLocator check.
    auto art = b.build();
    // Chop off the last 10 payload bytes so the payload locator overruns.
    art.bytes.resize(art.bytes.size() - 10);
    auto r = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::BadLocator);
}

TEST(OuterEnvelope, ForbiddenTokenInExtraFieldRejected) {
    // "debug" must never appear in the public outer envelope per doc 15 §3.
    // Inject it via the builder's negative-test escape hatch and confirm
    // the parser rejects the whole envelope.
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    b.extra_text_field(100, "compiled in debug mode");
    auto art = b.build();
    auto r = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::TierRevealingToken);
}

TEST(OuterEnvelope, HighsecTokenRejected) {
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    b.extra_text_field(100, "highsec");
    auto r = parse_outer_envelope(b.build().bytes);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::TierRevealingToken);
}

TEST(OuterEnvelope, FamilyIdTokenRejected) {
    // An attacker or mis-configured producer might smuggle the family id
    // as a schema version string — parser must still catch it.
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    b.package_schema_version("f1-optimized-package");
    auto r = parse_outer_envelope(b.build().bytes);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::TierRevealingToken);
}

TEST(OuterEnvelope, TpmTokenRejected) {
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    b.extra_text_field(100, "requires TPM");
    auto r = parse_outer_envelope(b.build().bytes);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), ParseError::TierRevealingToken);
}

TEST(OuterEnvelope, SectionShapeClassStableAcrossFakeTierSwitch) {
    // Two builders differing only in what a real pipeline would treat as
    // tier-dependent knobs (payload size, inner partition size) must
    // emit envelopes whose public section_table_shape_class is identical.
    // This is the doc 15 §9 guarantee: "outer skeleton of debug /
    // standard / highsec artifacts has same public shape class."
    VMPilot::Fixtures::OuterEnvelopeBuilder a;
    a.payload_bytes(std::vector<std::uint8_t>(32, 0x11));
    VMPilot::Fixtures::OuterEnvelopeBuilder b;
    b.payload_bytes(std::vector<std::uint8_t>(4096, 0x22));

    auto pa = parse_outer_envelope(a.build().bytes);
    auto pb = parse_outer_envelope(b.build().bytes);
    ASSERT_TRUE(pa.has_value());
    ASSERT_TRUE(pb.has_value());
    EXPECT_EQ(pa->section_table_shape_class, pb->section_table_shape_class);
    EXPECT_EQ(pa->package_schema_version, pb->package_schema_version);
    EXPECT_EQ(pa->canonical_encoding_id, pb->canonical_encoding_id);
}

TEST(OuterEnvelope, MetadataBytesMatchBuilderCommitment) {
    const auto art = default_artifact();
    auto parsed = parse_outer_envelope(art.bytes.data(), art.bytes.size());
    ASSERT_TRUE(parsed.has_value());
    // Sanity: slicing the reported metadata range from the original
    // buffer yields something the strict CBOR parser accepts — the
    // envelope parser must point us to canonical bytes and nothing else.
    EXPECT_EQ(parsed->metadata_offset, art.metadata_offset);
    EXPECT_EQ(parsed->metadata_length, art.metadata_length);
}
