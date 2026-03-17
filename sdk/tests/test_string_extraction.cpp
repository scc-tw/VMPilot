/// Unit tests for backward constant propagation in string argument extraction.
/// Each test crafts raw x86-64 machine code for a specific compiler pattern,
/// loads it through X86Handler with a synthetic rodata section, and verifies
/// that the function name is correctly extracted.

#include <gtest/gtest.h>

#include <CompilationContext.hpp>
#include <ReadOnlySection.hpp>
#include <X86Handler.hpp>
#include <utilities.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

using namespace VMPilot::SDK::Segmentator;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static constexpr uint64_t kTextBase = 0x1000;
static constexpr uint64_t kRodataBase = 0x2000;
static constexpr uint64_t kBeginPLT = 0x3000;
static constexpr uint64_t kEndPLT = 0x3010;

/// Append a CALL rel32 instruction targeting `target` to `code`.
static void appendCall(std::vector<uint8_t>& code, uint64_t target) {
    uint64_t call_addr = kTextBase + code.size();
    int32_t offset =
        static_cast<int32_t>(target - (call_addr + 5));
    code.push_back(0xE8);
    code.push_back(static_cast<uint8_t>(offset));
    code.push_back(static_cast<uint8_t>(offset >> 8));
    code.push_back(static_cast<uint8_t>(offset >> 16));
    code.push_back(static_cast<uint8_t>(offset >> 24));
}

/// Build a NativeSymbolTable with VMPilot_Begin/End entries at the PLT addrs.
static NativeSymbolTable makeSymbols() {
    NativeSymbolTable symbols;
    NativeSymbolTableEntry begin_entry;
    begin_entry.name = "_Z13VMPilot_BeginPKc";
    begin_entry.address = kBeginPLT;
    symbols.push_back(begin_entry);

    NativeSymbolTableEntry end_entry;
    end_entry.name = "_Z11VMPilot_EndPKc";
    end_entry.address = kEndPLT;
    symbols.push_back(end_entry);
    return symbols;
}

/// Build a rodata section: 4 zero bytes then "foo\0" → "foo" at VA 0x2004.
static CompilationContext makeContext() {
    std::vector<uint8_t> rodata = {0x00, 0x00, 0x00, 0x00,
                                   'f',  'o',  'o',  '\0'};
    CompilationContext ctx;
    ctx.arch = VMPilot::Common::FileArch::X86;
    ctx.mode = VMPilot::Common::FileMode::MODE_64;
    ctx.rodata_sections.push_back({std::move(rodata), kRodataBase});
    return ctx;
}

/// Run extraction: load code into X86Handler, set rodata, get regions.
/// Returns the name of the first region (or "" if no regions).
static std::string extractName(const std::vector<uint8_t>& code) {
    auto symbols = makeSymbols();
    X86Handler handler(VMPilot::Common::FileMode::MODE_64, symbols);
    EXPECT_TRUE(handler.Load(code, kTextBase));

    handler.setCompilationContext(makeContext());
    auto regions = handler.getNativeFunctions();
    if (regions.empty())
        return "";
    return regions[0]->getName();
}

/// Compute the RIP-relative displacement for a LEA at the given code offset
/// that should resolve to `target_va`. LEA size is 7 bytes (REX+opcode+modrm+disp32).
static int32_t ripDisp(size_t code_offset, uint64_t target_va) {
    // effective = (insn_addr + insn_size) + disp
    // disp = target_va - (insn_addr + insn_size)
    uint64_t next = kTextBase + code_offset + 7;
    return static_cast<int32_t>(target_va - next);
}

// ---------------------------------------------------------------------------
// x86-64 extraction tests
// ---------------------------------------------------------------------------

class StringExtractionX64 : public ::testing::Test {};

