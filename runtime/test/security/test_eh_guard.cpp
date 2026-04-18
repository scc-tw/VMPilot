// Stage 9 — Exception / unwind reserved-surface enforcement.
//
// Confirms:
//   • default ResolvedFamilyProfile fixtures carry a valid 1.0 reserved
//     ExceptionUnwindContract
//   • accept_unit_entry rejects any profile that upgrades executable EH,
//     enables cross-frame unwind, exposes non-empty handler/cleanup
//     tables, or flips native-boundary behavior away from fail-closed
//   • native boundary unwind events (C++ throw / SEH / signal) are
//     translated into a fail-closed runtime error instead of crossing a
//     protected VM frame

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "binding/package.hpp"
#include "binding/unit.hpp"
#include "cbor/strict.hpp"
#include "eh_guard.hpp"
#include "envelope/outer.hpp"
#include "trust_root.hpp"
#include "vm/domain_labels.hpp"
#include "vm_engine.hpp"
#include "vm_stub_entry.hpp"

#include "fixtures/fixture_generator.hpp"

#include "test_blob_builder.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <csignal>
#endif

namespace {

using VMPilot::Runtime::Binding::AcceptConfig;
using VMPilot::Runtime::Binding::AcceptedPackage;
using VMPilot::Runtime::Binding::RuntimeEpochState;
using VMPilot::Runtime::Binding::UnitAcceptError;
using VMPilot::Runtime::Binding::accept_package;
using VMPilot::Runtime::Binding::accept_unit_entry;
using VMPilot::Runtime::EH::ContractVerifyError;
using VMPilot::Runtime::EH::verify_reserved_exception_unwind_contract;
using VMPilot::Runtime::Envelope::parse_outer_envelope;
using VMPilot::Runtime::VmStubArtifactArgs;
using VMPilot::Runtime::VM_STUB_ARTIFACT_ABI_VERSION;
using VMPilot::Common::DiagnosticCode;
using VMPilot::Common::VM::VmOpcode;
using VMPilot::Test::build_test_blob;
using VMPilot::Test::fill_seed;
using VMPilot::Test::make_test_bb;
using VMPilot::Test::TestInstruction;
using VMPilot::Test::TestNativeCall;

AcceptConfig default_config() {
    return AcceptConfig{
        {"package-schema-v1"},
        {"canonical-metadata-bytes-v1"},
        RuntimeEpochState{2, 1},
    };
}

struct AcceptedBundle {
    VMPilot::Fixtures::PackageArtifactAssembly art;
    AcceptedPackage pkg;
    VMPilot::Runtime::Envelope::OuterEnvelope env;
};

std::array<std::uint8_t, 32> default_unit_identity() {
    return {
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    };
}

AcceptedBundle accept_bundle(VMPilot::Fixtures::PackageArtifactAssembly art,
                             const AcceptConfig& cfg = default_config()) {
    auto env = parse_outer_envelope(art.bytes);
    EXPECT_TRUE(env.has_value());
    auto pkg = accept_package(art.bytes.data(), art.bytes.size(),
                              VMPilot::Runtime::trust_root(), *env, cfg);
    EXPECT_TRUE(pkg.has_value());
    return {std::move(art), std::move(*pkg), std::move(*env)};
}

VMPilot::Fixtures::PackageArtifactAssembly build_artifact_with_contract(
    const VMPilot::Fixtures::ExceptionUnwindContractSpec& contract,
    std::string_view family_id = "f1",
    std::string_view policy_id = "standard") {
    using namespace VMPilot::Fixtures;

    const auto unit_identity = default_unit_identity();
    const std::vector<std::uint8_t> payload(96, 0xCD);
    const auto payload_hash = sha256_of(payload);
    const std::string family_id_s(family_id);
    const std::string policy_id_s(policy_id);
    const std::string profile_id = "rfp-happy";
    const std::string runtime_specialization_id =
        family_id_s + "-" + policy_id_s + "-v1";

    const auto desc = UnitDescriptorBuilder{}
        .unit_id("u-happy")
        .unit_identity_hash(unit_identity)
        .family_id(family_id_s)
        .requested_policy_id(policy_id_s)
        .resolved_family_profile_id(profile_id)
        .unit_binding_record_id("ubr-happy")
        .payload_sha256(payload_hash)
        .payload_size(payload.size())
        .build();
    const auto profile = ResolvedFamilyProfileBuilder{}
        .profile_id(profile_id)
        .family_id(family_id_s)
        .requested_policy_id(policy_id_s)
        .runtime_specialization_id(runtime_specialization_id)
        .exception_unwind_contract(contract)
        .build();

    const auto desc_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitDescriptor, desc);
    const auto profile_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedFamilyProfile, profile);

    UnitBindingAuthSpec auth;
    const auto ubr = UnitBindingRecordBuilder{}
        .unit_binding_record_id("ubr-happy")
        .unit_identity_hash(unit_identity)
        .unit_descriptor_hash(desc_hash)
        .family_id(family_id_s)
        .requested_policy_id(policy_id_s)
        .resolved_family_profile_id(profile_id)
        .resolved_family_profile_content_hash(profile_hash)
        .payload_sha256(payload_hash)
        .payload_size(payload.size())
        .anti_downgrade_epoch(1)
        .binding_auth(auth)
        .build();

    RegistryEntrySpec entry;
    entry.runtime_specialization_id = runtime_specialization_id;
    entry.family_id = family_id_s;
    entry.requested_policy_id = policy_id_s;
    entry.profile_revision = "rev1";
    entry.accepted_profile_content_hash = profile_hash;
    const auto registry = RuntimeSpecializationRegistryBuilder{}
        .clear_entries()
        .add_entry(entry)
        .build();

    return PackageArtifactBuilder{}
        .payload_bytes(payload)
        .unit_binding_table_bytes(build_unit_binding_table_bytes({ubr}))
        .unit_descriptor_table_bytes(build_unit_descriptor_table_bytes(
            {{"u-happy", desc}}))
        .resolved_profile_table_bytes(build_resolved_profile_table_bytes(
            {{profile_id, profile}}))
        .registry_bytes(registry)
        .build();
}

