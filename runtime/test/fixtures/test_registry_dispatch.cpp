// Stage 6 — RuntimeSpecializationRegistry parse + lookup.
//
// After Stage 5 accept_package returns, the runtime has `runtime_
// specialization_registry_hash` — a commitment to the registry bytes
// that live inside the inner partition. These tests walk the producer
// (fixture) → consumer (registry::parse + lookup) path and confirm:
//
//   • registry parses cleanly
//   • lookup on the happy-path tuple hits the embedded default entry
//   • disabled entries surface LookupError::Disabled rather than silently
//     falling through
//   • missing, wrong family, wrong policy, wrong revision all return
//     LookupError::NotFound
//   • duplicate (spec_id, family, policy, revision) tuples fail parse
//   • inner-partition → registry round-trip is stable
//   • mutating the registry bytes inside the inner partition breaks the
//     PackageBindingRecord commitment at accept_package time

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "binding/inner_partition.hpp"
#include "binding/package.hpp"
#include "envelope/outer.hpp"
#include "registry/registry.hpp"
#include "trust_root.hpp"

#include "fixtures/fixture_generator.hpp"

namespace {

using VMPilot::Runtime::Binding::accept_package;
using VMPilot::Runtime::Binding::AcceptError;
using VMPilot::Runtime::Binding::parse_inner_partition;
using VMPilot::Runtime::Envelope::parse_outer_envelope;
using VMPilot::Runtime::Registry::lookup;
using VMPilot::Runtime::Registry::LookupError;
using VMPilot::Runtime::Registry::parse;
using VMPilot::Runtime::Registry::parse_partition;
using VMPilot::Runtime::Registry::ParseError;
using VMPilot::Runtime::Registry::Registry;

VMPilot::Runtime::Binding::AcceptConfig default_config() {
    return VMPilot::Runtime::Binding::AcceptConfig{
        {"package-schema-v1"},
        {"canonical-metadata-bytes-v1"},
        VMPilot::Runtime::Binding::RuntimeEpochState{2, 1},
    };
}

}  // namespace

// ─── Registry parse + lookup happy path ──────────────────────────────────

TEST(Registry, HappyPathBuilderParses) {
    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value()) << "err = " << static_cast<int>(reg.error());
    EXPECT_EQ(reg->registry_version, "registry-v1");
    EXPECT_EQ(reg->runtime_build_id, "vmpilot-dev-runtime-build");
    EXPECT_EQ(reg->package_schema_version, "package-schema-v1");
    EXPECT_EQ(reg->registry_epoch, 1u);
    ASSERT_EQ(reg->entries.size(), 1u);
    EXPECT_EQ(reg->entries[0].runtime_specialization_id, "f1-standard-v1");
    EXPECT_EQ(reg->entries[0].family_id, VMPilot::DomainLabels::FamilyId::F1);
    EXPECT_EQ(reg->entries[0].requested_policy_id,
              VMPilot::DomainLabels::PolicyId::Standard);
    EXPECT_TRUE(reg->entries[0].enabled_in_this_runtime);
}

TEST(Registry, LookupFindsEnabledEntry) {
    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());
    auto hit =
        lookup(*reg, "f1-standard-v1", VMPilot::DomainLabels::FamilyId::F1,
               VMPilot::DomainLabels::PolicyId::Standard, "rev1");
    ASSERT_TRUE(hit.has_value());
    EXPECT_EQ((*hit)->runtime_specialization_id, "f1-standard-v1");
}

TEST(Registry, LookupWrongFamilyReturnsNotFound) {
    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());
    auto hit =
        lookup(*reg, "f1-standard-v1", VMPilot::DomainLabels::FamilyId::F2,
               VMPilot::DomainLabels::PolicyId::Standard, "rev1");
    ASSERT_FALSE(hit.has_value());
    EXPECT_EQ(hit.error(), LookupError::NotFound);
}

TEST(Registry, LookupWrongPolicyReturnsNotFound) {
    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());
    auto hit =
        lookup(*reg, "f1-standard-v1", VMPilot::DomainLabels::FamilyId::F1,
               VMPilot::DomainLabels::PolicyId::HighSec, "rev1");
    ASSERT_FALSE(hit.has_value());
    EXPECT_EQ(hit.error(), LookupError::NotFound);
}

