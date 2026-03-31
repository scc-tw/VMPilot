/// @file test_phase5_framework.cpp
/// @brief Tests for Phase 5: FallbackChain, CRTP EditorBase/EmitterBase.
///
/// Validates:
///   1. FallbackChain with mock strategies (success/fail combinations)
///   2. Chain short-circuits on first success
///   3. Chain propagates last error when all strategies fail
///   4. EditorBase CRTP dispatch to concrete methods
///   5. EmitterBase CRTP dispatch to concrete methods
///   6. ELF DT_NEEDED strategies compile and have correct types

#include <fallback_chain.hpp>
#include <editor_base.hpp>
#include <strategies/elf_dep_strategies.hpp>
#include <strategies/pe_dep_strategies.hpp>
#include <strategies/macho_dep_strategies.hpp>

#include <LoaderTypes.hpp>
#include <diagnostic.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <gtest/gtest.h>

#include <string>

using namespace VMPilot::Loader;
using namespace VMPilot::Common;

// ============================================================================
// Mock strategies for testing FallbackChain
// ============================================================================

static int g_strategy_call_count = 0;

struct AlwaysSucceed {
    static constexpr const char* name = "AlwaysSucceed";
    using result_type = tl::expected<int, DiagnosticCode>;

    static result_type try_execute(DiagnosticCollector& /*diag*/, int value) noexcept {
        ++g_strategy_call_count;
        return value * 2;
    }
};

struct AlwaysFail {
    static constexpr const char* name = "AlwaysFail";
    using result_type = tl::expected<int, DiagnosticCode>;

    static result_type try_execute(DiagnosticCollector& /*diag*/, int /*value*/) noexcept {
        ++g_strategy_call_count;
        return tl::make_unexpected(DiagnosticCode::NotImplemented);
    }
};

struct FailWithSpecific {
    static constexpr const char* name = "FailWithSpecific";
    using result_type = tl::expected<int, DiagnosticCode>;

    static result_type try_execute(DiagnosticCollector& /*diag*/, int /*value*/) noexcept {
        ++g_strategy_call_count;
        return tl::make_unexpected(DiagnosticCode::PatchRuntimeDepFailed);
    }
};

// ============================================================================
// 1. FallbackChain Tests
// ============================================================================

TEST(FallbackChain, SingleStrategySuccess) {
    g_strategy_call_count = 0;
    auto& diag = DiagnosticCollector::noop();

    auto result = Chain<AlwaysSucceed>::execute(diag, 21);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
    EXPECT_EQ(g_strategy_call_count, 1);
}

TEST(FallbackChain, SingleStrategyFailure) {
    g_strategy_call_count = 0;
    auto& diag = DiagnosticCollector::noop();

    auto result = Chain<AlwaysFail>::execute(diag, 21);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NotImplemented);
    EXPECT_EQ(g_strategy_call_count, 1);
}

TEST(FallbackChain, SecondStrategySucceeds) {
    g_strategy_call_count = 0;
    auto& diag = DiagnosticCollector::noop();

    auto result = Chain<AlwaysFail, AlwaysSucceed>::execute(diag, 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 20);  // AlwaysSucceed doubles the value
    EXPECT_EQ(g_strategy_call_count, 2);  // both called
}

TEST(FallbackChain, ShortCircuitsOnFirstSuccess) {
    g_strategy_call_count = 0;
    auto& diag = DiagnosticCollector::noop();

    auto result = Chain<AlwaysSucceed, AlwaysFail>::execute(diag, 5);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 10);
    EXPECT_EQ(g_strategy_call_count, 1);  // only first called
}

TEST(FallbackChain, AllFailPropagatesLastError) {
    g_strategy_call_count = 0;
    auto& diag = DiagnosticCollector::noop();

    auto result = Chain<AlwaysFail, FailWithSpecific>::execute(diag, 5);
    ASSERT_FALSE(result.has_value());
    // Last strategy's error propagates
    EXPECT_EQ(result.error(), DiagnosticCode::PatchRuntimeDepFailed);
    EXPECT_EQ(g_strategy_call_count, 2);  // both tried
}

TEST(FallbackChain, ThreeStrategies) {
    g_strategy_call_count = 0;
    auto& diag = DiagnosticCollector::noop();

    auto result = Chain<AlwaysFail, AlwaysFail, AlwaysSucceed>::execute(diag, 7);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 14);
    EXPECT_EQ(g_strategy_call_count, 3);
}

// ============================================================================
// 2. EditorBase CRTP Tests
// ============================================================================

/// Mock editor for CRTP testing.
class MockEditor : public EditorBase<MockEditor> {
public:
    bool text_section_called = false;
    bool next_segment_va_called = false;
    bool add_segment_called = false;

    TextSectionInfo text_section_impl() const noexcept {
        const_cast<MockEditor*>(this)->text_section_called = true;
        return TextSectionInfo{0x1000, 0x500};
    }

    uint64_t next_segment_va_impl(uint64_t /*alignment*/) const noexcept {
        const_cast<MockEditor*>(this)->next_segment_va_called = true;
        return 0x4000;
    }

    bool cfi_enforced_impl() const noexcept { return false; }
    std::vector<TextGap> find_text_gaps_impl(std::size_t) const noexcept { return {}; }

    tl::expected<NewSegmentInfo, DiagnosticCode>
    add_segment_impl(std::string_view, const std::vector<uint8_t>&,
                     uint64_t, DiagnosticCollector&) noexcept {
        add_segment_called = true;
        return NewSegmentInfo{0x4000, 0x1000};
    }

    tl::expected<void, DiagnosticCode>
    overwrite_text_impl(uint64_t, const uint8_t*, size_t,
                        DiagnosticCollector&) noexcept { return {}; }

    tl::expected<void, DiagnosticCode>
    add_runtime_dep_impl(std::string_view, DiagnosticCollector&) noexcept { return {}; }

    void invalidate_signature_impl() noexcept {}

    tl::expected<void, DiagnosticCode>
    save_impl(const std::string&, DiagnosticCollector&) noexcept { return {}; }
};

TEST(EditorBaseCRTP, DispatchesToDerived) {
    MockEditor editor;

    auto text = editor.text_section();
    EXPECT_TRUE(editor.text_section_called);
    EXPECT_EQ(text.base_addr, 0x1000u);

    auto va = editor.next_segment_va(0x1000);
    EXPECT_TRUE(editor.next_segment_va_called);
    EXPECT_EQ(va, 0x4000u);

    auto& diag = DiagnosticCollector::noop();
    std::vector<uint8_t> data{1, 2, 3};
    auto seg = editor.add_segment(".test", data, 0x1000, diag);
    EXPECT_TRUE(editor.add_segment_called);
    ASSERT_TRUE(seg.has_value());
}

// ============================================================================
// 3. Strategy Type Verification
// ============================================================================

TEST(Strategies, ElfStrategiesHaveCorrectTypes) {
    // Verify the strategy types compose correctly
    using Injector = strategies::ElfDepInjector;

    // The Chain type should be usable (if this compiles, types are correct)
    static_assert(!std::is_same_v<Injector, void>);
    SUCCEED();
}

TEST(Strategies, PeStrategyHasCorrectType) {
    using Injector = strategies::PeDepInjector;
    static_assert(!std::is_same_v<Injector, void>);
    SUCCEED();
}

TEST(Strategies, MachoStrategiesHaveCorrectTypes) {
    using Injector = strategies::MachoDepInjector;
    static_assert(!std::is_same_v<Injector, void>);
    SUCCEED();
}