std::vector<std::uint8_t> build_native_fault_blob(std::uint64_t target_addr) {
    std::uint8_t seed[32];
    fill_seed(seed, 1);
    const TestInstruction call_native{VmOpcode::NATIVE_CALL, 0, 0, 0, 0};
    const TestInstruction halt{VmOpcode::HALT, 0, 0, 0, 0};
    auto bb = make_test_bb(1, 0x70, {call_native, halt});

    TestNativeCall call{};
    call.call_site_ip = 0;
    call.arg_count = 0;
    call.target_addr = target_addr;

    return build_test_blob(seed, {bb}, {}, false, {call});
}

tl::expected<VMPilot::Runtime::VmExecResult, DiagnosticCode>
run_native_fault_blob(const std::vector<std::uint8_t>& blob) {
    std::uint8_t seed[32];
    fill_seed(seed, 1);
    auto engine = VMPilot::Runtime::VmEngine<
        VMPilot::Runtime::DebugPolicy,
        VMPilot::Runtime::DirectOram>::create(blob.data(), blob.size(), seed);
    EXPECT_TRUE(engine.has_value());
    if (!engine.has_value()) {
        return tl::make_unexpected(engine.error());
    }
    return engine->execute();
}

int64_t run_artifact_blob(const std::vector<std::uint8_t>& blob) {
    auto art = VMPilot::Fixtures::PackageArtifactBuilder{}
        .payload_bytes(blob)
        .build();

    std::uint8_t stored_seed[32];
    fill_seed(stored_seed, 1);
    std::array<std::uint64_t, 16> regs{};

    VmStubArtifactArgs args{};
    args.version = VM_STUB_ARTIFACT_ABI_VERSION;
    args.num_regs = static_cast<std::uint32_t>(regs.size());
    args.artifact_data = art.bytes.data();
    args.artifact_size = art.bytes.size();
    args.unit_id = "u-happy";
    args.stored_seed = stored_seed;
    args.initial_regs = regs.data();
    return vm_stub_entry_artifact(&args);
}

#if defined(_WIN32)
std::uint64_t native_cpp_throw(std::uint64_t, std::uint64_t, std::uint64_t,
                               std::uint64_t, std::uint64_t, std::uint64_t,
                               std::uint64_t, std::uint64_t) {
    throw std::runtime_error("native unwind escaped protected frame");
}

std::uint64_t native_seh_raise(std::uint64_t, std::uint64_t, std::uint64_t,
                               std::uint64_t, std::uint64_t, std::uint64_t,
                               std::uint64_t, std::uint64_t) {
    RaiseException(EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, 0, nullptr);
    return 0;
}
#elif defined(__unix__) || defined(__APPLE__)
std::uint64_t native_signal_raise(std::uint64_t, std::uint64_t, std::uint64_t,
                                  std::uint64_t, std::uint64_t, std::uint64_t,
                                  std::uint64_t, std::uint64_t) {
    std::raise(SIGSEGV);
    return 0;
}
#endif

}  // namespace

TEST(EhGuardContract, ReservedDefaultContractAccepted) {
    const auto profile = VMPilot::Fixtures::ResolvedFamilyProfileBuilder{}.build();
    auto verified = verify_reserved_exception_unwind_contract(
        profile, VMPilot::DomainLabels::FamilyId::F1);
    ASSERT_TRUE(verified.has_value()) << static_cast<int>(verified.error());
    EXPECT_EQ(verified->family_specific_unwind_surface_ref,
              "f1-unwind-surface-v1");
}

