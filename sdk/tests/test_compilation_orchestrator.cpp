#include <CompilationOrchestrator.hpp>
#include <SimpleBackend.hpp>
#include <segmentator.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;
using namespace VMPilot::SDK::Segmentator;
using namespace VMPilot::SDK::RegionRefiner;

static const std::string TEST_KEY =
    "01234567890123456789012345678901";

class OrchestratorTest : public ::testing::Test {
protected:
    CompileConfig config{TEST_KEY, false};

    SegmentationResult make_result(int num_regions) {
        SegmentationResult result;
        result.context.arch = Arch::X86;
        result.context.mode = Mode::MODE_32;

        for (int i = 0; i < num_regions; ++i) {
            std::string name = "func_" + std::to_string(i);
            uint64_t addr = 0x1000 + i * 0x100;
            std::vector<uint8_t> code = {0x55, 0x89, 0xe5,
                                         static_cast<uint8_t>(i), 0xc3};

            // Add to refined_regions
            result.refined_regions.emplace_back(addr, code.size(), name, code);

            // Add to groups
            ProtectedRegion group;
            group.source_name = name;
            RegionSite site;
            site.source_name = name;
            site.addr = addr;
            site.size = code.size();
            site.is_canonical = true;
            group.sites.push_back(std::move(site));
            group.canonical_index = 0;
            result.groups.push_back(std::move(group));
        }

        return result;
    }
};

TEST_F(OrchestratorTest, EmptyResult) {
    auto backend = std::make_unique<SimpleBackend>(TEST_KEY);
    CompilationOrchestrator orch(std::move(backend), config, 2);

    auto result = make_result(0);
    auto comp = orch.compile(result);
    ASSERT_TRUE(comp.has_value());
    EXPECT_EQ(comp->total_units, 0u);
    EXPECT_TRUE(comp->outputs.empty());
    EXPECT_TRUE(comp->errors.empty());
}

TEST_F(OrchestratorTest, SingleUnit) {
    auto backend = std::make_unique<SimpleBackend>(TEST_KEY);
    CompilationOrchestrator orch(std::move(backend), config, 2);

    auto result = make_result(1);
    auto comp = orch.compile(result);
    ASSERT_TRUE(comp.has_value());
    EXPECT_EQ(comp->total_units, 1u);
    EXPECT_EQ(comp->outputs.size(), 1u);
    EXPECT_TRUE(comp->errors.empty());
    EXPECT_EQ(comp->outputs[0].name, "func_0");
}

TEST_F(OrchestratorTest, MultipleUnitsParallel) {
    auto backend = std::make_unique<SimpleBackend>(TEST_KEY);
    CompilationOrchestrator orch(std::move(backend), config, 4);

    auto result = make_result(20);
    auto comp = orch.compile(result);
    ASSERT_TRUE(comp.has_value());
    EXPECT_EQ(comp->total_units, 20u);
    EXPECT_EQ(comp->outputs.size(), 20u);
    EXPECT_TRUE(comp->errors.empty());
}

TEST_F(OrchestratorTest, NullBackendReturnsError) {
    CompilationOrchestrator orch(nullptr, config, 2);
    auto result = make_result(1);
    auto comp = orch.compile(result);
    ASSERT_FALSE(comp.has_value());
}

TEST_F(OrchestratorTest, OutputBytecodesAreValid) {
    auto backend = std::make_unique<SimpleBackend>(TEST_KEY);
    CompilationOrchestrator orch(std::move(backend), config, 2);

    auto result = make_result(3);
    auto comp = orch.compile(result);
    ASSERT_TRUE(comp.has_value());

    VMPilot::Common::Instruction instr_helper;
    for (const auto& output : comp->outputs) {
        for (const auto& inst : output.bytecodes) {
            EXPECT_TRUE(instr_helper.check(inst));
        }
    }
}
