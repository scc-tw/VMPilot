window.BENCHMARK_DATA = {
  "lastUpdate": 1775453939558,
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
          "id": "6f82a7b47a18125cfda56e2c01dfae66cb56c8d7",
          "message": "refactor: unify step()/dispatch_unit() into single execute_one_instruction codepath\n\nMajor changes:\n\n1. extract execute_one_instruction<Policy, Oram>() (Phase A-K)\n   - Single codepath for all instruction execution — no duplicated pipeline\n   - step() now delegates to dispatch_unit() (was separate 180-line impl)\n   - Removes old step()'s branching Phase L (timing side-channel leak)\n\n2. Always-constant-time handlers (remove if-constexpr Policy branches)\n   - DIV/IDIV/MOD: always use ct_div/ct_idiv/ct_mod\n   - CMP/TEST: always use ct_cmp_flags / branchless flag computation\n   - JCC: always use evaluate_condition_ct + branchless MUX\n   - NOP: always run ghost ALU (no DebugPolicy shortcut)\n   - CMPXCHG: always use arithmetic bool → uint8_t\n   - Remove evaluate_condition() (branching variant) entirely\n\n3. Branchless Phase E (Doc 19 §4.2 Fix #3)\n   - Always compute FPE_Encode; select result via bitmask MUX\n   - Prevents EM/DPA leaking whether opcode writes a register\n\n4. Move opcode_writes_reg() to common/vm_opcode.hpp (shared)\n\n5. Consolidate ORAM API to unified access() only\n   - Remove read()/write()/dummy_scan() from both RollingKeyOram/DirectOram\n   - DirectOram::access: fix heap-buffer-overflow on PUSH with vm_sp=0\n\n6. Extract ExecSnapshot/VmEpoch::branchless_select to vm_state.hpp\n   - Replace inline 50-line MUX with reusable snapshot/restore types\n\n7. Update isomorphism tests: step() == dispatch_unit() (same function)",
          "timestamp": "2026-04-04T00:51:02+08:00",
          "tree_id": "ad76d570cceb1a25ee82b4507f068519ce124474",
          "url": "https://github.com/scc-tw/VMPilot/commit/6f82a7b47a18125cfda56e2c01dfae66cb56c8d7"
        },
        "date": 1775235127302,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 4167.97,
            "unit": "ns/DU",
            "extra": "median=520996ns  p95=543288ns  stddev=8920.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 239925,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 4176.65,
            "unit": "ns/DU",
            "extra": "median=522081ns  p95=532519ns  stddev=4301.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 8.68,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 239426,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 4160.9,
            "unit": "ns/DU",
            "extra": "median=520113ns  p95=524655ns  stddev=3078.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -7.06,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 240332,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 4143.32,
            "unit": "ns/DU",
            "extra": "median=517915ns  p95=529805ns  stddev=4136.9ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -24.65,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 241352,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 4174.46,
            "unit": "ns/DU",
            "extra": "median=521807ns  p95=524766ns  stddev=3317.4ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 6.49,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 239552,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 4181.31,
            "unit": "ns/DU",
            "extra": "median=522664ns  p95=545190ns  stddev=6915.4ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 13.34,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 239159,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 4142.42,
            "unit": "ns/DU",
            "extra": "median=517803ns  p95=526102ns  stddev=4285.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -25.54,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 241405,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 4172.76,
            "unit": "ns/DU",
            "extra": "median=521595ns  p95=527647ns  stddev=4218.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 4.79,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 239650,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 4164.26,
            "unit": "ns/DU",
            "extra": "median=520533ns  p95=523592ns  stddev=3384.9ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -3.7,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 240138,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 4155.82,
            "unit": "ns/DU",
            "extra": "median=519478ns  p95=524558ns  stddev=2348.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -12.14,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 240626,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 4159.87,
            "unit": "ns/DU",
            "extra": "median=519984ns  p95=525407ns  stddev=2575.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -8.1,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 240392,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 4183.42,
            "unit": "ns/DU",
            "extra": "median=522927ns  p95=526147ns  stddev=2694.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 15.45,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 239039,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 4193.27,
            "unit": "ns/DU",
            "extra": "median=524159ns  p95=525155ns  stddev=2622.3ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 25.3,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 238477,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 4193.65,
            "unit": "ns/DU",
            "extra": "median=524206ns  p95=527354ns  stddev=3376.6ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 25.68,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 238456,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 4193.57,
            "unit": "ns/DU",
            "extra": "median=524196ns  p95=530699ns  stddev=3599.3ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 25.6,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 238460,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 4147.21,
            "unit": "ns/DU",
            "extra": "median=518401ns  p95=522338ns  stddev=2665.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -20.76,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 241126,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 4195.91,
            "unit": "ns/DU",
            "extra": "median=524489ns  p95=525839ns  stddev=3077.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 27.94,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 238327,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 4187.02,
            "unit": "ns/DU",
            "extra": "median=523377ns  p95=526173ns  stddev=3113.1ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 19.05,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 238834,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 4188.14,
            "unit": "ns/DU",
            "extra": "median=523518ns  p95=525978ns  stddev=3053.4ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 20.18,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 238769,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 4219.78,
            "unit": "ns/DU",
            "extra": "median=527472ns  p95=629664ns  stddev=35123.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 51.81,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 236979,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 4172.65,
            "unit": "ns/DU",
            "extra": "median=521581ns  p95=527394ns  stddev=4070.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 4.68,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 239656,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 4200.45,
            "unit": "ns/DU",
            "extra": "median=525056ns  p95=527788ns  stddev=3882.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 32.48,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 238070,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 4186.95,
            "unit": "ns/DU",
            "extra": "median=523369ns  p95=533024ns  stddev=4697.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 18.98,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 238837,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 4157.41,
            "unit": "ns/DU",
            "extra": "median=519676ns  p95=526902ns  stddev=3025.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -10.56,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 240534,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 4157.42,
            "unit": "ns/DU",
            "extra": "median=519678ns  p95=526251ns  stddev=3302.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -10.54,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 240534,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 4204.75,
            "unit": "ns/DU",
            "extra": "median=525594ns  p95=664663ns  stddev=40496.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 36.78,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 237826,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 4194.11,
            "unit": "ns/DU",
            "extra": "median=524264ns  p95=534231ns  stddev=5477.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 26.14,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 238429,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 4189.13,
            "unit": "ns/DU",
            "extra": "median=523641ns  p95=527174ns  stddev=2983.6ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 21.16,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 238713,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 4142.07,
            "unit": "ns/DU",
            "extra": "median=517759ns  p95=524169ns  stddev=3868.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -25.9,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 241425,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 4165.8,
            "unit": "ns/DU",
            "extra": "median=520725ns  p95=522315ns  stddev=2994.9ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -2.17,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 240050,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 4107.61,
            "unit": "ns/DU",
            "extra": "median=513451ns  p95=523881ns  stddev=3973.2ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -60.36,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 243451,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 4160.1,
            "unit": "ns/DU",
            "extra": "median=520013ns  p95=598373ns  stddev=28828.6ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -7.86,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 240379,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 4254.64,
            "unit": "ns/DU",
            "extra": "median=531830ns  p95=544115ns  stddev=5768.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 86.67,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 235038,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 4139.82,
            "unit": "ns/DU",
            "extra": "median=517477ns  p95=525226ns  stddev=3804.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -28.15,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 241557,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 4174.54,
            "unit": "ns/DU",
            "extra": "median=521818ns  p95=532157ns  stddev=5035.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 6.58,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 239547,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 4146.99,
            "unit": "ns/DU",
            "extra": "median=518374ns  p95=527750ns  stddev=4172.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -20.98,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 241139,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 4167.31,
            "unit": "ns/DU",
            "extra": "median=520914ns  p95=668726ns  stddev=56885.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -0.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 239963,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 4182.58,
            "unit": "ns/DU",
            "extra": "median=522823ns  p95=595024ns  stddev=21666.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 14.62,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 239087,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 4144.07,
            "unit": "ns/DU",
            "extra": "median=518009ns  p95=523440ns  stddev=3300.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -23.9,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 241309,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 4167.44,
            "unit": "ns/DU",
            "extra": "median=520930ns  p95=524008ns  stddev=3075.7ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -0.53,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 239955,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 4133.22,
            "unit": "ns/DU",
            "extra": "median=516652ns  p95=524547ns  stddev=3027.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -34.75,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 241942,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 4175.92,
            "unit": "ns/DU",
            "extra": "median=521990ns  p95=624530ns  stddev=33852.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 7.95,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 239468,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 4138.63,
            "unit": "ns/DU",
            "extra": "median=517329ns  p95=524577ns  stddev=3543.6ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -29.34,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 241626,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 4148.74,
            "unit": "ns/DU",
            "extra": "median=518593ns  p95=524953ns  stddev=2930.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -19.22,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 241037,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 4109.1,
            "unit": "ns/DU",
            "extra": "median=513637ns  p95=520236ns  stddev=2972.8ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -58.87,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 243363,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 4134.8,
            "unit": "ns/DU",
            "extra": "median=516850ns  p95=524463ns  stddev=3447.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -33.17,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 241850,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 4107.68,
            "unit": "ns/DU",
            "extra": "median=513460ns  p95=520075ns  stddev=2839.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -60.29,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 243446,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 4142.06,
            "unit": "ns/DU",
            "extra": "median=517758ns  p95=656007ns  stddev=40318.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -25.9,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 241426,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 4179.06,
            "unit": "ns/DU",
            "extra": "median=522383ns  p95=531436ns  stddev=6771.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 11.1,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 239288,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 4136.99,
            "unit": "ns/DU",
            "extra": "median=517124ns  p95=519960ns  stddev=3017.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -30.98,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 241722,
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
          "distinct": true,
          "id": "c1856b036b1c5f3a92dd80376a1829f8f0d999f5",
          "message": "test: add security property coverage tests for Doc 15/16/17/19\n\nAudit of cryptographic properties from Doc 15-19 identified 8 gaps\nin test coverage.  New test_security_properties.cpp covers:\n\n- BB MAC rejection on instruction tamper and MAC bit-flip (Doc 15 §2.3)\n- CFG hijack rejection via enc_state corruption (Doc 17 §3.2)\n- Per-instruction ORAM scan invariant for NOP and mixed opcodes (Doc 19 App.C)\n- Fixed-iteration verify_bb_mac via max_bb_insn_count (Doc 19 §4.2 Fix #2)\n- Branchless Phase L all three cases: branch, fallthrough, mid-BB (Doc 19 §4.2 Fix #1)\n- ExecSnapshot branchless_restore roundtrip correctness\n- Branchless Phase E preserves non-writing opcode registers (Doc 19 §4.2 Fix #3)\n- Per-execution independence via RDRAND nonce diversification (Doc 16 §12 S4)\n- VmEpoch::branchless_select restore and keep-new correctness\n\n15 new tests, 889/889 passing.",
          "timestamp": "2026-04-04T01:00:55+08:00",
          "tree_id": "d2f5f9ee5fdd1a0c06f76e2ceeddb9e1da95de65",
          "url": "https://github.com/scc-tw/VMPilot/commit/c1856b036b1c5f3a92dd80376a1829f8f0d999f5"
        },
        "date": 1775238145752,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5050.3,
            "unit": "ns/DU",
            "extra": "median=631287ns  p95=648068ns  stddev=10945.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 198008,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5089.97,
            "unit": "ns/DU",
            "extra": "median=636246ns  p95=657716ns  stddev=8004.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 39.67,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 196465,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5072.81,
            "unit": "ns/DU",
            "extra": "median=634101ns  p95=635314ns  stddev=3971.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 22.51,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 197129,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5072.73,
            "unit": "ns/DU",
            "extra": "median=634091ns  p95=668075ns  stddev=12246.6ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 22.43,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 197133,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5046.53,
            "unit": "ns/DU",
            "extra": "median=630816ns  p95=634552ns  stddev=4293.8ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -3.77,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 198156,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5106.07,
            "unit": "ns/DU",
            "extra": "median=638259ns  p95=641165ns  stddev=4575.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 55.78,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 195845,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5059.51,
            "unit": "ns/DU",
            "extra": "median=632439ns  p95=646515ns  stddev=8681.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 9.22,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 197648,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5061.91,
            "unit": "ns/DU",
            "extra": "median=632739ns  p95=646725ns  stddev=6909.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 11.62,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 197554,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5065.2,
            "unit": "ns/DU",
            "extra": "median=633150ns  p95=636266ns  stddev=4993.0ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 14.9,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 197426,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5120.82,
            "unit": "ns/DU",
            "extra": "median=640103ns  p95=642778ns  stddev=5209.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 70.53,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 195281,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5092.61,
            "unit": "ns/DU",
            "extra": "median=636576ns  p95=638580ns  stddev=3113.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 42.31,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 196363,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5097.34,
            "unit": "ns/DU",
            "extra": "median=637167ns  p95=641265ns  stddev=5032.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 47.04,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 196181,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5105.43,
            "unit": "ns/DU",
            "extra": "median=638179ns  p95=648458ns  stddev=5400.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 55.14,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 195870,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5099.66,
            "unit": "ns/DU",
            "extra": "median=637458ns  p95=640553ns  stddev=4318.2ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 49.37,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 196091,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5105.76,
            "unit": "ns/DU",
            "extra": "median=638220ns  p95=640694ns  stddev=4377.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 55.46,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 195857,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5073.54,
            "unit": "ns/DU",
            "extra": "median=634192ns  p95=636927ns  stddev=4394.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 23.24,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 197101,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5097.74,
            "unit": "ns/DU",
            "extra": "median=637218ns  p95=639381ns  stddev=4000.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 47.45,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 196165,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5104.16,
            "unit": "ns/DU",
            "extra": "median=638020ns  p95=650332ns  stddev=6195.5ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 53.86,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 195919,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5100.06,
            "unit": "ns/DU",
            "extra": "median=637508ns  p95=647928ns  stddev=6004.2ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 49.77,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 196076,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5089.49,
            "unit": "ns/DU",
            "extra": "median=636186ns  p95=639271ns  stddev=3926.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 39.19,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 196483,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5073.86,
            "unit": "ns/DU",
            "extra": "median=634232ns  p95=646304ns  stddev=5997.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 23.56,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 197089,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5105.11,
            "unit": "ns/DU",
            "extra": "median=638139ns  p95=641315ns  stddev=3759.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 54.82,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 195882,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5103.27,
            "unit": "ns/DU",
            "extra": "median=637909ns  p95=640523ns  stddev=4409.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 52.98,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 195953,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5100.46,
            "unit": "ns/DU",
            "extra": "median=637558ns  p95=640273ns  stddev=4089.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 50.17,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 196061,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5098.78,
            "unit": "ns/DU",
            "extra": "median=637348ns  p95=640193ns  stddev=4621.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 48.49,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 196125,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5091.41,
            "unit": "ns/DU",
            "extra": "median=636426ns  p95=638770ns  stddev=3816.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 41.11,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 196409,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5089.48,
            "unit": "ns/DU",
            "extra": "median=636185ns  p95=647717ns  stddev=5629.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 39.18,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 196484,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5079.7,
            "unit": "ns/DU",
            "extra": "median=634963ns  p95=637357ns  stddev=3796.6ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 29.41,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 196862,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5031.05,
            "unit": "ns/DU",
            "extra": "median=628881ns  p95=642908ns  stddev=5818.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -19.25,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 198766,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5066.64,
            "unit": "ns/DU",
            "extra": "median=633330ns  p95=637899ns  stddev=4062.4ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 16.34,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 197369,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5035.46,
            "unit": "ns/DU",
            "extra": "median=629433ns  p95=632259ns  stddev=5096.5ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -14.83,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 198591,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5032.18,
            "unit": "ns/DU",
            "extra": "median=629023ns  p95=630866ns  stddev=3981.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -18.11,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 198721,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5145.43,
            "unit": "ns/DU",
            "extra": "median=643179ns  p95=653007ns  stddev=5870.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 95.14,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 194347,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5065.52,
            "unit": "ns/DU",
            "extra": "median=633190ns  p95=636025ns  stddev=4164.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 15.22,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 197413,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5060.3,
            "unit": "ns/DU",
            "extra": "median=632538ns  p95=637477ns  stddev=4428.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 10.01,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 197617,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5077.94,
            "unit": "ns/DU",
            "extra": "median=634742ns  p95=637177ns  stddev=4277.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 27.64,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 196930,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5067.85,
            "unit": "ns/DU",
            "extra": "median=633481ns  p95=644772ns  stddev=5521.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 17.55,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 197322,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5064,
            "unit": "ns/DU",
            "extra": "median=633000ns  p95=637138ns  stddev=4469.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 13.7,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 197472,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5074.42,
            "unit": "ns/DU",
            "extra": "median=634302ns  p95=638260ns  stddev=4787.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 24.12,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 197067,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5071.3,
            "unit": "ns/DU",
            "extra": "median=633912ns  p95=644942ns  stddev=5852.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 21,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 197188,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5083.39,
            "unit": "ns/DU",
            "extra": "median=635424ns  p95=668566ns  stddev=11987.1ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 33.1,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 196719,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5053.74,
            "unit": "ns/DU",
            "extra": "median=631718ns  p95=649290ns  stddev=9176.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 3.45,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 197873,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5075.3,
            "unit": "ns/DU",
            "extra": "median=634413ns  p95=636856ns  stddev=3813.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 25.01,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 197033,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5040.75,
            "unit": "ns/DU",
            "extra": "median=630094ns  p95=632750ns  stddev=4381.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -9.54,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 198383,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5004.28,
            "unit": "ns/DU",
            "extra": "median=625535ns  p95=663667ns  stddev=13275.2ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -46.02,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 199829,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5085.24,
            "unit": "ns/DU",
            "extra": "median=635655ns  p95=638440ns  stddev=4346.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 34.94,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 196648,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5023.12,
            "unit": "ns/DU",
            "extra": "median=627890ns  p95=629914ns  stddev=4639.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -27.18,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 199079,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5030.33,
            "unit": "ns/DU",
            "extra": "median=628791ns  p95=641155ns  stddev=6246.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -19.97,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 198794,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5033.78,
            "unit": "ns/DU",
            "extra": "median=629222ns  p95=630876ns  stddev=3637.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -16.52,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 198658,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5043.64,
            "unit": "ns/DU",
            "extra": "median=630455ns  p95=708040ns  stddev=25829.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -6.66,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 198270,
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
          "distinct": true,
          "id": "ab10bc2c0a4beb1f8811072cec731c2635840a5a",
          "message": "test: add opcode_writes_reg, make_operand_flags, VM internal opcode tests\n\n- opcode_writes_reg: exhaustive test for all 55 opcodes (writing vs non-writing)\n- make_operand_flags + OP_FLAGS_*: constexpr flag composition correctness\n- to_string: covers all opcodes (no nullptr, no empty)\n- vm_opcode_category: range [0,7] for all opcodes\n- VmInsn condition nibble extraction\n- VM internal opcodes: CHECK_INTEGRITY, CHECK_DEBUG, MUTATE_ISA execute without crash\n- HALT as first instruction: returns 0\n- RET_VM without CALL_VM: StackUnderflow",
          "timestamp": "2026-04-04T06:05:51+08:00",
          "tree_id": "4d67018b2cebcf9d9732eda3dfc5a7846e6ca523",
          "url": "https://github.com/scc-tw/VMPilot/commit/ab10bc2c0a4beb1f8811072cec731c2635840a5a"
        },
        "date": 1775254086794,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5160.36,
            "unit": "ns/DU",
            "extra": "median=645045ns  p95=658549ns  stddev=10645.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 193785,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5135.83,
            "unit": "ns/DU",
            "extra": "median=641979ns  p95=715846ns  stddev=22495.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": -24.53,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 194710,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5118.04,
            "unit": "ns/DU",
            "extra": "median=639755ns  p95=643462ns  stddev=4339.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -42.32,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 195387,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5110.34,
            "unit": "ns/DU",
            "extra": "median=638793ns  p95=641437ns  stddev=4256.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -50.02,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 195682,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5082.61,
            "unit": "ns/DU",
            "extra": "median=635326ns  p95=639825ns  stddev=4769.1ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -77.75,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 196749,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5150.5,
            "unit": "ns/DU",
            "extra": "median=643813ns  p95=661526ns  stddev=8358.4ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": -9.86,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 194156,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5079,
            "unit": "ns/DU",
            "extra": "median=634875ns  p95=639874ns  stddev=5554.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -81.36,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 196889,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5101.21,
            "unit": "ns/DU",
            "extra": "median=637651ns  p95=642219ns  stddev=5270.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -59.15,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 196032,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 6655.46,
            "unit": "ns/DU",
            "extra": "median=831933ns  p95=880834ns  stddev=69059.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 1495.1,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 150252,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5103.61,
            "unit": "ns/DU",
            "extra": "median=637951ns  p95=641126ns  stddev=4535.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -56.75,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 195940,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5080.85,
            "unit": "ns/DU",
            "extra": "median=635106ns  p95=639164ns  stddev=4406.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -79.51,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 196818,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5133.9,
            "unit": "ns/DU",
            "extra": "median=641738ns  p95=667005ns  stddev=9370.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": -26.46,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 194784,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5122.04,
            "unit": "ns/DU",
            "extra": "median=640255ns  p95=645165ns  stddev=4504.1ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": -38.32,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 195235,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5090.38,
            "unit": "ns/DU",
            "extra": "median=636298ns  p95=640496ns  stddev=4270.9ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -69.98,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 196449,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5116.59,
            "unit": "ns/DU",
            "extra": "median=639574ns  p95=645134ns  stddev=5214.9ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -43.77,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 195443,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5110.58,
            "unit": "ns/DU",
            "extra": "median=638822ns  p95=680520ns  stddev=13851.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -49.78,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 195673,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5104.25,
            "unit": "ns/DU",
            "extra": "median=638031ns  p95=642509ns  stddev=5104.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -56.11,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 195915,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5116.67,
            "unit": "ns/DU",
            "extra": "median=639584ns  p95=645445ns  stddev=4849.5ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": -43.69,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 195440,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5115.07,
            "unit": "ns/DU",
            "extra": "median=639384ns  p95=640415ns  stddev=4368.6ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -45.29,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 195501,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 6795.25,
            "unit": "ns/DU",
            "extra": "median=849406ns  p95=881315ns  stddev=99381.5ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 1634.89,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 147162,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 6719.18,
            "unit": "ns/DU",
            "extra": "median=839898ns  p95=876296ns  stddev=93493.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 1558.82,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 148828,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5178.7,
            "unit": "ns/DU",
            "extra": "median=647338ns  p95=654352ns  stddev=10213.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 18.34,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 193099,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5111.7,
            "unit": "ns/DU",
            "extra": "median=638963ns  p95=640225ns  stddev=4323.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": -48.66,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 195629,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 6661.72,
            "unit": "ns/DU",
            "extra": "median=832715ns  p95=866017ns  stddev=88889.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 1501.36,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 150111,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5111.7,
            "unit": "ns/DU",
            "extra": "median=638962ns  p95=640265ns  stddev=4262.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -48.66,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 195630,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5108.58,
            "unit": "ns/DU",
            "extra": "median=638573ns  p95=640766ns  stddev=4630.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": -51.78,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 195749,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5082.13,
            "unit": "ns/DU",
            "extra": "median=635266ns  p95=639263ns  stddev=4069.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -78.23,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 196768,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5070.18,
            "unit": "ns/DU",
            "extra": "median=633773ns  p95=638782ns  stddev=4513.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": -90.18,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 197232,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5028.59,
            "unit": "ns/DU",
            "extra": "median=628574ns  p95=655754ns  stddev=10849.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -131.77,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 198863,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5098.64,
            "unit": "ns/DU",
            "extra": "median=637330ns  p95=640175ns  stddev=5236.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -61.72,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 196131,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5027.22,
            "unit": "ns/DU",
            "extra": "median=628403ns  p95=647409ns  stddev=7849.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -133.14,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 198917,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5062.82,
            "unit": "ns/DU",
            "extra": "median=632852ns  p95=637741ns  stddev=5077.9ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -97.54,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 197519,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5174.38,
            "unit": "ns/DU",
            "extra": "median=646798ns  p95=682484ns  stddev=11671.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 14.02,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 193260,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5080.77,
            "unit": "ns/DU",
            "extra": "median=635096ns  p95=637511ns  stddev=3976.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -79.59,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 196821,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5076.43,
            "unit": "ns/DU",
            "extra": "median=634554ns  p95=645474ns  stddev=6069.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -83.93,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 196989,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5085.58,
            "unit": "ns/DU",
            "extra": "median=635697ns  p95=773845ns  stddev=40684.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -74.78,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 196635,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5064.58,
            "unit": "ns/DU",
            "extra": "median=633072ns  p95=640496ns  stddev=4869.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -95.78,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 197450,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5091.03,
            "unit": "ns/DU",
            "extra": "median=636379ns  p95=646667ns  stddev=6294.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -69.33,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 196424,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5079.25,
            "unit": "ns/DU",
            "extra": "median=634906ns  p95=647649ns  stddev=5521.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -81.11,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 196880,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5086.14,
            "unit": "ns/DU",
            "extra": "median=635767ns  p95=643070ns  stddev=5336.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -74.22,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 196613,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5097.36,
            "unit": "ns/DU",
            "extra": "median=637170ns  p95=655223ns  stddev=7076.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -63,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 196180,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5091.34,
            "unit": "ns/DU",
            "extra": "median=636417ns  p95=640857ns  stddev=4919.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -69.02,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 196412,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5118.11,
            "unit": "ns/DU",
            "extra": "median=639764ns  p95=658259ns  stddev=7108.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -42.25,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 195385,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5090.14,
            "unit": "ns/DU",
            "extra": "median=636268ns  p95=685289ns  stddev=17094.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -70.22,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 196458,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5034.91,
            "unit": "ns/DU",
            "extra": "median=629364ns  p95=672406ns  stddev=14114.7ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -125.45,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 198613,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5100.88,
            "unit": "ns/DU",
            "extra": "median=637610ns  p95=643030ns  stddev=4899.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -59.48,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 196045,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5058.72,
            "unit": "ns/DU",
            "extra": "median=632340ns  p95=635487ns  stddev=4832.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -101.64,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 197678,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5068.58,
            "unit": "ns/DU",
            "extra": "median=633572ns  p95=639194ns  stddev=4650.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -91.78,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 197294,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5065.14,
            "unit": "ns/DU",
            "extra": "median=633143ns  p95=635546ns  stddev=4889.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -95.22,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 197428,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5068.5,
            "unit": "ns/DU",
            "extra": "median=633562ns  p95=641197ns  stddev=5530.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -91.86,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 197297,
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
          "distinct": true,
          "id": "ac221315352a3bf1c082be939bd7f99e78d5d6ba",
          "message": "docs: update README for frontend/backend/serializer/linker restructuring\n\n- Title: \"VM SDK\" → \"VM Protection Framework\"\n- Architecture: SDK → Frontend + Backend + Serializer, Loader → Linker\n- Pipeline: split into Frontend (analysis), Backend (compilation),\n  Serializer (persistence), Linker (patching) sections\n- Runtime pipeline: updated to dispatch_unit model (doc 19)\n- Security model: add doc 17 (8-byte ratchet) and doc 19 (branchless)\n- Mermaid diagram: reflects new 5-component architecture\n- Project structure: fully updated directory tree\n- Roadmap: doc 17 moved to completed, test count updated\n- Platform table: SDK → Frontend, Loader → Linker",
          "timestamp": "2026-04-04T08:25:37+08:00",
          "tree_id": "1f4e538292b448a3ab7bfaf600850cfa9d9968c7",
          "url": "https://github.com/scc-tw/VMPilot/commit/ac221315352a3bf1c082be939bd7f99e78d5d6ba"
        },
        "date": 1775265223616,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5035.94,
            "unit": "ns/DU",
            "extra": "median=629492ns  p95=646965ns  stddev=6690.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 198573,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5114.4,
            "unit": "ns/DU",
            "extra": "median=639300ns  p95=649239ns  stddev=7552.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 78.46,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 195526,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5083.46,
            "unit": "ns/DU",
            "extra": "median=635432ns  p95=637818ns  stddev=5008.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 47.52,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 196717,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5102.78,
            "unit": "ns/DU",
            "extra": "median=637847ns  p95=639891ns  stddev=4490.3ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 66.84,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 195972,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5074.89,
            "unit": "ns/DU",
            "extra": "median=634361ns  p95=668588ns  stddev=12972.1ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 38.95,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 197049,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5135.72,
            "unit": "ns/DU",
            "extra": "median=641965ns  p95=647124ns  stddev=4365.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 99.78,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 194715,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5072.4,
            "unit": "ns/DU",
            "extra": "median=634050ns  p95=637196ns  stddev=4842.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 36.46,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 197145,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5075.36,
            "unit": "ns/DU",
            "extra": "median=634420ns  p95=647716ns  stddev=6611.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 39.42,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 197030,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5102.62,
            "unit": "ns/DU",
            "extra": "median=637827ns  p95=653346ns  stddev=7766.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 66.68,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 195978,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5130.59,
            "unit": "ns/DU",
            "extra": "median=641324ns  p95=658856ns  stddev=9223.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 94.66,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 194909,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5093.64,
            "unit": "ns/DU",
            "extra": "median=636705ns  p95=649630ns  stddev=7049.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 57.7,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 196323,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5114.56,
            "unit": "ns/DU",
            "extra": "median=639320ns  p95=641163ns  stddev=4882.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 78.62,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 195520,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5117.21,
            "unit": "ns/DU",
            "extra": "median=639651ns  p95=676029ns  stddev=12245.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 81.27,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 195419,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5133.32,
            "unit": "ns/DU",
            "extra": "median=641665ns  p95=753505ns  stddev=33217.5ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 97.38,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 194806,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5126.42,
            "unit": "ns/DU",
            "extra": "median=640803ns  p95=644550ns  stddev=5193.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 90.49,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 195068,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5098.45,
            "unit": "ns/DU",
            "extra": "median=637306ns  p95=639611ns  stddev=4568.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 62.51,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 196138,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5123.62,
            "unit": "ns/DU",
            "extra": "median=640452ns  p95=646825ns  stddev=4517.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 87.68,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 195175,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5116.96,
            "unit": "ns/DU",
            "extra": "median=639620ns  p95=649660ns  stddev=5638.4ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 81.02,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 195429,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5115.04,
            "unit": "ns/DU",
            "extra": "median=639380ns  p95=642437ns  stddev=4786.8ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 79.1,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 195502,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5120.01,
            "unit": "ns/DU",
            "extra": "median=640001ns  p95=646052ns  stddev=4645.6ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 84.07,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 195312,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5099.18,
            "unit": "ns/DU",
            "extra": "median=637397ns  p95=639451ns  stddev=4844.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 63.24,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 196110,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5106.79,
            "unit": "ns/DU",
            "extra": "median=638349ns  p95=642847ns  stddev=4422.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 70.86,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 195818,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5123.14,
            "unit": "ns/DU",
            "extra": "median=640392ns  p95=643217ns  stddev=4622.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 87.2,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 195193,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5119.21,
            "unit": "ns/DU",
            "extra": "median=639901ns  p95=663265ns  stddev=8049.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 83.27,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 195343,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5117.53,
            "unit": "ns/DU",
            "extra": "median=639691ns  p95=711366ns  stddev=21921.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 81.59,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 195407,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5190.86,
            "unit": "ns/DU",
            "extra": "median=648858ns  p95=655650ns  stddev=9631.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 154.93,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 192646,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5074.89,
            "unit": "ns/DU",
            "extra": "median=634361ns  p95=657734ns  stddev=9027.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 38.95,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 197049,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5090.11,
            "unit": "ns/DU",
            "extra": "median=636264ns  p95=639149ns  stddev=4373.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 54.18,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 196459,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5017.1,
            "unit": "ns/DU",
            "extra": "median=627137ns  p95=630043ns  stddev=4160.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -18.84,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 199318,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5087.39,
            "unit": "ns/DU",
            "extra": "median=635924ns  p95=640603ns  stddev=5288.2ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 51.46,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 196564,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5031.36,
            "unit": "ns/DU",
            "extra": "median=628920ns  p95=645762ns  stddev=7154.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -4.58,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 198753,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5059.5,
            "unit": "ns/DU",
            "extra": "median=632438ns  p95=653297ns  stddev=8219.5ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 23.57,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 197648,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5165.38,
            "unit": "ns/DU",
            "extra": "median=645672ns  p95=649429ns  stddev=4208.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 129.44,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 193597,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5097.33,
            "unit": "ns/DU",
            "extra": "median=637166ns  p95=648598ns  stddev=5639.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 61.39,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 196181,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5101.58,
            "unit": "ns/DU",
            "extra": "median=637697ns  p95=639690ns  stddev=4598.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 65.64,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 196018,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5092.76,
            "unit": "ns/DU",
            "extra": "median=636595ns  p95=763103ns  stddev=37440.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 56.82,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 196357,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5100.21,
            "unit": "ns/DU",
            "extra": "median=637526ns  p95=652535ns  stddev=8224.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 64.27,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 196070,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5102.94,
            "unit": "ns/DU",
            "extra": "median=637867ns  p95=642667ns  stddev=4622.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 67,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 195966,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5089.15,
            "unit": "ns/DU",
            "extra": "median=636144ns  p95=640513ns  stddev=4132.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 53.22,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 196496,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5197.35,
            "unit": "ns/DU",
            "extra": "median=649669ns  p95=683853ns  stddev=14976.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 161.42,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 192406,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5105.18,
            "unit": "ns/DU",
            "extra": "median=638148ns  p95=641425ns  stddev=4501.1ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 69.25,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 195879,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5064.7,
            "unit": "ns/DU",
            "extra": "median=633088ns  p95=639440ns  stddev=5094.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 28.77,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 197445,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5078.09,
            "unit": "ns/DU",
            "extra": "median=634761ns  p95=637867ns  stddev=4512.8ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 42.15,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 196925,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5061.58,
            "unit": "ns/DU",
            "extra": "median=632697ns  p95=641494ns  stddev=4949.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 25.64,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 197567,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5021.42,
            "unit": "ns/DU",
            "extra": "median=627678ns  p95=655450ns  stddev=9537.1ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -14.51,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 199147,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5096.21,
            "unit": "ns/DU",
            "extra": "median=637026ns  p95=657004ns  stddev=8306.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 60.27,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 196224,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5042.42,
            "unit": "ns/DU",
            "extra": "median=630303ns  p95=645983ns  stddev=7110.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 6.49,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 198317,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5049.24,
            "unit": "ns/DU",
            "extra": "median=631155ns  p95=636434ns  stddev=5164.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 13.3,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 198050,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5063.66,
            "unit": "ns/DU",
            "extra": "median=632958ns  p95=663936ns  stddev=10634.2ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 27.73,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 197485,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5048.44,
            "unit": "ns/DU",
            "extra": "median=631055ns  p95=633710ns  stddev=4306.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 12.5,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 198081,
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
          "distinct": true,
          "id": "d0be69eb773c0bf4a5d2a741ed4fca045be9124f",
          "message": "fix(ci): prevent benchmark workflow from poisoning submodule cache\n\nThe benchmark workflow runs `git submodule deinit --all --force` before\nthe gh-pages push. The actions/cache@v4 post-step then saves the emptied\ndirectories (~400 bytes) to the shared cache key. Subsequent build runs\nhit that poisoned cache, skip submodule init, and fail at configure with\n\"does not contain a CMakeLists.txt\".\n\n- benchmark.yml: split into cache/restore + cache/save so the save\n  happens before deinit, preventing future cache poisoning\n- build.yml: add a verify step that re-inits submodules if the cache\n  restored empty directories (defense-in-depth)\n- Deleted the three poisoned cache entries via `gh cache delete`",
          "timestamp": "2026-04-04T11:01:14+08:00",
          "tree_id": "3a7fbbdef1b0f130e4c6903ed529aefc613544af",
          "url": "https://github.com/scc-tw/VMPilot/commit/d0be69eb773c0bf4a5d2a741ed4fca045be9124f"
        },
        "date": 1775271758613,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5171.53,
            "unit": "ns/DU",
            "extra": "median=646441ns  p95=657782ns  stddev=12367.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 193366,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5079.92,
            "unit": "ns/DU",
            "extra": "median=634990ns  p95=653183ns  stddev=7442.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": -91.61,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 196853,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5106.6,
            "unit": "ns/DU",
            "extra": "median=638325ns  p95=651991ns  stddev=7267.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -64.93,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 195825,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5100.51,
            "unit": "ns/DU",
            "extra": "median=637564ns  p95=658935ns  stddev=9227.6ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -71.02,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 196059,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5084.89,
            "unit": "ns/DU",
            "extra": "median=635611ns  p95=641321ns  stddev=5495.3ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -86.64,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 196661,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5096.83,
            "unit": "ns/DU",
            "extra": "median=637104ns  p95=639849ns  stddev=4252.0ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": -74.7,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 196200,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5060.28,
            "unit": "ns/DU",
            "extra": "median=632535ns  p95=648626ns  stddev=8719.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -111.25,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 197618,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5085.37,
            "unit": "ns/DU",
            "extra": "median=635671ns  p95=687819ns  stddev=16631.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -86.16,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 196643,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5093.86,
            "unit": "ns/DU",
            "extra": "median=636733ns  p95=710010ns  stddev=22154.0ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -77.66,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 196315,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5119.58,
            "unit": "ns/DU",
            "extra": "median=639948ns  p95=654586ns  stddev=8634.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -51.94,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 195328,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5099.79,
            "unit": "ns/DU",
            "extra": "median=637474ns  p95=652382ns  stddev=9584.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -71.74,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 196086,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5111.42,
            "unit": "ns/DU",
            "extra": "median=638927ns  p95=652402ns  stddev=8523.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": -60.11,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 195641,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5091.46,
            "unit": "ns/DU",
            "extra": "median=636432ns  p95=655568ns  stddev=7963.7ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": -80.07,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 196407,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5118.78,
            "unit": "ns/DU",
            "extra": "median=639848ns  p95=644357ns  stddev=5635.0ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -52.74,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 195359,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5107.97,
            "unit": "ns/DU",
            "extra": "median=638496ns  p95=681386ns  stddev=13826.8ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -63.56,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 195773,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5077.27,
            "unit": "ns/DU",
            "extra": "median=634659ns  p95=759703ns  stddev=36919.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -94.26,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 196956,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5109.74,
            "unit": "ns/DU",
            "extra": "median=638717ns  p95=648575ns  stddev=6686.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -61.79,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 195705,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5094.66,
            "unit": "ns/DU",
            "extra": "median=636832ns  p95=638957ns  stddev=4078.6ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": -76.87,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 196284,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5115.58,
            "unit": "ns/DU",
            "extra": "median=639448ns  p95=657341ns  stddev=7487.0ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -55.94,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 195481,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5194.94,
            "unit": "ns/DU",
            "extra": "median=649367ns  p95=678020ns  stddev=13654.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 23.41,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 192495,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5076.95,
            "unit": "ns/DU",
            "extra": "median=634619ns  p95=639408ns  stddev=4404.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": -94.58,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 196969,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5082.08,
            "unit": "ns/DU",
            "extra": "median=635260ns  p95=652812ns  stddev=6925.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": -89.45,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 196770,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5091.54,
            "unit": "ns/DU",
            "extra": "median=636442ns  p95=640139ns  stddev=4336.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": -79.99,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 196404,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5095.39,
            "unit": "ns/DU",
            "extra": "median=636924ns  p95=657772ns  stddev=8641.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -76.14,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 196256,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5081.03,
            "unit": "ns/DU",
            "extra": "median=635129ns  p95=641291ns  stddev=5224.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -90.5,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 196810,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5107.57,
            "unit": "ns/DU",
            "extra": "median=638446ns  p95=768870ns  stddev=38245.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": -63.96,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 195788,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5166.32,
            "unit": "ns/DU",
            "extra": "median=645790ns  p95=654777ns  stddev=9570.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -5.21,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 193561,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5116.55,
            "unit": "ns/DU",
            "extra": "median=639569ns  p95=675315ns  stddev=12711.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": -54.98,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 195444,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5056.35,
            "unit": "ns/DU",
            "extra": "median=632044ns  p95=640500ns  stddev=5331.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -115.18,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 197771,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5077.51,
            "unit": "ns/DU",
            "extra": "median=634689ns  p95=639247ns  stddev=4691.2ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -94.02,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 196947,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5021.89,
            "unit": "ns/DU",
            "extra": "median=627736ns  p95=632124ns  stddev=4188.2ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -149.64,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 199128,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5028.38,
            "unit": "ns/DU",
            "extra": "median=628548ns  p95=631823ns  stddev=4554.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -143.14,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 198871,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5157.66,
            "unit": "ns/DU",
            "extra": "median=644708ns  p95=647463ns  stddev=4904.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -13.86,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 193886,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5067.74,
            "unit": "ns/DU",
            "extra": "median=633467ns  p95=635921ns  stddev=4337.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -103.79,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 197327,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5084.96,
            "unit": "ns/DU",
            "extra": "median=635620ns  p95=650188ns  stddev=6479.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -86.57,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 196658,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5073.34,
            "unit": "ns/DU",
            "extra": "median=634168ns  p95=637734ns  stddev=4565.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -98.18,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 197109,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5079.11,
            "unit": "ns/DU",
            "extra": "median=634889ns  p95=639317ns  stddev=4683.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -92.42,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 196885,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5080.4,
            "unit": "ns/DU",
            "extra": "median=635050ns  p95=636482ns  stddev=4828.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -91.13,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 196835,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5085.61,
            "unit": "ns/DU",
            "extra": "median=635701ns  p95=638686ns  stddev=4052.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -85.92,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 196633,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5082.08,
            "unit": "ns/DU",
            "extra": "median=635260ns  p95=638516ns  stddev=5676.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -89.45,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 196770,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5079.92,
            "unit": "ns/DU",
            "extra": "median=634990ns  p95=637434ns  stddev=4563.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -91.61,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 196853,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5054.02,
            "unit": "ns/DU",
            "extra": "median=631753ns  p95=633637ns  stddev=4141.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -117.5,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 197862,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5079.84,
            "unit": "ns/DU",
            "extra": "median=634980ns  p95=674093ns  stddev=13657.3ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -91.69,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 196857,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5063.73,
            "unit": "ns/DU",
            "extra": "median=632966ns  p95=638596ns  stddev=3422.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -107.8,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 197483,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5025.34,
            "unit": "ns/DU",
            "extra": "median=628167ns  p95=632355ns  stddev=4559.1ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -146.19,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 198992,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5110.22,
            "unit": "ns/DU",
            "extra": "median=638777ns  p95=722023ns  stddev=29287.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -61.31,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 195686,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5065.34,
            "unit": "ns/DU",
            "extra": "median=633167ns  p95=638977ns  stddev=5154.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -106.19,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 197420,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5063.89,
            "unit": "ns/DU",
            "extra": "median=632986ns  p95=635991ns  stddev=5053.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -107.64,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 197477,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5051.22,
            "unit": "ns/DU",
            "extra": "median=631403ns  p95=634298ns  stddev=4632.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -120.3,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 197972,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5072.94,
            "unit": "ns/DU",
            "extra": "median=634118ns  p95=655758ns  stddev=8008.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -98.58,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 197124,
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
          "distinct": true,
          "id": "c72098444d445c3315958d25f4d5e5e8d711845a",
          "message": "fix(linker): add RegionRefiner dependency to Segmentator\n\nsegmentator.cpp calls RegionRefiner::refine() and RegionRefiner::group()\nbut never declared a link dependency on VMPilot_SDK_RegionRefiner.\nGNU ld processes static archives left-to-right and discards unreferenced\nsymbols, so RegionRefiner.a was already consumed before Segmentator.a\nintroduced the undefined references.",
          "timestamp": "2026-04-04T11:09:10+08:00",
          "tree_id": "166864612529b7f57953e07462d21ab8fddc16c6",
          "url": "https://github.com/scc-tw/VMPilot/commit/c72098444d445c3315958d25f4d5e5e8d711845a"
        },
        "date": 1775272205494,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5056.32,
            "unit": "ns/DU",
            "extra": "median=632040ns  p95=655154ns  stddev=11880.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 197772,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5086.7,
            "unit": "ns/DU",
            "extra": "median=635837ns  p95=650645ns  stddev=7560.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 30.38,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 196591,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5065.14,
            "unit": "ns/DU",
            "extra": "median=633142ns  p95=636278ns  stddev=4184.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 8.82,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 197428,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5087.58,
            "unit": "ns/DU",
            "extra": "median=635947ns  p95=638762ns  stddev=4573.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 31.26,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 196557,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5053.91,
            "unit": "ns/DU",
            "extra": "median=631739ns  p95=634174ns  stddev=4591.5ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -2.41,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 197867,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5190.01,
            "unit": "ns/DU",
            "extra": "median=648751ns  p95=681572ns  stddev=13372.7ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 133.69,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 192678,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5041.65,
            "unit": "ns/DU",
            "extra": "median=630206ns  p95=640967ns  stddev=7263.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -14.67,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 198348,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5188.81,
            "unit": "ns/DU",
            "extra": "median=648601ns  p95=785326ns  stddev=58006.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 132.49,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 192722,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5070.83,
            "unit": "ns/DU",
            "extra": "median=633854ns  p95=745672ns  stddev=33307.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 14.51,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 197206,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5722.84,
            "unit": "ns/DU",
            "extra": "median=715355ns  p95=823167ns  stddev=75636.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 666.52,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 174738,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5172.06,
            "unit": "ns/DU",
            "extra": "median=646507ns  p95=710497ns  stddev=26517.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 115.74,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 193347,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5098.32,
            "unit": "ns/DU",
            "extra": "median=637290ns  p95=661144ns  stddev=11980.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 42,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 196143,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5073.3,
            "unit": "ns/DU",
            "extra": "median=634163ns  p95=638231ns  stddev=3687.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 16.98,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 197110,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5139.43,
            "unit": "ns/DU",
            "extra": "median=642429ns  p95=752474ns  stddev=33296.7ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 83.11,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 194574,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5107.78,
            "unit": "ns/DU",
            "extra": "median=638472ns  p95=652068ns  stddev=8056.7ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 51.46,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 195780,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5071.54,
            "unit": "ns/DU",
            "extra": "median=633943ns  p95=678878ns  stddev=14926.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 15.22,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 197179,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5113.78,
            "unit": "ns/DU",
            "extra": "median=639223ns  p95=644083ns  stddev=5669.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 57.46,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 195550,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5105.94,
            "unit": "ns/DU",
            "extra": "median=638242ns  p95=656155ns  stddev=6742.3ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 49.62,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 195850,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5084.86,
            "unit": "ns/DU",
            "extra": "median=635607ns  p95=641718ns  stddev=4587.6ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 28.54,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 196662,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5106.25,
            "unit": "ns/DU",
            "extra": "median=638281ns  p95=640586ns  stddev=3928.9ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 49.93,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 195839,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5083.57,
            "unit": "ns/DU",
            "extra": "median=635446ns  p95=662777ns  stddev=9734.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 27.25,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 196712,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5139.67,
            "unit": "ns/DU",
            "extra": "median=642459ns  p95=657718ns  stddev=10182.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 83.35,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 194565,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5115.47,
            "unit": "ns/DU",
            "extra": "median=639434ns  p95=652729ns  stddev=8583.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 59.15,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 195485,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5114.1,
            "unit": "ns/DU",
            "extra": "median=639263ns  p95=640575ns  stddev=3660.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 57.78,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 195538,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5115.14,
            "unit": "ns/DU",
            "extra": "median=639393ns  p95=644162ns  stddev=5259.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 58.82,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 195498,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5111.46,
            "unit": "ns/DU",
            "extra": "median=638932ns  p95=640255ns  stddev=3802.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 55.14,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 195639,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5099.12,
            "unit": "ns/DU",
            "extra": "median=637390ns  p95=653329ns  stddev=7186.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 42.8,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 196112,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5066.02,
            "unit": "ns/DU",
            "extra": "median=633252ns  p95=642088ns  stddev=5198.7ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 9.7,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 197394,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5010.23,
            "unit": "ns/DU",
            "extra": "median=626279ns  p95=663178ns  stddev=12699.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -46.09,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 199592,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5081.16,
            "unit": "ns/DU",
            "extra": "median=635145ns  p95=654742ns  stddev=7820.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 24.84,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 196805,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5021.78,
            "unit": "ns/DU",
            "extra": "median=627722ns  p95=646978ns  stddev=9575.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -34.54,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 199133,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5038.84,
            "unit": "ns/DU",
            "extra": "median=629855ns  p95=649953ns  stddev=8545.2ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -17.48,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 198458,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5189.13,
            "unit": "ns/DU",
            "extra": "median=648641ns  p95=693927ns  stddev=16677.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 132.81,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 192711,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5084.14,
            "unit": "ns/DU",
            "extra": "median=635517ns  p95=643892ns  stddev=5381.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 27.82,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 196690,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5090.54,
            "unit": "ns/DU",
            "extra": "median=636318ns  p95=640405ns  stddev=4339.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 34.22,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 196443,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5143.04,
            "unit": "ns/DU",
            "extra": "median=642880ns  p95=697963ns  stddev=19900.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 86.72,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 194438,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5080.93,
            "unit": "ns/DU",
            "extra": "median=635116ns  p95=652277ns  stddev=7072.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 24.61,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 196814,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5092.14,
            "unit": "ns/DU",
            "extra": "median=636518ns  p95=653700ns  stddev=7258.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 35.82,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 196381,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5088.62,
            "unit": "ns/DU",
            "extra": "median=636078ns  p95=638853ns  stddev=5315.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 32.3,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 196517,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5438.79,
            "unit": "ns/DU",
            "extra": "median=679849ns  p95=758166ns  stddev=46870.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 382.47,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 183864,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5165.57,
            "unit": "ns/DU",
            "extra": "median=645696ns  p95=683266ns  stddev=19187.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 109.25,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 193590,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5140.71,
            "unit": "ns/DU",
            "extra": "median=642589ns  p95=687934ns  stddev=18108.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 84.39,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 194526,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5184.24,
            "unit": "ns/DU",
            "extra": "median=648030ns  p95=799943ns  stddev=46462.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 127.92,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 192892,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5127.74,
            "unit": "ns/DU",
            "extra": "median=640967ns  p95=643802ns  stddev=9445.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 71.42,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 195018,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5009.02,
            "unit": "ns/DU",
            "extra": "median=626128ns  p95=631268ns  stddev=4974.8ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -47.3,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 199640,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5053.43,
            "unit": "ns/DU",
            "extra": "median=631679ns  p95=636689ns  stddev=5577.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -2.89,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 197885,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5028.99,
            "unit": "ns/DU",
            "extra": "median=628624ns  p95=632009ns  stddev=5016.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -27.33,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 198847,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5046.7,
            "unit": "ns/DU",
            "extra": "median=630837ns  p95=788983ns  stddev=46482.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -9.62,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 198149,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5038.21,
            "unit": "ns/DU",
            "extra": "median=629776ns  p95=634544ns  stddev=4968.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -18.11,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 198483,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5054.8,
            "unit": "ns/DU",
            "extra": "median=631850ns  p95=659591ns  stddev=10642.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -1.52,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 197832,
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
          "id": "c72098444d445c3315958d25f4d5e5e8d711845a",
          "message": "fix(linker): add RegionRefiner dependency to Segmentator\n\nsegmentator.cpp calls RegionRefiner::refine() and RegionRefiner::group()\nbut never declared a link dependency on VMPilot_SDK_RegionRefiner.\nGNU ld processes static archives left-to-right and discards unreferenced\nsymbols, so RegionRefiner.a was already consumed before Segmentator.a\nintroduced the undefined references.",
          "timestamp": "2026-04-04T11:09:10+08:00",
          "tree_id": "166864612529b7f57953e07462d21ab8fddc16c6",
          "url": "https://github.com/scc-tw/VMPilot/commit/c72098444d445c3315958d25f4d5e5e8d711845a"
        },
        "date": 1775272384164,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 4198.46,
            "unit": "ns/DU",
            "extra": "median=524807ns  p95=543270ns  stddev=9649.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 238183,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 4173.93,
            "unit": "ns/DU",
            "extra": "median=521741ns  p95=544371ns  stddev=9036.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": -24.53,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 239582,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 4150.72,
            "unit": "ns/DU",
            "extra": "median=518840ns  p95=538164ns  stddev=8918.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -47.74,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 240922,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 4162.01,
            "unit": "ns/DU",
            "extra": "median=520251ns  p95=554493ns  stddev=12275.0ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -36.45,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 240269,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 4134.7,
            "unit": "ns/DU",
            "extra": "median=516838ns  p95=540169ns  stddev=7979.1ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -63.75,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 241855,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 4182.62,
            "unit": "ns/DU",
            "extra": "median=522828ns  p95=532336ns  stddev=3744.1ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": -15.83,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 239084,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 4120.11,
            "unit": "ns/DU",
            "extra": "median=515014ns  p95=547665ns  stddev=10459.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -78.34,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 242712,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 4207.57,
            "unit": "ns/DU",
            "extra": "median=525946ns  p95=534126ns  stddev=6596.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 9.11,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 237667,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 4186.01,
            "unit": "ns/DU",
            "extra": "median=523251ns  p95=539182ns  stddev=6857.4ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -12.45,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 238891,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 4193.02,
            "unit": "ns/DU",
            "extra": "median=524127ns  p95=526947ns  stddev=3414.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -5.44,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 238492,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 4192.84,
            "unit": "ns/DU",
            "extra": "median=524105ns  p95=546705ns  stddev=7333.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -5.62,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 238502,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 4220.64,
            "unit": "ns/DU",
            "extra": "median=527580ns  p95=529649ns  stddev=4036.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 22.18,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 236931,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 4209.23,
            "unit": "ns/DU",
            "extra": "median=526154ns  p95=536356ns  stddev=6572.9ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 10.78,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 237573,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 4198.32,
            "unit": "ns/DU",
            "extra": "median=524790ns  p95=576095ns  stddev=15584.5ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -0.14,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 238191,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 4198.35,
            "unit": "ns/DU",
            "extra": "median=524794ns  p95=535848ns  stddev=4864.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -0.1,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 238189,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 4135.87,
            "unit": "ns/DU",
            "extra": "median=516984ns  p95=538410ns  stddev=6657.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -62.58,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 241787,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 4179.08,
            "unit": "ns/DU",
            "extra": "median=522385ns  p95=529362ns  stddev=3847.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -19.38,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 239287,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5688.18,
            "unit": "ns/DU",
            "extra": "median=711023ns  p95=750199ns  stddev=87727.1ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 1489.73,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 175803,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 6000.9,
            "unit": "ns/DU",
            "extra": "median=750112ns  p95=810921ns  stddev=29555.8ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 1802.44,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 166642,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5818.02,
            "unit": "ns/DU",
            "extra": "median=727253ns  p95=758349ns  stddev=13254.3ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 1619.57,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 171880,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5880.17,
            "unit": "ns/DU",
            "extra": "median=735021ns  p95=783688ns  stddev=19143.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 1681.71,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 170063,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 4282.26,
            "unit": "ns/DU",
            "extra": "median=535282ns  p95=744151ns  stddev=87813.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 83.8,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 233522,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 4216.87,
            "unit": "ns/DU",
            "extra": "median=527109ns  p95=536186ns  stddev=6349.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 18.42,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 237143,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 4193.45,
            "unit": "ns/DU",
            "extra": "median=524181ns  p95=528658ns  stddev=3447.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -5.01,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 238467,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 4158.74,
            "unit": "ns/DU",
            "extra": "median=519842ns  p95=527218ns  stddev=3343.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -39.72,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 240458,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 4165.9,
            "unit": "ns/DU",
            "extra": "median=520737ns  p95=528130ns  stddev=3651.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": -32.56,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 240044,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 4173.34,
            "unit": "ns/DU",
            "extra": "median=521667ns  p95=587316ns  stddev=18898.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -25.12,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 239616,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 4211.05,
            "unit": "ns/DU",
            "extra": "median=526381ns  p95=541841ns  stddev=7255.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 12.59,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 237471,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 4156.71,
            "unit": "ns/DU",
            "extra": "median=519589ns  p95=524961ns  stddev=4113.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -41.74,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 240575,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 4170.79,
            "unit": "ns/DU",
            "extra": "median=521349ns  p95=525840ns  stddev=3278.6ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -27.66,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 239763,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 4114.26,
            "unit": "ns/DU",
            "extra": "median=514283ns  p95=521416ns  stddev=3135.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -84.19,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 243057,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 4139.9,
            "unit": "ns/DU",
            "extra": "median=517488ns  p95=524686ns  stddev=3587.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -58.55,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 241551,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 4248.64,
            "unit": "ns/DU",
            "extra": "median=531080ns  p95=534592ns  stddev=2931.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 50.18,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 235369,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 4182.64,
            "unit": "ns/DU",
            "extra": "median=522830ns  p95=544990ns  stddev=7902.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -15.82,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 239083,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 4138.62,
            "unit": "ns/DU",
            "extra": "median=517327ns  p95=527043ns  stddev=3863.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -59.84,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 241627,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 4173.14,
            "unit": "ns/DU",
            "extra": "median=521642ns  p95=525282ns  stddev=3676.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -25.32,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 239628,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 4139.65,
            "unit": "ns/DU",
            "extra": "median=517456ns  p95=523695ns  stddev=2911.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -58.81,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 241566,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 4187.9,
            "unit": "ns/DU",
            "extra": "median=523487ns  p95=556901ns  stddev=10671.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -10.56,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 238783,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 4138.86,
            "unit": "ns/DU",
            "extra": "median=517357ns  p95=525197ns  stddev=3501.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -59.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 241613,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 4166.17,
            "unit": "ns/DU",
            "extra": "median=520771ns  p95=526182ns  stddev=3393.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -32.29,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 240029,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 4142.19,
            "unit": "ns/DU",
            "extra": "median=517774ns  p95=523445ns  stddev=3120.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -56.26,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 241418,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 4179.02,
            "unit": "ns/DU",
            "extra": "median=522377ns  p95=547701ns  stddev=8460.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -19.44,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 239291,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 4191.86,
            "unit": "ns/DU",
            "extra": "median=523983ns  p95=547152ns  stddev=8643.6ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -6.59,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 238557,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 4180.58,
            "unit": "ns/DU",
            "extra": "median=522572ns  p95=526056ns  stddev=3563.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -17.88,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 239201,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 4159.59,
            "unit": "ns/DU",
            "extra": "median=519949ns  p95=526676ns  stddev=4928.8ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -38.86,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 240408,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 4196,
            "unit": "ns/DU",
            "extra": "median=524500ns  p95=530232ns  stddev=5273.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -2.46,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 238322,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 4109.74,
            "unit": "ns/DU",
            "extra": "median=513717ns  p95=524588ns  stddev=4233.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -88.72,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 243325,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 4144.97,
            "unit": "ns/DU",
            "extra": "median=518121ns  p95=529758ns  stddev=4919.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -53.49,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 241256,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 4151.94,
            "unit": "ns/DU",
            "extra": "median=518992ns  p95=527315ns  stddev=4741.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -46.52,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 240851,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 4111.96,
            "unit": "ns/DU",
            "extra": "median=513995ns  p95=521245ns  stddev=3390.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -86.5,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 243193,
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
          "distinct": true,
          "id": "4f9d182b1053a06a26741b1b8a01a59bc8a0f8b4",
          "message": "docs: update README to clarify Runtime VM features and remove unused references",
          "timestamp": "2026-04-06T13:36:36+08:00",
          "tree_id": "c5a75a0c96556edf3dd115b36307b1608e91bddd",
          "url": "https://github.com/scc-tw/VMPilot/commit/4f9d182b1053a06a26741b1b8a01a59bc8a0f8b4"
        },
        "date": 1775453938598,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 5034.1,
            "unit": "ns/DU",
            "extra": "median=629262ns  p95=634963ns  stddev=5606.5ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 198645,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOVE (total)",
            "value": 5031.46,
            "unit": "ns/DU",
            "extra": "median=628932ns  p95=636576ns  stddev=6444.6ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOVE (handler)",
            "value": -2.64,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOVE (DU/s)",
            "value": 198750,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD (total)",
            "value": 5036.26,
            "unit": "ns/DU",
            "extra": "median=629533ns  p95=638089ns  stddev=6265.5ns  iterations=110  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD (handler)",
            "value": 2.17,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD (DU/s)",
            "value": 198560,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE (total)",
            "value": 5028.89,
            "unit": "ns/DU",
            "extra": "median=628611ns  p95=633741ns  stddev=4790.0ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE (handler)",
            "value": -5.21,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE (DU/s)",
            "value": 198851,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] PUSH (total)",
            "value": 5043.47,
            "unit": "ns/DU",
            "extra": "median=630434ns  p95=638089ns  stddev=5193.6ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] PUSH (handler)",
            "value": 9.38,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] PUSH (DU/s)",
            "value": 198276,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] POP (total)",
            "value": 5026.81,
            "unit": "ns/DU",
            "extra": "median=628351ns  p95=633771ns  stddev=5144.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] POP (handler)",
            "value": -7.29,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] POP (DU/s)",
            "value": 198933,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (total)",
            "value": 5024.64,
            "unit": "ns/DU",
            "extra": "median=628080ns  p95=634562ns  stddev=5837.8ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (handler)",
            "value": -9.46,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (DU/s)",
            "value": 199019,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (total)",
            "value": 5053.41,
            "unit": "ns/DU",
            "extra": "median=631676ns  p95=635254ns  stddev=4700.2ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (handler)",
            "value": 19.31,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (DU/s)",
            "value": 197886,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (total)",
            "value": 5036.42,
            "unit": "ns/DU",
            "extra": "median=629552ns  p95=634111ns  stddev=4834.3ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (handler)",
            "value": 2.32,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (DU/s)",
            "value": 198554,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ADD (total)",
            "value": 5044.91,
            "unit": "ns/DU",
            "extra": "median=630614ns  p95=634402ns  stddev=5512.5ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] ADD (handler)",
            "value": 10.82,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ADD (DU/s)",
            "value": 198220,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SUB (total)",
            "value": 5038.02,
            "unit": "ns/DU",
            "extra": "median=629753ns  p95=633580ns  stddev=5095.2ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] SUB (handler)",
            "value": 3.93,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SUB (DU/s)",
            "value": 198491,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUL (total)",
            "value": 5035.94,
            "unit": "ns/DU",
            "extra": "median=629492ns  p95=637728ns  stddev=5841.0ns  iterations=110  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUL (handler)",
            "value": 1.84,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUL (DU/s)",
            "value": 198573,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IMUL (total)",
            "value": 5033.38,
            "unit": "ns/DU",
            "extra": "median=629172ns  p95=633339ns  stddev=4412.3ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] IMUL (handler)",
            "value": -0.72,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IMUL (DU/s)",
            "value": 198674,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] DIV (total)",
            "value": 5039.7,
            "unit": "ns/DU",
            "extra": "median=629963ns  p95=633700ns  stddev=4610.3ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] DIV (handler)",
            "value": 5.61,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] DIV (DU/s)",
            "value": 198424,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IDIV (total)",
            "value": 5043.15,
            "unit": "ns/DU",
            "extra": "median=630394ns  p95=635595ns  stddev=5479.5ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] IDIV (handler)",
            "value": 9.06,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IDIV (DU/s)",
            "value": 198289,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NEG (total)",
            "value": 5035.54,
            "unit": "ns/DU",
            "extra": "median=629443ns  p95=635313ns  stddev=4992.2ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] NEG (handler)",
            "value": 1.45,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NEG (DU/s)",
            "value": 198588,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOD (total)",
            "value": 5043.39,
            "unit": "ns/DU",
            "extra": "median=630424ns  p95=634893ns  stddev=4852.5ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOD (handler)",
            "value": 9.3,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOD (DU/s)",
            "value": 198279,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] AND (total)",
            "value": 5035.62,
            "unit": "ns/DU",
            "extra": "median=629452ns  p95=635634ns  stddev=5283.6ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] AND (handler)",
            "value": 1.52,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] AND (DU/s)",
            "value": 198585,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] OR (total)",
            "value": 5043.16,
            "unit": "ns/DU",
            "extra": "median=630395ns  p95=635133ns  stddev=5050.1ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] OR (handler)",
            "value": 9.06,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] OR (DU/s)",
            "value": 198288,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XOR (total)",
            "value": 5040.11,
            "unit": "ns/DU",
            "extra": "median=630014ns  p95=635805ns  stddev=5303.5ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] XOR (handler)",
            "value": 6.02,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XOR (DU/s)",
            "value": 198408,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOT (total)",
            "value": 5045.56,
            "unit": "ns/DU",
            "extra": "median=630695ns  p95=635504ns  stddev=4759.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOT (handler)",
            "value": 11.46,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOT (DU/s)",
            "value": 198194,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHL (total)",
            "value": 5026.56,
            "unit": "ns/DU",
            "extra": "median=628320ns  p95=631215ns  stddev=4261.0ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHL (handler)",
            "value": -7.54,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHL (DU/s)",
            "value": 198943,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHR (total)",
            "value": 5026.72,
            "unit": "ns/DU",
            "extra": "median=628340ns  p95=634151ns  stddev=5418.4ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHR (handler)",
            "value": -7.38,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHR (DU/s)",
            "value": 198937,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SAR (total)",
            "value": 5039.63,
            "unit": "ns/DU",
            "extra": "median=629954ns  p95=639080ns  stddev=5836.4ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] SAR (handler)",
            "value": 5.54,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SAR (DU/s)",
            "value": 198427,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROL (total)",
            "value": 5035.86,
            "unit": "ns/DU",
            "extra": "median=629482ns  p95=634122ns  stddev=5063.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROL (handler)",
            "value": 1.76,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROL (DU/s)",
            "value": 198576,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROR (total)",
            "value": 5039.07,
            "unit": "ns/DU",
            "extra": "median=629884ns  p95=634812ns  stddev=5272.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROR (handler)",
            "value": 4.98,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROR (DU/s)",
            "value": 198449,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMP (total)",
            "value": 5018.3,
            "unit": "ns/DU",
            "extra": "median=627288ns  p95=631437ns  stddev=4899.5ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMP (handler)",
            "value": -15.79,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMP (DU/s)",
            "value": 199271,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TEST (total)",
            "value": 5035.22,
            "unit": "ns/DU",
            "extra": "median=629402ns  p95=633790ns  stddev=4651.1ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] TEST (handler)",
            "value": 1.12,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TEST (DU/s)",
            "value": 198601,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (total)",
            "value": 5045,
            "unit": "ns/DU",
            "extra": "median=630625ns  p95=636596ns  stddev=5174.9ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (handler)",
            "value": 10.9,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (DU/s)",
            "value": 198216,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (total)",
            "value": 5050.13,
            "unit": "ns/DU",
            "extra": "median=631266ns  p95=635834ns  stddev=4409.6ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (handler)",
            "value": 16.03,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (DU/s)",
            "value": 198015,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JMP (total)",
            "value": 5043.63,
            "unit": "ns/DU",
            "extra": "median=630454ns  p95=634833ns  stddev=4727.9ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] JMP (handler)",
            "value": 9.54,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JMP (DU/s)",
            "value": 198270,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JCC (total)",
            "value": 5031.94,
            "unit": "ns/DU",
            "extra": "median=628992ns  p95=634372ns  stddev=4860.2ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] JCC (handler)",
            "value": -2.16,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JCC (DU/s)",
            "value": 198731,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (total)",
            "value": 5156.97,
            "unit": "ns/DU",
            "extra": "median=644621ns  p95=648828ns  stddev=4667.1ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (handler)",
            "value": 122.87,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (DU/s)",
            "value": 193912,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT8 (total)",
            "value": 5043.24,
            "unit": "ns/DU",
            "extra": "median=630405ns  p95=635554ns  stddev=5129.0ns  iterations=110  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT8 (handler)",
            "value": 9.14,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT8 (DU/s)",
            "value": 198285,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT16 (total)",
            "value": 5048.92,
            "unit": "ns/DU",
            "extra": "median=631115ns  p95=635985ns  stddev=4486.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT16 (handler)",
            "value": 14.82,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT16 (DU/s)",
            "value": 198062,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT32 (total)",
            "value": 5045.55,
            "unit": "ns/DU",
            "extra": "median=630694ns  p95=637077ns  stddev=5374.8ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT32 (handler)",
            "value": 11.46,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT32 (DU/s)",
            "value": 198194,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (total)",
            "value": 5041.87,
            "unit": "ns/DU",
            "extra": "median=630234ns  p95=633520ns  stddev=4940.3ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (handler)",
            "value": 7.78,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (DU/s)",
            "value": 198339,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (total)",
            "value": 5046.04,
            "unit": "ns/DU",
            "extra": "median=630755ns  p95=634231ns  stddev=4248.1ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (handler)",
            "value": 11.94,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (DU/s)",
            "value": 198175,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (total)",
            "value": 5044.6,
            "unit": "ns/DU",
            "extra": "median=630575ns  p95=635735ns  stddev=5365.4ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (handler)",
            "value": 10.5,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (DU/s)",
            "value": 198232,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (total)",
            "value": 5042.52,
            "unit": "ns/DU",
            "extra": "median=630315ns  p95=635584ns  stddev=4882.7ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (handler)",
            "value": 8.42,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (DU/s)",
            "value": 198314,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (total)",
            "value": 5043.79,
            "unit": "ns/DU",
            "extra": "median=630474ns  p95=638830ns  stddev=5905.1ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (handler)",
            "value": 9.7,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (DU/s)",
            "value": 198264,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (total)",
            "value": 5016.86,
            "unit": "ns/DU",
            "extra": "median=627108ns  p95=631557ns  stddev=5170.9ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (handler)",
            "value": -17.23,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (DU/s)",
            "value": 199328,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XCHG (total)",
            "value": 5033.46,
            "unit": "ns/DU",
            "extra": "median=629182ns  p95=635935ns  stddev=5551.2ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] XCHG (handler)",
            "value": -0.64,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XCHG (DU/s)",
            "value": 198671,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (total)",
            "value": 5026.88,
            "unit": "ns/DU",
            "extra": "median=628360ns  p95=633470ns  stddev=5059.4ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (handler)",
            "value": -7.22,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (DU/s)",
            "value": 198931,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] FENCE (total)",
            "value": 5019.5,
            "unit": "ns/DU",
            "extra": "median=627438ns  p95=633280ns  stddev=4959.6ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] FENCE (handler)",
            "value": -14.59,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] FENCE (DU/s)",
            "value": 199223,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (total)",
            "value": 5040.74,
            "unit": "ns/DU",
            "extra": "median=630093ns  p95=636937ns  stddev=5504.2ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (handler)",
            "value": 6.65,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (DU/s)",
            "value": 198383,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 5045.63,
            "unit": "ns/DU",
            "extra": "median=630704ns  p95=633841ns  stddev=4558.4ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": 11.54,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 198191,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (total)",
            "value": 5039.38,
            "unit": "ns/DU",
            "extra": "median=629923ns  p95=634332ns  stddev=4576.3ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (handler)",
            "value": 5.29,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 198437,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (total)",
            "value": 5047.25,
            "unit": "ns/DU",
            "extra": "median=630906ns  p95=636756ns  stddev=4951.3ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (handler)",
            "value": 13.15,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (DU/s)",
            "value": 198128,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (total)",
            "value": 5042.67,
            "unit": "ns/DU",
            "extra": "median=630334ns  p95=634953ns  stddev=4917.9ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (handler)",
            "value": 8.58,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (DU/s)",
            "value": 198308,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 30886.09,
            "unit": "ns/DU",
            "extra": "median=3860761ns  p95=3883375ns  stddev=12030.7ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 32377,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOVE (total)",
            "value": 30820.53,
            "unit": "ns/DU",
            "extra": "median=3852566ns  p95=3872242ns  stddev=11273.2ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOVE (handler)",
            "value": -65.56,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOVE (DU/s)",
            "value": 32446,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD (total)",
            "value": 30819.33,
            "unit": "ns/DU",
            "extra": "median=3852416ns  p95=3878823ns  stddev=11938.4ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD (handler)",
            "value": -66.76,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD (DU/s)",
            "value": 32447,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE (total)",
            "value": 30805.23,
            "unit": "ns/DU",
            "extra": "median=3850654ns  p95=3881118ns  stddev=13530.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE (handler)",
            "value": -80.86,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE (DU/s)",
            "value": 32462,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] PUSH (total)",
            "value": 30923.83,
            "unit": "ns/DU",
            "extra": "median=3865479ns  p95=3886019ns  stddev=11090.7ns  iterations=104  DUs=125"
          },
          {
            "name": "[StandardPolicy] PUSH (handler)",
            "value": 37.74,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] PUSH (DU/s)",
            "value": 32338,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] POP (total)",
            "value": 30906.77,
            "unit": "ns/DU",
            "extra": "median=3863346ns  p95=3894475ns  stddev=13963.2ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] POP (handler)",
            "value": 20.68,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] POP (DU/s)",
            "value": 32355,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (total)",
            "value": 30897.72,
            "unit": "ns/DU",
            "extra": "median=3862215ns  p95=3888021ns  stddev=12513.9ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (handler)",
            "value": 11.63,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (DU/s)",
            "value": 32365,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (total)",
            "value": 30911.82,
            "unit": "ns/DU",
            "extra": "median=3863978ns  p95=3889657ns  stddev=11564.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (handler)",
            "value": 25.74,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (DU/s)",
            "value": 32350,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (total)",
            "value": 30882.88,
            "unit": "ns/DU",
            "extra": "median=3860360ns  p95=3888973ns  stddev=11597.8ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (handler)",
            "value": -3.21,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (DU/s)",
            "value": 32380,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ADD (total)",
            "value": 30798.5,
            "unit": "ns/DU",
            "extra": "median=3849813ns  p95=3871550ns  stddev=8358.9ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] ADD (handler)",
            "value": -87.58,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ADD (DU/s)",
            "value": 32469,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SUB (total)",
            "value": 30829.51,
            "unit": "ns/DU",
            "extra": "median=3853689ns  p95=3878843ns  stddev=12492.1ns  iterations=107  DUs=125"
          },
          {
            "name": "[StandardPolicy] SUB (handler)",
            "value": -56.58,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SUB (DU/s)",
            "value": 32436,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUL (total)",
            "value": 30833.12,
            "unit": "ns/DU",
            "extra": "median=3854140ns  p95=3872703ns  stddev=10748.5ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUL (handler)",
            "value": -52.97,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUL (DU/s)",
            "value": 32433,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IMUL (total)",
            "value": 30815.07,
            "unit": "ns/DU",
            "extra": "median=3851884ns  p95=3878686ns  stddev=11106.9ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] IMUL (handler)",
            "value": -71.02,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IMUL (DU/s)",
            "value": 32452,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] DIV (total)",
            "value": 30820.68,
            "unit": "ns/DU",
            "extra": "median=3852585ns  p95=3870218ns  stddev=9874.5ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] DIV (handler)",
            "value": -65.41,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] DIV (DU/s)",
            "value": 32446,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IDIV (total)",
            "value": 30842.48,
            "unit": "ns/DU",
            "extra": "median=3855310ns  p95=3881331ns  stddev=12663.0ns  iterations=107  DUs=125"
          },
          {
            "name": "[StandardPolicy] IDIV (handler)",
            "value": -43.61,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IDIV (DU/s)",
            "value": 32423,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NEG (total)",
            "value": 30908.92,
            "unit": "ns/DU",
            "extra": "median=3863615ns  p95=3880739ns  stddev=10597.3ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] NEG (handler)",
            "value": 22.83,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NEG (DU/s)",
            "value": 32353,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOD (total)",
            "value": 30820.87,
            "unit": "ns/DU",
            "extra": "median=3852609ns  p95=3870430ns  stddev=10405.6ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOD (handler)",
            "value": -65.22,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOD (DU/s)",
            "value": 32446,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] AND (total)",
            "value": 30810.67,
            "unit": "ns/DU",
            "extra": "median=3851334ns  p95=3877201ns  stddev=12059.0ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] AND (handler)",
            "value": -75.42,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] AND (DU/s)",
            "value": 32456,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] OR (total)",
            "value": 30829.74,
            "unit": "ns/DU",
            "extra": "median=3853717ns  p95=3888963ns  stddev=14959.9ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] OR (handler)",
            "value": -56.35,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] OR (DU/s)",
            "value": 32436,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XOR (total)",
            "value": 30803.86,
            "unit": "ns/DU",
            "extra": "median=3850483ns  p95=3868656ns  stddev=9635.1ns  iterations=104  DUs=125"
          },
          {
            "name": "[StandardPolicy] XOR (handler)",
            "value": -82.22,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XOR (DU/s)",
            "value": 32463,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOT (total)",
            "value": 30872.06,
            "unit": "ns/DU",
            "extra": "median=3859007ns  p95=3879485ns  stddev=11344.0ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOT (handler)",
            "value": -14.03,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOT (DU/s)",
            "value": 32392,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHL (total)",
            "value": 30835.75,
            "unit": "ns/DU",
            "extra": "median=3854469ns  p95=3877774ns  stddev=11809.3ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHL (handler)",
            "value": -50.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHL (DU/s)",
            "value": 32430,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHR (total)",
            "value": 30824.37,
            "unit": "ns/DU",
            "extra": "median=3853046ns  p95=3870651ns  stddev=9648.1ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHR (handler)",
            "value": -61.72,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHR (DU/s)",
            "value": 32442,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SAR (total)",
            "value": 30814.45,
            "unit": "ns/DU",
            "extra": "median=3851806ns  p95=3877092ns  stddev=10971.0ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] SAR (handler)",
            "value": -71.64,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SAR (DU/s)",
            "value": 32452,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROL (total)",
            "value": 30854.34,
            "unit": "ns/DU",
            "extra": "median=3856793ns  p95=3880717ns  stddev=10645.8ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROL (handler)",
            "value": -31.74,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROL (DU/s)",
            "value": 32410,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROR (total)",
            "value": 30827.66,
            "unit": "ns/DU",
            "extra": "median=3853458ns  p95=3871933ns  stddev=10025.0ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROR (handler)",
            "value": -58.42,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROR (DU/s)",
            "value": 32438,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMP (total)",
            "value": 30807.55,
            "unit": "ns/DU",
            "extra": "median=3850944ns  p95=3879355ns  stddev=11114.2ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMP (handler)",
            "value": -78.54,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMP (DU/s)",
            "value": 32460,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TEST (total)",
            "value": 30814.59,
            "unit": "ns/DU",
            "extra": "median=3851824ns  p95=3885358ns  stddev=13902.5ns  iterations=104  DUs=125"
          },
          {
            "name": "[StandardPolicy] TEST (handler)",
            "value": -71.5,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TEST (DU/s)",
            "value": 32452,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (total)",
            "value": 30910.14,
            "unit": "ns/DU",
            "extra": "median=3863768ns  p95=3885398ns  stddev=11323.1ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (handler)",
            "value": 24.06,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (DU/s)",
            "value": 32352,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (total)",
            "value": 30896.27,
            "unit": "ns/DU",
            "extra": "median=3862034ns  p95=3884805ns  stddev=10489.8ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (handler)",
            "value": 10.18,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (DU/s)",
            "value": 32366,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JMP (total)",
            "value": 30913.82,
            "unit": "ns/DU",
            "extra": "median=3864228ns  p95=3897920ns  stddev=16129.4ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] JMP (handler)",
            "value": 27.74,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JMP (DU/s)",
            "value": 32348,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JCC (total)",
            "value": 30921.52,
            "unit": "ns/DU",
            "extra": "median=3865190ns  p95=3884987ns  stddev=10990.5ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] JCC (handler)",
            "value": 35.43,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JCC (DU/s)",
            "value": 32340,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (total)",
            "value": 31015.12,
            "unit": "ns/DU",
            "extra": "median=3876890ns  p95=3893503ns  stddev=8226.5ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (handler)",
            "value": 129.03,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (DU/s)",
            "value": 32242,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT8 (total)",
            "value": 30903.07,
            "unit": "ns/DU",
            "extra": "median=3862884ns  p95=3889386ns  stddev=12737.9ns  iterations=104  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT8 (handler)",
            "value": 16.98,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT8 (DU/s)",
            "value": 32359,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT16 (total)",
            "value": 30881.83,
            "unit": "ns/DU",
            "extra": "median=3860229ns  p95=3892141ns  stddev=13539.3ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT16 (handler)",
            "value": -4.26,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT16 (DU/s)",
            "value": 32381,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT32 (total)",
            "value": 30892.5,
            "unit": "ns/DU",
            "extra": "median=3861563ns  p95=3895385ns  stddev=14024.9ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT32 (handler)",
            "value": 6.42,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT32 (DU/s)",
            "value": 32370,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (total)",
            "value": 30909.49,
            "unit": "ns/DU",
            "extra": "median=3863686ns  p95=3892058ns  stddev=12271.1ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (handler)",
            "value": 23.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (DU/s)",
            "value": 32353,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (total)",
            "value": 30883.54,
            "unit": "ns/DU",
            "extra": "median=3860442ns  p95=3885709ns  stddev=11548.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (handler)",
            "value": -2.55,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (DU/s)",
            "value": 32380,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (total)",
            "value": 30901.63,
            "unit": "ns/DU",
            "extra": "median=3862704ns  p95=3884136ns  stddev=12476.6ns  iterations=107  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (handler)",
            "value": 15.54,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (DU/s)",
            "value": 32361,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (total)",
            "value": 30866.45,
            "unit": "ns/DU",
            "extra": "median=3858306ns  p95=3875848ns  stddev=10297.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (handler)",
            "value": -19.64,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (DU/s)",
            "value": 32398,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (total)",
            "value": 30892.99,
            "unit": "ns/DU",
            "extra": "median=3861624ns  p95=3886691ns  stddev=12294.2ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (handler)",
            "value": 6.9,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (DU/s)",
            "value": 32370,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (total)",
            "value": 30816.37,
            "unit": "ns/DU",
            "extra": "median=3852046ns  p95=3880937ns  stddev=13397.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (handler)",
            "value": -69.72,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (DU/s)",
            "value": 32450,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XCHG (total)",
            "value": 30827.74,
            "unit": "ns/DU",
            "extra": "median=3853468ns  p95=3879205ns  stddev=11953.4ns  iterations=106  DUs=125"
          },
          {
            "name": "[StandardPolicy] XCHG (handler)",
            "value": -58.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XCHG (DU/s)",
            "value": 32438,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (total)",
            "value": 30813.08,
            "unit": "ns/DU",
            "extra": "median=3851635ns  p95=3871793ns  stddev=10107.3ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (handler)",
            "value": -73.01,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (DU/s)",
            "value": 32454,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] FENCE (total)",
            "value": 30889.3,
            "unit": "ns/DU",
            "extra": "median=3861163ns  p95=3880137ns  stddev=9544.4ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] FENCE (handler)",
            "value": 3.22,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] FENCE (DU/s)",
            "value": 32374,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (total)",
            "value": 30816.53,
            "unit": "ns/DU",
            "extra": "median=3852066ns  p95=3878415ns  stddev=12101.3ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (handler)",
            "value": -69.56,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (DU/s)",
            "value": 32450,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 30886.98,
            "unit": "ns/DU",
            "extra": "median=3860872ns  p95=3895757ns  stddev=14769.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 0.89,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 32376,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (total)",
            "value": 30905.74,
            "unit": "ns/DU",
            "extra": "median=3863218ns  p95=3888325ns  stddev=11656.8ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (handler)",
            "value": 19.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 32356,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (total)",
            "value": 30882.15,
            "unit": "ns/DU",
            "extra": "median=3860269ns  p95=3882380ns  stddev=11123.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (handler)",
            "value": -3.94,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (DU/s)",
            "value": 32381,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (total)",
            "value": 30892.26,
            "unit": "ns/DU",
            "extra": "median=3861532ns  p95=3883354ns  stddev=10635.6ns  iterations=105  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (handler)",
            "value": 6.17,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (DU/s)",
            "value": 32371,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 58421.13,
            "unit": "ns/DU",
            "extra": "median=7302641ns  p95=7336102ns  stddev=18663.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 17117,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOVE (total)",
            "value": 58415.5,
            "unit": "ns/DU",
            "extra": "median=7301938ns  p95=7342814ns  stddev=18791.5ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOVE (handler)",
            "value": -5.62,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOVE (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD (total)",
            "value": 58429.14,
            "unit": "ns/DU",
            "extra": "median=7303642ns  p95=7338242ns  stddev=19416.1ns  iterations=108  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD (handler)",
            "value": 8.01,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD (DU/s)",
            "value": 17115,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE (total)",
            "value": 58418.09,
            "unit": "ns/DU",
            "extra": "median=7302261ns  p95=7341322ns  stddev=16566.5ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE (handler)",
            "value": -3.04,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE (DU/s)",
            "value": 17118,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] PUSH (total)",
            "value": 58416.8,
            "unit": "ns/DU",
            "extra": "median=7302100ns  p95=7341091ns  stddev=18164.3ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] PUSH (handler)",
            "value": -4.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] PUSH (DU/s)",
            "value": 17118,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] POP (total)",
            "value": 58306.2,
            "unit": "ns/DU",
            "extra": "median=7288275ns  p95=7326698ns  stddev=17599.9ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] POP (handler)",
            "value": -114.93,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] POP (DU/s)",
            "value": 17151,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (total)",
            "value": 58423.42,
            "unit": "ns/DU",
            "extra": "median=7302928ns  p95=7352948ns  stddev=22054.0ns  iterations=108  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (handler)",
            "value": 2.3,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (DU/s)",
            "value": 17116,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (total)",
            "value": 58429.86,
            "unit": "ns/DU",
            "extra": "median=7303732ns  p95=7348380ns  stddev=18738.2ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (handler)",
            "value": 8.73,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (DU/s)",
            "value": 17115,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (total)",
            "value": 58421.06,
            "unit": "ns/DU",
            "extra": "median=7302632ns  p95=7344738ns  stddev=19447.9ns  iterations=109  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (handler)",
            "value": -0.07,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (DU/s)",
            "value": 17117,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ADD (total)",
            "value": 58389.14,
            "unit": "ns/DU",
            "extra": "median=7298643ns  p95=7343271ns  stddev=19928.2ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ADD (handler)",
            "value": -31.98,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ADD (DU/s)",
            "value": 17126,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SUB (total)",
            "value": 58450.58,
            "unit": "ns/DU",
            "extra": "median=7306322ns  p95=7339338ns  stddev=16392.2ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SUB (handler)",
            "value": 29.45,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SUB (DU/s)",
            "value": 17108,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUL (total)",
            "value": 58422.57,
            "unit": "ns/DU",
            "extra": "median=7302821ns  p95=7331881ns  stddev=14776.7ns  iterations=108  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUL (handler)",
            "value": 1.44,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUL (DU/s)",
            "value": 17117,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IMUL (total)",
            "value": 58424.44,
            "unit": "ns/DU",
            "extra": "median=7303055ns  p95=7339449ns  stddev=16679.5ns  iterations=109  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IMUL (handler)",
            "value": 3.31,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IMUL (DU/s)",
            "value": 17116,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] DIV (total)",
            "value": 58415.62,
            "unit": "ns/DU",
            "extra": "median=7301953ns  p95=7343794ns  stddev=18718.3ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] DIV (handler)",
            "value": -5.5,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] DIV (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IDIV (total)",
            "value": 58386.02,
            "unit": "ns/DU",
            "extra": "median=7298252ns  p95=7355145ns  stddev=20365.0ns  iterations=108  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IDIV (handler)",
            "value": -35.11,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IDIV (DU/s)",
            "value": 17127,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NEG (total)",
            "value": 58414.58,
            "unit": "ns/DU",
            "extra": "median=7301822ns  p95=7346427ns  stddev=20780.6ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NEG (handler)",
            "value": -6.55,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NEG (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOD (total)",
            "value": 58411.62,
            "unit": "ns/DU",
            "extra": "median=7301452ns  p95=7328247ns  stddev=13745.7ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOD (handler)",
            "value": -9.51,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOD (DU/s)",
            "value": 17120,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] AND (total)",
            "value": 58409.24,
            "unit": "ns/DU",
            "extra": "median=7301155ns  p95=7336617ns  stddev=17268.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] AND (handler)",
            "value": -11.89,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] AND (DU/s)",
            "value": 17121,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] OR (total)",
            "value": 58410.46,
            "unit": "ns/DU",
            "extra": "median=7301307ns  p95=7335433ns  stddev=18465.9ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] OR (handler)",
            "value": -10.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] OR (DU/s)",
            "value": 17120,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XOR (total)",
            "value": 58421.58,
            "unit": "ns/DU",
            "extra": "median=7302697ns  p95=7356019ns  stddev=22134.9ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XOR (handler)",
            "value": 0.45,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XOR (DU/s)",
            "value": 17117,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOT (total)",
            "value": 58433.22,
            "unit": "ns/DU",
            "extra": "median=7304153ns  p95=7348447ns  stddev=19946.5ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOT (handler)",
            "value": 12.1,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOT (DU/s)",
            "value": 17114,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHL (total)",
            "value": 58431.76,
            "unit": "ns/DU",
            "extra": "median=7303970ns  p95=7344823ns  stddev=19617.4ns  iterations=108  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHL (handler)",
            "value": 10.63,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHL (DU/s)",
            "value": 17114,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHR (total)",
            "value": 58416.31,
            "unit": "ns/DU",
            "extra": "median=7302039ns  p95=7335566ns  stddev=17554.8ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHR (handler)",
            "value": -4.82,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHR (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SAR (total)",
            "value": 58422.4,
            "unit": "ns/DU",
            "extra": "median=7302800ns  p95=7348880ns  stddev=22320.8ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SAR (handler)",
            "value": 1.27,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SAR (DU/s)",
            "value": 17117,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROL (total)",
            "value": 58398.07,
            "unit": "ns/DU",
            "extra": "median=7299759ns  p95=7342104ns  stddev=18137.1ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROL (handler)",
            "value": -23.06,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROL (DU/s)",
            "value": 17124,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROR (total)",
            "value": 58447.53,
            "unit": "ns/DU",
            "extra": "median=7305941ns  p95=7338671ns  stddev=18912.0ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROR (handler)",
            "value": 26.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROR (DU/s)",
            "value": 17109,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMP (total)",
            "value": 58417.42,
            "unit": "ns/DU",
            "extra": "median=7302178ns  p95=7345551ns  stddev=20254.9ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMP (handler)",
            "value": -3.7,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMP (DU/s)",
            "value": 17118,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TEST (total)",
            "value": 58401.32,
            "unit": "ns/DU",
            "extra": "median=7300165ns  p95=7335812ns  stddev=16263.1ns  iterations=108  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TEST (handler)",
            "value": -19.81,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TEST (DU/s)",
            "value": 17123,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (total)",
            "value": 58416.24,
            "unit": "ns/DU",
            "extra": "median=7302030ns  p95=7327731ns  stddev=16765.5ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (handler)",
            "value": -4.89,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (total)",
            "value": 58400.84,
            "unit": "ns/DU",
            "extra": "median=7300105ns  p95=7338978ns  stddev=16855.3ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (handler)",
            "value": -20.29,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (DU/s)",
            "value": 17123,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JMP (total)",
            "value": 58406.78,
            "unit": "ns/DU",
            "extra": "median=7300847ns  p95=7345344ns  stddev=20259.6ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JMP (handler)",
            "value": -14.35,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JMP (DU/s)",
            "value": 17121,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JCC (total)",
            "value": 58414.1,
            "unit": "ns/DU",
            "extra": "median=7301763ns  p95=7352692ns  stddev=22433.4ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JCC (handler)",
            "value": -7.02,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JCC (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (total)",
            "value": 58571.56,
            "unit": "ns/DU",
            "extra": "median=7321445ns  p95=7353765ns  stddev=17206.6ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (handler)",
            "value": 150.43,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (DU/s)",
            "value": 17073,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (total)",
            "value": 58384.26,
            "unit": "ns/DU",
            "extra": "median=7298032ns  p95=7332259ns  stddev=15526.7ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (handler)",
            "value": -36.87,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (DU/s)",
            "value": 17128,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (total)",
            "value": 58348.35,
            "unit": "ns/DU",
            "extra": "median=7293544ns  p95=7333317ns  stddev=19010.5ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (handler)",
            "value": -72.78,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (DU/s)",
            "value": 17138,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (total)",
            "value": 58374.35,
            "unit": "ns/DU",
            "extra": "median=7296794ns  p95=7327457ns  stddev=15139.9ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (handler)",
            "value": -46.78,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (DU/s)",
            "value": 17131,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (total)",
            "value": 58388.83,
            "unit": "ns/DU",
            "extra": "median=7298604ns  p95=7337008ns  stddev=17970.6ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (handler)",
            "value": -32.3,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (DU/s)",
            "value": 17127,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (total)",
            "value": 58373.43,
            "unit": "ns/DU",
            "extra": "median=7296679ns  p95=7340090ns  stddev=20096.4ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (handler)",
            "value": -47.7,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (DU/s)",
            "value": 17131,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (total)",
            "value": 58369.35,
            "unit": "ns/DU",
            "extra": "median=7296169ns  p95=7327896ns  stddev=15191.5ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (handler)",
            "value": -51.78,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (DU/s)",
            "value": 17132,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (total)",
            "value": 58413.65,
            "unit": "ns/DU",
            "extra": "median=7301706ns  p95=7340030ns  stddev=19889.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (handler)",
            "value": -7.48,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (total)",
            "value": 58403.25,
            "unit": "ns/DU",
            "extra": "median=7300406ns  p95=7342909ns  stddev=18765.0ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (handler)",
            "value": -17.88,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (DU/s)",
            "value": 17122,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (total)",
            "value": 58412.56,
            "unit": "ns/DU",
            "extra": "median=7301570ns  p95=7333471ns  stddev=16226.4ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (handler)",
            "value": -8.57,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (DU/s)",
            "value": 17120,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XCHG (total)",
            "value": 58415.78,
            "unit": "ns/DU",
            "extra": "median=7301973ns  p95=7332225ns  stddev=15371.7ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XCHG (handler)",
            "value": -5.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XCHG (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (total)",
            "value": 58486.36,
            "unit": "ns/DU",
            "extra": "median=7310795ns  p95=7353585ns  stddev=22931.7ns  iterations=109  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (handler)",
            "value": 65.23,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (DU/s)",
            "value": 17098,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] FENCE (total)",
            "value": 58398.91,
            "unit": "ns/DU",
            "extra": "median=7299864ns  p95=7336809ns  stddev=18026.1ns  iterations=109  DUs=125"
          },
          {
            "name": "[HighSecPolicy] FENCE (handler)",
            "value": -22.22,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] FENCE (DU/s)",
            "value": 17124,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (total)",
            "value": 58431.23,
            "unit": "ns/DU",
            "extra": "median=7303904ns  p95=7350039ns  stddev=17729.0ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (handler)",
            "value": 10.1,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (DU/s)",
            "value": 17114,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 58372.06,
            "unit": "ns/DU",
            "extra": "median=7296508ns  p95=7340200ns  stddev=18650.2ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": -49.06,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 17131,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (total)",
            "value": 58414.26,
            "unit": "ns/DU",
            "extra": "median=7301783ns  p95=7342917ns  stddev=20566.4ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (handler)",
            "value": -6.86,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 17119,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (total)",
            "value": 58382.95,
            "unit": "ns/DU",
            "extra": "median=7297869ns  p95=7333538ns  stddev=16934.8ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (handler)",
            "value": -38.18,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (DU/s)",
            "value": 17128,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (total)",
            "value": 58399.39,
            "unit": "ns/DU",
            "extra": "median=7299924ns  p95=7343807ns  stddev=20825.7ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (handler)",
            "value": -21.74,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (DU/s)",
            "value": 17123,
            "unit": "DU/s"
          }
        ]
      }
    ]
  }
}