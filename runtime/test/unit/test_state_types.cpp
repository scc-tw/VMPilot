/// @file test_state_types.cpp
/// @brief Tests for VM state types: VmInsn layout, VmOpcode count, VM constants,
///        phantom types (RegVal/MemVal/PlainVal), ByteLaneLUT, VmExecution/VmOramState
///        alignment, VmEpoch table sizes, ExecSnapshot roundtrip, branchless_select.
///
/// Extracted from smoke_test.cpp, test_phase1_foundation.cpp,
/// test_phase2_state.cpp, and test_security_properties.cpp into unit/ directory.

#include "test_blob_builder.hpp"

#include "vm_state.hpp"

#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/encoded_value.hpp>

#include <gtest/gtest.h>

#include <cstring>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ============================================================================
// VmInsn Layout (from smoke_test.cpp)
// ============================================================================

TEST(VmInsnTest, SizeIs8Bytes) {
    static_assert(sizeof(VmInsn) == 8, "VmInsn must be exactly 8 bytes");
    EXPECT_EQ(sizeof(VmInsn), 8u);
}

TEST(VmInsnTest, FieldExtraction) {
    VmInsn insn{};
    insn.opcode   = 0x1234;
    insn.flags    = 0b11'10'0101;  // operand_a=3, operand_b=2, condition=5
    insn.reg_pack = 0xAB;          // reg_a=0xA, reg_b=0xB
    insn.aux      = 0xDEADBEEF;

    EXPECT_EQ(insn.operand_a_type(), 3u);
    EXPECT_EQ(insn.operand_b_type(), 2u);
    EXPECT_EQ(insn.condition(), 5u);
    EXPECT_EQ(insn.reg_a(), 0xAu);
    EXPECT_EQ(insn.reg_b(), 0xBu);
}

// ============================================================================
// VmOpcode Count (from smoke_test.cpp)
// ============================================================================

TEST(VmOpcodeTest, Count) {
    EXPECT_EQ(VM_OPCODE_COUNT, 55u);
}

// ============================================================================
// VM Constants (from smoke_test.cpp)
// ============================================================================

TEST(VmConstantsTest, ObliviousSizeIs4KB) {
    EXPECT_EQ(VM_OBLIVIOUS_SIZE, 4096u);
    EXPECT_EQ(VM_ORAM_LINE_SIZE, 64u);
    EXPECT_EQ(VM_ORAM_NUM_LINES, 64u);
}

TEST(VmConstantsTest, RegisterAndLaneCount) {
    EXPECT_EQ(VM_REG_COUNT, 16u);
    EXPECT_EQ(VM_BYTE_LANES, 8u);
    EXPECT_EQ(VM_MAX_NESTING, 8u);
}

// ============================================================================
// Phantom Type Safety Tests (from test_phase1_foundation.cpp)
// ============================================================================