TEST(EhGuardContract, ExecutableEhUpgradeRejected) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.executable_eh_status = "executable_v1_1";
    auto b = accept_bundle(build_artifact_with_contract(contract));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(),
              UnitAcceptError::ExecutableEhStatusNotReservedDisabled);
}

TEST(EhGuardContract, HandlerTableMustRemainReservedEmpty) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.handler_table_status = "profile-specific";
    auto b = accept_bundle(build_artifact_with_contract(contract));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), UnitAcceptError::HandlerTableNotReservedEmpty);
}

TEST(EhGuardContract, CleanupTableMustRemainReservedEmpty) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.cleanup_table_status = "profile-specific";
    auto b = accept_bundle(build_artifact_with_contract(contract));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), UnitAcceptError::CleanupTableNotReservedEmpty);
}

TEST(EhGuardContract, CrossProtectedFrameUnwindRejected) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.cross_protected_frame_unwind = "permitted_by_profile";
    auto b = accept_bundle(build_artifact_with_contract(contract));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), UnitAcceptError::CrossProtectedFrameUnwindPermitted);
}

TEST(EhGuardContract, NativeBoundaryBehaviorMustStayFailClosed) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.native_boundary_unwind_behavior = "profile-upgraded";
    auto b = accept_bundle(build_artifact_with_contract(contract));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(),
              UnitAcceptError::NativeBoundaryUnwindBehaviorNotFailClosed);
}

TEST(EhGuardContract, UnknownCriticalExtensionRejected) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.critical_extensions.push_back("resume-cookie-v1");
    auto b = accept_bundle(build_artifact_with_contract(contract));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), UnitAcceptError::UnknownEhCriticalExtension);
}

TEST(EhGuardContract, FamilySpecificSurfaceRefMustMatchFamily) {
    VMPilot::Fixtures::ExceptionUnwindContractSpec contract;
    contract.family_specific_unwind_surface_ref = "f2-unwind-surface-v1";
    auto b = accept_bundle(build_artifact_with_contract(contract, "f1"));
    auto result = accept_unit_entry(
        b.art.bytes.data(), b.art.bytes.size(), b.env, b.pkg,
        "u-happy", default_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), UnitAcceptError::FamilySpecificUnwindSurfaceMismatch);
}

// Note on the MSVC ASAN interaction:
//
//   /fsanitize=address hooks the Windows SEH unwind path. When an
//   exception (C++ throw or RaiseException) crosses the ASM
//   platform_call trampoline while ASAN is loaded, ASAN's shadow-memory
//   bookkeeping corrupts the unwind context before catch(...) /
//   __except can run. The runtime contract we are asserting is correct
//   — exercised at the unit level by the EhGuardContract.* suite — so
//   the Windows runtime-level tests skip under ASAN and remain active
//   on non-ASAN builds and on POSIX.

#if defined(_WIN32)
#if !defined(__SANITIZE_ADDRESS__)
TEST(EhGuardRuntime, CppThrowFailsClosedAtNativeBoundary) {
    const auto blob = build_native_fault_blob(
        reinterpret_cast<std::uint64_t>(&native_cpp_throw));
    auto result = run_native_fault_blob(blob);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeBoundaryUnwindTrapped);
}

TEST(EhGuardRuntime, StructuredExceptionFailsClosedAtNativeBoundary) {
    const auto blob = build_native_fault_blob(
        reinterpret_cast<std::uint64_t>(&native_seh_raise));
    auto result = run_native_fault_blob(blob);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeBoundaryUnwindTrapped);
}

TEST(EhGuardRuntime, ArtifactEntryCollapsesNativeBoundaryFaultToGenericFailure) {
    const auto blob = build_native_fault_blob(
        reinterpret_cast<std::uint64_t>(&native_seh_raise));
    EXPECT_EQ(run_artifact_blob(blob), INT64_MIN);
}
#else
TEST(EhGuardRuntime, DISABLED_NativeBoundaryUnwindTestsDisabledUnderAsan) {
    GTEST_SKIP() << "MSVC /fsanitize=address corrupts SEH unwind through "
                    "the ASM platform_call trampoline; see contract-level "
                    "EhGuardContract suite for rule coverage.";
}
#endif  // !__SANITIZE_ADDRESS__
#elif defined(__unix__) || defined(__APPLE__)
TEST(EhGuardRuntime, SignalFailsClosedAtNativeBoundary) {
    const auto blob = build_native_fault_blob(
        reinterpret_cast<std::uint64_t>(&native_signal_raise));
    auto result = run_native_fault_blob(blob);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeBoundaryUnwindTrapped);
}

TEST(EhGuardRuntime, ArtifactEntryCollapsesSignalToGenericFailure) {
    const auto blob = build_native_fault_blob(
        reinterpret_cast<std::uint64_t>(&native_signal_raise));
    EXPECT_EQ(run_artifact_blob(blob), INT64_MIN);
}
#endif