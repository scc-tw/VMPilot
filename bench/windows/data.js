window.BENCHMARK_DATA = {
  "lastUpdate": 1775162524992,
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
      }
    ]
  }
}