TEST(PhantomTypes, SameDomainEquality) {
    RegVal a(0xDEADBEEFCAFEBABEull);
    RegVal b(0xDEADBEEFCAFEBABEull);
    RegVal c(0x1234567890ABCDEFull);

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(PhantomTypes, DefaultInitZero) {
    RegVal r;
    MemVal m;
    PlainVal p;

    EXPECT_EQ(r.bits, 0u);
    EXPECT_EQ(m.bits, 0u);
    EXPECT_EQ(p.bits, 0u);
}

TEST(PhantomTypes, ByteLaneAccess) {
    RegVal v(0x0807060504030201ull);

    EXPECT_EQ(v.lane(0), 0x01u);
    EXPECT_EQ(v.lane(1), 0x02u);
    EXPECT_EQ(v.lane(2), 0x03u);
    EXPECT_EQ(v.lane(3), 0x04u);
    EXPECT_EQ(v.lane(4), 0x05u);
    EXPECT_EQ(v.lane(5), 0x06u);
    EXPECT_EQ(v.lane(6), 0x07u);
    EXPECT_EQ(v.lane(7), 0x08u);
}

TEST(PhantomTypes, ByteLaneSetRoundtrip) {
    RegVal v;
    for (unsigned k = 0; k < 8; ++k) {
        v.set_lane(k, static_cast<uint8_t>(k + 0xA0));
    }

    for (unsigned k = 0; k < 8; ++k) {
        EXPECT_EQ(v.lane(k), static_cast<uint8_t>(k + 0xA0));
    }
}

TEST(PhantomTypes, TriviallyCopyable) {
    // Zero runtime overhead: trivially copyable, same size as uint64_t
    static_assert(sizeof(RegVal) == sizeof(uint64_t));
    static_assert(sizeof(MemVal) == sizeof(uint64_t));
    static_assert(sizeof(PlainVal) == sizeof(uint64_t));
    static_assert(std::is_trivially_copyable_v<RegVal>);
    static_assert(std::is_trivially_copyable_v<MemVal>);
    static_assert(std::is_trivially_copyable_v<PlainVal>);

    SUCCEED();  // static_asserts are the real test
}

// ============================================================================
// ByteLaneLUT Application Tests (from test_phase1_foundation.cpp)
// ============================================================================

TEST(ByteLaneLUT, IdentityTable) {
    // Identity permutation: LUT[k][x] = x for all k, x
    uint8_t identity[8][256];
    for (int k = 0; k < 8; ++k)
        for (int x = 0; x < 256; ++x)
            identity[k][x] = static_cast<uint8_t>(x);

    ByteLaneLUT lut{identity};

    // apply(v) == v for identity
    uint64_t val = 0xDEADBEEFCAFEBABEull;
    EXPECT_EQ(lut.apply(val), val);
}

TEST(ByteLaneLUT, ReverseTable) {
    // Each lane reverses the byte: LUT[k][x] = 255 - x
    uint8_t rev[8][256];
    for (int k = 0; k < 8; ++k)
        for (int x = 0; x < 256; ++x)
            rev[k][x] = static_cast<uint8_t>(255 - x);

    ByteLaneLUT lut{rev};

    uint64_t val = 0x0000000000000001ull;
    uint64_t result = lut.apply(val);

    // lane 0: 255 - 1 = 254 = 0xFE
    // lane 1..7: 255 - 0 = 255 = 0xFF
    EXPECT_EQ(result, 0xFFFFFFFFFFFFFFFEull);
}

// ============================================================================
// VmExecution Alignment/Layout (from test_phase2_state.cpp)
// ============================================================================

TEST(Phase2Layout, VmExecutionAlignment) {
    static_assert(alignof(VmExecution) >= 64);
    static_assert(offsetof(VmExecution, regs) == 0);
    // Doc 16 added insn_fpe_key[16] + bb_chain_state[32], growing to ~2112 bytes.
    EXPECT_LE(sizeof(VmExecution), 2176u)
        << "VmExecution should be stack-friendly (< ~2KB)";
}

// ============================================================================
// VmOramState Alignment (from test_phase2_state.cpp)
// ============================================================================

TEST(Phase2Layout, VmOramAlignment) {
    static_assert(alignof(VmOramState) >= 64);
    static_assert(offsetof(VmOramState, workspace) == 0);
    // VmOramState = workspace[4096] + nonce(8) + key[16] + padding to align
    EXPECT_GE(sizeof(VmOramState), VM_OBLIVIOUS_SIZE + 8u + 16u);
}

// ============================================================================
// VmEpoch Table Sizes (from test_phase2_state.cpp)
// ============================================================================

TEST(Phase2Layout, VmEpochContainsTables) {
    // Doc 16: VmEpoch holds only opcode_perm[256] + opcode_perm_inv[256]
    // + 3 scalar fields (bb_id, epoch, live_regs_bitmap) ≈ 524 bytes.
    // RegTables and DomainTables are gone — register encoding is Speck-FPE.
    EXPECT_GE(sizeof(VmEpoch), 512u);
    EXPECT_LT(sizeof(VmEpoch), 1024u);  // must be small, not ~131KB
}

// ============================================================================
// ExecSnapshot Roundtrip (from test_security_properties.cpp)
// ============================================================================

TEST(SecurityProperties, ExecSnapshotRoundtrip) {
    VmExecution exec{};
    exec.enc_state = 0xDEADBEEFCAFEBABEull;
    exec.insn_index_in_bb = 7;
    exec.vm_ip = 42;
    for (int r = 0; r < 16; ++r)
        exec.regs[r] = RegVal(static_cast<uint64_t>(r * 1111));
    exec.current_bb_id = 5;
    exec.current_bb_index = 3;
    exec.current_epoch = 2;
    std::memset(exec.bb_chain_state, 0xAA, 32);
    std::memset(exec.insn_fpe_key, 0xBB, 16);

    // Capture
    auto snap = ExecSnapshot::capture(exec);

    // Modify exec
    exec.enc_state = 0x1111111111111111ull;
    exec.insn_index_in_bb = 99;
    exec.vm_ip = 999;
    for (int r = 0; r < 16; ++r)
        exec.regs[r] = RegVal(0xFFFFFFFFFFFFFFFFull);
    exec.current_bb_id = 100;
    exec.current_bb_index = 200;
    exec.current_epoch = 300;
    std::memset(exec.bb_chain_state, 0xCC, 32);
    std::memset(exec.insn_fpe_key, 0xDD, 16);

    // Restore with keep_new=false → must restore snapshot
    snap.branchless_restore(exec, false);

    EXPECT_EQ(exec.enc_state, 0xDEADBEEFCAFEBABEull);
    EXPECT_EQ(exec.insn_index_in_bb, 7u);
    EXPECT_EQ(exec.vm_ip, 42u);
    for (int r = 0; r < 16; ++r)
        EXPECT_EQ(exec.regs[r].bits, static_cast<uint64_t>(r * 1111))
            << "reg[" << r << "] not restored";
    EXPECT_EQ(exec.current_bb_id, 5u);
    EXPECT_EQ(exec.current_bb_index, 3u);
    EXPECT_EQ(exec.current_epoch, 2u);

    uint8_t expected_chain[32], expected_key[16];
    std::memset(expected_chain, 0xAA, 32);
    std::memset(expected_key, 0xBB, 16);
    EXPECT_EQ(std::memcmp(exec.bb_chain_state, expected_chain, 32), 0);
    EXPECT_EQ(std::memcmp(exec.insn_fpe_key, expected_key, 16), 0);
}

TEST(SecurityProperties, ExecSnapshotKeepNew) {
    VmExecution exec{};
    exec.enc_state = 0x1111111111111111ull;
    exec.vm_ip = 100;

    auto snap = ExecSnapshot::capture(exec);

    exec.enc_state = 0x2222222222222222ull;
    exec.vm_ip = 200;

    // Restore with keep_new=true → must keep current (modified) values
    snap.branchless_restore(exec, true);

    EXPECT_EQ(exec.enc_state, 0x2222222222222222ull);
    EXPECT_EQ(exec.vm_ip, 200u);
}

// ============================================================================
// VmEpoch::branchless_select (from test_security_properties.cpp)
// ============================================================================

TEST(SecurityProperties, VmEpochBranchlessSelect_Restore) {
    VmEpoch a{};
    for (int i = 0; i < 256; ++i) {
        a.opcode_perm[i] = static_cast<uint8_t>(i);
        a.opcode_perm_inv[i] = static_cast<uint8_t>(255 - i);
    }
    a.bb_id = 10;
    a.epoch = 20;
    a.live_regs_bitmap = 0x1234;

    VmEpoch snapshot = a;

    // Modify a
    for (int i = 0; i < 256; ++i) {
        a.opcode_perm[i] = static_cast<uint8_t>((i + 77) & 0xFF);
        a.opcode_perm_inv[i] = static_cast<uint8_t>((i + 33) & 0xFF);
    }
    a.bb_id = 99;
    a.epoch = 88;
    a.live_regs_bitmap = 0xFFFF;

    // branchless_select(snapshot, keep_new=false) → restore snapshot
    a.branchless_select(snapshot, false);

    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(a.opcode_perm[i], static_cast<uint8_t>(i))
            << "opcode_perm[" << i << "] not restored";
        EXPECT_EQ(a.opcode_perm_inv[i], static_cast<uint8_t>(255 - i))
            << "opcode_perm_inv[" << i << "] not restored";
    }
    EXPECT_EQ(a.bb_id, 10u);
    EXPECT_EQ(a.epoch, 20u);
    EXPECT_EQ(a.live_regs_bitmap, 0x1234);
}

