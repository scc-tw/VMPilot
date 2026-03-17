#include <bytecode_compiler.hpp>
#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;
using VMPilot::Common::DiagnosticCollector;

static const std::string TEST_KEY =
    "01234567890123456789012345678901";

TEST(BackendFactory, CreateSimple) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("simple", config);
    ASSERT_NE(backend, nullptr);
    EXPECT_EQ(backend->name(), "simple");
}

TEST(BackendFactory, CreateUnknown) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("unknown", config);
    EXPECT_EQ(backend, nullptr);
}

TEST(CompileConfigDefaults, DebugModeDefaultFalse) {
    CompileConfig config;
    EXPECT_FALSE(config.debug_mode);
    EXPECT_TRUE(config.opcode_key.empty());
}

TEST(DiagnosticCollectorBasic, NoopDoesNotCrash) {
    auto& noop = DiagnosticCollector::noop();
    noop.error("test", VMPilot::Common::DiagnosticCode::None, "msg");
    EXPECT_FALSE(noop.has_errors());  // noop discards everything
    EXPECT_EQ(noop.size(), 0u);
}

TEST(DiagnosticCollectorBasic, CollectsErrors) {
    DiagnosticCollector diag;
    EXPECT_FALSE(diag.has_errors());
    diag.error("test", VMPilot::Common::DiagnosticCode::InvalidInput, "bad input");
    EXPECT_TRUE(diag.has_errors());
    EXPECT_EQ(diag.error_count(), 1u);
}

TEST(DiagnosticCollectorBasic, Summary) {
    DiagnosticCollector diag;
    diag.error("compiler", VMPilot::Common::DiagnosticCode::InvalidInput,
               "empty code", "foo", 0x1000);
    diag.warn("serializer", VMPilot::Common::DiagnosticCode::OrphanSiteSkipped,
              "orphan site", "bar", 0x2000);
    auto s = diag.summary();
    EXPECT_NE(s.find("foo"), std::string::npos);
    EXPECT_NE(s.find("bar"), std::string::npos);
    EXPECT_NE(s.find("ERROR"), std::string::npos);
    EXPECT_NE(s.find("WARN"), std::string::npos);
}