TEST(Registry, LookupWrongRevisionReturnsNotFound) {
    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());
    auto hit =
        lookup(*reg, "f1-standard-v1", VMPilot::DomainLabels::FamilyId::F1,
               VMPilot::DomainLabels::PolicyId::Standard, "rev99");
    ASSERT_FALSE(hit.has_value());
    EXPECT_EQ(hit.error(), LookupError::NotFound);
}

TEST(Registry, LookupWrongSpecIdReturnsNotFound) {
    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());
    auto hit =
        lookup(*reg, "does-not-exist", VMPilot::DomainLabels::FamilyId::F1,
               VMPilot::DomainLabels::PolicyId::Standard, "rev1");
    ASSERT_FALSE(hit.has_value());
    EXPECT_EQ(hit.error(), LookupError::NotFound);
}

// ─── Disabled entry ─────────────────────────────────────────────────────

TEST(Registry, DisabledEntrySurfacesDistinctError) {
    VMPilot::Fixtures::RegistryEntrySpec e;
    e.runtime_specialization_id = "f3-experimental";
    e.family_id = "f3";
    e.requested_policy_id = "debug";
    e.profile_revision = "rev1";
    e.enabled_in_this_runtime = false;

    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .clear_entries()
                           .add_entry(std::move(e))
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());

    auto hit =
        lookup(*reg, "f3-experimental", VMPilot::DomainLabels::FamilyId::F3,
               VMPilot::DomainLabels::PolicyId::Debug, "rev1");
    ASSERT_FALSE(hit.has_value());
    EXPECT_EQ(hit.error(), LookupError::Disabled);
}

TEST(Registry, EnabledOverridesDisabledWhenBothMatch) {
    // Defensive: if the registry ever contains both an enabled and a
    // disabled entry with the same tuple, lookup should prefer the
    // enabled one rather than falling through to Disabled. The
    // duplicate-tuple parse reject blocks this today, but the lookup
    // implementation is still written defensively. (We construct via
    // different profile revisions so the parse check does not fire.)
    VMPilot::Fixtures::RegistryEntrySpec enabled;
    enabled.runtime_specialization_id = "shared";
    enabled.family_id = "f1";
    enabled.requested_policy_id = "standard";
    enabled.profile_revision = "rev1";
    enabled.enabled_in_this_runtime = true;

    VMPilot::Fixtures::RegistryEntrySpec disabled = enabled;
    disabled.profile_revision = "rev2";
    disabled.enabled_in_this_runtime = false;

    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .clear_entries()
                           .add_entry(enabled)
                           .add_entry(disabled)
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_TRUE(reg.has_value());

    auto hit = lookup(*reg, "shared", VMPilot::DomainLabels::FamilyId::F1,
                      VMPilot::DomainLabels::PolicyId::Standard, "rev1");
    ASSERT_TRUE(hit.has_value());
    EXPECT_TRUE((*hit)->enabled_in_this_runtime);
}

// ─── Duplicate tuple rejected at parse ─────────────────────────────────

TEST(Registry, DuplicateLookupTupleRejected) {
    VMPilot::Fixtures::RegistryEntrySpec a;
    a.runtime_specialization_id = "dup";
    a.family_id = "f1";
    a.requested_policy_id = "standard";
    a.profile_revision = "rev1";

    VMPilot::Fixtures::RegistryEntrySpec b = a;
    // Same lookup tuple — only non-lookup fields differ — must reject.
    b.semantic_contract_version = "different-contract";

    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .clear_entries()
                           .add_entry(a)
                           .add_entry(b)
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_FALSE(reg.has_value());
    EXPECT_EQ(reg.error(), ParseError::DuplicateEntry);
}

// ─── Malformed registry bytes rejected ──────────────────────────────────

TEST(Registry, EmptyBytesRejected) {
    auto reg = parse(nullptr, 0);
    ASSERT_FALSE(reg.has_value());
}