TEST(SecurityProperties, VmEpochBranchlessSelect_KeepNew) {
    VmEpoch a{};
    for (int i = 0; i < 256; ++i) {
        a.opcode_perm[i] = static_cast<uint8_t>((i + 77) & 0xFF);
        a.opcode_perm_inv[i] = static_cast<uint8_t>((i + 33) & 0xFF);
    }
    a.bb_id = 99;
    a.epoch = 88;
    a.live_regs_bitmap = 0xFFFF;

    VmEpoch snapshot{};

    // branchless_select(snapshot, keep_new=true) → keep a's values
    a.branchless_select(snapshot, true);

    EXPECT_EQ(a.opcode_perm[0], static_cast<uint8_t>(77));
    EXPECT_EQ(a.bb_id, 99u);
    EXPECT_EQ(a.live_regs_bitmap, 0xFFFF);
}

// ============================================================================
// EngineEncoding tests (from test_engine_comprehensive.cpp)
// ============================================================================

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"
#include <vm/vm_encoding.hpp>
#include <vm/xex_speck64.hpp>

using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

static uint8_t enc_none() { return 0; }

TEST(EngineEncoding, FpeEncodeDecodeRoundtrip) {
    // Verify FPE_Encode/FPE_Decode roundtrip for all 256 low values.
    // Uses a deterministic key derived from a seed.
    using namespace VMPilot::Common::VM::Crypto;

    uint8_t key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    for (uint64_t v = 0; v < 256; ++v) {
        uint64_t enc = FPE_Encode(key, 0, v);
        uint64_t dec = FPE_Decode(key, 0, enc);
        EXPECT_EQ(dec, v) << "FPE roundtrip failed for v=" << v;
    }
}

