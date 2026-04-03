window.BENCHMARK_DATA = {
  "lastUpdate": 1775222423961,
  "repoUrl": "https://github.com/scc-tw/VMPilot",
  "entries": {
    "linux": [
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
        "date": 1775162458481,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 1690.21,
            "unit": "ns/insn",
            "extra": "median=845104ns  p95=863839ns  stddev=7777.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 591643,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 1774.71,
            "unit": "ns/insn",
            "extra": "median=887353ns  p95=914103ns  stddev=9821.7ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 84.5,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 563474,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 1750.02,
            "unit": "ns/insn",
            "extra": "median=875009ns  p95=881762ns  stddev=5884.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 59.81,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 571423,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 1720.42,
            "unit": "ns/insn",
            "extra": "median=860212ns  p95=1144671ns  stddev=131828.6ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 30.22,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 581252,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 1709.66,
            "unit": "ns/insn",
            "extra": "median=854832ns  p95=979634ns  stddev=36286.6ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 19.46,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 584910,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 3453.51,
            "unit": "ns/insn",
            "extra": "median=1726756ns  p95=1771899ns  stddev=17277.8ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 1763.3,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 289560,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 1715.81,
            "unit": "ns/insn",
            "extra": "median=857907ns  p95=878686ns  stddev=7416.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 25.61,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 582814,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 1734.53,
            "unit": "ns/insn",
            "extra": "median=867265ns  p95=900226ns  stddev=12115.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 44.32,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 576525,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 1704.07,
            "unit": "ns/insn",
            "extra": "median=852037ns  p95=867505ns  stddev=6973.5ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 13.87,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 586829,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 1765.89,
            "unit": "ns/insn",
            "extra": "median=882944ns  p95=889447ns  stddev=4300.5ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 75.68,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 566287,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 1768.73,
            "unit": "ns/insn",
            "extra": "median=884367ns  p95=911177ns  stddev=11337.2ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 78.53,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 565376,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 1774.54,
            "unit": "ns/insn",
            "extra": "median=887272ns  p95=1174397ns  stddev=114270.0ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 84.34,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 563525,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 1769.78,
            "unit": "ns/insn",
            "extra": "median=884888ns  p95=887984ns  stddev=3113.8ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 79.57,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 565043,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 1773.42,
            "unit": "ns/insn",
            "extra": "median=886712ns  p95=889096ns  stddev=4214.9ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 83.22,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 563881,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 1772.92,
            "unit": "ns/insn",
            "extra": "median=886461ns  p95=891240ns  stddev=3494.6ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 82.71,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 564041,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 1739.26,
            "unit": "ns/insn",
            "extra": "median=869629ns  p95=872314ns  stddev=3171.2ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 49.05,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 574958,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 1766.35,
            "unit": "ns/insn",
            "extra": "median=883175ns  p95=904745ns  stddev=7678.3ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 76.14,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 566139,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 1769.06,
            "unit": "ns/insn",
            "extra": "median=884528ns  p95=886821ns  stddev=4439.5ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 78.85,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 565273,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 1770.64,
            "unit": "ns/insn",
            "extra": "median=885318ns  p95=918039ns  stddev=10533.3ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 80.43,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 564769,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 1799.65,
            "unit": "ns/insn",
            "extra": "median=899826ns  p95=1163286ns  stddev=121132.1ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 109.44,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 555663,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 1737.24,
            "unit": "ns/insn",
            "extra": "median=868618ns  p95=876161ns  stddev=5655.8ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 47.03,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 575627,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 1766.47,
            "unit": "ns/insn",
            "extra": "median=883235ns  p95=887353ns  stddev=3300.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 76.26,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 566101,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 1766.85,
            "unit": "ns/insn",
            "extra": "median=883425ns  p95=885439ns  stddev=3733.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 76.64,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 565979,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 1765.33,
            "unit": "ns/insn",
            "extra": "median=882664ns  p95=888314ns  stddev=3226.8ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 75.12,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 566467,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 1769.88,
            "unit": "ns/insn",
            "extra": "median=884938ns  p95=916006ns  stddev=11001.3ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 79.67,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 565011,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 1767.41,
            "unit": "ns/insn",
            "extra": "median=883706ns  p95=890729ns  stddev=3401.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 77.2,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 565799,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 1731.36,
            "unit": "ns/insn",
            "extra": "median=865682ns  p95=880400ns  stddev=5612.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 41.16,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 577579,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 1737.74,
            "unit": "ns/insn",
            "extra": "median=868869ns  p95=1100229ns  stddev=89069.2ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 47.53,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 575461,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 1693.81,
            "unit": "ns/insn",
            "extra": "median=846907ns  p95=891410ns  stddev=15169.2ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 3.61,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 590384,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 1734.43,
            "unit": "ns/insn",
            "extra": "median=867215ns  p95=884086ns  stddev=7046.6ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 44.22,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 576558,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 8449.11,
            "unit": "ns/insn",
            "extra": "median=4224557ns  p95=4275232ns  stddev=15563.6ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 6758.91,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 118356,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 8543.39,
            "unit": "ns/insn",
            "extra": "median=4271695ns  p95=4903262ns  stddev=232327.8ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 6853.18,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 117050,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 1842.11,
            "unit": "ns/insn",
            "extra": "median=921055ns  p95=1047220ns  stddev=36627.7ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 151.9,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 542856,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 1740.94,
            "unit": "ns/insn",
            "extra": "median=870472ns  p95=874668ns  stddev=4823.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 50.74,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 574401,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 1730.5,
            "unit": "ns/insn",
            "extra": "median=865251ns  p95=876733ns  stddev=4687.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 40.29,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 577867,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 1737.98,
            "unit": "ns/insn",
            "extra": "median=868988ns  p95=950730ns  stddev=25252.1ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 47.77,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 575382,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 1750.4,
            "unit": "ns/insn",
            "extra": "median=875200ns  p95=887092ns  stddev=8963.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 60.19,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 571298,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 1825.14,
            "unit": "ns/insn",
            "extra": "median=912570ns  p95=1143320ns  stddev=111847.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 134.93,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 547903,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 1737.4,
            "unit": "ns/insn",
            "extra": "median=868698ns  p95=882023ns  stddev=5892.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 47.19,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 575574,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 1737.12,
            "unit": "ns/insn",
            "extra": "median=868558ns  p95=873226ns  stddev=4274.6ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 46.91,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 575667,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 1734.17,
            "unit": "ns/insn",
            "extra": "median=867085ns  p95=904645ns  stddev=11868.4ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 43.96,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 576645,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 1741.4,
            "unit": "ns/insn",
            "extra": "median=870701ns  p95=884527ns  stddev=6399.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 51.19,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 574250,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 1734.55,
            "unit": "ns/insn",
            "extra": "median=867275ns  p95=870491ns  stddev=4457.6ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 44.34,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 576518,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 1730.64,
            "unit": "ns/insn",
            "extra": "median=865321ns  p95=874028ns  stddev=3884.8ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 40.43,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 577820,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 1669.89,
            "unit": "ns/insn",
            "extra": "median=834944ns  p95=841096ns  stddev=4331.2ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -20.32,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 598843,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 1748.98,
            "unit": "ns/insn",
            "extra": "median=874489ns  p95=1126398ns  stddev=110032.1ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 58.77,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 571762,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 1694.62,
            "unit": "ns/insn",
            "extra": "median=847308ns  p95=852558ns  stddev=8232.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 4.41,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 590104,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 1679.99,
            "unit": "ns/insn",
            "extra": "median=839994ns  p95=1121990ns  stddev=123428.8ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -10.22,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 595242,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 2219.53,
            "unit": "ns/insn",
            "extra": "median=1109767ns  p95=1137738ns  stddev=15871.5ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 529.33,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 450545,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 1696.46,
            "unit": "ns/insn",
            "extra": "median=848229ns  p95=855724ns  stddev=8832.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 6.25,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 589463,
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
        "date": 1775162689538,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 1692.6,
            "unit": "ns/insn",
            "extra": "median=846299ns  p95=857070ns  stddev=7783.3ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 590808,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 1775.65,
            "unit": "ns/insn",
            "extra": "median=887827ns  p95=889090ns  stddev=3059.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 83.06,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 563173,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 1740.51,
            "unit": "ns/insn",
            "extra": "median=870254ns  p95=875033ns  stddev=3491.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 47.91,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 574545,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 1717.45,
            "unit": "ns/insn",
            "extra": "median=858723ns  p95=869123ns  stddev=3991.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 24.85,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 582260,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 1713.3,
            "unit": "ns/insn",
            "extra": "median=856649ns  p95=861158ns  stddev=5247.2ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 20.7,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 583670,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 3451.94,
            "unit": "ns/insn",
            "extra": "median=1725971ns  p95=1729228ns  stddev=4461.2ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 1759.34,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 289692,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 1714.3,
            "unit": "ns/insn",
            "extra": "median=857150ns  p95=880704ns  stddev=8405.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 21.7,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 583328,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 1732.37,
            "unit": "ns/insn",
            "extra": "median=866187ns  p95=869763ns  stddev=3033.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 39.78,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 577243,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 1718.41,
            "unit": "ns/insn",
            "extra": "median=859203ns  p95=874482ns  stddev=7894.6ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 25.81,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 581935,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 1771.67,
            "unit": "ns/insn",
            "extra": "median=885834ns  p95=890963ns  stddev=4320.1ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 79.07,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 564440,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 1775.49,
            "unit": "ns/insn",
            "extra": "median=887747ns  p95=890072ns  stddev=3307.7ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 82.9,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 563224,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 1776.22,
            "unit": "ns/insn",
            "extra": "median=888108ns  p95=891614ns  stddev=3092.6ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 83.62,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 562995,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 1773.67,
            "unit": "ns/insn",
            "extra": "median=886835ns  p95=912372ns  stddev=8335.3ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 81.07,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 563803,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 1774.13,
            "unit": "ns/insn",
            "extra": "median=887066ns  p95=891684ns  stddev=4327.9ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 81.53,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 563656,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 1774.51,
            "unit": "ns/insn",
            "extra": "median=887256ns  p95=897375ns  stddev=4687.1ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 81.91,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 563535,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 1741.11,
            "unit": "ns/insn",
            "extra": "median=870555ns  p95=872879ns  stddev=4765.9ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 48.51,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 574346,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 1775.89,
            "unit": "ns/insn",
            "extra": "median=887947ns  p95=912103ns  stddev=9667.0ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 83.3,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 563097,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 1772.75,
            "unit": "ns/insn",
            "extra": "median=886374ns  p95=889650ns  stddev=3123.1ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 80.15,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 564096,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 1771.83,
            "unit": "ns/insn",
            "extra": "median=885913ns  p95=893187ns  stddev=4091.0ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 79.23,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 564390,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 1776.18,
            "unit": "ns/insn",
            "extra": "median=888088ns  p95=896673ns  stddev=4878.4ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 83.58,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 563007,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 1726.58,
            "unit": "ns/insn",
            "extra": "median=863291ns  p95=866848ns  stddev=4432.3ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 33.98,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 579179,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 1773.13,
            "unit": "ns/insn",
            "extra": "median=886565ns  p95=890292ns  stddev=3641.7ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 80.53,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 563974,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 1770.39,
            "unit": "ns/insn",
            "extra": "median=885193ns  p95=888708ns  stddev=4331.6ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 77.79,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 564849,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 1773.77,
            "unit": "ns/insn",
            "extra": "median=886885ns  p95=890302ns  stddev=4415.1ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 81.17,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 563771,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 1778.58,
            "unit": "ns/insn",
            "extra": "median=889290ns  p95=898456ns  stddev=4740.0ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 85.98,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 562246,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 1773.33,
            "unit": "ns/insn",
            "extra": "median=886665ns  p95=889480ns  stddev=4145.5ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 80.73,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 563911,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 1740.43,
            "unit": "ns/insn",
            "extra": "median=870214ns  p95=875163ns  stddev=3182.0ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 47.83,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 574571,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 1739.79,
            "unit": "ns/insn",
            "extra": "median=869894ns  p95=873561ns  stddev=2934.2ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 47.19,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 574783,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 1679.89,
            "unit": "ns/insn",
            "extra": "median=839947ns  p95=844397ns  stddev=4311.0ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -12.7,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 595276,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 1740.65,
            "unit": "ns/insn",
            "extra": "median=870325ns  p95=879121ns  stddev=5760.9ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 48.05,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 574498,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 8454.42,
            "unit": "ns/insn",
            "extra": "median=4227210ns  p95=4308472ns  stddev=28439.0ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 6761.82,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 118281,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 8901.61,
            "unit": "ns/insn",
            "extra": "median=4450807ns  p95=5117662ns  stddev=299526.1ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 7209.02,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 112339,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 1840.37,
            "unit": "ns/insn",
            "extra": "median=920187ns  p95=925948ns  stddev=3705.3ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 147.78,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 543368,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 1734.68,
            "unit": "ns/insn",
            "extra": "median=867339ns  p95=870374ns  stddev=3677.7ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 42.08,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 576476,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 1732.09,
            "unit": "ns/insn",
            "extra": "median=866047ns  p95=872468ns  stddev=3919.5ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 39.5,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 577336,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 1735.2,
            "unit": "ns/insn",
            "extra": "median=867600ns  p95=869413ns  stddev=2607.5ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 42.6,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 576302,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 1733.11,
            "unit": "ns/insn",
            "extra": "median=866557ns  p95=870675ns  stddev=4612.3ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 40.52,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 576996,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 1731.39,
            "unit": "ns/insn",
            "extra": "median=865696ns  p95=875514ns  stddev=4917.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 38.79,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 577570,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 1736,
            "unit": "ns/insn",
            "extra": "median=868000ns  p95=912623ns  stddev=17119.3ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 43.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 576037,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 1738.25,
            "unit": "ns/insn",
            "extra": "median=869123ns  p95=873120ns  stddev=4690.9ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 45.65,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 575293,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 1736.2,
            "unit": "ns/insn",
            "extra": "median=868101ns  p95=891995ns  stddev=9207.6ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 43.6,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 575970,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 1740.83,
            "unit": "ns/insn",
            "extra": "median=870414ns  p95=880644ns  stddev=4334.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 48.23,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 574439,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 1735.68,
            "unit": "ns/insn",
            "extra": "median=867839ns  p95=918414ns  stddev=15950.0ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 43.08,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 576144,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 1737.02,
            "unit": "ns/insn",
            "extra": "median=868511ns  p95=872679ns  stddev=3064.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 44.42,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 575698,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 1685.47,
            "unit": "ns/insn",
            "extra": "median=842733ns  p95=845127ns  stddev=4233.2ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -7.13,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 593308,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 1750.97,
            "unit": "ns/insn",
            "extra": "median=875484ns  p95=878811ns  stddev=5427.7ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 58.37,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 571113,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 1681.42,
            "unit": "ns/insn",
            "extra": "median=840709ns  p95=844206ns  stddev=4203.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -11.18,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 594736,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 1680.78,
            "unit": "ns/insn",
            "extra": "median=840388ns  p95=843864ns  stddev=2226.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -11.82,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 594963,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 1681.66,
            "unit": "ns/insn",
            "extra": "median=840830ns  p95=844296ns  stddev=3449.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -10.94,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 594651,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 1681.8,
            "unit": "ns/insn",
            "extra": "median=840899ns  p95=846360ns  stddev=5775.2ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -10.8,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 594602,
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
        "date": 1775163834428,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 1692.57,
            "unit": "ns/insn",
            "extra": "median=846286ns  p95=857989ns  stddev=9001.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 590817,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 1757.71,
            "unit": "ns/insn",
            "extra": "median=878857ns  p95=883325ns  stddev=4417.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 65.14,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 568921,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 1745.73,
            "unit": "ns/insn",
            "extra": "median=872866ns  p95=906889ns  stddev=14884.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 53.16,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 572826,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 1712.81,
            "unit": "ns/insn",
            "extra": "median=856406ns  p95=902412ns  stddev=14313.7ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 20.24,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 583835,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 1710.85,
            "unit": "ns/insn",
            "extra": "median=855424ns  p95=860524ns  stddev=2444.2ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 18.28,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 584505,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 3426.89,
            "unit": "ns/insn",
            "extra": "median=1713443ns  p95=1720055ns  stddev=5212.0ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 1734.31,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 291810,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 1701.13,
            "unit": "ns/insn",
            "extra": "median=850565ns  p95=855604ns  stddev=2772.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 8.56,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 587845,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 1718.4,
            "unit": "ns/insn",
            "extra": "median=859201ns  p95=885360ns  stddev=9010.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 25.83,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 581936,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 1707.16,
            "unit": "ns/insn",
            "extra": "median=853581ns  p95=857057ns  stddev=2918.3ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 14.59,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 585767,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 1760.72,
            "unit": "ns/insn",
            "extra": "median=880360ns  p95=884448ns  stddev=4299.1ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 68.15,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 567949,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 1752.28,
            "unit": "ns/insn",
            "extra": "median=876142ns  p95=881593ns  stddev=3368.2ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 59.71,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 570684,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 1753.15,
            "unit": "ns/insn",
            "extra": "median=876573ns  p95=880220ns  stddev=3834.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 60.57,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 570403,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 1762.4,
            "unit": "ns/insn",
            "extra": "median=881202ns  p95=911739ns  stddev=11507.0ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 69.83,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 567407,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 1754.39,
            "unit": "ns/insn",
            "extra": "median=877194ns  p95=883737ns  stddev=4602.5ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 61.82,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 569999,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 1751.52,
            "unit": "ns/insn",
            "extra": "median=875762ns  p95=883907ns  stddev=4638.8ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 58.95,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 570931,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 1728.36,
            "unit": "ns/insn",
            "extra": "median=864180ns  p95=875081ns  stddev=4773.7ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 35.79,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 578583,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 1765.37,
            "unit": "ns/insn",
            "extra": "median=882685ns  p95=933229ns  stddev=16889.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 72.8,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 566453,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 1764.15,
            "unit": "ns/insn",
            "extra": "median=882073ns  p95=903203ns  stddev=10254.1ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 71.57,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 566847,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 1766.15,
            "unit": "ns/insn",
            "extra": "median=883076ns  p95=890740ns  stddev=5833.8ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 73.58,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 566203,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 1758.56,
            "unit": "ns/insn",
            "extra": "median=879279ns  p95=887594ns  stddev=4915.5ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 65.99,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 568648,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 1737,
            "unit": "ns/insn",
            "extra": "median=868498ns  p95=871344ns  stddev=3262.0ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 44.42,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 575707,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 1759.3,
            "unit": "ns/insn",
            "extra": "median=879649ns  p95=884368ns  stddev=5416.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 66.73,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 568409,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 1754.47,
            "unit": "ns/insn",
            "extra": "median=877235ns  p95=883326ns  stddev=2631.5ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 61.9,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 569973,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 1755.79,
            "unit": "ns/insn",
            "extra": "median=877896ns  p95=919965ns  stddev=16214.1ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 63.22,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 569544,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 1760.18,
            "unit": "ns/insn",
            "extra": "median=880089ns  p95=990035ns  stddev=39495.4ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 67.61,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 568124,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 1883.97,
            "unit": "ns/insn",
            "extra": "median=941985ns  p95=1055937ns  stddev=59082.0ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 191.4,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 530794,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 1746.49,
            "unit": "ns/insn",
            "extra": "median=873247ns  p95=884739ns  stddev=7974.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 53.92,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 572576,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 1734.45,
            "unit": "ns/insn",
            "extra": "median=867226ns  p95=880841ns  stddev=7285.6ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 41.88,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 576551,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 1671.97,
            "unit": "ns/insn",
            "extra": "median=835987ns  p95=842149ns  stddev=5225.8ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -20.6,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 598095,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 1734.19,
            "unit": "ns/insn",
            "extra": "median=867095ns  p95=873057ns  stddev=3088.7ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 41.62,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 576638,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 8439.79,
            "unit": "ns/insn",
            "extra": "median=4219893ns  p95=4314643ns  stddev=28771.4ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 6747.21,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 118486,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 8449.42,
            "unit": "ns/insn",
            "extra": "median=4224711ns  p95=4236674ns  stddev=6258.0ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 6756.85,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 118351,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 1834.72,
            "unit": "ns/insn",
            "extra": "median=917359ns  p95=934191ns  stddev=6566.8ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 142.15,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 545043,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 1742.23,
            "unit": "ns/insn",
            "extra": "median=871113ns  p95=881432ns  stddev=6922.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 49.65,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 573978,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 1728.62,
            "unit": "ns/insn",
            "extra": "median=864311ns  p95=980818ns  stddev=33395.7ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 36.05,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 578495,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 1734.19,
            "unit": "ns/insn",
            "extra": "median=867095ns  p95=871885ns  stddev=5741.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 41.62,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 576638,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 1727.5,
            "unit": "ns/insn",
            "extra": "median=863749ns  p95=870051ns  stddev=6099.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 34.93,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 578872,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 1740.28,
            "unit": "ns/insn",
            "extra": "median=870141ns  p95=880120ns  stddev=4633.8ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 47.71,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 574620,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 1734.03,
            "unit": "ns/insn",
            "extra": "median=867016ns  p95=899165ns  stddev=10129.9ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 41.46,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 576691,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 1721.83,
            "unit": "ns/insn",
            "extra": "median=860914ns  p95=866274ns  stddev=2372.8ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 29.26,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 580778,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 1723.37,
            "unit": "ns/insn",
            "extra": "median=861685ns  p95=868108ns  stddev=4086.2ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 30.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 580258,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 1727.68,
            "unit": "ns/insn",
            "extra": "median=863840ns  p95=869781ns  stddev=4311.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 35.11,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 578811,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 1736.44,
            "unit": "ns/insn",
            "extra": "median=868218ns  p95=871133ns  stddev=3988.8ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 43.86,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 575892,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 1734.87,
            "unit": "ns/insn",
            "extra": "median=867436ns  p95=868488ns  stddev=2002.6ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 42.3,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 576411,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 1676.02,
            "unit": "ns/insn",
            "extra": "median=838011ns  p95=842138ns  stddev=4055.7ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -16.55,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 596651,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 1735.39,
            "unit": "ns/insn",
            "extra": "median=867696ns  p95=877325ns  stddev=4467.4ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 42.82,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 576239,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 1682.27,
            "unit": "ns/insn",
            "extra": "median=841137ns  p95=1136448ns  stddev=122366.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -10.3,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 594433,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 1693.92,
            "unit": "ns/insn",
            "extra": "median=846958ns  p95=899756ns  stddev=16198.8ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 1.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 590348,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 1674.96,
            "unit": "ns/insn",
            "extra": "median=837481ns  p95=841949ns  stddev=4688.9ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -17.61,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 597028,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 1671.19,
            "unit": "ns/insn",
            "extra": "median=835597ns  p95=841338ns  stddev=5140.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -21.38,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 598375,
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
        "date": 1775206118544,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 21622.43,
            "unit": "ns/insn",
            "extra": "median=10811216ns  p95=13122452ns  stddev=708224.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46248,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 21798.18,
            "unit": "ns/insn",
            "extra": "median=10899090ns  p95=10931421ns  stddev=14184.7ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 175.75,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 45875,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 21753.7,
            "unit": "ns/insn",
            "extra": "median=10876849ns  p95=10894472ns  stddev=8513.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 131.27,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 45969,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 21753.86,
            "unit": "ns/insn",
            "extra": "median=10876929ns  p95=12084043ns  stddev=361915.5ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 131.43,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 45969,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 21665.87,
            "unit": "ns/insn",
            "extra": "median=10832937ns  p95=10951899ns  stddev=44239.7ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 43.44,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 46156,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 70944.99,
            "unit": "ns/insn",
            "extra": "median=35472493ns  p95=38375108ns  stddev=939473.0ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 49322.55,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 14095,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 21608.47,
            "unit": "ns/insn",
            "extra": "median=10804234ns  p95=10844448ns  stddev=14376.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -13.96,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 46278,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 21641.71,
            "unit": "ns/insn",
            "extra": "median=10820855ns  p95=10837505ns  stddev=7557.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 19.28,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 46207,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 21660.74,
            "unit": "ns/insn",
            "extra": "median=10830372ns  p95=10850900ns  stddev=8908.6ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 38.31,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 46166,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 21829.62,
            "unit": "ns/insn",
            "extra": "median=10914809ns  p95=11090914ns  stddev=53614.7ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 207.19,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 45809,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 21823.28,
            "unit": "ns/insn",
            "extra": "median=10911640ns  p95=10987902ns  stddev=28160.7ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 200.85,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 45823,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 21818.33,
            "unit": "ns/insn",
            "extra": "median=10909165ns  p95=11054687ns  stddev=42970.0ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 195.9,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 45833,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 21822.46,
            "unit": "ns/insn",
            "extra": "median=10911229ns  p95=10935615ns  stddev=15209.7ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 200.03,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 45824,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 21828.13,
            "unit": "ns/insn",
            "extra": "median=10914065ns  p95=11112041ns  stddev=58373.3ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 205.7,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 45812,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 21797.85,
            "unit": "ns/insn",
            "extra": "median=10898926ns  p95=10940674ns  stddev=14394.9ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 175.42,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 45876,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 21695.66,
            "unit": "ns/insn",
            "extra": "median=10847831ns  p95=10886453ns  stddev=17332.7ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 73.23,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 46092,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 21787.61,
            "unit": "ns/insn",
            "extra": "median=10893806ns  p95=10979857ns  stddev=27765.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 165.18,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 45898,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 21775.09,
            "unit": "ns/insn",
            "extra": "median=10887544ns  p95=10939061ns  stddev=21331.2ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 152.66,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 45924,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 21791.5,
            "unit": "ns/insn",
            "extra": "median=10895751ns  p95=11553798ns  stddev=208236.4ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 169.07,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 45889,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 21786.07,
            "unit": "ns/insn",
            "extra": "median=10893035ns  p95=10928451ns  stddev=15389.4ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 163.64,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 45901,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 21686.36,
            "unit": "ns/insn",
            "extra": "median=10843182ns  p95=10991569ns  stddev=46179.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 63.93,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 46112,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 21784.75,
            "unit": "ns/insn",
            "extra": "median=10892374ns  p95=10924834ns  stddev=13376.3ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 162.32,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 45904,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 21801.42,
            "unit": "ns/insn",
            "extra": "median=10900710ns  p95=10937739ns  stddev=15342.7ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 178.99,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 45869,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 21805.87,
            "unit": "ns/insn",
            "extra": "median=10902934ns  p95=10959690ns  stddev=19797.0ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 183.44,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 45859,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 21789.84,
            "unit": "ns/insn",
            "extra": "median=10894918ns  p95=10957024ns  stddev=18780.0ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 167.4,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 45893,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 21794.02,
            "unit": "ns/insn",
            "extra": "median=10897012ns  p95=10937809ns  stddev=16253.3ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 171.59,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 45884,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 21766.07,
            "unit": "ns/insn",
            "extra": "median=10883036ns  p95=10920777ns  stddev=14158.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 143.64,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 45943,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 21771.88,
            "unit": "ns/insn",
            "extra": "median=10885942ns  p95=10948318ns  stddev=23437.7ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 149.45,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 45931,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 21567.62,
            "unit": "ns/insn",
            "extra": "median=10783812ns  p95=10796745ns  stddev=6084.5ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -54.81,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 46366,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 21702.53,
            "unit": "ns/insn",
            "extra": "median=10851267ns  p95=10870764ns  stddev=8994.7ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 80.1,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 46078,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 16581.2,
            "unit": "ns/insn",
            "extra": "median=8290599ns  p95=8315977ns  stddev=11718.2ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -5041.23,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 60309,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 16633.88,
            "unit": "ns/insn",
            "extra": "median=8316939ns  p95=9781451ns  stddev=422421.8ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -4988.55,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 60118,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 21766.85,
            "unit": "ns/insn",
            "extra": "median=10883427ns  p95=10953848ns  stddev=21827.5ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 144.42,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 45941,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 21699.79,
            "unit": "ns/insn",
            "extra": "median=10849895ns  p95=10933591ns  stddev=28842.3ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 77.36,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 46083,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 21674.92,
            "unit": "ns/insn",
            "extra": "median=10837461ns  p95=10909956ns  stddev=21779.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 52.49,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 46136,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 21683.96,
            "unit": "ns/insn",
            "extra": "median=10841979ns  p95=10900078ns  stddev=19507.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 61.53,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 46117,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 21683.28,
            "unit": "ns/insn",
            "extra": "median=10841639ns  p95=10929794ns  stddev=26315.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 60.85,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 46118,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 21669.33,
            "unit": "ns/insn",
            "extra": "median=10834667ns  p95=10867658ns  stddev=12053.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 46.9,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 46148,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 21672.66,
            "unit": "ns/insn",
            "extra": "median=10836330ns  p95=11001698ns  stddev=53128.5ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 50.23,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 46141,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 21681.82,
            "unit": "ns/insn",
            "extra": "median=10840908ns  p95=10872667ns  stddev=13646.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 59.38,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 46122,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 21692.8,
            "unit": "ns/insn",
            "extra": "median=10846398ns  p95=11050599ns  stddev=63281.4ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 70.36,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 46098,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 21747.84,
            "unit": "ns/insn",
            "extra": "median=10873919ns  p95=10882536ns  stddev=3417.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 125.41,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 45982,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 21789.58,
            "unit": "ns/insn",
            "extra": "median=10894789ns  p95=11191121ns  stddev=122915.2ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 167.15,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 45894,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 21768.4,
            "unit": "ns/insn",
            "extra": "median=10884199ns  p95=10933070ns  stddev=15438.0ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 145.97,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 45938,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 21588.08,
            "unit": "ns/insn",
            "extra": "median=10794040ns  p95=10832081ns  stddev=14389.4ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -34.35,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 46322,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 21774.39,
            "unit": "ns/insn",
            "extra": "median=10887194ns  p95=11015494ns  stddev=42130.3ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 151.96,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 45926,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 21575.18,
            "unit": "ns/insn",
            "extra": "median=10787588ns  p95=10799110ns  stddev=7489.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -47.26,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46350,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 21597.54,
            "unit": "ns/insn",
            "extra": "median=10798769ns  p95=10854462ns  stddev=23804.2ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -24.89,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 46302,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 21576.04,
            "unit": "ns/insn",
            "extra": "median=10788019ns  p95=10812926ns  stddev=12222.2ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -46.39,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 46348,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 21555.9,
            "unit": "ns/insn",
            "extra": "median=10777950ns  p95=10815430ns  stddev=13469.2ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -66.53,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 46391,
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
        "date": 1775222051203,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 21597.97,
            "unit": "ns/insn",
            "extra": "median=10798985ns  p95=12277650ns  stddev=524492.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46301,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 21744.18,
            "unit": "ns/insn",
            "extra": "median=10872091ns  p95=10901417ns  stddev=12451.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 146.21,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 45989,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 21730.12,
            "unit": "ns/insn",
            "extra": "median=10865059ns  p95=11022563ns  stddev=61744.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 132.15,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 46019,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 21712.14,
            "unit": "ns/insn",
            "extra": "median=10856072ns  p95=10868725ns  stddev=8259.8ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 114.17,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 46057,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 21621.98,
            "unit": "ns/insn",
            "extra": "median=10810988ns  p95=10852746ns  stddev=16918.0ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 24.01,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 46249,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 70933.53,
            "unit": "ns/insn",
            "extra": "median=35466763ns  p95=36159521ns  stddev=238125.4ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 49335.56,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 14098,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 21586.47,
            "unit": "ns/insn",
            "extra": "median=10793234ns  p95=10824171ns  stddev=11648.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -11.5,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 46325,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 21583.86,
            "unit": "ns/insn",
            "extra": "median=10791932ns  p95=10848337ns  stddev=22610.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -14.11,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 46331,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 21617.97,
            "unit": "ns/insn",
            "extra": "median=10808984ns  p95=10942794ns  stddev=47881.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 20,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 46258,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 21732.42,
            "unit": "ns/insn",
            "extra": "median=10866211ns  p95=10896988ns  stddev=12493.1ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 134.45,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 46014,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 21740.08,
            "unit": "ns/insn",
            "extra": "median=10870038ns  p95=11028885ns  stddev=45825.2ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 142.11,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 45998,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 21738.23,
            "unit": "ns/insn",
            "extra": "median=10869116ns  p95=10882821ns  stddev=6864.0ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 140.26,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 46002,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 21749.03,
            "unit": "ns/insn",
            "extra": "median=10874516ns  p95=11024958ns  stddev=47280.2ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 151.06,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 45979,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 21742.2,
            "unit": "ns/insn",
            "extra": "median=10871099ns  p95=10880598ns  stddev=5488.6ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 144.23,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 45994,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 21748.09,
            "unit": "ns/insn",
            "extra": "median=10874045ns  p95=10935571ns  stddev=19665.4ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 150.12,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 45981,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 21640.43,
            "unit": "ns/insn",
            "extra": "median=10820215ns  p95=10910584ns  stddev=29850.0ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 42.46,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 46210,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 21742.6,
            "unit": "ns/insn",
            "extra": "median=10871300ns  p95=10954636ns  stddev=29672.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 144.63,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 45993,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 21756.95,
            "unit": "ns/insn",
            "extra": "median=10878474ns  p95=10981326ns  stddev=32900.3ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 158.98,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 45962,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 21750.31,
            "unit": "ns/insn",
            "extra": "median=10875157ns  p95=10954726ns  stddev=24233.8ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 152.34,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 45976,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 21752.06,
            "unit": "ns/insn",
            "extra": "median=10876029ns  p95=11034426ns  stddev=48049.9ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 154.09,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 45973,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 21647.06,
            "unit": "ns/insn",
            "extra": "median=10823530ns  p95=10853396ns  stddev=12414.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 49.09,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 46196,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 21753.06,
            "unit": "ns/insn",
            "extra": "median=10876530ns  p95=10951000ns  stddev=28692.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 155.09,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 45971,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 21735.17,
            "unit": "ns/insn",
            "extra": "median=10867583ns  p95=10982880ns  stddev=32875.0ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 137.2,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 46008,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 21756.03,
            "unit": "ns/insn",
            "extra": "median=10878013ns  p95=10932224ns  stddev=18906.7ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 158.06,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 45964,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 21752.78,
            "unit": "ns/insn",
            "extra": "median=10876389ns  p95=10986967ns  stddev=33244.6ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 154.81,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 45971,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 21752.88,
            "unit": "ns/insn",
            "extra": "median=10876439ns  p95=10898150ns  stddev=8771.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 154.91,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 45971,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 21728.85,
            "unit": "ns/insn",
            "extra": "median=10864427ns  p95=10944156ns  stddev=26952.3ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 130.88,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 46022,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 21724.33,
            "unit": "ns/insn",
            "extra": "median=10862163ns  p95=10882641ns  stddev=7831.9ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 126.36,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 46031,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 21525.79,
            "unit": "ns/insn",
            "extra": "median=10762896ns  p95=10787613ns  stddev=12753.8ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -72.18,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 46456,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 21644.48,
            "unit": "ns/insn",
            "extra": "median=10822238ns  p95=10985824ns  stddev=49739.7ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 46.51,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 46201,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 16622.99,
            "unit": "ns/insn",
            "extra": "median=8311496ns  p95=8372069ns  stddev=23428.6ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -4974.98,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 60158,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 16616.42,
            "unit": "ns/insn",
            "extra": "median=8308210ns  p95=8347794ns  stddev=13529.8ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -4981.55,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 60181,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 21705.41,
            "unit": "ns/insn",
            "extra": "median=10852706ns  p95=10967119ns  stddev=33642.9ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 107.44,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 46071,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 21640.57,
            "unit": "ns/insn",
            "extra": "median=10820284ns  p95=10996625ns  stddev=51636.7ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 42.6,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 46210,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 21635.42,
            "unit": "ns/insn",
            "extra": "median=10817709ns  p95=10854308ns  stddev=12301.2ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 37.45,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 46221,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 21630.67,
            "unit": "ns/insn",
            "extra": "median=10815335ns  p95=10881859ns  stddev=21018.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 32.7,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 46231,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 21631.47,
            "unit": "ns/insn",
            "extra": "median=10815736ns  p95=10863395ns  stddev=18533.5ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 33.5,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 46229,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 21637.66,
            "unit": "ns/insn",
            "extra": "median=10818831ns  p95=10886068ns  stddev=25856.4ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 39.69,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 46216,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 21644.7,
            "unit": "ns/insn",
            "extra": "median=10822349ns  p95=10895094ns  stddev=24449.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 46.73,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 46201,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 21644.92,
            "unit": "ns/insn",
            "extra": "median=10822459ns  p95=10882381ns  stddev=21932.1ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 46.95,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 46200,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 21621.73,
            "unit": "ns/insn",
            "extra": "median=10810867ns  p95=10846854ns  stddev=12165.8ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 23.76,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 46250,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 21724.47,
            "unit": "ns/insn",
            "extra": "median=10862233ns  p95=10921384ns  stddev=21855.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 126.5,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 46031,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 21719.89,
            "unit": "ns/insn",
            "extra": "median=10859944ns  p95=10937510ns  stddev=30965.8ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 121.92,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 46041,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 21718.73,
            "unit": "ns/insn",
            "extra": "median=10859364ns  p95=10926109ns  stddev=21678.6ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 120.76,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 46043,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 21541.53,
            "unit": "ns/insn",
            "extra": "median=10770767ns  p95=10846230ns  stddev=27148.3ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -56.44,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 46422,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 21727.6,
            "unit": "ns/insn",
            "extra": "median=10863802ns  p95=10878600ns  stddev=7627.6ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 129.63,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 46024,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 21534.26,
            "unit": "ns/insn",
            "extra": "median=10767131ns  p95=10831893ns  stddev=21102.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -63.71,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46438,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 21529.53,
            "unit": "ns/insn",
            "extra": "median=10764767ns  p95=10769546ns  stddev=6004.0ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -68.44,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 46448,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 21529.23,
            "unit": "ns/insn",
            "extra": "median=10764616ns  p95=10867839ns  stddev=35543.4ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -68.74,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 46448,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 21523.92,
            "unit": "ns/insn",
            "extra": "median=10761961ns  p95=10777009ns  stddev=6295.9ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -74.05,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 46460,
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
        "date": 1775222315079,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 21577.61,
            "unit": "ns/insn",
            "extra": "median=10788806ns  p95=12980382ns  stddev=653169.4ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46344,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 21824.13,
            "unit": "ns/insn",
            "extra": "median=10912064ns  p95=11152041ns  stddev=92500.7ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 246.52,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 45821,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 21777.28,
            "unit": "ns/insn",
            "extra": "median=10888641ns  p95=11070209ns  stddev=62121.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 199.67,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 45919,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 21739.41,
            "unit": "ns/insn",
            "extra": "median=10869706ns  p95=10974731ns  stddev=35018.4ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 161.8,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 45999,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 21712.98,
            "unit": "ns/insn",
            "extra": "median=10856492ns  p95=11154185ns  stddev=106150.2ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 135.37,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 46055,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 70880.13,
            "unit": "ns/insn",
            "extra": "median=35440064ns  p95=35595292ns  stddev=61012.0ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 49302.52,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 14108,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 21630.23,
            "unit": "ns/insn",
            "extra": "median=10815115ns  p95=10886597ns  stddev=30761.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 52.62,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 46232,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 21606.89,
            "unit": "ns/insn",
            "extra": "median=10803443ns  p95=11082823ns  stddev=83532.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 29.27,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 46282,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 21613.52,
            "unit": "ns/insn",
            "extra": "median=10806759ns  p95=10828189ns  stddev=10318.6ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 35.91,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 46267,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 21742.56,
            "unit": "ns/insn",
            "extra": "median=10871279ns  p95=10933825ns  stddev=24174.3ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 164.95,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 45993,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 21758.59,
            "unit": "ns/insn",
            "extra": "median=10879294ns  p95=10920331ns  stddev=17088.1ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 180.98,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 45959,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 21746.81,
            "unit": "ns/insn",
            "extra": "median=10873403ns  p95=11026628ns  stddev=44783.2ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 169.19,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 45984,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 21746.82,
            "unit": "ns/insn",
            "extra": "median=10873412ns  p95=11049270ns  stddev=52696.7ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 169.21,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 45984,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 21752.26,
            "unit": "ns/insn",
            "extra": "median=10876128ns  p95=11079073ns  stddev=59906.4ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 174.64,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 45972,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 21758.39,
            "unit": "ns/insn",
            "extra": "median=10879194ns  p95=10932003ns  stddev=24000.5ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 180.78,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 45959,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 21641.25,
            "unit": "ns/insn",
            "extra": "median=10820625ns  p95=10946509ns  stddev=48548.2ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 63.64,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 46208,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 21783.41,
            "unit": "ns/insn",
            "extra": "median=10891707ns  p95=10907426ns  stddev=11363.0ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 205.8,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 45906,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 21756.46,
            "unit": "ns/insn",
            "extra": "median=10878232ns  p95=10917014ns  stddev=18384.9ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 178.85,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 45963,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 21748.77,
            "unit": "ns/insn",
            "extra": "median=10874385ns  p95=10933675ns  stddev=25519.8ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 171.16,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 45980,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 21766.9,
            "unit": "ns/insn",
            "extra": "median=10883452ns  p95=11418577ns  stddev=166147.4ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 189.29,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 45941,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 21632.75,
            "unit": "ns/insn",
            "extra": "median=10816377ns  p95=10891827ns  stddev=23001.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 55.14,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 46226,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 21750.77,
            "unit": "ns/insn",
            "extra": "median=10875387ns  p95=10943223ns  stddev=24753.0ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 173.16,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 45975,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 21749.75,
            "unit": "ns/insn",
            "extra": "median=10874877ns  p95=10893209ns  stddev=10363.7ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 172.14,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 45978,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 21759.55,
            "unit": "ns/insn",
            "extra": "median=10879775ns  p95=10900484ns  stddev=12256.2ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 181.94,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 45957,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 21760.77,
            "unit": "ns/insn",
            "extra": "median=10880386ns  p95=10890065ns  stddev=8422.1ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 183.16,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 45954,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 21772.85,
            "unit": "ns/insn",
            "extra": "median=10886427ns  p95=11061934ns  stddev=52661.8ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 195.24,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 45929,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 21747.21,
            "unit": "ns/insn",
            "extra": "median=10873603ns  p95=10996242ns  stddev=48315.6ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 169.59,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 45983,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 21717.31,
            "unit": "ns/insn",
            "extra": "median=10858656ns  p95=10891908ns  stddev=12688.2ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 139.7,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 46046,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 21548.38,
            "unit": "ns/insn",
            "extra": "median=10774189ns  p95=10815105ns  stddev=21154.3ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -29.23,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 46407,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 21631.15,
            "unit": "ns/insn",
            "extra": "median=10815576ns  p95=10868264ns  stddev=17275.0ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 53.54,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 46230,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 16556.45,
            "unit": "ns/insn",
            "extra": "median=8278226ns  p95=8308843ns  stddev=15138.3ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -5021.16,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 60399,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 16552.87,
            "unit": "ns/insn",
            "extra": "median=8276433ns  p95=8320365ns  stddev=16133.6ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -5024.75,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 60412,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 21719.1,
            "unit": "ns/insn",
            "extra": "median=10859548ns  p95=12174673ns  stddev=436088.2ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 141.48,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 46042,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 21661.89,
            "unit": "ns/insn",
            "extra": "median=10830944ns  p95=10945216ns  stddev=42658.6ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 84.28,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 46164,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 21626.06,
            "unit": "ns/insn",
            "extra": "median=10813031ns  p95=10868975ns  stddev=19785.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 48.45,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 46241,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 21637.56,
            "unit": "ns/insn",
            "extra": "median=10818781ns  p95=10827147ns  stddev=7726.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 59.95,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 46216,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 21633.48,
            "unit": "ns/insn",
            "extra": "median=10816738ns  p95=10871910ns  stddev=26087.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 55.86,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 46225,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 21622.31,
            "unit": "ns/insn",
            "extra": "median=10811157ns  p95=10825464ns  stddev=8035.9ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 44.7,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 46249,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 21615.96,
            "unit": "ns/insn",
            "extra": "median=10807982ns  p95=10849699ns  stddev=15070.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 38.35,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 46262,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 21620.89,
            "unit": "ns/insn",
            "extra": "median=10810447ns  p95=10848658ns  stddev=14706.5ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 43.28,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 46252,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 21644.34,
            "unit": "ns/insn",
            "extra": "median=10822169ns  p95=10961588ns  stddev=43442.9ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 66.73,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 46201,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 21715.23,
            "unit": "ns/insn",
            "extra": "median=10857614ns  p95=10951178ns  stddev=27561.3ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 137.62,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 46051,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 21733.6,
            "unit": "ns/insn",
            "extra": "median=10866802ns  p95=10887680ns  stddev=13149.0ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 155.99,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 46012,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 21735.47,
            "unit": "ns/insn",
            "extra": "median=10867733ns  p95=10910172ns  stddev=21494.2ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 157.85,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 46008,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 21524.17,
            "unit": "ns/insn",
            "extra": "median=10762086ns  p95=10779930ns  stddev=8548.8ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -53.44,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 46459,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 21713.05,
            "unit": "ns/insn",
            "extra": "median=10856523ns  p95=10926532ns  stddev=25223.8ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 135.43,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 46055,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 21531.99,
            "unit": "ns/insn",
            "extra": "median=10765994ns  p95=10806600ns  stddev=15866.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -45.62,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46443,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 21514.92,
            "unit": "ns/insn",
            "extra": "median=10757458ns  p95=10973460ns  stddev=61712.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -62.7,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 46479,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 21532.93,
            "unit": "ns/insn",
            "extra": "median=10766465ns  p95=10796551ns  stddev=11620.8ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -44.68,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 46440,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 21531.55,
            "unit": "ns/insn",
            "extra": "median=10765773ns  p95=10799457ns  stddev=14332.5ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -46.07,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 46443,
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
          "distinct": false,
          "id": "652c190e41423870c56a48509017777f2cc0834c",
          "message": "test: add Doc 17/19 cryptographic property tests (15 tests)\n\nGroup A — Doc 17 enc_state entanglement (5 tests):\n  - Flags/reg_pack field differences drive ratchet divergence\n  - Old 6-byte ratchet was blind to flags; new 8-byte is not\n  - 1-bit tamper in encrypted blob cascades into MAC failure\n  - Phase G key ratchet includes full 8-byte instruction\n\nGroup B — Doc 19 Theorem 4.1 isomorphism (4 tests):\n  - N x step() == 1 x dispatch_unit() for HighSec/Standard/Debug\n  - Multi-DU cumulative isomorphism (8 x step == 2 x DU)\n\nGroup C — Doc 19 chaff welding (3 tests):\n  - Removing chaff NOP breaks enc_state chain\n  - Different CSPRNG aux produces different enc_state\n  - Adding chaff lengthens one-way composition chain\n\nGroup D — Edge cases (3 tests):\n  - HALT mid-dispatch-unit returns correct value\n  - Branch at last DU slot triggers Phase L transition\n  - Enhanced NOP ghost ALU writes to trash_regs",
          "timestamp": "2026-04-03T21:15:55+08:00",
          "tree_id": "109f6ff21177393cdac1562c6824cd189d976369",
          "url": "https://github.com/scc-tw/VMPilot/commit/652c190e41423870c56a48509017777f2cc0834c"
        },
        "date": 1775222423628,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 21570.87,
            "unit": "ns/insn",
            "extra": "median=10785437ns  p95=10805074ns  stddev=7982.4ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46359,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 21929.26,
            "unit": "ns/insn",
            "extra": "median=10964632ns  p95=14489650ns  stddev=1093404.1ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 358.39,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 45601,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 21949,
            "unit": "ns/insn",
            "extra": "median=10974500ns  p95=11403099ns  stddev=183784.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 378.13,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 45560,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 21838.09,
            "unit": "ns/insn",
            "extra": "median=10919046ns  p95=13582417ns  stddev=776499.9ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 267.22,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 45792,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 21641.11,
            "unit": "ns/insn",
            "extra": "median=10820553ns  p95=10916211ns  stddev=30075.3ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 70.23,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 46208,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 70882.95,
            "unit": "ns/insn",
            "extra": "median=35441473ns  p95=35564902ns  stddev=44594.7ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 49312.07,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 14108,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 21617.98,
            "unit": "ns/insn",
            "extra": "median=10808992ns  p95=10929847ns  stddev=40157.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 47.11,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 46258,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 21619.79,
            "unit": "ns/insn",
            "extra": "median=10809894ns  p95=10873151ns  stddev=25428.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 48.91,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 46254,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 21626.22,
            "unit": "ns/insn",
            "extra": "median=10813109ns  p95=11056763ns  stddev=70885.0ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 55.34,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 46240,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 21769.14,
            "unit": "ns/insn",
            "extra": "median=10884572ns  p95=11027979ns  stddev=47223.6ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 198.27,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 45937,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 21773.07,
            "unit": "ns/insn",
            "extra": "median=10886535ns  p95=10991331ns  stddev=33440.4ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 202.2,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 45928,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 21758.32,
            "unit": "ns/insn",
            "extra": "median=10879162ns  p95=10972325ns  stddev=30149.2ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 187.45,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 45959,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 21742.85,
            "unit": "ns/insn",
            "extra": "median=10871427ns  p95=10908406ns  stddev=14426.1ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 171.98,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 45992,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 21768.04,
            "unit": "ns/insn",
            "extra": "median=10884021ns  p95=11007571ns  stddev=36442.5ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 197.17,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 45939,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 21769.36,
            "unit": "ns/insn",
            "extra": "median=10884681ns  p95=10915109ns  stddev=16519.8ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 198.49,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 45936,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 21651.52,
            "unit": "ns/insn",
            "extra": "median=10825762ns  p95=10846040ns  stddev=11896.8ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 80.65,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 46186,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 21732.15,
            "unit": "ns/insn",
            "extra": "median=10866077ns  p95=10947549ns  stddev=26196.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 161.28,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 46015,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 21769.22,
            "unit": "ns/insn",
            "extra": "median=10884612ns  p95=11025374ns  stddev=43294.7ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 198.35,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 45936,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 21767.78,
            "unit": "ns/insn",
            "extra": "median=10883891ns  p95=10929996ns  stddev=18496.1ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 196.91,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 45939,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 21758.64,
            "unit": "ns/insn",
            "extra": "median=10879322ns  p95=10911011ns  stddev=12616.1ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 187.77,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 45959,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 21699.97,
            "unit": "ns/insn",
            "extra": "median=10849987ns  p95=11303713ns  stddev=135556.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 129.1,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 46083,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 21771.07,
            "unit": "ns/insn",
            "extra": "median=10885533ns  p95=10895742ns  stddev=6718.9ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 200.19,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 45933,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 21744.78,
            "unit": "ns/insn",
            "extra": "median=10872389ns  p95=10922773ns  stddev=18442.6ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 173.9,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 45988,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 21747,
            "unit": "ns/insn",
            "extra": "median=10873501ns  p95=10976753ns  stddev=33396.5ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 176.13,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 45983,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 21762.33,
            "unit": "ns/insn",
            "extra": "median=10881165ns  p95=10949483ns  stddev=30346.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 191.46,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 45951,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 21760.71,
            "unit": "ns/insn",
            "extra": "median=10880354ns  p95=10925167ns  stddev=19905.0ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 189.83,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 45954,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 21713.56,
            "unit": "ns/insn",
            "extra": "median=10856780ns  p95=10872118ns  stddev=8171.1ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 142.69,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 46054,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 21738.33,
            "unit": "ns/insn",
            "extra": "median=10869163ns  p95=10973106ns  stddev=33572.1ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 167.45,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 46002,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 21542.26,
            "unit": "ns/insn",
            "extra": "median=10771131ns  p95=10972124ns  stddev=61373.5ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -28.61,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 46420,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 21657.64,
            "unit": "ns/insn",
            "extra": "median=10828818ns  p95=10909358ns  stddev=25525.4ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 86.76,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 46173,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 16539.76,
            "unit": "ns/insn",
            "extra": "median=8269880ns  p95=8293634ns  stddev=11150.4ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -5031.11,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 60460,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 16542.36,
            "unit": "ns/insn",
            "extra": "median=8271182ns  p95=8372441ns  stddev=38432.4ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -5028.51,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 60451,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 21738.31,
            "unit": "ns/insn",
            "extra": "median=10869153ns  p95=10927962ns  stddev=18919.8ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 167.43,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 46002,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 21661.6,
            "unit": "ns/insn",
            "extra": "median=10830801ns  p95=10948360ns  stddev=33937.3ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 90.73,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 46165,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 21657.98,
            "unit": "ns/insn",
            "extra": "median=10828988ns  p95=10869163ns  stddev=14477.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 87.1,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 46172,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 21659.94,
            "unit": "ns/insn",
            "extra": "median=10829970ns  p95=10839898ns  stddev=6968.5ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 89.07,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 46168,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 21694.58,
            "unit": "ns/insn",
            "extra": "median=10847292ns  p95=10888780ns  stddev=20599.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 123.71,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 46094,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 21669.94,
            "unit": "ns/insn",
            "extra": "median=10834969ns  p95=10860577ns  stddev=13339.8ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 99.06,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 46147,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 21675.27,
            "unit": "ns/insn",
            "extra": "median=10837635ns  p95=10932471ns  stddev=30931.5ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 104.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 46136,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 21664.31,
            "unit": "ns/insn",
            "extra": "median=10832155ns  p95=10851330ns  stddev=9300.5ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 93.44,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 46159,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 21676.89,
            "unit": "ns/insn",
            "extra": "median=10838446ns  p95=10886815ns  stddev=20424.6ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 106.02,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 46132,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 21753.17,
            "unit": "ns/insn",
            "extra": "median=10876587ns  p95=10915249ns  stddev=11784.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 182.3,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 45970,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 21743.45,
            "unit": "ns/insn",
            "extra": "median=10871727ns  p95=10891835ns  stddev=7389.3ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 172.58,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 45991,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 21794.67,
            "unit": "ns/insn",
            "extra": "median=10897335ns  p95=12191707ns  stddev=416098.6ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 223.8,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 45883,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 21558.39,
            "unit": "ns/insn",
            "extra": "median=10779195ns  p95=10791167ns  stddev=7732.1ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -12.48,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 46386,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 21794.21,
            "unit": "ns/insn",
            "extra": "median=10897105ns  p95=10990809ns  stddev=38338.8ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 223.34,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 45884,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 21561.9,
            "unit": "ns/insn",
            "extra": "median=10780948ns  p95=10820102ns  stddev=15273.9ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -8.98,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 46378,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 21570.07,
            "unit": "ns/insn",
            "extra": "median=10785036ns  p95=10854906ns  stddev=22968.5ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -0.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 46361,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 21566.39,
            "unit": "ns/insn",
            "extra": "median=10783193ns  p95=10814130ns  stddev=12840.1ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -4.49,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 46368,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 21552.68,
            "unit": "ns/insn",
            "extra": "median=10776340ns  p95=10870506ns  stddev=29557.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -18.19,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 46398,
            "unit": "insn/s"
          }
        ]
      }
    ]
  }
}