TEST(Registry, RandomNonCborBytesRejected) {
    const std::vector<std::uint8_t> garbage(32, 0xff);
    auto reg = parse(garbage);
    ASSERT_FALSE(reg.has_value());
    EXPECT_EQ(reg.error(), ParseError::BadCbor);
}

// ─── End-to-end: inner partition carries the registry ──────────────────

TEST(Registry, FullArtifactRegistryIsReachable) {
    const auto art = VMPilot::Fixtures::PackageArtifactBuilder{}.build();
    auto env = parse_outer_envelope(art.bytes);
    ASSERT_TRUE(env.has_value());
    auto accepted =
        accept_package(art.bytes.data(), art.bytes.size(),
                       VMPilot::Runtime::trust_root(), *env, default_config());
    ASSERT_TRUE(accepted.has_value());

    // Walk through the same inner-partition path a real runtime would:
    // envelope → inner partition bytes → parse_inner_partition →
    // registry bytes → Registry::parse → lookup.
    auto inner = parse_inner_partition(
        art.bytes.data() + env->inner_metadata_partition.offset,
        env->inner_metadata_partition.length);
    ASSERT_TRUE(inner.has_value());

    auto reg = parse_partition(inner->runtime_specialization_registry,
                               VMPilot::Runtime::trust_root());
    ASSERT_TRUE(reg.has_value());
    auto hit =
        lookup(*reg, "f1-standard-v1", VMPilot::DomainLabels::FamilyId::F1,
               VMPilot::DomainLabels::PolicyId::Standard, "rev1");
    ASSERT_TRUE(hit.has_value());
}

TEST(Registry, MutatingRegistryBytesBreaksPbrCommitment) {
    // Locate the registry sub-range inside the inner partition and flip a
    // byte inside its payload. PBR commits to the registry bytes under a
    // domain label; any content mutation must surface as a
    // RuntimeSpecializationRegistryHashMismatch during accept_package.
    auto art = VMPilot::Fixtures::PackageArtifactBuilder{}.build();
    auto env = parse_outer_envelope(art.bytes);
    ASSERT_TRUE(env.has_value());

    // The inner partition is a strict-CBOR map; parse it and re-scan the
    // buffer to find where sub-table 3 (registry) begins.
    auto inner = VMPilot::Runtime::Binding::parse_inner_partition(
        art.bytes.data() + env->inner_metadata_partition.offset,
        env->inner_metadata_partition.length);
    ASSERT_TRUE(inner.has_value());
    const auto& reg_bytes = inner->runtime_specialization_registry;
    ASSERT_FALSE(reg_bytes.empty());

    // Find the registry bytes within the artifact — search for the first
    // occurrence of the full registry payload inside the inner partition
    // range. The test encoder emits the registry as a single contiguous
    // byte string, so this is a reliable fingerprint.
    const std::size_t inner_start = env->inner_metadata_partition.offset;
    const std::size_t inner_end =
        inner_start + env->inner_metadata_partition.length;
    std::size_t reg_offset = 0;
    bool found = false;
    for (std::size_t i = inner_start; i + reg_bytes.size() <= inner_end; ++i) {
        if (std::memcmp(art.bytes.data() + i, reg_bytes.data(),
                        reg_bytes.size()) == 0) {
            reg_offset = i;
            found = true;
            break;
        }
    }
    ASSERT_TRUE(found);

    // Flip a byte well inside the registry payload so we don't hit a
    // CBOR structural byte of either the registry envelope or the
    // enclosing inner partition map.
    art.bytes[reg_offset + reg_bytes.size() / 2] ^= 0x01;

    auto accepted =
        accept_package(art.bytes.data(), art.bytes.size(),
                       VMPilot::Runtime::trust_root(), *env, default_config());
    ASSERT_FALSE(accepted.has_value());
    EXPECT_EQ(accepted.error(),
              AcceptError::RuntimeSpecializationRegistryHashMismatch);
}