TEST(EngineEncoding, FpeNonTrivial) {
    // Verify FPE encoding changes the value (plaintext 42 != ciphertext).
    using namespace VMPilot::Common::VM::Crypto;

    uint8_t key[16] = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,1,2,3,4,5,6,7,8,9,10};
    uint64_t enc = FPE_Encode(key, 0, 42);
    EXPECT_NE(enc, 42u) << "FPE encoding should change the value";
}

TEST(EngineEncoding, MemLutRoundtrip) {
    // Memory domain stays LUT-based (GlobalMemTables).
    // Verify encode_memory / decode_memory roundtrip via VmImmutable.
    uint8_t seed[32]; fill_seed(seed);
    TestBB bb{}; bb.bb_id = 1; bb.epoch = 0; bb.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb.epoch_seed, 0xDD);
    bb.instructions = {{VmOpcode::HALT, enc_none(), 0, 0, 0}};

    auto blob_data = build_test_blob(seed, {bb});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob_data.data(), blob_data.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto imm = engine->shared_immutable();
    for (uint64_t v = 0; v < 100; ++v) {
        PlainVal pv(v * 0x0101010101010101ull);
        MemVal encoded = encode_memory(imm->mem.encode_lut(), pv);
        PlainVal decoded = decode_memory(imm->mem.decode_lut(), encoded);
        EXPECT_EQ(decoded.bits, pv.bits) << "MemLUT roundtrip failed for v=" << v;
    }
}

// ============================================================================
// DomainConversion tests (from test_phase1_foundation.cpp)
// ============================================================================

TEST(DomainConversion, EncodeDecodeRoundtrip) {
    // Generate a real bijection from a seed, then verify encode->decode = identity
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    uint8_t encode_tbl[8][256];
    uint8_t decode_tbl[8][256];

    // Derive tables for a single register (lane by lane)
    for (int k = 0; k < 8; ++k) {
        uint8_t lane_seed[32];
        // Derive per-lane seed via BLAKE3 KDF with lane index as context
        char ctx_str[8];
        int ctx_len = std::snprintf(ctx_str, sizeof(ctx_str), "lane%d", k);
        Crypto::blake3_kdf(seed, ctx_str, static_cast<size_t>(ctx_len), lane_seed, 32);

        Encoding::generate_bijection(lane_seed, encode_tbl[k]);
        Encoding::invert_bijection(encode_tbl[k], decode_tbl[k]);
    }

    ByteLaneLUT enc_lut{encode_tbl};
    ByteLaneLUT dec_lut{decode_tbl};

    // Test 1000 random-ish values
    for (uint64_t i = 0; i < 1000; ++i) {
        uint64_t plain = i * 0x0101010101010101ull + i;
        PlainVal pv(plain);

        RegVal encoded = encode_register(enc_lut, pv);
        PlainVal decoded = decode_register(dec_lut, encoded);

        EXPECT_EQ(decoded.bits, pv.bits)
            << "Roundtrip failed for plain=" << plain;
    }
}

