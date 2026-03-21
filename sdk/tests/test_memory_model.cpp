#include "MemoryModel.hpp"
#include <DataReference.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

using VMPilot::SDK::Core::SectionKind;
using VMPilot::SDK::ReferenceAnalyzer::MemoryModel;
using VMPilot::SDK::ReferenceAnalyzer::ReadResult;
using VMPilot::SDK::ReferenceAnalyzer::SectionData;

// ---------------------------------------------------------------------------
// MemoryModel_EmptyModel
// ---------------------------------------------------------------------------
TEST(MemoryModel, EmptyModel) {
    MemoryModel model;
    EXPECT_EQ(model.section_count(), 0u);
    EXPECT_FALSE(model.read(0x400000, 4).has_value());
    EXPECT_EQ(model.classify(0x400000), SectionKind::Unknown);
    EXPECT_FALSE(model.contains(0x400000));
}

// ---------------------------------------------------------------------------
// MemoryModel_ReadFromRodata
// ---------------------------------------------------------------------------
TEST(MemoryModel, ReadFromRodata) {
    // 8 bytes: 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08
    uint8_t rodata[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    SectionData sec{0x400000, sizeof(rodata), SectionKind::Rodata, rodata,
                    false};
    MemoryModel model({sec});

    // 1-byte read
    {
        auto r = model.read(0x400000, 1);
        ASSERT_TRUE(r.has_value());
        EXPECT_EQ(r->value, 0x01u);
        EXPECT_EQ(r->from_section, SectionKind::Rodata);
        EXPECT_FALSE(r->is_mutable);
    }

    // 2-byte read (little-endian: 0x02 0x01 at offset 0 would be 0x0201)
    {
        auto r = model.read(0x400000, 2);
        ASSERT_TRUE(r.has_value());
        EXPECT_EQ(r->value, 0x0201u);
        EXPECT_EQ(r->from_section, SectionKind::Rodata);
        EXPECT_FALSE(r->is_mutable);
    }

    // 4-byte read
    {
        auto r = model.read(0x400000, 4);
        ASSERT_TRUE(r.has_value());
        EXPECT_EQ(r->value, 0x04030201u);
        EXPECT_EQ(r->from_section, SectionKind::Rodata);
        EXPECT_FALSE(r->is_mutable);
    }

    // 8-byte read
    {
        auto r = model.read(0x400000, 8);
        ASSERT_TRUE(r.has_value());
        EXPECT_EQ(r->value, 0x0807060504030201ULL);
        EXPECT_EQ(r->from_section, SectionKind::Rodata);
        EXPECT_FALSE(r->is_mutable);
    }
}

// ---------------------------------------------------------------------------
// MemoryModel_ReadFromData
// ---------------------------------------------------------------------------
TEST(MemoryModel, ReadFromData) {
    uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    SectionData sec{0x600000, sizeof(data), SectionKind::Data, data, true};
    MemoryModel model({sec});

    auto r = model.read(0x600000, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->value, 0xDDCCBBAAu);
    EXPECT_EQ(r->from_section, SectionKind::Data);
    EXPECT_TRUE(r->is_mutable);
}

// ---------------------------------------------------------------------------
// MemoryModel_ReadFromGot
// ---------------------------------------------------------------------------
TEST(MemoryModel, ReadFromGot) {
    uint8_t got[] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
    SectionData sec{0x700000, sizeof(got), SectionKind::Got, got, false};
    MemoryModel model({sec});

    auto r = model.read(0x700000, 8);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->value, 0x8070605040302010ULL);
    EXPECT_EQ(r->from_section, SectionKind::Got);
    EXPECT_FALSE(r->is_mutable);
}

// ---------------------------------------------------------------------------
// MemoryModel_CrossSectionBoundary
// ---------------------------------------------------------------------------
TEST(MemoryModel, CrossSectionBoundary) {
    uint8_t sec1_data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t sec2_data[] = {0x05, 0x06, 0x07, 0x08};

    SectionData sec1{0x400000, sizeof(sec1_data), SectionKind::Rodata,
                     sec1_data, false};
    SectionData sec2{0x400004, sizeof(sec2_data), SectionKind::Data, sec2_data,
                     true};
    MemoryModel model({sec1, sec2});

    // 8-byte read starting at 0x400000 would span into sec2 — must fail
    EXPECT_FALSE(model.read(0x400000, 8).has_value());

    // But reading within each section should work
    EXPECT_TRUE(model.read(0x400000, 4).has_value());
    EXPECT_TRUE(model.read(0x400004, 4).has_value());
}

// ---------------------------------------------------------------------------
// MemoryModel_OutOfBounds
// ---------------------------------------------------------------------------
TEST(MemoryModel, OutOfBounds) {
    uint8_t rodata[] = {0x01, 0x02, 0x03, 0x04};
    SectionData sec{0x400000, sizeof(rodata), SectionKind::Rodata, rodata,
                    false};
    MemoryModel model({sec});

    // Read beyond section end
    EXPECT_FALSE(model.read(0x400002, 4).has_value());

    // Read completely outside
    EXPECT_FALSE(model.read(0x500000, 4).has_value());

    // Read before section start
    EXPECT_FALSE(model.read(0x3FFFFF, 2).has_value());
}

