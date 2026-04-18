// Stage 8 — end-to-end: vm_stub_entry_artifact drives Stage 4-7 acceptance,
// registry lookup, and VmEngine dispatch in a single call.
//
// Builds a real VM blob (one basic block with a trivial HALT), wraps it
// as the payload of a signed artifact produced by PackageArtifactBuilder,
// and confirms the runtime executes to completion — proving the full
// signed-metadata → registry → VmEngine chain works end to end.

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

#include "vm_stub_entry.hpp"

#include "fixtures/fixture_generator.hpp"

#include "test_blob_builder.hpp"

namespace {

using VMPilot::Runtime::VmStubArtifactArgs;
using VMPilot::Runtime::VM_STUB_ARTIFACT_ABI_VERSION;
using VMPilot::Test::build_test_blob;
using VMPilot::Test::fill_seed;
using VMPilot::Test::make_test_bb;
using VMPilot::Test::TestInstruction;
using VMPilot::Common::VM::VmOpcode;

// Build a minimal blob whose only BB is a single HALT instruction. The
// default VM policy (StandardPolicy) runs this in constant-time mode.
std::vector<std::uint8_t> make_halting_blob(std::uint8_t seed_base = 1) {
    std::uint8_t seed[32];
    fill_seed(seed, seed_base);
    const TestInstruction halt{
        /* opcode */ VmOpcode::HALT,
        /* flags  */ 0,
        /* reg_a  */ 0,
        /* reg_b  */ 0,
        /* aux    */ 0,
    };
    auto bb = make_test_bb(/*bb_id*/ 1, /*epoch_base*/ 0x50, {halt});
    return build_test_blob(seed, {bb});
}

}  // namespace

TEST(VmStubEntryArtifact, HappyPathExecutesAndReturns) {
    const auto blob = make_halting_blob();

    auto art = VMPilot::Fixtures::PackageArtifactBuilder{}
        .payload_bytes(blob)
        .build();

    std::uint8_t seed[32];
    fill_seed(seed, 1);
    std::array<std::uint64_t, 16> regs{};
    regs[0] = 0;  // result register; HALT returns register 0 plaintext

    VmStubArtifactArgs args{};
    args.version          = VM_STUB_ARTIFACT_ABI_VERSION;
    args.num_regs         = static_cast<std::uint32_t>(regs.size());
    args.load_base_delta  = 0;
    args.artifact_data    = art.bytes.data();
    args.artifact_size    = art.bytes.size();
    args.unit_id          = "u-happy";
    args.stored_seed      = seed;
    args.initial_regs     = regs.data();

    const int64_t rc = vm_stub_entry_artifact(&args);
    EXPECT_EQ(rc, 0);
}

TEST(VmStubEntryArtifact, WrongAbiVersionRejected) {
    const auto blob = make_halting_blob();
    auto art = VMPilot::Fixtures::PackageArtifactBuilder{}
        .payload_bytes(blob)
        .build();

    std::uint8_t seed[32];
    fill_seed(seed, 1);
    std::array<std::uint64_t, 16> regs{};

    VmStubArtifactArgs args{};
    args.version          = 99;  // wrong
    args.num_regs         = static_cast<std::uint32_t>(regs.size());
    args.artifact_data    = art.bytes.data();
    args.artifact_size    = art.bytes.size();
    args.unit_id          = "u-happy";
    args.stored_seed      = seed;
    args.initial_regs     = regs.data();

    EXPECT_EQ(vm_stub_entry_artifact(&args), INT64_MIN);
}

TEST(VmStubEntryArtifact, UnknownUnitIdRejected) {
    const auto blob = make_halting_blob();
    auto art = VMPilot::Fixtures::PackageArtifactBuilder{}
        .payload_bytes(blob)
        .build();

    std::uint8_t seed[32];
    fill_seed(seed, 1);
    std::array<std::uint64_t, 16> regs{};

    VmStubArtifactArgs args{};
    args.version          = VM_STUB_ARTIFACT_ABI_VERSION;
    args.num_regs         = static_cast<std::uint32_t>(regs.size());
    args.artifact_data    = art.bytes.data();
    args.artifact_size    = art.bytes.size();
    args.unit_id          = "nope-not-here";   // not in UnitDescriptorTable
    args.stored_seed      = seed;
    args.initial_regs     = regs.data();

    EXPECT_EQ(vm_stub_entry_artifact(&args), INT64_MIN);
}

TEST(VmStubEntryArtifact, MissingArtifactDataRejected) {
    std::uint8_t seed[32];
    fill_seed(seed, 1);
    std::array<std::uint64_t, 16> regs{};

    VmStubArtifactArgs args{};
    args.version          = VM_STUB_ARTIFACT_ABI_VERSION;
    args.num_regs         = static_cast<std::uint32_t>(regs.size());
    args.artifact_data    = nullptr;
    args.artifact_size    = 0;
    args.unit_id          = "u-happy";
    args.stored_seed      = seed;
    args.initial_regs     = regs.data();

    EXPECT_EQ(vm_stub_entry_artifact(&args), INT64_MIN);
}

TEST(VmStubEntryArtifact, DispatchResolvesToSignedRegistryEntry) {
    // Sanity: the happy-path fixture carries a signed registry entry
    // matching the default profile (f1 / standard / rev1). Re-running
    // the full chain must hit that entry — if the registry commitment
    // were broken or the lookup tuple drifted, accept_package or lookup
    // would have fired an INT64_MIN before VmEngine ran. Reaching rc=0
    // is the end-to-end proof.
    const auto blob = make_halting_blob();
    auto art = VMPilot::Fixtures::PackageArtifactBuilder{}
        .payload_bytes(blob)
        .build();

    std::uint8_t seed[32];
    fill_seed(seed, 1);
    std::array<std::uint64_t, 16> regs{};

    VmStubArtifactArgs args{};
    args.version          = VM_STUB_ARTIFACT_ABI_VERSION;
    args.num_regs         = static_cast<std::uint32_t>(regs.size());
    args.artifact_data    = art.bytes.data();
    args.artifact_size    = art.bytes.size();
    args.unit_id          = "u-happy";
    args.stored_seed      = seed;
    args.initial_regs     = regs.data();

    EXPECT_EQ(vm_stub_entry_artifact(&args), 0);
}