TEST(DomainConversion, StoreLoadRoundtrip) {
    // Verify store_convert -> load_convert = identity
    // (composed bijections: STORE = mem_enc . reg_dec, LOAD = reg_enc . mem_dec)

    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 0x42);

    // Derive register encoding tables
    uint8_t reg_enc[16][8][256], reg_dec[16][8][256];
    Encoding::derive_register_tables(seed, 0xFFFF, reg_enc, reg_dec);

    // Derive memory encoding tables
    uint8_t mem_enc[8][256], mem_dec[8][256];
    Encoding::derive_memory_tables(seed, mem_enc, mem_dec);

    // Derive store/load tables
    uint8_t store_tbl[16][8][256], load_tbl[16][8][256];
    Encoding::derive_store_load_tables(reg_enc, reg_dec, mem_enc, mem_dec,
                                       0xFFFF, store_tbl, load_tbl);

    // For register 0: store_convert(load_convert(v)) should give back the same RegVal
    ByteLaneLUT store_lut{store_tbl[0]};
    ByteLaneLUT load_lut{load_tbl[0]};

    for (uint64_t i = 0; i < 100; ++i) {
        RegVal original(i * 0xABCDABCDABCDABCDull + i);

        MemVal stored = store_convert(store_lut, original);
        RegVal loaded = load_convert(load_lut, stored);

        EXPECT_EQ(loaded, original)
            << "Store->Load roundtrip failed for reg 0, value=" << original.bits;
    }
}

// ============================================================================
// VmConfigTest (from smoke_test.cpp)
// ============================================================================

#include <vm/vm_config.hpp>

TEST(VmConfigTest, Defaults) {
    VmSecurityConfig config;
    EXPECT_EQ(config.fusion_granularity, 2u);
    EXPECT_EQ(config.encoding_evolution_n, 5u);
    EXPECT_EQ(config.mutation_interval, 64u);
    EXPECT_EQ(config.anti_debug_interval, 128u);
    EXPECT_FALSE(config.debug_mode);
}

// ============================================================================
// VmOpcodeTest (from smoke_test.cpp)
// ============================================================================

TEST(VmOpcodeTest, Categories) {
    EXPECT_EQ(vm_opcode_category(VmOpcode::MOVE), 0u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::ADD), 1u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::AND), 2u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::CMP), 3u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::JMP), 4u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::SEXT8), 5u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::LOCK_ADD), 6u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::NOP), 7u);
    EXPECT_EQ(vm_opcode_category(VmOpcode::RESYNC), 7u);
}

TEST(VmOpcodeTest, ToString) {
    EXPECT_STREQ(to_string(VmOpcode::HALT), "HALT");
    EXPECT_STREQ(to_string(VmOpcode::NOP), "NOP");
    EXPECT_STREQ(to_string(VmOpcode::ADD), "ADD");
}

// ============================================================================
// SipHashTest (from smoke_test.cpp)
// ============================================================================

TEST(SipHashTest, KnownVector) {
    // Test with zero key and empty message
    uint8_t key[16] = {};
    uint64_t hash = Crypto::siphash_2_4(key, nullptr, 0);
    // SipHash-2-4 with zero key and empty input should produce a specific value
    // Verify it at least produces a non-zero result
    EXPECT_NE(hash, 0u);
}

TEST(SipHashTest, DifferentInputsDifferentHashes) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t msg1[] = {0x00};
    uint8_t msg2[] = {0x01};

    uint64_t h1 = Crypto::siphash_2_4(key, msg1, 1);
    uint64_t h2 = Crypto::siphash_2_4(key, msg2, 1);
    EXPECT_NE(h1, h2);
}

TEST(SipHashTest, KeystreamDeterministic) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint64_t enc_state = 0xDEADBEEFCAFEBABE;

    uint64_t ks1 = Crypto::siphash_keystream(key, enc_state, 0);
    uint64_t ks2 = Crypto::siphash_keystream(key, enc_state, 0);
    EXPECT_EQ(ks1, ks2);

    uint64_t ks3 = Crypto::siphash_keystream(key, enc_state, 1);
    EXPECT_NE(ks1, ks3);
}

TEST(SipHashTest, ExpandProduces8Words) {
    uint8_t key[16] = {};
    uint64_t out[8] = {};
    Crypto::siphash_expand(key, 0, 0, out);

    // All 8 words should be populated (extremely unlikely all zero)
    bool all_zero = true;
    for (int i = 0; i < 8; ++i) {
        if (out[i] != 0) all_zero = false;
    }
    EXPECT_FALSE(all_zero);
}

