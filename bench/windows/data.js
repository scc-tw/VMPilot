window.BENCHMARK_DATA = {
  "lastUpdate": 1775222361042,
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
          "id": "3a1b9c832eb7613d99485b2dfb21738ac28ce3ae",
          "message": "refactor: Move BuilderBB + NativeRegistry to common, add chaff expansion\n\nMove shared types to common/ so both sdk compiler and runtime can use them:\n\n- Extract BuilderInstruction, BuilderBB, BuilderPoolEntry from\n  runtime/include/program_builder.hpp to common/include/vm/builder_types.hpp\n  (namespace VMPilot::Common::VM; runtime adds using aliases)\n- Move NativeRegistry from runtime/ to common/ (same namespace, new location)\n\nAdd compiler-side chaff expansion (Doc 19 §2, §5):\n\n- common/include/vm/chaff_expansion.hpp: expand_to_fixed_width() inserts\n  N-1 chaff NOPs after each real instruction; pad_bb_dispatch_units()\n  quantizes BB length to N×Q_D multiples\n- Chaff NOPs have SplitMix64-randomized reg_a, reg_b, flags, aux fields\n  for ratchet diversification (Doc 19 Appendix D)\n- No branch target relocation needed: VM branches target BB IDs (not\n  instruction offsets), and entry_ip is auto-computed by blob builder",
          "timestamp": "2026-04-03T16:38:04+08:00",
          "tree_id": "f7b1ef6694d37d635089ff3ca911a36a1e0c8799",
          "url": "https://github.com/scc-tw/VMPilot/commit/3a1b9c832eb7613d99485b2dfb21738ac28ce3ae"
        },
        "date": 1775206326114,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 22786,
            "unit": "ns/insn",
            "extra": "median=11393000ns  p95=15321800ns  stddev=1215155.4ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 43887,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 23018.4,
            "unit": "ns/insn",
            "extra": "median=11509200ns  p95=11679200ns  stddev=64269.3ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 232.4,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 43444,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 23006.2,
            "unit": "ns/insn",
            "extra": "median=11503100ns  p95=14171100ns  stddev=786376.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 220.2,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 43467,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 23019.8,
            "unit": "ns/insn",
            "extra": "median=11509900ns  p95=14264800ns  stddev=789660.7ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 233.8,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 43441,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 22879.2,
            "unit": "ns/insn",
            "extra": "median=11439600ns  p95=11460600ns  stddev=14186.2ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 93.2,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 43708,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 75788,
            "unit": "ns/insn",
            "extra": "median=37894000ns  p95=40919200ns  stddev=928640.3ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 53002,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 13195,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 22811.8,
            "unit": "ns/insn",
            "extra": "median=11405900ns  p95=11736100ns  stddev=102301.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 25.8,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 43837,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 22836,
            "unit": "ns/insn",
            "extra": "median=11418000ns  p95=11981800ns  stddev=166900.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 50,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 43791,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 22825.8,
            "unit": "ns/insn",
            "extra": "median=11412900ns  p95=11639800ns  stddev=85867.8ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 39.8,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 43810,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 22961.4,
            "unit": "ns/insn",
            "extra": "median=11480700ns  p95=11536500ns  stddev=49456.3ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 175.4,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 43551,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 22965.4,
            "unit": "ns/insn",
            "extra": "median=11482700ns  p95=11536300ns  stddev=38065.8ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 179.4,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 43544,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 23029.4,
            "unit": "ns/insn",
            "extra": "median=11514700ns  p95=11633800ns  stddev=47057.9ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 243.4,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 43423,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 22906,
            "unit": "ns/insn",
            "extra": "median=11453000ns  p95=11560900ns  stddev=53226.8ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 120,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 43657,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 23017.6,
            "unit": "ns/insn",
            "extra": "median=11508800ns  p95=11757900ns  stddev=95432.8ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 231.6,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 43445,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 22957.6,
            "unit": "ns/insn",
            "extra": "median=11478800ns  p95=11963800ns  stddev=153531.4ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 171.6,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 43559,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 22889,
            "unit": "ns/insn",
            "extra": "median=11444500ns  p95=11758000ns  stddev=109875.4ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 103,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 43689,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 22972,
            "unit": "ns/insn",
            "extra": "median=11486000ns  p95=11737200ns  stddev=76219.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 186,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 43531,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 22991.8,
            "unit": "ns/insn",
            "extra": "median=11495900ns  p95=11562800ns  stddev=30621.4ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 205.8,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 43494,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 22982.2,
            "unit": "ns/insn",
            "extra": "median=11491100ns  p95=12137000ns  stddev=201425.2ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 196.2,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 43512,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 22947.4,
            "unit": "ns/insn",
            "extra": "median=11473700ns  p95=11784600ns  stddev=91546.1ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 161.4,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 43578,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 22877.2,
            "unit": "ns/insn",
            "extra": "median=11438600ns  p95=11603300ns  stddev=62038.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 91.2,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 43712,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 22982.2,
            "unit": "ns/insn",
            "extra": "median=11491100ns  p95=11760300ns  stddev=80961.5ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 196.2,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 43512,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 22930.2,
            "unit": "ns/insn",
            "extra": "median=11465100ns  p95=11616900ns  stddev=62605.4ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 144.2,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 43611,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 22980.2,
            "unit": "ns/insn",
            "extra": "median=11490100ns  p95=11748200ns  stddev=91704.6ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 194.2,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 43516,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 23029,
            "unit": "ns/insn",
            "extra": "median=11514500ns  p95=11548100ns  stddev=23196.6ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 243,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 43424,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 23012,
            "unit": "ns/insn",
            "extra": "median=11506000ns  p95=12034000ns  stddev=177929.3ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 226,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 43456,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 22951.6,
            "unit": "ns/insn",
            "extra": "median=11475800ns  p95=11518700ns  stddev=28606.9ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 165.6,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 43570,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 22936.6,
            "unit": "ns/insn",
            "extra": "median=11468300ns  p95=11489900ns  stddev=14888.6ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 150.6,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 43598,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 22871,
            "unit": "ns/insn",
            "extra": "median=11435500ns  p95=11459100ns  stddev=37509.4ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 85,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 43723,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 22955,
            "unit": "ns/insn",
            "extra": "median=11477500ns  p95=12056300ns  stddev=190848.6ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 169,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 43563,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 17709,
            "unit": "ns/insn",
            "extra": "median=8854500ns  p95=8903500ns  stddev=22867.8ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -5077,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 56468,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 17697,
            "unit": "ns/insn",
            "extra": "median=8848500ns  p95=8900100ns  stddev=21298.3ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -5089,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 56507,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 23055.2,
            "unit": "ns/insn",
            "extra": "median=11527600ns  p95=12942500ns  stddev=407730.4ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 269.2,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 43374,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 22996.8,
            "unit": "ns/insn",
            "extra": "median=11498400ns  p95=12007500ns  stddev=206918.2ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 210.8,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 43484,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 22861.8,
            "unit": "ns/insn",
            "extra": "median=11430900ns  p95=11651700ns  stddev=75590.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 75.8,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 43741,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 22879.6,
            "unit": "ns/insn",
            "extra": "median=11439800ns  p95=11610700ns  stddev=62379.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 93.6,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 43707,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 23086.6,
            "unit": "ns/insn",
            "extra": "median=11543300ns  p95=11674500ns  stddev=96829.8ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 300.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 43315,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 22850.2,
            "unit": "ns/insn",
            "extra": "median=11425100ns  p95=11595600ns  stddev=71804.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 64.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 43763,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 22920.2,
            "unit": "ns/insn",
            "extra": "median=11460100ns  p95=11572800ns  stddev=76771.9ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 134.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 43630,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 22913.8,
            "unit": "ns/insn",
            "extra": "median=11456900ns  p95=11719300ns  stddev=91377.3ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 127.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 43642,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 23109.2,
            "unit": "ns/insn",
            "extra": "median=11554600ns  p95=12153500ns  stddev=216569.9ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 323.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 43273,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 23101.6,
            "unit": "ns/insn",
            "extra": "median=11550800ns  p95=11768700ns  stddev=84645.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 315.6,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 43287,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 22987,
            "unit": "ns/insn",
            "extra": "median=11493500ns  p95=11574900ns  stddev=49584.0ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 201,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 43503,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 23000.6,
            "unit": "ns/insn",
            "extra": "median=11500300ns  p95=11531300ns  stddev=20891.7ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 214.6,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 43477,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 22753.2,
            "unit": "ns/insn",
            "extra": "median=11376600ns  p95=12047100ns  stddev=210867.0ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -32.8,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 43950,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 22994.8,
            "unit": "ns/insn",
            "extra": "median=11497400ns  p95=11562000ns  stddev=30441.9ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 208.8,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 43488,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 22766.8,
            "unit": "ns/insn",
            "extra": "median=11383400ns  p95=11505900ns  stddev=47008.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -19.2,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 43924,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 22803,
            "unit": "ns/insn",
            "extra": "median=11401500ns  p95=11440900ns  stddev=32518.9ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 17,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 43854,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 22755,
            "unit": "ns/insn",
            "extra": "median=11377500ns  p95=11437100ns  stddev=20202.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -31,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 43946,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 22738,
            "unit": "ns/insn",
            "extra": "median=11369000ns  p95=11444700ns  stddev=34550.8ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -48,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 43979,
            "unit": "insn/s"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "scc@sccdeMacBook-Air.local",
            "name": "scc"
          },
          "committer": {
            "email": "scc@sccdeMacBook-Air.local",
            "name": "scc"
          },
          "distinct": true,
          "id": "6b140f104a2e10bb1421cc4607cb459926853019",
          "message": "fix: replace std::deque with std::list in ThreadPool to fix ASan false positives\n\nApple libc++ (clang-1700+) deque ASan annotations incorrectly poison\nvalid elements in the work-stealing queue, causing container-overflow\naborts in std::function move-assignment across 8 test suites. All deque\naccesses are correctly mutex-serialized — no data race exists.\n\nCo-Authored-By: Claude Opus 4.6 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-04-03T18:44:27+08:00",
          "tree_id": "e6ee06e6abac7b9cbac01acca8e7a092f2efb23a",
          "url": "https://github.com/scc-tw/VMPilot/commit/6b140f104a2e10bb1421cc4607cb459926853019"
        },
        "date": 1775222137351,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 25455.2,
            "unit": "ns/insn",
            "extra": "median=12727600ns  p95=13551400ns  stddev=512260.0ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 39285,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 23527.4,
            "unit": "ns/insn",
            "extra": "median=11763700ns  p95=13788600ns  stddev=771454.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": -1927.8,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 42504,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 24162.8,
            "unit": "ns/insn",
            "extra": "median=12081400ns  p95=18070200ns  stddev=2607539.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": -1292.4,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 41386,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 23086.8,
            "unit": "ns/insn",
            "extra": "median=11543400ns  p95=11803100ns  stddev=81285.2ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": -2368.4,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 43315,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 23523.8,
            "unit": "ns/insn",
            "extra": "median=11761900ns  p95=16194800ns  stddev=1445480.6ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": -1931.4,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 42510,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 76098,
            "unit": "ns/insn",
            "extra": "median=38049000ns  p95=45264300ns  stddev=2194349.4ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 50642.8,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 13141,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 22895.4,
            "unit": "ns/insn",
            "extra": "median=11447700ns  p95=17534700ns  stddev=1819891.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -2559.8,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 43677,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 23918,
            "unit": "ns/insn",
            "extra": "median=11959000ns  p95=16008000ns  stddev=1540495.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -1537.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 41810,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 23729.6,
            "unit": "ns/insn",
            "extra": "median=11864800ns  p95=14704200ns  stddev=1075596.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -1725.6,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 42141,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 23111.8,
            "unit": "ns/insn",
            "extra": "median=11555900ns  p95=18025200ns  stddev=1864903.3ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": -2343.4,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 43268,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 23466,
            "unit": "ns/insn",
            "extra": "median=11733000ns  p95=15213100ns  stddev=1314296.7ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": -1989.2,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 42615,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 23969.4,
            "unit": "ns/insn",
            "extra": "median=11984700ns  p95=12131600ns  stddev=118333.3ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": -1485.8,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 41720,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 23474,
            "unit": "ns/insn",
            "extra": "median=11737000ns  p95=17454400ns  stddev=1770230.8ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": -1981.2,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 42600,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 25954.8,
            "unit": "ns/insn",
            "extra": "median=12977400ns  p95=14556300ns  stddev=1101140.2ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 499.6,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 38529,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 23714.8,
            "unit": "ns/insn",
            "extra": "median=11857400ns  p95=12140400ns  stddev=103836.0ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": -1740.4,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 42168,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 22967,
            "unit": "ns/insn",
            "extra": "median=11483500ns  p95=11935700ns  stddev=203288.6ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": -2488.2,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 43541,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 26109.4,
            "unit": "ns/insn",
            "extra": "median=13054700ns  p95=17451700ns  stddev=1459316.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 654.2,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 38300,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 23621.8,
            "unit": "ns/insn",
            "extra": "median=11810900ns  p95=13967800ns  stddev=690630.0ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": -1833.4,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 42334,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 26455.4,
            "unit": "ns/insn",
            "extra": "median=13227700ns  p95=17527500ns  stddev=1280229.9ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 1000.2,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 37799,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 26100.4,
            "unit": "ns/insn",
            "extra": "median=13050200ns  p95=17358500ns  stddev=1572897.1ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 645.2,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 38314,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 23003,
            "unit": "ns/insn",
            "extra": "median=11501500ns  p95=17686100ns  stddev=2062498.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": -2452.2,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 43473,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 23173.8,
            "unit": "ns/insn",
            "extra": "median=11586900ns  p95=11853200ns  stddev=90360.9ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": -2281.4,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 43152,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 23035.4,
            "unit": "ns/insn",
            "extra": "median=11517700ns  p95=11641700ns  stddev=57775.7ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": -2419.8,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 43411,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 23451.4,
            "unit": "ns/insn",
            "extra": "median=11725700ns  p95=17251500ns  stddev=1667873.0ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": -2003.8,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 42641,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 23414.6,
            "unit": "ns/insn",
            "extra": "median=11707300ns  p95=13336200ns  stddev=631069.2ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": -2040.6,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 42708,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 23624.6,
            "unit": "ns/insn",
            "extra": "median=11812300ns  p95=13514700ns  stddev=787176.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": -1830.6,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 42329,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 27290.8,
            "unit": "ns/insn",
            "extra": "median=13645400ns  p95=15072500ns  stddev=1293695.2ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 1835.6,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 36642,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 23774.2,
            "unit": "ns/insn",
            "extra": "median=11887100ns  p95=12884200ns  stddev=453973.2ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": -1681,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 42062,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 23577.6,
            "unit": "ns/insn",
            "extra": "median=11788800ns  p95=17866500ns  stddev=2072821.0ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -1877.6,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 42413,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 23191.4,
            "unit": "ns/insn",
            "extra": "median=11595700ns  p95=16659600ns  stddev=1494386.2ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -2263.8,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 43119,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 17806,
            "unit": "ns/insn",
            "extra": "median=8903000ns  p95=11705100ns  stddev=805146.1ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -7649.2,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 56161,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 19417.8,
            "unit": "ns/insn",
            "extra": "median=9708900ns  p95=11671800ns  stddev=727056.8ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -6037.4,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 51499,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 25571.8,
            "unit": "ns/insn",
            "extra": "median=12785900ns  p95=18923800ns  stddev=2295410.8ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 116.6,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 39106,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 25507.2,
            "unit": "ns/insn",
            "extra": "median=12753600ns  p95=17932700ns  stddev=2187755.1ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 52,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 39205,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 23526.2,
            "unit": "ns/insn",
            "extra": "median=11763100ns  p95=12391500ns  stddev=291314.3ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -1929,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 42506,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 25766.2,
            "unit": "ns/insn",
            "extra": "median=12883100ns  p95=16928500ns  stddev=1807012.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 311,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 38811,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 22909.8,
            "unit": "ns/insn",
            "extra": "median=11454900ns  p95=11969900ns  stddev=204087.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -2545.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 43649,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 24648.4,
            "unit": "ns/insn",
            "extra": "median=12324200ns  p95=16574900ns  stddev=1565967.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -806.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 40571,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 27493,
            "unit": "ns/insn",
            "extra": "median=13746500ns  p95=17242700ns  stddev=1750784.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 2037.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 36373,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 23467,
            "unit": "ns/insn",
            "extra": "median=11733500ns  p95=17859300ns  stddev=1932083.9ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -1988.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 42613,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 23057.6,
            "unit": "ns/insn",
            "extra": "median=11528800ns  p95=17987100ns  stddev=2692488.2ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -2397.6,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 43370,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 25212,
            "unit": "ns/insn",
            "extra": "median=12606000ns  p95=14787200ns  stddev=917236.3ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -243.2,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 39664,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 31811.2,
            "unit": "ns/insn",
            "extra": "median=15905600ns  p95=18335500ns  stddev=1954291.8ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 6356,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 31435,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 25436.2,
            "unit": "ns/insn",
            "extra": "median=12718100ns  p95=13523100ns  stddev=437088.7ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -19,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 39314,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 25250.8,
            "unit": "ns/insn",
            "extra": "median=12625400ns  p95=18282100ns  stddev=2527038.1ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -204.4,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 39603,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 23326.6,
            "unit": "ns/insn",
            "extra": "median=11663300ns  p95=12675500ns  stddev=314763.6ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -2128.6,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 42870,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 25411.2,
            "unit": "ns/insn",
            "extra": "median=12705600ns  p95=13343700ns  stddev=260480.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -44,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 39353,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 31807.6,
            "unit": "ns/insn",
            "extra": "median=15903800ns  p95=17872100ns  stddev=2366298.8ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 6352.4,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 31439,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 24929,
            "unit": "ns/insn",
            "extra": "median=12464500ns  p95=16603300ns  stddev=1518859.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -526.2,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 40114,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 31221,
            "unit": "ns/insn",
            "extra": "median=15610500ns  p95=17922200ns  stddev=2360171.2ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 5765.8,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 32030,
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
          "id": "652c190e41423870c56a48509017777f2cc0834c",
          "message": "test: add Doc 17/19 cryptographic property tests (15 tests)\n\nGroup A — Doc 17 enc_state entanglement (5 tests):\n  - Flags/reg_pack field differences drive ratchet divergence\n  - Old 6-byte ratchet was blind to flags; new 8-byte is not\n  - 1-bit tamper in encrypted blob cascades into MAC failure\n  - Phase G key ratchet includes full 8-byte instruction\n\nGroup B — Doc 19 Theorem 4.1 isomorphism (4 tests):\n  - N x step() == 1 x dispatch_unit() for HighSec/Standard/Debug\n  - Multi-DU cumulative isomorphism (8 x step == 2 x DU)\n\nGroup C — Doc 19 chaff welding (3 tests):\n  - Removing chaff NOP breaks enc_state chain\n  - Different CSPRNG aux produces different enc_state\n  - Adding chaff lengthens one-way composition chain\n\nGroup D — Edge cases (3 tests):\n  - HALT mid-dispatch-unit returns correct value\n  - Branch at last DU slot triggers Phase L transition\n  - Enhanced NOP ghost ALU writes to trash_regs",
          "timestamp": "2026-04-03T21:15:55+08:00",
          "tree_id": "109f6ff21177393cdac1562c6824cd189d976369",
          "url": "https://github.com/scc-tw/VMPilot/commit/652c190e41423870c56a48509017777f2cc0834c"
        },
        "date": 1775222358508,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 22725.4,
            "unit": "ns/insn",
            "extra": "median=11362700ns  p95=11585500ns  stddev=93453.9ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 44004,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 22977.4,
            "unit": "ns/insn",
            "extra": "median=11488700ns  p95=11669400ns  stddev=70869.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 252,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 43521,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 22942.8,
            "unit": "ns/insn",
            "extra": "median=11471400ns  p95=12229700ns  stddev=244125.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 217.4,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 43587,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 22924.4,
            "unit": "ns/insn",
            "extra": "median=11462200ns  p95=12220100ns  stddev=221815.5ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 199,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 43622,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 22857.8,
            "unit": "ns/insn",
            "extra": "median=11428900ns  p95=11852400ns  stddev=148901.0ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 132.4,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 43749,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 75237.2,
            "unit": "ns/insn",
            "extra": "median=37618600ns  p95=37887800ns  stddev=148126.1ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 52511.8,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 13291,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 22733.4,
            "unit": "ns/insn",
            "extra": "median=11366700ns  p95=11870100ns  stddev=158380.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 8,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 43988,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 22733,
            "unit": "ns/insn",
            "extra": "median=11366500ns  p95=11411600ns  stddev=46234.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 7.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 43989,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 22829.8,
            "unit": "ns/insn",
            "extra": "median=11414900ns  p95=11487700ns  stddev=38607.7ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 104.4,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 43802,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 22946.4,
            "unit": "ns/insn",
            "extra": "median=11473200ns  p95=12099900ns  stddev=205224.3ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 221,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 43580,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 22931.2,
            "unit": "ns/insn",
            "extra": "median=11465600ns  p95=11492200ns  stddev=17448.4ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 205.8,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 43609,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 22942.2,
            "unit": "ns/insn",
            "extra": "median=11471100ns  p95=11655600ns  stddev=60494.1ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 216.8,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 43588,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 22965.6,
            "unit": "ns/insn",
            "extra": "median=11482800ns  p95=11577100ns  stddev=41755.6ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 240.2,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 43543,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 22933.4,
            "unit": "ns/insn",
            "extra": "median=11466700ns  p95=11920600ns  stddev=160424.3ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 208,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 43605,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 22924.2,
            "unit": "ns/insn",
            "extra": "median=11462100ns  p95=11478700ns  stddev=15397.2ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 198.8,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 43622,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 22812.8,
            "unit": "ns/insn",
            "extra": "median=11406400ns  p95=11683700ns  stddev=81709.0ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 87.4,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 43835,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 22915.6,
            "unit": "ns/insn",
            "extra": "median=11457800ns  p95=11490000ns  stddev=18997.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 190.2,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 43638,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 22918.2,
            "unit": "ns/insn",
            "extra": "median=11459100ns  p95=12191200ns  stddev=219369.6ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 192.8,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 43633,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 22928.2,
            "unit": "ns/insn",
            "extra": "median=11464100ns  p95=11494200ns  stddev=17580.0ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 202.8,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 43614,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 22929.8,
            "unit": "ns/insn",
            "extra": "median=11464900ns  p95=11946000ns  stddev=139825.3ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 204.4,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 43611,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 22839.4,
            "unit": "ns/insn",
            "extra": "median=11419700ns  p95=11926200ns  stddev=156555.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 114,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 43784,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 22962.6,
            "unit": "ns/insn",
            "extra": "median=11481300ns  p95=11551400ns  stddev=30395.4ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 237.2,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 43549,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 22972.2,
            "unit": "ns/insn",
            "extra": "median=11486100ns  p95=12951300ns  stddev=426247.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 246.8,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 43531,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 22955.2,
            "unit": "ns/insn",
            "extra": "median=11477600ns  p95=11721900ns  stddev=76316.1ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 229.8,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 43563,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 22980.8,
            "unit": "ns/insn",
            "extra": "median=11490400ns  p95=11980200ns  stddev=156086.5ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 255.4,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 43515,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 23206.2,
            "unit": "ns/insn",
            "extra": "median=11603100ns  p95=14820200ns  stddev=1057730.8ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 480.8,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 43092,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 22911,
            "unit": "ns/insn",
            "extra": "median=11455500ns  p95=11496900ns  stddev=20181.8ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 185.6,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 43647,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 22916.4,
            "unit": "ns/insn",
            "extra": "median=11458200ns  p95=11476100ns  stddev=14084.1ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 191,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 43637,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 22737.4,
            "unit": "ns/insn",
            "extra": "median=11368700ns  p95=11920000ns  stddev=162486.7ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 43980,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 22817.4,
            "unit": "ns/insn",
            "extra": "median=11408700ns  p95=11478100ns  stddev=25125.0ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 92,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 43826,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 17593.2,
            "unit": "ns/insn",
            "extra": "median=8796600ns  p95=9078700ns  stddev=85482.2ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -5132.2,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 56840,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 17598.6,
            "unit": "ns/insn",
            "extra": "median=8799300ns  p95=8844000ns  stddev=21071.0ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -5126.8,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 56823,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 23169.2,
            "unit": "ns/insn",
            "extra": "median=11584600ns  p95=12108800ns  stddev=194791.5ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 443.8,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 43161,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 22805,
            "unit": "ns/insn",
            "extra": "median=11402500ns  p95=11462800ns  stddev=49931.2ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 79.6,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 43850,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 22839.8,
            "unit": "ns/insn",
            "extra": "median=11419900ns  p95=11443900ns  stddev=18578.5ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 114.4,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 43783,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 22851.2,
            "unit": "ns/insn",
            "extra": "median=11425600ns  p95=11904600ns  stddev=161425.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 125.8,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 43761,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 22758.6,
            "unit": "ns/insn",
            "extra": "median=11379300ns  p95=11529100ns  stddev=74855.8ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 33.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 43939,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 22846.2,
            "unit": "ns/insn",
            "extra": "median=11423100ns  p95=11751700ns  stddev=111083.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 120.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 43771,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 22847.2,
            "unit": "ns/insn",
            "extra": "median=11423600ns  p95=12186800ns  stddev=237300.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 121.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 43769,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 22924.6,
            "unit": "ns/insn",
            "extra": "median=11462300ns  p95=12043500ns  stddev=198236.6ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 199.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 43621,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 22865.4,
            "unit": "ns/insn",
            "extra": "median=11432700ns  p95=11603000ns  stddev=54326.2ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 140,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 43734,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 22926.6,
            "unit": "ns/insn",
            "extra": "median=11463300ns  p95=11531200ns  stddev=39899.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 201.2,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 43617,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 22960.2,
            "unit": "ns/insn",
            "extra": "median=11480100ns  p95=11747400ns  stddev=80278.5ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 234.8,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 43554,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 22986.2,
            "unit": "ns/insn",
            "extra": "median=11493100ns  p95=11996200ns  stddev=150061.3ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 260.8,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 43504,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 22742.2,
            "unit": "ns/insn",
            "extra": "median=11371100ns  p95=11415900ns  stddev=25288.7ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": 16.8,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 43971,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 22921.6,
            "unit": "ns/insn",
            "extra": "median=11460800ns  p95=11636300ns  stddev=65362.2ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 196.2,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 43627,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 22723.2,
            "unit": "ns/insn",
            "extra": "median=11361600ns  p95=11694700ns  stddev=122654.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -2.2,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 44008,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 22722,
            "unit": "ns/insn",
            "extra": "median=11361000ns  p95=11435800ns  stddev=36486.4ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -3.4,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 44010,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 22702.6,
            "unit": "ns/insn",
            "extra": "median=11351300ns  p95=11712400ns  stddev=111058.9ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -22.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 44048,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 22719,
            "unit": "ns/insn",
            "extra": "median=11359500ns  p95=11397500ns  stddev=19654.5ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -6.4,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 44016,
            "unit": "insn/s"
          }
        ]
      }
    ]
  }
}