#include <gtest/gtest.h>

#include "RelocationTraits.hpp"
#include "RelocationLayer.hpp"

using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::ReferenceAnalyzer;

// ---- X86RelocTraits::classify() unit tests ----

TEST(X86RelocTraits, DirectRefPC32) {
    auto cls = X86RelocTraits::classify(2);  // R_X86_64_PC32
    EXPECT_EQ(cls.kind, DataRefKind::Unknown);
    EXPECT_EQ(cls.tls_model, TlsModel::None);
    EXPECT_TRUE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, DirectRef64) {
    auto cls = X86RelocTraits::classify(1);  // R_X86_64_64
    EXPECT_EQ(cls.kind, DataRefKind::Unknown);
    EXPECT_EQ(cls.tls_model, TlsModel::None);
    EXPECT_FALSE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, GotRelocation) {
    auto cls = X86RelocTraits::classify(9);  // R_X86_64_GOTPCREL
    EXPECT_EQ(cls.kind, DataRefKind::GotLoad);
    EXPECT_EQ(cls.tls_model, TlsModel::None);
    EXPECT_TRUE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, GotpcrelxVariants) {
    auto cls41 = X86RelocTraits::classify(41);  // GOTPCRELX
    EXPECT_EQ(cls41.kind, DataRefKind::GotLoad);
    EXPECT_TRUE(cls41.is_pc_relative);
    EXPECT_FALSE(cls41.skip);

    auto cls42 = X86RelocTraits::classify(42);  // REX_GOTPCRELX
    EXPECT_EQ(cls42.kind, DataRefKind::GotLoad);
    EXPECT_TRUE(cls42.is_pc_relative);
    EXPECT_FALSE(cls42.skip);
}

TEST(X86RelocTraits, TlsLocalExec) {
    auto cls = X86RelocTraits::classify(23);  // R_X86_64_TPOFF32
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::LocalExec);
    EXPECT_FALSE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, Tpoff64) {
    auto cls = X86RelocTraits::classify(18);  // R_X86_64_TPOFF64
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::LocalExec);
    EXPECT_FALSE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, TlsInitialExec) {
    auto cls = X86RelocTraits::classify(22);  // R_X86_64_GOTTPOFF
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::InitialExec);
    EXPECT_TRUE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, TlsGeneralDynamic) {
    auto cls = X86RelocTraits::classify(19);  // R_X86_64_TLSGD
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::GeneralDynamic);
    EXPECT_FALSE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, TlsLocalDynamic) {
    auto cls = X86RelocTraits::classify(20);  // R_X86_64_TLSLD
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::LocalDynamic);
    EXPECT_FALSE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

TEST(X86RelocTraits, Plt32Skipped) {
    auto cls = X86RelocTraits::classify(4);  // R_X86_64_PLT32
    EXPECT_TRUE(cls.skip);
}

TEST(X86RelocTraits, NoneSkipped) {
    auto cls = X86RelocTraits::classify(0);  // R_X86_64_NONE
    EXPECT_TRUE(cls.skip);
}

TEST(X86RelocTraits, UnknownType) {
    auto cls = X86RelocTraits::classify(999);
    EXPECT_EQ(cls.kind, DataRefKind::Unknown);
    EXPECT_EQ(cls.tls_model, TlsModel::None);
    EXPECT_FALSE(cls.is_pc_relative);
    EXPECT_FALSE(cls.skip);
}

// ---- ARM64RelocTraits::classify() unit tests ----

TEST(ARM64RelocTraits, DirectRef) {
    auto cls = ARM64RelocTraits::classify(257);  // R_AARCH64_ABS64
    EXPECT_EQ(cls.kind, DataRefKind::Unknown);
    EXPECT_FALSE(cls.skip);
}

TEST(ARM64RelocTraits, GotReloc) {
    auto cls = ARM64RelocTraits::classify(311);  // R_AARCH64_ADR_GOT_PAGE
    EXPECT_EQ(cls.kind, DataRefKind::GotLoad);
    EXPECT_FALSE(cls.skip);
}

TEST(ARM64RelocTraits, TlsLocalExec) {
    auto cls = ARM64RelocTraits::classify(549);  // TLSLE_ADD_TPREL_HI12
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::LocalExec);
    EXPECT_FALSE(cls.skip);
}

TEST(ARM64RelocTraits, TlsInitialExec) {
    auto cls = ARM64RelocTraits::classify(539);  // TLSIE_ADR_GOTTPREL_PAGE21
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::InitialExec);
    EXPECT_FALSE(cls.skip);
}

TEST(ARM64RelocTraits, TlsGeneralDynamic) {
    auto cls = ARM64RelocTraits::classify(513);  // TLSGD_ADR_PAGE21
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::GeneralDynamic);
    EXPECT_FALSE(cls.skip);
}

TEST(ARM64RelocTraits, TlsLocalDynamic) {
    auto cls = ARM64RelocTraits::classify(517);  // TLSLD_ADR_PAGE21
    EXPECT_EQ(cls.kind, DataRefKind::TlsVar);
    EXPECT_EQ(cls.tls_model, TlsModel::LocalDynamic);
    EXPECT_FALSE(cls.skip);
}

// ---- RelocationLayer (analyzeRelocations<>) integration tests ----

TEST(RelocationLayer, FilterToRegion) {
    std::vector<RelocationEntry> relocs = {
        {0x401010, 2 /*R_X86_64_PC32*/, 1, -4, "some_global"},
        {0x401500, 2, 2, -4, "other_global"},   // in region
        {0x402000, 2, 3, 0, "outside"},          // outside
    };

    // Region is [0x401400, 0x401600)
    auto refs = analyzeRelocations<X86RelocTraits>(relocs, 0x401400, 0x200);

    // Only the reloc at 0x401500 should be in the region
    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].insn_offset, 0x401500u);
    EXPECT_EQ(refs[0].source, DataRefSource::Relocation);
}

TEST(RelocationLayer, SymbolNamePreserved) {
    std::vector<RelocationEntry> relocs = {
        {0x401050, 2 /*R_X86_64_PC32*/, 1, -4, "my_special_symbol"},
    };

    auto refs = analyzeRelocations<X86RelocTraits>(relocs, 0x401000, 0x1000);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].target_symbol, "my_special_symbol");
}

TEST(RelocationLayer, MultipleRelocsAllPreserved) {
    std::vector<RelocationEntry> relocs = {
        {0x401010, 23 /*TPOFF32*/, 1, 0, "tls_a"},
        {0x401020, 9 /*GOTPCREL*/, 2, -4, "got_b"},
        {0x401030, 2 /*PC32*/, 3, -4, "pc32_c"},
        {0x401040, 22 /*GOTTPOFF*/, 4, -4, "ie_d"},
    };

    auto refs = analyzeRelocations<X86RelocTraits>(relocs, 0x401000, 0x1000);

    ASSERT_EQ(refs.size(), 4u);
    for (const auto& ref : refs) {
        EXPECT_EQ(ref.source, DataRefSource::Relocation);
    }
}