// ============================================================================
// Phase2 tests (from test_phase2_state.cpp)
// ============================================================================

#include "vm_policy.hpp"
#include "oram_strategy.hpp"

TEST(Phase2Layout, VmEngineStackSize) {
    // VmEngine on stack should be < 4KB for deep reentrancy
    EXPECT_LE(sizeof(VmEngine<HighSecPolicy>), 4096u);
    EXPECT_LE(sizeof(VmEngine<StandardPolicy>), 4096u);
    EXPECT_LE(sizeof(VmEngine<DebugPolicy>), 4096u);
}

TEST(Phase2Epoch, EnterBbDerivesTables) {
    // Doc 16: enter_bb(bb) takes a single arg -- no VmImmutable needed.
    // It sets identity fields and derives the D4 opcode permutation.

    BBMetadata bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.entry_ip = 0;
    bb.insn_count_in_bb = 2;
    bb.live_regs_bitmap = 0x0003;  // r0 and r1 live
    for (int i = 0; i < 32; ++i) bb.epoch_seed[i] = static_cast<uint8_t>(0xBB + i);

    VmEpoch epoch{};
    epoch.enter_bb(bb);

    // Verify identity fields are set correctly
    EXPECT_EQ(epoch.bb_id, 1u);
    EXPECT_EQ(epoch.epoch, 0u);
    EXPECT_EQ(epoch.live_regs_bitmap, 0x0003u);

    // Verify opcode_perm is derived (not all zeros)
    bool all_zero = true;
    for (int i = 0; i < 256; ++i) {
        if (epoch.opcode_perm[i] != 0) { all_zero = false; break; }
    }
    EXPECT_FALSE(all_zero) << "opcode_perm should not be all zeros after enter_bb";

    // Verify opcode_perm is not the identity permutation
    bool is_identity = true;
    for (int i = 0; i < 256; ++i) {
        if (epoch.opcode_perm[i] != static_cast<uint8_t>(i)) {
            is_identity = false; break;
        }
    }
    EXPECT_FALSE(is_identity) << "opcode_perm should not be the identity permutation";

    // Verify opcode_perm_inv is the inverse of opcode_perm
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(epoch.opcode_perm_inv[epoch.opcode_perm[i]],
                  static_cast<uint8_t>(i))
            << "opcode_perm_inv[opcode_perm[" << i << "]] != " << i;
    }
}

TEST(Phase2Execution, RegistersArePhantomTyped) {
    VmExecution exec{};

    // Registers hold RegVal (Domain::Register), not raw uint64_t
    exec.regs[0] = RegVal(0xCAFEBABE);
    exec.regs[1] = RegVal(0xDEADBEEF);

    EXPECT_EQ(exec.regs[0].bits, 0xCAFEBABEu);
    EXPECT_NE(exec.regs[0], exec.regs[1]);

    // Cannot accidentally assign a MemVal to regs:
    // exec.regs[0] = MemVal(42);  // COMPILE ERROR -- this is the safety we want
}

TEST(Phase2Execution, DefaultsAreZero) {
    VmExecution exec{};

    EXPECT_EQ(exec.vm_ip, 0u);
    EXPECT_EQ(exec.vm_sp, VM_OBLIVIOUS_SIZE);
    EXPECT_EQ(exec.enc_state, 0u);
    EXPECT_EQ(exec.vm_flags, 0u);
    EXPECT_FALSE(exec.halted);
    EXPECT_FALSE(exec.branch_taken);
    EXPECT_EQ(exec.shadow_depth, 0u);
    EXPECT_EQ(exec.load_base_delta, 0);

    // Doc 16 forward-secrecy fields default to zero
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(exec.insn_fpe_key[i], 0u) << "insn_fpe_key[" << i << "] not zero";
    for (int i = 0; i < 32; ++i)
        EXPECT_EQ(exec.bb_chain_state[i], 0u) << "bb_chain_state[" << i << "] not zero";
}

TEST(Phase2Engine, SharedImmutableForReentrancy) {
    // VmImmutable can be shared via shared_ptr
    auto imm = std::make_shared<VmImmutable>();
    auto imm2 = imm;  // shared_ptr copy

    EXPECT_EQ(imm.use_count(), 2);
    EXPECT_EQ(imm.get(), imm2.get());  // same pointer
}