// Pattern 1: lea rdi, [rip+disp] → call (direct, no intervening instructions)
TEST_F(StringExtractionX64, DirectLEA) {
    std::vector<uint8_t> code;
    // lea rdi, [rip+disp] → target 0x2004
    int32_t d = ripDisp(code.size(), 0x2004);
    code.insert(code.end(), {0x48, 0x8D, 0x3D});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 2: lea rbp, [rip+disp] → mov rdi, rbp → call
TEST_F(StringExtractionX64, LEA_MOV_Indirect) {
    std::vector<uint8_t> code;
    // lea rbp, [rip+disp] → 0x2004
    int32_t d = ripDisp(code.size(), 0x2004);
    code.insert(code.end(), {0x48, 0x8D, 0x2D});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    // mov rdi, rbp
    code.insert(code.end(), {0x48, 0x89, 0xEF});
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 3: lea rax, [rip+disp] → mov rbp, rax → mov rdi, rbp → call
// (multi-hop register forwarding chain)
TEST_F(StringExtractionX64, MultiHopMOV) {
    std::vector<uint8_t> code;
    // lea rax, [rip+disp] → 0x2004
    int32_t d = ripDisp(code.size(), 0x2004);
    code.insert(code.end(), {0x48, 0x8D, 0x05});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    // mov rbp, rax
    code.insert(code.end(), {0x48, 0x89, 0xC5});
    // mov rdi, rbp
    code.insert(code.end(), {0x48, 0x89, 0xEF});
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 4: movabs rdi, 0x2004 → call (absolute address, non-PIE)
TEST_F(StringExtractionX64, MovImmediate) {
    std::vector<uint8_t> code;
    // movabs rdi, 0x2004  (REX.W + 0xBF + imm64)
    code.insert(code.end(), {0x48, 0xBF});
    uint64_t imm = 0x2004;
    for (int i = 0; i < 8; ++i)
        code.push_back(static_cast<uint8_t>(imm >> (i * 8)));
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 5: lea rdi, [rip+disp] (→ 0x2000) → add rdi, 4 → call
// (arithmetic adjustment on address)
TEST_F(StringExtractionX64, LEA_ADD_Adjustment) {
    std::vector<uint8_t> code;
    // lea rdi, [rip+disp] → 0x2000 (base of rodata, not the string)
    int32_t d = ripDisp(code.size(), 0x2000);
    code.insert(code.end(), {0x48, 0x8D, 0x3D});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    // add rdi, 4
    code.insert(code.end(), {0x48, 0x83, 0xC7, 0x04});
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 6: lea rdi, [rip+disp] → push rbx → sub rsp, 8 → call
// (intervening non-clobbering instructions between LEA and CALL)
TEST_F(StringExtractionX64, InterveningInstructions) {
    std::vector<uint8_t> code;
    // lea rdi, [rip+disp] → 0x2004
    int32_t d = ripDisp(code.size(), 0x2004);
    code.insert(code.end(), {0x48, 0x8D, 0x3D});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    // push rbx
    code.push_back(0x53);
    // sub rsp, 8
    code.insert(code.end(), {0x48, 0x83, 0xEC, 0x08});
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 7: mov rbx, imm → mov rdi, rbx → call
// (movabs to intermediate callee-saved, then forward)
TEST_F(StringExtractionX64, MovImmIndirect) {
    std::vector<uint8_t> code;
    // movabs rbx, 0x2004  (REX.W + 0xBB + imm64)
    code.insert(code.end(), {0x48, 0xBB});
    uint64_t imm = 0x2004;
    for (int i = 0; i < 8; ++i)
        code.push_back(static_cast<uint8_t>(imm >> (i * 8)));
    // mov rdi, rbx
    code.insert(code.end(), {0x48, 0x89, 0xDF});
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 8: lea rbp, [rip+disp] → push rbx → mov edi, ebx → sub rsp, 8
//            → mov rdi, rbp → call
// (real-world -O2 pattern: LEA far from CALL, MOV not adjacent)
TEST_F(StringExtractionX64, RealWorldO2Pattern) {
    std::vector<uint8_t> code;
    // lea rbp, [rip+disp] → 0x2004
    int32_t d = ripDisp(code.size(), 0x2004);
    code.insert(code.end(), {0x48, 0x8D, 0x2D});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    // push rbx (53)
    code.push_back(0x53);
    // mov ebx, edi (89 FB) — clobbers ebx but NOT rdi or rbp
    code.insert(code.end(), {0x89, 0xFB});
    // mov rdi, rbp (48 89 EF)
    code.insert(code.end(), {0x48, 0x89, 0xEF});
    // sub rsp, 8 (48 83 EC 08)
    code.insert(code.end(), {0x48, 0x83, 0xEC, 0x08});
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    EXPECT_EQ(extractName(code), "foo");
}

// Pattern 9: No rodata → fallback to address-based name
TEST_F(StringExtractionX64, FallbackWithoutRodata) {
    std::vector<uint8_t> code;
    // lea rdi, [rip+disp] → 0x2004
    int32_t d = ripDisp(code.size(), 0x2004);
    code.insert(code.end(), {0x48, 0x8D, 0x3D});
    code.push_back(static_cast<uint8_t>(d));
    code.push_back(static_cast<uint8_t>(d >> 8));
    code.push_back(static_cast<uint8_t>(d >> 16));
    code.push_back(static_cast<uint8_t>(d >> 24));
    appendCall(code, kBeginPLT);
    appendCall(code, kEndPLT);

    auto symbols = makeSymbols();
    X86Handler handler(VMPilot::Common::FileMode::MODE_64, symbols);
    ASSERT_TRUE(handler.Load(code, kTextBase));
    // No setCompilationContext → no rodata
    auto regions = handler.getNativeFunctions();
    ASSERT_EQ(regions.size(), 1u);
    // Should have fallback name "vmpilot_region_0x..."
    EXPECT_TRUE(regions[0]->getName().substr(0, 17) == "vmpilot_region_0x")
        << "Got: " << regions[0]->getName();
}

// ---------------------------------------------------------------------------
// x86-32 extraction tests
// ---------------------------------------------------------------------------

class StringExtractionX86 : public ::testing::Test {};

/// Helper for 32-bit: append a CALL rel32 targeting `target`.
static void appendCall32(std::vector<uint8_t>& code, uint64_t target) {
    uint64_t call_addr = kTextBase + code.size();
    int32_t offset =
        static_cast<int32_t>(target - (call_addr + 5));
    code.push_back(0xE8);
    code.push_back(static_cast<uint8_t>(offset));
    code.push_back(static_cast<uint8_t>(offset >> 8));
    code.push_back(static_cast<uint8_t>(offset >> 16));
    code.push_back(static_cast<uint8_t>(offset >> 24));
}

static std::string extractName32(const std::vector<uint8_t>& code) {
    auto symbols = makeSymbols();
    X86Handler handler(VMPilot::Common::FileMode::MODE_32, symbols);
    EXPECT_TRUE(handler.Load(code, kTextBase));
    handler.setCompilationContext(makeContext());
    auto regions = handler.getNativeFunctions();
    if (regions.empty())
        return "";
    return regions[0]->getName();
}

// x86-32 pattern: push imm32 → call
TEST_F(StringExtractionX86, PushImmediate) {
    std::vector<uint8_t> code;
    // push 0x2004  (68 04 20 00 00)
    code.insert(code.end(), {0x68, 0x04, 0x20, 0x00, 0x00});
    appendCall32(code, kBeginPLT);
    appendCall32(code, kEndPLT);

    EXPECT_EQ(extractName32(code), "foo");
}

// x86-32 pattern: mov eax, imm32 → push eax → call
TEST_F(StringExtractionX86, MovRegThenPush) {
    std::vector<uint8_t> code;
    // mov eax, 0x2004  (B8 04 20 00 00)
    code.insert(code.end(), {0xB8, 0x04, 0x20, 0x00, 0x00});
    // push eax (50)
    code.push_back(0x50);
    appendCall32(code, kBeginPLT);
    appendCall32(code, kEndPLT);

    EXPECT_EQ(extractName32(code), "foo");
}
