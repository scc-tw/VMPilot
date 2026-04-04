#include <CompilationOrchestrator.hpp>
#include <SimpleBackend.hpp>
#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;
using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::Segmentator;
using VMPilot::Common::DiagnosticCollector;

class OrchestratorTest : public ::testing::Test {
protected:
    CompileConfig config{false};

    std::vector<CompilationUnit> make_units(int count) {
        CompilationContext ctx_val;
        ctx_val.arch = Arch::X86;
        ctx_val.mode = Mode::MODE_32;
        auto ctx = std::make_shared<const CompilationContext>(
            std::move(ctx_val));

        std::vector<CompilationUnit> units;
        for (int i = 0; i < count; ++i) {
            CompilationUnit unit;
            unit.name = "func_" + std::to_string(i);
            unit.addr = 0x1000 + static_cast<uint64_t>(i) * 0x100;
            unit.code = {0x55, 0x89, 0xe5,
                         static_cast<uint8_t>(i), 0xc3};
            unit.size = unit.code.size();
            unit.is_canonical = true;
            unit.context = ctx;
            units.push_back(std::move(unit));
        }
        return units;
    }
};

TEST_F(OrchestratorTest, EmptyUnits) {
    DiagnosticCollector diag;
    auto backend = std::make_unique<SimpleBackend>();
    CompilationOrchestrator orch(std::move(backend), config, 2);

    auto units = make_units(0);
    auto comp = orch.compile(units, diag);
    ASSERT_TRUE(comp.has_value());
    EXPECT_EQ(comp->total_units, 0u);
    EXPECT_TRUE(comp->outputs.empty());
    EXPECT_EQ(comp->failed_units, 0u);
}

TEST_F(OrchestratorTest, SingleUnit) {
    DiagnosticCollector diag;
    auto backend = std::make_unique<SimpleBackend>();
    CompilationOrchestrator orch(std::move(backend), config, 2);

    auto units = make_units(1);
    auto comp = orch.compile(units, diag);
    ASSERT_TRUE(comp.has_value());
    EXPECT_EQ(comp->total_units, 1u);
    EXPECT_EQ(comp->outputs.size(), 1u);
    EXPECT_EQ(comp->failed_units, 0u);
    EXPECT_EQ(comp->outputs[0].name, "func_0");
}

TEST_F(OrchestratorTest, MultipleUnitsParallel) {
    DiagnosticCollector diag;
    auto backend = std::make_unique<SimpleBackend>();
    CompilationOrchestrator orch(std::move(backend), config, 4);

    auto units = make_units(20);
    auto comp = orch.compile(units, diag);
    ASSERT_TRUE(comp.has_value());
    EXPECT_EQ(comp->total_units, 20u);
    EXPECT_EQ(comp->outputs.size(), 20u);
    EXPECT_EQ(comp->failed_units, 0u);
}

TEST_F(OrchestratorTest, NullBackendReturnsError) {
    DiagnosticCollector diag;
    CompilationOrchestrator orch(nullptr, config, 2);
    auto units = make_units(1);
    auto comp = orch.compile(units, diag);
    ASSERT_FALSE(comp.has_value());
    EXPECT_TRUE(diag.has_errors());
}

TEST_F(OrchestratorTest, OutputBytecodesAreValid) {
    DiagnosticCollector diag;
    auto backend = std::make_unique<SimpleBackend>();
    CompilationOrchestrator orch(std::move(backend), config, 2);

    auto units = make_units(3);
    auto comp = orch.compile(units, diag);
    ASSERT_TRUE(comp.has_value());

    for (size_t i = 0; i < comp->outputs.size(); ++i) {
        EXPECT_FALSE(comp->outputs[i].bytecodes.empty())
            << "output " << i << " should have non-empty bytecodes";
        // Stub backend copies code bytes verbatim
        EXPECT_EQ(comp->outputs[i].bytecodes.size(), 5u);
    }
}
