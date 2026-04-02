window.BENCHMARK_DATA = {
  "lastUpdate": 1775162458783,
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
      }
    ]
  }
}