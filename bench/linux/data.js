window.BENCHMARK_DATA = {
  "lastUpdate": 1775233508931,
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
          "id": "8d5bd09e091ab1924c48c2c418232c2a3fa6cc9c",
          "message": "fix: update benchmark scripts for DU-based metrics\n\nBenchmark output changed from per-instruction to per-dispatch-unit:\n  ns_per_insn → ns_per_du, handler_ns → delta_ns,\n  insn_count → du_count, ips → du_per_sec,\n  --insns → --dus\n\nScripts accept both old and new field names for backward compatibility.",
          "timestamp": "2026-04-04T00:23:51+08:00",
          "tree_id": "2eac5612a2d2d8e35a4e51976b038887ad38627d",
          "url": "https://github.com/scc-tw/VMPilot/commit/8d5bd09e091ab1924c48c2c418232c2a3fa6cc9c"
        },
        "date": 1775233484270,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5505.01,
            "unit": "ns/DU",
            "extra": "median=688126ns  p95=707321ns  stddev=10221.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 181653,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5603.02,
            "unit": "ns/DU",
            "extra": "median=700378ns  p95=703534ns  stddev=4414.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 98.02,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 178475,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5579.22,
            "unit": "ns/DU",
            "extra": "median=697402ns  p95=708293ns  stddev=5573.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 74.21,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 179237,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5559.18,
            "unit": "ns/DU",
            "extra": "median=694897ns  p95=700037ns  stddev=5610.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 54.17,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 179883,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5557.82,
            "unit": "ns/DU",
            "extra": "median=694727ns  p95=700919ns  stddev=4858.5ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 52.81,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 179927,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5584.9,
            "unit": "ns/DU",
            "extra": "median=698113ns  p95=702111ns  stddev=3841.6ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 79.9,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 179054,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5554.13,
            "unit": "ns/DU",
            "extra": "median=694266ns  p95=697142ns  stddev=4529.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 49.12,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 180046,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5579.94,
            "unit": "ns/DU",
            "extra": "median=697493ns  p95=702943ns  stddev=5247.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 74.94,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 179213,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5545.15,
            "unit": "ns/DU",
            "extra": "median=693144ns  p95=705929ns  stddev=6283.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 40.14,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 180338,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5608.56,
            "unit": "ns/DU",
            "extra": "median=701070ns  p95=704997ns  stddev=5061.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 103.55,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 178299,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5559.98,
            "unit": "ns/DU",
            "extra": "median=694998ns  p95=698855ns  stddev=4325.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 54.98,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 179857,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5599.98,
            "unit": "ns/DU",
            "extra": "median=699998ns  p95=703634ns  stddev=4584.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 94.98,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 178572,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5574.26,
            "unit": "ns/DU",
            "extra": "median=696782ns  p95=700749ns  stddev=3583.8ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 69.25,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 179396,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5587.23,
            "unit": "ns/DU",
            "extra": "median=698404ns  p95=704646ns  stddev=4466.9ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 82.22,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 178980,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5576.09,
            "unit": "ns/DU",
            "extra": "median=697011ns  p95=700999ns  stddev=4663.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 71.08,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 179337,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5580.74,
            "unit": "ns/DU",
            "extra": "median=697593ns  p95=816487ns  stddev=35060.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 75.74,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 179188,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5607.91,
            "unit": "ns/DU",
            "extra": "median=700989ns  p95=706890ns  stddev=6218.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 102.9,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 178319,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5590.53,
            "unit": "ns/DU",
            "extra": "median=698816ns  p95=716198ns  stddev=6949.4ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 85.52,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 178874,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5587.79,
            "unit": "ns/DU",
            "extra": "median=698474ns  p95=702822ns  stddev=5346.7ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 82.78,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 178962,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5585.55,
            "unit": "ns/DU",
            "extra": "median=698194ns  p95=703514ns  stddev=4165.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 80.54,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 179033,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5546.44,
            "unit": "ns/DU",
            "extra": "median=693305ns  p95=698043ns  stddev=4483.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 41.43,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 180296,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5599.5,
            "unit": "ns/DU",
            "extra": "median=699938ns  p95=713953ns  stddev=6401.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 94.5,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 178587,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5602.54,
            "unit": "ns/DU",
            "extra": "median=700318ns  p95=706740ns  stddev=4272.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 97.54,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 178490,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5602.54,
            "unit": "ns/DU",
            "extra": "median=700318ns  p95=729993ns  stddev=12607.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 97.54,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 178490,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5597.49,
            "unit": "ns/DU",
            "extra": "median=699686ns  p95=703895ns  stddev=5021.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 92.48,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 178652,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5598.38,
            "unit": "ns/DU",
            "extra": "median=699797ns  p95=702563ns  stddev=4060.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 93.37,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 178623,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5568.63,
            "unit": "ns/DU",
            "extra": "median=696079ns  p95=701319ns  stddev=5584.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 63.62,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 179577,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5579.06,
            "unit": "ns/DU",
            "extra": "median=697383ns  p95=715327ns  stddev=6601.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 74.06,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 179242,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5529.69,
            "unit": "ns/DU",
            "extra": "median=691211ns  p95=704976ns  stddev=5844.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 24.68,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 180842,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5570.41,
            "unit": "ns/DU",
            "extra": "median=696301ns  p95=698795ns  stddev=3177.8ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 65.4,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 179520,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5497.7,
            "unit": "ns/DU",
            "extra": "median=687213ns  p95=689999ns  stddev=4679.1ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -7.3,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 181894,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5504.93,
            "unit": "ns/DU",
            "extra": "median=688116ns  p95=691100ns  stddev=4401.1ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -0.08,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 181655,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5639.1,
            "unit": "ns/DU",
            "extra": "median=704887ns  p95=708453ns  stddev=4904.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 134.09,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 177333,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5579.54,
            "unit": "ns/DU",
            "extra": "median=697442ns  p95=699396ns  stddev=4385.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 74.53,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 179226,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5566.63,
            "unit": "ns/DU",
            "extra": "median=695829ns  p95=702342ns  stddev=4446.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 61.62,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 179642,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5578.42,
            "unit": "ns/DU",
            "extra": "median=697302ns  p95=712010ns  stddev=6387.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 73.41,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 179262,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5574.33,
            "unit": "ns/DU",
            "extra": "median=696791ns  p95=705437ns  stddev=4863.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 69.32,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 179394,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5581.38,
            "unit": "ns/DU",
            "extra": "median=697673ns  p95=703814ns  stddev=4167.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 76.38,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 179167,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5581.62,
            "unit": "ns/DU",
            "extra": "median=697702ns  p95=701249ns  stddev=5365.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 76.61,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 179160,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5602.86,
            "unit": "ns/DU",
            "extra": "median=700358ns  p95=729633ns  stddev=12964.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 97.86,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 178480,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5525.92,
            "unit": "ns/DU",
            "extra": "median=690740ns  p95=699676ns  stddev=4566.2ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 20.91,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 180965,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5565.03,
            "unit": "ns/DU",
            "extra": "median=695629ns  p95=701881ns  stddev=4562.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 60.02,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 179693,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5587.23,
            "unit": "ns/DU",
            "extra": "median=698404ns  p95=715197ns  stddev=7223.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 82.22,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 178980,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5568.32,
            "unit": "ns/DU",
            "extra": "median=696040ns  p95=697844ns  stddev=4139.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 63.31,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 179587,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5500.43,
            "unit": "ns/DU",
            "extra": "median=687554ns  p95=705417ns  stddev=7799.9ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -4.58,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 181804,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5602.54,
            "unit": "ns/DU",
            "extra": "median=700318ns  p95=702934ns  stddev=5071.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 97.54,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 178490,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5518.62,
            "unit": "ns/DU",
            "extra": "median=689828ns  p95=692363ns  stddev=3653.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 13.62,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 181205,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5521.11,
            "unit": "ns/DU",
            "extra": "median=690139ns  p95=693795ns  stddev=5384.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 16.1,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 181123,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5536.98,
            "unit": "ns/DU",
            "extra": "median=692122ns  p95=695710ns  stddev=4609.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 31.97,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 180604,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5545.88,
            "unit": "ns/DU",
            "extra": "median=693235ns  p95=695659ns  stddev=4267.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 40.87,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 180314,
            "unit": "DU/s"
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
          "id": "8d5bd09e091ab1924c48c2c418232c2a3fa6cc9c",
          "message": "fix: update benchmark scripts for DU-based metrics\n\nBenchmark output changed from per-instruction to per-dispatch-unit:\n  ns_per_insn → ns_per_du, handler_ns → delta_ns,\n  insn_count → du_count, ips → du_per_sec,\n  --insns → --dus\n\nScripts accept both old and new field names for backward compatibility.",
          "timestamp": "2026-04-04T00:23:51+08:00",
          "tree_id": "2eac5612a2d2d8e35a4e51976b038887ad38627d",
          "url": "https://github.com/scc-tw/VMPilot/commit/8d5bd09e091ab1924c48c2c418232c2a3fa6cc9c"
        },
        "date": 1775233508438,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5530.68,
            "unit": "ns/DU",
            "extra": "median=691335ns  p95=707555ns  stddev=10690.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 180810,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5639.68,
            "unit": "ns/DU",
            "extra": "median=704960ns  p95=712965ns  stddev=8735.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 109,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 177315,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5516.09,
            "unit": "ns/DU",
            "extra": "median=689511ns  p95=693229ns  stddev=4591.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -14.59,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 181288,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5528.59,
            "unit": "ns/DU",
            "extra": "median=691074ns  p95=701794ns  stddev=5293.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -2.09,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 180878,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5501.18,
            "unit": "ns/DU",
            "extra": "median=687647ns  p95=694992ns  stddev=4693.4ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -29.5,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 181779,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5590.23,
            "unit": "ns/DU",
            "extra": "median=698779ns  p95=709479ns  stddev=5504.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 59.55,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 178883,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5497.42,
            "unit": "ns/DU",
            "extra": "median=687177ns  p95=704540ns  stddev=8135.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -33.26,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 181904,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5521.46,
            "unit": "ns/DU",
            "extra": "median=690183ns  p95=712564ns  stddev=10951.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -9.22,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 181111,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5554.32,
            "unit": "ns/DU",
            "extra": "median=694290ns  p95=715059ns  stddev=10459.6ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 23.64,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 180040,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5547.9,
            "unit": "ns/DU",
            "extra": "median=693488ns  p95=710861ns  stddev=7297.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 17.22,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 180248,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5549.43,
            "unit": "ns/DU",
            "extra": "median=693679ns  p95=696074ns  stddev=2374.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 18.75,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 180199,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5548.15,
            "unit": "ns/DU",
            "extra": "median=693519ns  p95=696735ns  stddev=5334.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 17.47,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 180240,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5562.34,
            "unit": "ns/DU",
            "extra": "median=695292ns  p95=697937ns  stddev=3759.7ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 31.66,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 179781,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5561.62,
            "unit": "ns/DU",
            "extra": "median=695203ns  p95=698758ns  stddev=5219.1ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 30.94,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 179804,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5557.05,
            "unit": "ns/DU",
            "extra": "median=694631ns  p95=698558ns  stddev=4463.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 26.37,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 179952,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 6747.1,
            "unit": "ns/DU",
            "extra": "median=843387ns  p95=1021097ns  stddev=51638.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 1216.42,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 148212,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5916.03,
            "unit": "ns/DU",
            "extra": "median=739504ns  p95=851823ns  stddev=49026.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 385.35,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 169032,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5728.33,
            "unit": "ns/DU",
            "extra": "median=716041ns  p95=833430ns  stddev=55668.4ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 197.65,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 174571,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5545.98,
            "unit": "ns/DU",
            "extra": "median=693248ns  p95=698048ns  stddev=4798.3ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 15.3,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 180311,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5556.81,
            "unit": "ns/DU",
            "extra": "median=694601ns  p95=698819ns  stddev=4792.9ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 26.13,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 179959,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5555.04,
            "unit": "ns/DU",
            "extra": "median=694380ns  p95=794417ns  stddev=32187.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 24.36,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 180017,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5554.65,
            "unit": "ns/DU",
            "extra": "median=694331ns  p95=699611ns  stddev=4953.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 23.97,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 180029,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5548.95,
            "unit": "ns/DU",
            "extra": "median=693619ns  p95=698208ns  stddev=3583.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 18.27,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 180214,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5562.5,
            "unit": "ns/DU",
            "extra": "median=695312ns  p95=731459ns  stddev=14955.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 31.82,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 179775,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5554.56,
            "unit": "ns/DU",
            "extra": "median=694320ns  p95=695502ns  stddev=5082.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 23.88,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 180032,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5581.98,
            "unit": "ns/DU",
            "extra": "median=697747ns  p95=702335ns  stddev=5172.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 51.3,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 179148,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5528.51,
            "unit": "ns/DU",
            "extra": "median=691064ns  p95=696624ns  stddev=4845.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -2.17,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 180880,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5538.69,
            "unit": "ns/DU",
            "extra": "median=692336ns  p95=696505ns  stddev=4967.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 8.01,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 180548,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5499.66,
            "unit": "ns/DU",
            "extra": "median=687458ns  p95=719717ns  stddev=10449.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -31.02,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 181829,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5529.15,
            "unit": "ns/DU",
            "extra": "median=691144ns  p95=694241ns  stddev=4961.0ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -1.53,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 180860,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5494.77,
            "unit": "ns/DU",
            "extra": "median=686846ns  p95=697677ns  stddev=6242.0ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -35.91,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 181991,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5499.1,
            "unit": "ns/DU",
            "extra": "median=687387ns  p95=706573ns  stddev=10215.9ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -31.58,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 181848,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5629.02,
            "unit": "ns/DU",
            "extra": "median=703628ns  p95=719507ns  stddev=6637.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 98.34,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 177651,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5542.46,
            "unit": "ns/DU",
            "extra": "median=692807ns  p95=694491ns  stddev=4273.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 11.78,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 180425,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5523.14,
            "unit": "ns/DU",
            "extra": "median=690393ns  p95=696675ns  stddev=4913.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -7.54,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 181056,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5533.41,
            "unit": "ns/DU",
            "extra": "median=691676ns  p95=701132ns  stddev=5777.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 2.73,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 180720,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5548.38,
            "unit": "ns/DU",
            "extra": "median=693548ns  p95=696985ns  stddev=4262.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 17.7,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 180233,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5513.68,
            "unit": "ns/DU",
            "extra": "median=689210ns  p95=695713ns  stddev=3629.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -17,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 181367,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5608.5,
            "unit": "ns/DU",
            "extra": "median=701062ns  p95=760503ns  stddev=19987.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 77.82,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 178301,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5549.27,
            "unit": "ns/DU",
            "extra": "median=693659ns  p95=705992ns  stddev=6455.6ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 18.59,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 180204,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5533.88,
            "unit": "ns/DU",
            "extra": "median=691735ns  p95=693870ns  stddev=4554.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 180705,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5503.43,
            "unit": "ns/DU",
            "extra": "median=687929ns  p95=692928ns  stddev=4807.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -27.25,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 181705,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5531.08,
            "unit": "ns/DU",
            "extra": "median=691385ns  p95=920000ns  stddev=66023.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 0.4,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 180797,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5592.63,
            "unit": "ns/DU",
            "extra": "median=699079ns  p95=928846ns  stddev=91692.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 61.95,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 178807,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5552.39,
            "unit": "ns/DU",
            "extra": "median=694049ns  p95=750645ns  stddev=22497.7ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 21.71,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 180103,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5535.17,
            "unit": "ns/DU",
            "extra": "median=691896ns  p95=697045ns  stddev=5612.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 4.49,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 180663,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5483.07,
            "unit": "ns/DU",
            "extra": "median=685384ns  p95=689261ns  stddev=4411.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -47.61,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 182380,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5471.77,
            "unit": "ns/DU",
            "extra": "median=683971ns  p95=715169ns  stddev=10646.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -58.91,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 182756,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5474.5,
            "unit": "ns/DU",
            "extra": "median=684312ns  p95=689381ns  stddev=5790.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -56.18,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 182665,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5506.07,
            "unit": "ns/DU",
            "extra": "median=688259ns  p95=691355ns  stddev=4426.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -24.61,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 181618,
            "unit": "DU/s"
          }
        ]
      }
    ]
  }
}