// ---------------------------------------------------------------------------
// MemoryModel_ClassifyAddress
// ---------------------------------------------------------------------------
TEST(MemoryModel, ClassifyAddress) {
    uint8_t rodata[] = {0x01};
    uint8_t data[] = {0x02};
    uint8_t got[] = {0x03};
    uint8_t tls[] = {0x04};

    std::vector<SectionData> sections = {
        {0x400000, 1, SectionKind::Rodata, rodata, false},
        {0x500000, 1, SectionKind::Data, data, true},
        {0x600000, 1, SectionKind::Got, got, false},
        {0x700000, 1, SectionKind::Tls, tls, false},
    };
    MemoryModel model(sections);

    EXPECT_EQ(model.classify(0x400000), SectionKind::Rodata);
    EXPECT_EQ(model.classify(0x500000), SectionKind::Data);
    EXPECT_EQ(model.classify(0x600000), SectionKind::Got);
    EXPECT_EQ(model.classify(0x700000), SectionKind::Tls);
}

// ---------------------------------------------------------------------------
// MemoryModel_ClassifyUnknown
// ---------------------------------------------------------------------------
TEST(MemoryModel, ClassifyUnknown) {
    uint8_t rodata[] = {0x01};
    SectionData sec{0x400000, 1, SectionKind::Rodata, rodata, false};
    MemoryModel model({sec});

    // Address not in any section
    EXPECT_EQ(model.classify(0x999999), SectionKind::Unknown);
    // Address just past section end
    EXPECT_EQ(model.classify(0x400001), SectionKind::Unknown);
}

// ---------------------------------------------------------------------------
// MemoryModel_Contains
// ---------------------------------------------------------------------------
TEST(MemoryModel, Contains) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    SectionData sec{0x400000, sizeof(data), SectionKind::Rodata, data, false};
    MemoryModel model({sec});

    EXPECT_TRUE(model.contains(0x400000));
    EXPECT_TRUE(model.contains(0x400001));
    EXPECT_TRUE(model.contains(0x400003));
    EXPECT_FALSE(model.contains(0x400004));  // past end
    EXPECT_FALSE(model.contains(0x3FFFFF));  // before start
    EXPECT_FALSE(model.contains(0x999999));  // nowhere near
}

// ---------------------------------------------------------------------------
// MemoryModel_NullDataSkipped
// ---------------------------------------------------------------------------
TEST(MemoryModel, NullDataSkipped) {
    SectionData null_sec{0x400000, 100, SectionKind::Rodata, nullptr, false};
    MemoryModel model({null_sec});

    EXPECT_EQ(model.section_count(), 0u);
    EXPECT_FALSE(model.contains(0x400000));
}

// ---------------------------------------------------------------------------
// MemoryModel_ZeroSizeSkipped
// ---------------------------------------------------------------------------
TEST(MemoryModel, ZeroSizeSkipped) {
    uint8_t data[] = {0x01};
    SectionData zero_sec{0x400000, 0, SectionKind::Rodata, data, false};
    MemoryModel model({zero_sec});

    EXPECT_EQ(model.section_count(), 0u);
    EXPECT_FALSE(model.contains(0x400000));
}

// ---------------------------------------------------------------------------
// MemoryModel_InvalidSize
// ---------------------------------------------------------------------------
TEST(MemoryModel, InvalidSize) {
    uint8_t rodata[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    SectionData sec{0x400000, sizeof(rodata), SectionKind::Rodata, rodata,
                    false};
    MemoryModel model({sec});

    EXPECT_FALSE(model.read(0x400000, 0).has_value());
    EXPECT_FALSE(model.read(0x400000, 3).has_value());
    EXPECT_FALSE(model.read(0x400000, 5).has_value());
    EXPECT_FALSE(model.read(0x400000, 6).has_value());
    EXPECT_FALSE(model.read(0x400000, 7).has_value());
}

// ---------------------------------------------------------------------------
// MemoryModel_MultipleRodataSections
// ---------------------------------------------------------------------------
TEST(MemoryModel, MultipleRodataSections) {
    uint8_t rodata1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t rodata2[] = {0x11, 0x22, 0x33, 0x44};

    SectionData sec1{0x400000, sizeof(rodata1), SectionKind::Rodata, rodata1,
                     false};
    SectionData sec2{0x500000, sizeof(rodata2), SectionKind::Rodata, rodata2,
                     false};
    MemoryModel model({sec1, sec2});

    EXPECT_EQ(model.section_count(), 2u);

    auto r1 = model.read(0x400000, 4);
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(r1->value, 0xDDCCBBAAu);

    auto r2 = model.read(0x500000, 4);
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r2->value, 0x44332211u);
}

// ---------------------------------------------------------------------------
// DataReference_IsMutableDefault
// ---------------------------------------------------------------------------
TEST(DataReference, IsMutableDefault) {
    VMPilot::SDK::Core::DataReference ref;
    EXPECT_FALSE(ref.is_mutable_target);
}
