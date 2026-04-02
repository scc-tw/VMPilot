window.BENCHMARK_DATA = {
  "lastUpdate": 1775163885116,
  "repoUrl": "https://github.com/scc-tw/VMPilot",
  "entries": {
    "windows": [
      {
        "commit": {
          "author": {
            "email": "scc@scc.tw",
            "name": "scc",
            "username": "scc-tw"
          },
          "committer": {
            "email": "scc@scc.tw",
            "name": "scc",
            "username": "scc-tw"
          },
          "distinct": true,
          "id": "cff327330048790ca49fb668f3d9d86b30c0cf98",
          "message": "fix: Add missing cstddef include for size_t in opcode_spec.hpp",
          "timestamp": "2026-04-03T04:40:04+08:00",
          "tree_id": "18ea829370e32f46e76a98f5b28c75b7ca90f5bd",
          "url": "https://github.com/scc-tw/VMPilot/commit/cff327330048790ca49fb668f3d9d86b30c0cf98"
        },
        "date": 1775162519972,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 1952,
            "unit": "ns/insn",
            "extra": "median=976000ns  p95=989000ns  stddev=8040.4ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 512295,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 2041.6,
            "unit": "ns/insn",
            "extra": "median=1020800ns  p95=1033200ns  stddev=6722.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 89.6,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 489812,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 1996,
            "unit": "ns/insn",
            "extra": "median=998000ns  p95=1029300ns  stddev=12450.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 44,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 501002,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 2009.4,
            "unit": "ns/insn",
            "extra": "median=1004700ns  p95=1040000ns  stddev=16179.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 57.4,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 497661,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 1989.8,
            "unit": "ns/insn",
            "extra": "median=994900ns  p95=1026900ns  stddev=14259.3ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 37.8,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 502563,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 3991.4,
            "unit": "ns/insn",
            "extra": "median=1995700ns  p95=2036600ns  stddev=12929.8ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 2039.4,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 250539,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 2010.6,
            "unit": "ns/insn",
            "extra": "median=1005300ns  p95=1299800ns  stddev=114539.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 58.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 497364,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 1989.2,
            "unit": "ns/insn",
            "extra": "median=994600ns  p95=1016000ns  stddev=10825.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 37.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 502715,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 1990.6,
            "unit": "ns/insn",
            "extra": "median=995300ns  p95=1515500ns  stddev=150185.2ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 38.6,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 502361,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 2690.8,
            "unit": "ns/insn",
            "extra": "median=1345400ns  p95=1355200ns  stddev=106426.0ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 738.8,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 371637,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 2062.2,
            "unit": "ns/insn",
            "extra": "median=1031100ns  p95=1113700ns  stddev=28013.1ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 110.2,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 484919,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 2067.4,
            "unit": "ns/insn",
            "extra": "median=1033700ns  p95=1043400ns  stddev=13389.3ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 115.4,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 483699,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 2026.6,
            "unit": "ns/insn",
            "extra": "median=1013300ns  p95=1065800ns  stddev=17298.7ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 74.6,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 493437,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 2064.4,
            "unit": "ns/insn",
            "extra": "median=1032200ns  p95=1033900ns  stddev=11031.9ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 112.4,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 484402,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 2022.6,
            "unit": "ns/insn",
            "extra": "median=1011300ns  p95=1111100ns  stddev=28804.3ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 70.6,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 494413,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 2269.4,
            "unit": "ns/insn",
            "extra": "median=1134700ns  p95=1234200ns  stddev=71730.0ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 317.4,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 440645,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 2343.6,
            "unit": "ns/insn",
            "extra": "median=1171800ns  p95=1230300ns  stddev=31392.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 391.6,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 426694,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 2219.8,
            "unit": "ns/insn",
            "extra": "median=1109900ns  p95=1179900ns  stddev=37277.9ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 267.8,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 450491,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 2227.8,
            "unit": "ns/insn",
            "extra": "median=1113900ns  p95=1213400ns  stddev=41769.7ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 275.8,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 448873,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 2127.4,
            "unit": "ns/insn",
            "extra": "median=1063700ns  p95=1086300ns  stddev=16255.0ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 175.4,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 470057,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 2076.2,
            "unit": "ns/insn",
            "extra": "median=1038100ns  p95=1065700ns  stddev=13180.9ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 124.2,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 481649,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 2109.4,
            "unit": "ns/insn",
            "extra": "median=1054700ns  p95=1082000ns  stddev=20598.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 157.4,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 474068,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 2077.4,
            "unit": "ns/insn",
            "extra": "median=1038700ns  p95=1066600ns  stddev=12504.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 125.4,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 481371,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 2081.2,
            "unit": "ns/insn",
            "extra": "median=1040600ns  p95=1059400ns  stddev=12303.4ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 129.2,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 480492,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 2114.8,
            "unit": "ns/insn",
            "extra": "median=1057400ns  p95=1081300ns  stddev=16467.5ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 162.8,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 472858,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 2126,
            "unit": "ns/insn",
            "extra": "median=1063000ns  p95=1104500ns  stddev=21514.8ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 174,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 470367,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 2097.2,
            "unit": "ns/insn",
            "extra": "median=1048600ns  p95=1059400ns  stddev=11816.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 145.2,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 476826,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 2087.8,
            "unit": "ns/insn",
            "extra": "median=1043900ns  p95=1052400ns  stddev=10690.2ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 135.8,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 478973,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 2019.4,
            "unit": "ns/insn",
            "extra": "median=1009700ns  p95=1208200ns  stddev=59490.7ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 67.4,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 495197,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 2030.6,
            "unit": "ns/insn",
            "extra": "median=1015300ns  p95=1277100ns  stddev=79458.5ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 78.6,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 492465,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 9232.4,
            "unit": "ns/insn",
            "extra": "median=4616200ns  p95=4809800ns  stddev=82092.0ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 7280.4,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 108314,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 9500.8,
            "unit": "ns/insn",
            "extra": "median=4750400ns  p95=4770600ns  stddev=18817.4ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 7548.8,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 105254,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 2223.6,
            "unit": "ns/insn",
            "extra": "median=1111800ns  p95=1133800ns  stddev=14230.5ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 271.6,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 449721,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 2088.8,
            "unit": "ns/insn",
            "extra": "median=1044400ns  p95=1056900ns  stddev=16353.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 136.8,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 478744,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 2043.4,
            "unit": "ns/insn",
            "extra": "median=1021700ns  p95=1066100ns  stddev=19674.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 91.4,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 489380,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 2043.6,
            "unit": "ns/insn",
            "extra": "median=1021800ns  p95=1053500ns  stddev=15934.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 91.6,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 489333,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 2060,
            "unit": "ns/insn",
            "extra": "median=1030000ns  p95=1051200ns  stddev=12691.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 108,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 485437,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 2038.8,
            "unit": "ns/insn",
            "extra": "median=1019400ns  p95=1052700ns  stddev=17693.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 86.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 490485,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 2026.2,
            "unit": "ns/insn",
            "extra": "median=1013100ns  p95=1138000ns  stddev=40592.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 74.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 493535,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 2006.8,
            "unit": "ns/insn",
            "extra": "median=1003400ns  p95=1106900ns  stddev=31536.2ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 54.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 498306,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 2013,
            "unit": "ns/insn",
            "extra": "median=1006500ns  p95=1021300ns  stddev=9536.4ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 61,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 496771,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 2027.4,
            "unit": "ns/insn",
            "extra": "median=1013700ns  p95=1062300ns  stddev=20032.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 75.4,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 493243,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 2035.4,
            "unit": "ns/insn",
            "extra": "median=1017700ns  p95=1030600ns  stddev=13943.2ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 83.4,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 491304,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 1997.2,
            "unit": "ns/insn",
            "extra": "median=998600ns  p95=1084800ns  stddev=26935.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 45.2,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 500701,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 1937.4,
            "unit": "ns/insn",
            "extra": "median=968700ns  p95=999700ns  stddev=15584.1ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -14.6,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 516156,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 2037,
            "unit": "ns/insn",
            "extra": "median=1018500ns  p95=1107500ns  stddev=31456.7ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 85,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 490918,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 1967.8,
            "unit": "ns/insn",
            "extra": "median=983900ns  p95=993500ns  stddev=10842.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 15.8,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 508182,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 1937.6,
            "unit": "ns/insn",
            "extra": "median=968800ns  p95=1007200ns  stddev=14633.3ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 516102,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 1977.8,
            "unit": "ns/insn",
            "extra": "median=988900ns  p95=1165000ns  stddev=67286.7ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 25.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 505612,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 1969,
            "unit": "ns/insn",
            "extra": "median=984500ns  p95=993800ns  stddev=11669.1ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 17,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 507872,
            "unit": "insn/s"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "scc@scc.tw",
            "name": "scc",
            "username": "scc-tw"
          },
          "committer": {
            "email": "scc@scc.tw",
            "name": "scc",
            "username": "scc-tw"
          },
          "distinct": true,
          "id": "ea5486e95a51d3bcf8c8bba51f7355cc0b312416",
          "message": "fix: Deinit submodules before gh-pages switch in benchmark CI",
          "timestamp": "2026-04-03T04:43:57+08:00",
          "tree_id": "94ae8bc59b38a02d4cf1b9e810c602073311f096",
          "url": "https://github.com/scc-tw/VMPilot/commit/ea5486e95a51d3bcf8c8bba51f7355cc0b312416"
        },
        "date": 1775162750586,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 2093,
            "unit": "ns/insn",
            "extra": "median=1046500ns  p95=1075400ns  stddev=17854.4ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 477783,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 2200.8,
            "unit": "ns/insn",
            "extra": "median=1100400ns  p95=1141700ns  stddev=78117.9ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 107.8,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 454380,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 1797.6,
            "unit": "ns/insn",
            "extra": "median=898800ns  p95=910800ns  stddev=11424.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": -295.4,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 556297,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 1783.6,
            "unit": "ns/insn",
            "extra": "median=891800ns  p95=912200ns  stddev=15382.6ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": -309.4,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 560664,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 1796.8,
            "unit": "ns/insn",
            "extra": "median=898400ns  p95=1109000ns  stddev=103988.7ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": -296.2,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 556545,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 3626.4,
            "unit": "ns/insn",
            "extra": "median=1813200ns  p95=2015100ns  stddev=94751.5ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 1533.4,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 275756,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 1881.6,
            "unit": "ns/insn",
            "extra": "median=940800ns  p95=1081200ns  stddev=83331.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -211.4,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 531463,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 1878,
            "unit": "ns/insn",
            "extra": "median=939000ns  p95=1101300ns  stddev=81874.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -215,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 532481,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 1862,
            "unit": "ns/insn",
            "extra": "median=931000ns  p95=1074100ns  stddev=62857.5ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -231,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 537057,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 1941.2,
            "unit": "ns/insn",
            "extra": "median=970600ns  p95=1108600ns  stddev=61766.2ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": -151.8,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 515145,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 1921,
            "unit": "ns/insn",
            "extra": "median=960500ns  p95=1138200ns  stddev=84222.4ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": -172,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 520562,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 1939.6,
            "unit": "ns/insn",
            "extra": "median=969800ns  p95=1107800ns  stddev=76169.3ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": -153.4,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 515570,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 1941.6,
            "unit": "ns/insn",
            "extra": "median=970800ns  p95=1109600ns  stddev=62086.7ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": -151.4,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 515039,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 2136.6,
            "unit": "ns/insn",
            "extra": "median=1068300ns  p95=1126700ns  stddev=70896.6ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 43.6,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 468033,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 1898.2,
            "unit": "ns/insn",
            "extra": "median=949100ns  p95=1112900ns  stddev=68175.3ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": -194.8,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 526815,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 2126.4,
            "unit": "ns/insn",
            "extra": "median=1063200ns  p95=1319100ns  stddev=130396.6ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 33.4,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 470278,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 1976.2,
            "unit": "ns/insn",
            "extra": "median=988100ns  p95=1119800ns  stddev=65296.4ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": -116.8,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 506022,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 1932.2,
            "unit": "ns/insn",
            "extra": "median=966100ns  p95=1116400ns  stddev=65650.5ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": -160.8,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 517545,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 1920.4,
            "unit": "ns/insn",
            "extra": "median=960200ns  p95=1110000ns  stddev=69505.6ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": -172.6,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 520725,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 1914,
            "unit": "ns/insn",
            "extra": "median=957000ns  p95=1392900ns  stddev=128937.2ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": -179,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 522466,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 1871.4,
            "unit": "ns/insn",
            "extra": "median=935700ns  p95=1116300ns  stddev=79700.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": -221.6,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 534359,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 1938.6,
            "unit": "ns/insn",
            "extra": "median=969300ns  p95=1191000ns  stddev=80691.3ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": -154.4,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 515836,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 1949.8,
            "unit": "ns/insn",
            "extra": "median=974900ns  p95=1165100ns  stddev=81293.1ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": -143.2,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 512873,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 1913.8,
            "unit": "ns/insn",
            "extra": "median=956900ns  p95=1115300ns  stddev=75073.9ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": -179.2,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 522521,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 1937.4,
            "unit": "ns/insn",
            "extra": "median=968700ns  p95=1120400ns  stddev=67938.2ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": -155.6,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 516156,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 2302.2,
            "unit": "ns/insn",
            "extra": "median=1151100ns  p95=1215400ns  stddev=39608.3ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 209.2,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 434367,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 1898.2,
            "unit": "ns/insn",
            "extra": "median=949100ns  p95=1096900ns  stddev=74103.8ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": -194.8,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 526815,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 2366.8,
            "unit": "ns/insn",
            "extra": "median=1183400ns  p95=1370700ns  stddev=105427.0ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 273.8,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 422511,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 2220.6,
            "unit": "ns/insn",
            "extra": "median=1110300ns  p95=1457500ns  stddev=190884.4ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 127.6,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 450329,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 1799.8,
            "unit": "ns/insn",
            "extra": "median=899900ns  p95=1096600ns  stddev=61542.0ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -293.2,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 555617,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 8054,
            "unit": "ns/insn",
            "extra": "median=4027000ns  p95=4981500ns  stddev=315754.8ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 5961,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 124162,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 8030.8,
            "unit": "ns/insn",
            "extra": "median=4015400ns  p95=4975600ns  stddev=464272.2ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 5937.8,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 124521,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 2358.2,
            "unit": "ns/insn",
            "extra": "median=1179100ns  p95=1283500ns  stddev=138643.9ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 265.2,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 424052,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 1858.6,
            "unit": "ns/insn",
            "extra": "median=929300ns  p95=1088100ns  stddev=73617.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -234.4,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 538039,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 1929.8,
            "unit": "ns/insn",
            "extra": "median=964900ns  p95=1105700ns  stddev=70113.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -163.2,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 518188,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 1886.2,
            "unit": "ns/insn",
            "extra": "median=943100ns  p95=1072900ns  stddev=61227.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -206.8,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 530166,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 1898.6,
            "unit": "ns/insn",
            "extra": "median=949300ns  p95=1093800ns  stddev=65965.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -194.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 526704,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 1903,
            "unit": "ns/insn",
            "extra": "median=951500ns  p95=1100100ns  stddev=64159.9ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -190,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 525486,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 2089.2,
            "unit": "ns/insn",
            "extra": "median=1044600ns  p95=1365900ns  stddev=155085.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -3.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 478652,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 2408.4,
            "unit": "ns/insn",
            "extra": "median=1204200ns  p95=1366400ns  stddev=73009.6ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 315.4,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 415213,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 2456.6,
            "unit": "ns/insn",
            "extra": "median=1228300ns  p95=1362700ns  stddev=89615.4ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 363.6,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 407067,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 2323.6,
            "unit": "ns/insn",
            "extra": "median=1161800ns  p95=1422400ns  stddev=108223.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 230.6,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 430367,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 1865.8,
            "unit": "ns/insn",
            "extra": "median=932900ns  p95=1116700ns  stddev=70093.0ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": -227.2,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 535963,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 1886.8,
            "unit": "ns/insn",
            "extra": "median=943400ns  p95=1097100ns  stddev=74912.2ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -206.2,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 529998,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 2105.2,
            "unit": "ns/insn",
            "extra": "median=1052600ns  p95=1284600ns  stddev=113643.5ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": 12.2,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 475014,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 2223.6,
            "unit": "ns/insn",
            "extra": "median=1111800ns  p95=1531100ns  stddev=125346.2ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 130.6,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 449721,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 2129.4,
            "unit": "ns/insn",
            "extra": "median=1064700ns  p95=1592500ns  stddev=224401.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 36.4,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 469616,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 1768,
            "unit": "ns/insn",
            "extra": "median=884000ns  p95=1072500ns  stddev=65985.5ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -325,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 565611,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 1728,
            "unit": "ns/insn",
            "extra": "median=864000ns  p95=960900ns  stddev=38891.0ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -365,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 578704,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 1790.6,
            "unit": "ns/insn",
            "extra": "median=895300ns  p95=938200ns  stddev=30667.9ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -302.4,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 558472,
            "unit": "insn/s"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "scc@scc.tw",
            "name": "scc",
            "username": "scc-tw"
          },
          "committer": {
            "email": "scc@scc.tw",
            "name": "scc",
            "username": "scc-tw"
          },
          "distinct": true,
          "id": "902aecd1096a5507d737f0244fbc709a1508ee39",
          "message": "feat: set chart limit to 300",
          "timestamp": "2026-04-03T05:02:57+08:00",
          "tree_id": "3c3a6c4da03ebb200c8f50c6913d613c434685ab",
          "url": "https://github.com/scc-tw/VMPilot/commit/902aecd1096a5507d737f0244fbc709a1508ee39"
        },
        "date": 1775163882583,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 1962.2,
            "unit": "ns/insn",
            "extra": "median=981100ns  p95=1007900ns  stddev=12818.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 509632,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 2098.8,
            "unit": "ns/insn",
            "extra": "median=1049400ns  p95=1071600ns  stddev=16089.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 136.6,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 476463,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 2061,
            "unit": "ns/insn",
            "extra": "median=1030500ns  p95=1288500ns  stddev=77088.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 98.8,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 485201,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 2045.4,
            "unit": "ns/insn",
            "extra": "median=1022700ns  p95=1044400ns  stddev=17546.9ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 83.2,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 488902,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 2040,
            "unit": "ns/insn",
            "extra": "median=1020000ns  p95=1053500ns  stddev=18693.8ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 77.8,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 490196,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 4045.4,
            "unit": "ns/insn",
            "extra": "median=2022700ns  p95=2085200ns  stddev=18022.8ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 2083.2,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 247194,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 1985.4,
            "unit": "ns/insn",
            "extra": "median=992700ns  p95=1022700ns  stddev=13332.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 23.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 503677,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 2053.4,
            "unit": "ns/insn",
            "extra": "median=1026700ns  p95=1055500ns  stddev=16716.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 91.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 486997,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 2036.4,
            "unit": "ns/insn",
            "extra": "median=1018200ns  p95=1031800ns  stddev=13891.2ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 74.2,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 491063,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 2098.4,
            "unit": "ns/insn",
            "extra": "median=1049200ns  p95=1112500ns  stddev=25580.3ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 136.2,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 476554,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 2101.2,
            "unit": "ns/insn",
            "extra": "median=1050600ns  p95=1057200ns  stddev=14678.1ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 139,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 475919,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 2097.2,
            "unit": "ns/insn",
            "extra": "median=1048600ns  p95=1414600ns  stddev=108828.5ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 135,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 476826,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 2097.2,
            "unit": "ns/insn",
            "extra": "median=1048600ns  p95=1057100ns  stddev=13856.3ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 135,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 476826,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 2093.2,
            "unit": "ns/insn",
            "extra": "median=1046600ns  p95=1056000ns  stddev=13287.9ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 131,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 477737,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 2093.4,
            "unit": "ns/insn",
            "extra": "median=1046700ns  p95=1048700ns  stddev=12070.3ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 131.2,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 477692,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 2061.6,
            "unit": "ns/insn",
            "extra": "median=1030800ns  p95=1193000ns  stddev=52645.8ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 99.4,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 485060,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 2094.8,
            "unit": "ns/insn",
            "extra": "median=1047400ns  p95=1059300ns  stddev=15105.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 132.6,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 477373,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 2100.6,
            "unit": "ns/insn",
            "extra": "median=1050300ns  p95=1189400ns  stddev=45316.8ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 138.4,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 476054,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 2280,
            "unit": "ns/insn",
            "extra": "median=1140000ns  p95=1512200ns  stddev=156471.0ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 317.8,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 438596,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 2085.6,
            "unit": "ns/insn",
            "extra": "median=1042800ns  p95=1352300ns  stddev=99489.6ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 123.4,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 479478,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 2059.8,
            "unit": "ns/insn",
            "extra": "median=1029900ns  p95=1074000ns  stddev=22189.0ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 97.6,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 485484,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 2109,
            "unit": "ns/insn",
            "extra": "median=1054500ns  p95=1220900ns  stddev=55064.4ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 146.8,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 474158,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 2097.6,
            "unit": "ns/insn",
            "extra": "median=1048800ns  p95=1110800ns  stddev=25133.6ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 135.4,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 476735,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 2074.4,
            "unit": "ns/insn",
            "extra": "median=1037200ns  p95=1052200ns  stddev=12154.3ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 112.2,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 482067,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 2073,
            "unit": "ns/insn",
            "extra": "median=1036500ns  p95=1056100ns  stddev=12992.3ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 110.8,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 482393,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 2073.6,
            "unit": "ns/insn",
            "extra": "median=1036800ns  p95=1053800ns  stddev=10228.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 111.4,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 482253,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 2067.8,
            "unit": "ns/insn",
            "extra": "median=1033900ns  p95=1064500ns  stddev=15972.7ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 105.6,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 483606,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 2068.8,
            "unit": "ns/insn",
            "extra": "median=1034400ns  p95=1044000ns  stddev=13192.3ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 106.6,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 483372,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 1958.6,
            "unit": "ns/insn",
            "extra": "median=979300ns  p95=1047600ns  stddev=21120.3ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -3.6,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 510569,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 2015,
            "unit": "ns/insn",
            "extra": "median=1007500ns  p95=1046600ns  stddev=14864.5ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 52.8,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 496278,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 9332.4,
            "unit": "ns/insn",
            "extra": "median=4666200ns  p95=5076200ns  stddev=121077.4ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 7370.2,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 107154,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 9320,
            "unit": "ns/insn",
            "extra": "median=4660000ns  p95=4854700ns  stddev=64113.1ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 7357.8,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 107296,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 2235.6,
            "unit": "ns/insn",
            "extra": "median=1117800ns  p95=1125600ns  stddev=14471.8ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 273.4,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 447307,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 2059.2,
            "unit": "ns/insn",
            "extra": "median=1029600ns  p95=1039700ns  stddev=13305.5ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 97,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 485625,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 2058.4,
            "unit": "ns/insn",
            "extra": "median=1029200ns  p95=1034700ns  stddev=11898.7ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 96.2,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 485814,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 2037.2,
            "unit": "ns/insn",
            "extra": "median=1018600ns  p95=1059800ns  stddev=16699.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 75,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 490870,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 2053.6,
            "unit": "ns/insn",
            "extra": "median=1026800ns  p95=1041700ns  stddev=14508.8ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 91.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 486950,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 2059.2,
            "unit": "ns/insn",
            "extra": "median=1029600ns  p95=1200200ns  stddev=52806.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 97,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 485625,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 2061.4,
            "unit": "ns/insn",
            "extra": "median=1030700ns  p95=1218700ns  stddev=58458.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 99.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 485107,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 2033.2,
            "unit": "ns/insn",
            "extra": "median=1016600ns  p95=1036800ns  stddev=12735.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 71,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 491836,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 2015.2,
            "unit": "ns/insn",
            "extra": "median=1007600ns  p95=1031900ns  stddev=12042.3ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 53,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 496229,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 2052.6,
            "unit": "ns/insn",
            "extra": "median=1026300ns  p95=1064300ns  stddev=16322.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 90.4,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 487187,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 2051.4,
            "unit": "ns/insn",
            "extra": "median=1025700ns  p95=1044900ns  stddev=12654.4ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 89.2,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 487472,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 2047.4,
            "unit": "ns/insn",
            "extra": "median=1023700ns  p95=1041300ns  stddev=11918.9ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 85.2,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 488424,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 1981.8,
            "unit": "ns/insn",
            "extra": "median=990900ns  p95=1029400ns  stddev=14527.7ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": 19.6,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 504592,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 2060.6,
            "unit": "ns/insn",
            "extra": "median=1030300ns  p95=1083900ns  stddev=24566.8ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 98.4,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 485296,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 2001.4,
            "unit": "ns/insn",
            "extra": "median=1000700ns  p95=1060500ns  stddev=22994.3ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 39.2,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 499650,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 1988,
            "unit": "ns/insn",
            "extra": "median=994000ns  p95=1005500ns  stddev=12294.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 25.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 503018,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 2004.2,
            "unit": "ns/insn",
            "extra": "median=1002100ns  p95=1012200ns  stddev=14125.9ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 42,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 498952,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 2008.2,
            "unit": "ns/insn",
            "extra": "median=1004100ns  p95=1012600ns  stddev=14275.9ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 46,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 497958,
            "unit": "insn/s"
          }
        ]
      }
    ]
  }
}