TEST(Registry, InnerPartitionUnitTableAndRegistryDistinctHashes) {
    // Defense in depth: if unit_binding_table and registry bytes happen
    // to hash to the same digest under different domains, a swap
    // between them would go undetected. Confirm they differ today.
    const auto art = VMPilot::Fixtures::PackageArtifactBuilder{}.build();
    EXPECT_NE(art.unit_binding_table_hash,
              art.runtime_specialization_registry_hash);
    EXPECT_NE(art.unit_binding_table_hash, art.resolved_profile_table_hash);
    EXPECT_NE(art.resolved_profile_table_hash,
              art.runtime_specialization_registry_hash);
}

// ─── Option 3 field-13 / field-9 consistency ─────────────────────────────

namespace {

// An entry that carries a concrete PolicyRequirement (bytes + matching
// hash) so consistency-mismatch tests can perturb one side only.
VMPilot::Fixtures::RegistryEntrySpec make_entry_with_requirement() {
    VMPilot::Fixtures::RegistryEntrySpec e;
    e.runtime_specialization_id = "f1-standard-v1";
    e.family_id = "f1";
    e.requested_policy_id = "standard";
    e.profile_revision = "rev1";
    e.policy_requirement = VMPilot::Fixtures::PolicyRequirementSpec{};
    return e;
}

}  // namespace

TEST(Registry, NonEmptyBytesWithZeroHashRejected) {
    // Producer bug: PolicyRequirement bytes present, committed hash
    // is all-zero. parser must reject before lookup can run.
    auto entry = make_entry_with_requirement();
    entry.provider_requirement_hash.fill(0);  // force zero while keeping bytes
    // Prevent add_entry() from recomputing — we want the mismatch on disk.
    entry.provider_requirement_canonical_bytes =
        VMPilot::Fixtures::encode_policy_requirement(*entry.policy_requirement);
    entry.policy_requirement.reset();  // add_entry() now preserves our raw fields

    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .clear_entries()
                           .add_entry(entry)
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_FALSE(reg.has_value());
    EXPECT_EQ(reg.error(),
              ParseError::InconsistentRequirementCommitment);
}

TEST(Registry, EmptyBytesWithNonZeroHashRejected) {
    // Mirror: no canonical bytes supplied but a non-zero hash claims
    // to commit to something. parser must reject.
    VMPilot::Fixtures::RegistryEntrySpec entry;
    entry.runtime_specialization_id = "f1-standard-v1";
    entry.family_id = "f1";
    entry.requested_policy_id = "standard";
    entry.profile_revision = "rev1";
    entry.provider_requirement_canonical_bytes.clear();
    entry.provider_requirement_hash.fill(0xAB);

    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .clear_entries()
                           .add_entry(entry)
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_FALSE(reg.has_value());
    EXPECT_EQ(reg.error(),
              ParseError::InconsistentRequirementCommitment);
}

TEST(Registry, BytesAndHashBothPresentButHashStaleRejected) {
    // Producer flipped one byte of either the canonical form or the
    // hash after signing. Both fields are well-formed shapes but the
    // recomputed hash doesn't match — parser must fail fail-closed.
    auto entry = make_entry_with_requirement();
    // Let add_entry auto-fill the matching pair, then corrupt the hash.
    auto reg_builder = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                          .clear_entries()
                          .add_entry(entry);
    // Rebuild with a deliberate mismatch: canonical bytes intact, hash
    // flipped. Easiest path: construct the mismatch directly.
    VMPilot::Fixtures::RegistryEntrySpec bad_entry;
    bad_entry.runtime_specialization_id = "f1-standard-v1";
    bad_entry.family_id = "f1";
    bad_entry.requested_policy_id = "standard";
    bad_entry.profile_revision = "rev1";
    bad_entry.provider_requirement_canonical_bytes =
        VMPilot::Fixtures::encode_policy_requirement(
            VMPilot::Fixtures::PolicyRequirementSpec{});
    bad_entry.provider_requirement_hash.fill(0xCC);  // not the real hash

    const auto bytes = VMPilot::Fixtures::RuntimeSpecializationRegistryBuilder{}
                           .clear_entries()
                           .add_entry(bad_entry)
                           .build_canonical_bytes();
    auto reg = parse(bytes);
    ASSERT_FALSE(reg.has_value());
    EXPECT_EQ(reg.error(),
              ParseError::InconsistentRequirementCommitment);
}
