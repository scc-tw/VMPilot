window.BENCHMARK_DATA = {
  "lastUpdate": 1775272257225,
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
          "id": "8d5bd09e091ab1924c48c2c418232c2a3fa6cc9c",
          "message": "fix: update benchmark scripts for DU-based metrics\n\nBenchmark output changed from per-instruction to per-dispatch-unit:\n  ns_per_insn → ns_per_du, handler_ns → delta_ns,\n  insn_count → du_count, ips → du_per_sec,\n  --insns → --dus\n\nScripts accept both old and new field names for backward compatibility.",
          "timestamp": "2026-04-04T00:23:51+08:00",
          "tree_id": "2eac5612a2d2d8e35a4e51976b038887ad38627d",
          "url": "https://github.com/scc-tw/VMPilot/commit/8d5bd09e091ab1924c48c2c418232c2a3fa6cc9c"
        },
        "date": 1775233541515,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5749.6,
            "unit": "ns/DU",
            "extra": "median=718700ns  p95=768800ns  stddev=19701.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 173925,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5819.2,
            "unit": "ns/DU",
            "extra": "median=727400ns  p95=768000ns  stddev=15636.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 69.6,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 171845,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5804.8,
            "unit": "ns/DU",
            "extra": "median=725600ns  p95=751900ns  stddev=12806.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 55.2,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 172271,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5774.4,
            "unit": "ns/DU",
            "extra": "median=721800ns  p95=746500ns  stddev=9931.4ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 24.8,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 173178,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=749100ns  stddev=12626.2ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 7.2,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5928,
            "unit": "ns/DU",
            "extra": "median=741000ns  p95=877900ns  stddev=41768.1ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 178.4,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 168691,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5845.6,
            "unit": "ns/DU",
            "extra": "median=730700ns  p95=773000ns  stddev=16438.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 96,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 171069,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5873.6,
            "unit": "ns/DU",
            "extra": "median=734200ns  p95=737500ns  stddev=5123.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 124,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 170253,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5768.8,
            "unit": "ns/DU",
            "extra": "median=721100ns  p95=814500ns  stddev=27894.9ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 19.2,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 173346,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5859.2,
            "unit": "ns/DU",
            "extra": "median=732400ns  p95=778800ns  stddev=17880.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 109.6,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 170672,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5816.8,
            "unit": "ns/DU",
            "extra": "median=727100ns  p95=757900ns  stddev=13933.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 67.2,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 171916,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5814.4,
            "unit": "ns/DU",
            "extra": "median=726800ns  p95=770900ns  stddev=14808.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 64.8,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 171987,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5935.2,
            "unit": "ns/DU",
            "extra": "median=741900ns  p95=790600ns  stddev=22166.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 185.6,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 168486,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5964.8,
            "unit": "ns/DU",
            "extra": "median=745600ns  p95=799500ns  stddev=19906.4ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 215.2,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 167650,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5960.8,
            "unit": "ns/DU",
            "extra": "median=745100ns  p95=759200ns  stddev=10430.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 211.2,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 167763,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5891.2,
            "unit": "ns/DU",
            "extra": "median=736400ns  p95=776000ns  stddev=18602.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 141.6,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 169745,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 6031.2,
            "unit": "ns/DU",
            "extra": "median=753900ns  p95=766700ns  stddev=15071.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 281.6,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 165804,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5829.6,
            "unit": "ns/DU",
            "extra": "median=728700ns  p95=769900ns  stddev=17273.2ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 80,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 171538,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 6017.6,
            "unit": "ns/DU",
            "extra": "median=752200ns  p95=846000ns  stddev=32414.6ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 268,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 166179,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5817.6,
            "unit": "ns/DU",
            "extra": "median=727200ns  p95=827200ns  stddev=35399.0ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 68,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 171892,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5987.2,
            "unit": "ns/DU",
            "extra": "median=748400ns  p95=789000ns  stddev=22935.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 237.6,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 167023,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5820.8,
            "unit": "ns/DU",
            "extra": "median=727600ns  p95=798400ns  stddev=32598.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 71.2,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 171798,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 6020,
            "unit": "ns/DU",
            "extra": "median=752500ns  p95=781200ns  stddev=21367.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 270.4,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 166113,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5824,
            "unit": "ns/DU",
            "extra": "median=728000ns  p95=782600ns  stddev=21600.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 74.4,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 171703,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5853.6,
            "unit": "ns/DU",
            "extra": "median=731700ns  p95=780400ns  stddev=17862.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 104,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 170835,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5816,
            "unit": "ns/DU",
            "extra": "median=727000ns  p95=774800ns  stddev=21136.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 171939,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5789.6,
            "unit": "ns/DU",
            "extra": "median=723700ns  p95=807300ns  stddev=25748.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 40,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 172724,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5920,
            "unit": "ns/DU",
            "extra": "median=740000ns  p95=751600ns  stddev=7501.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 170.4,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 168919,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5728.8,
            "unit": "ns/DU",
            "extra": "median=716100ns  p95=745400ns  stddev=12962.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -20.8,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 174557,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5795.2,
            "unit": "ns/DU",
            "extra": "median=724400ns  p95=753800ns  stddev=13209.0ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 45.6,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 172557,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=747800ns  stddev=12920.8ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -15.2,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5732.8,
            "unit": "ns/DU",
            "extra": "median=716600ns  p95=775400ns  stddev=23984.1ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -16.8,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 174435,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 6208.8,
            "unit": "ns/DU",
            "extra": "median=776100ns  p95=806300ns  stddev=23404.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 459.2,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 161062,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5990.4,
            "unit": "ns/DU",
            "extra": "median=748800ns  p95=960300ns  stddev=66466.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 240.8,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 166934,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5949.6,
            "unit": "ns/DU",
            "extra": "median=743700ns  p95=780900ns  stddev=18126.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 200,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 168079,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 6012.8,
            "unit": "ns/DU",
            "extra": "median=751600ns  p95=962300ns  stddev=74556.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 263.2,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 166312,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5808.8,
            "unit": "ns/DU",
            "extra": "median=726100ns  p95=776200ns  stddev=17631.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 59.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 172153,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5954.4,
            "unit": "ns/DU",
            "extra": "median=744300ns  p95=761000ns  stddev=16511.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 204.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 167943,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5784,
            "unit": "ns/DU",
            "extra": "median=723000ns  p95=803900ns  stddev=24211.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 34.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 172891,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 6006.4,
            "unit": "ns/DU",
            "extra": "median=750800ns  p95=852400ns  stddev=40808.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 256.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 166489,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5778.4,
            "unit": "ns/DU",
            "extra": "median=722300ns  p95=784000ns  stddev=23214.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 28.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 173058,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5971.2,
            "unit": "ns/DU",
            "extra": "median=746400ns  p95=772700ns  stddev=18245.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 221.6,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 167471,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5780,
            "unit": "ns/DU",
            "extra": "median=722500ns  p95=765700ns  stddev=15546.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 173010,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5796,
            "unit": "ns/DU",
            "extra": "median=724500ns  p95=750500ns  stddev=11930.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 46.4,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 172533,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5739.2,
            "unit": "ns/DU",
            "extra": "median=717400ns  p95=745200ns  stddev=12998.2ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -10.4,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 174240,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5798.4,
            "unit": "ns/DU",
            "extra": "median=724800ns  p95=775300ns  stddev=18750.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 48.8,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 172461,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5740.8,
            "unit": "ns/DU",
            "extra": "median=717600ns  p95=752600ns  stddev=14863.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -8.8,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 174192,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5740,
            "unit": "ns/DU",
            "extra": "median=717500ns  p95=775200ns  stddev=19392.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 174216,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5747.2,
            "unit": "ns/DU",
            "extra": "median=718400ns  p95=788500ns  stddev=22713.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -2.4,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 173998,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5735.2,
            "unit": "ns/DU",
            "extra": "median=716900ns  p95=779900ns  stddev=20107.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 174362,
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
        "date": 1775233573643,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5813.6,
            "unit": "ns/DU",
            "extra": "median=726700ns  p95=791200ns  stddev=21691.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 172010,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5840.8,
            "unit": "ns/DU",
            "extra": "median=730100ns  p95=776000ns  stddev=15754.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 27.2,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 171209,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5804,
            "unit": "ns/DU",
            "extra": "median=725500ns  p95=797400ns  stddev=21945.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 172295,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5796.8,
            "unit": "ns/DU",
            "extra": "median=724600ns  p95=751300ns  stddev=10282.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -16.8,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 172509,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5849.6,
            "unit": "ns/DU",
            "extra": "median=731200ns  p95=743200ns  stddev=7791.0ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 36,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 170952,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5846.4,
            "unit": "ns/DU",
            "extra": "median=730800ns  p95=754100ns  stddev=10904.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 32.8,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 171045,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5793.6,
            "unit": "ns/DU",
            "extra": "median=724200ns  p95=749000ns  stddev=11513.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -20,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 172604,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5801.6,
            "unit": "ns/DU",
            "extra": "median=725200ns  p95=751900ns  stddev=12165.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -12,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 172366,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5799.2,
            "unit": "ns/DU",
            "extra": "median=724900ns  p95=755700ns  stddev=13116.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 172438,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5852.8,
            "unit": "ns/DU",
            "extra": "median=731600ns  p95=762400ns  stddev=12857.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 39.2,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 170858,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5845.6,
            "unit": "ns/DU",
            "extra": "median=730700ns  p95=755100ns  stddev=11585.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 32,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 171069,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5843.2,
            "unit": "ns/DU",
            "extra": "median=730400ns  p95=782900ns  stddev=16588.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 29.6,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 171139,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 6009.6,
            "unit": "ns/DU",
            "extra": "median=751200ns  p95=1013100ns  stddev=78940.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 196,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 166400,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5821.6,
            "unit": "ns/DU",
            "extra": "median=727700ns  p95=750900ns  stddev=10557.5ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 8,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 171774,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5842.4,
            "unit": "ns/DU",
            "extra": "median=730300ns  p95=758200ns  stddev=9681.5ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 28.8,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 171163,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5808,
            "unit": "ns/DU",
            "extra": "median=726000ns  p95=854800ns  stddev=36875.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -5.6,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 172176,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5942.4,
            "unit": "ns/DU",
            "extra": "median=742800ns  p95=750500ns  stddev=8142.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 128.8,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 168282,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5840.8,
            "unit": "ns/DU",
            "extra": "median=730100ns  p95=788800ns  stddev=18953.2ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 27.2,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 171209,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5886.4,
            "unit": "ns/DU",
            "extra": "median=735800ns  p95=881700ns  stddev=44952.0ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 72.8,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 169883,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5844,
            "unit": "ns/DU",
            "extra": "median=730500ns  p95=782600ns  stddev=17713.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 171116,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5807.2,
            "unit": "ns/DU",
            "extra": "median=725900ns  p95=748600ns  stddev=11037.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": -6.4,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 172200,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5865.6,
            "unit": "ns/DU",
            "extra": "median=733200ns  p95=756500ns  stddev=11041.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 52,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 170486,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5848,
            "unit": "ns/DU",
            "extra": "median=731000ns  p95=768100ns  stddev=14205.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 34.4,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 170999,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5924.8,
            "unit": "ns/DU",
            "extra": "median=740600ns  p95=756100ns  stddev=9874.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 111.2,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 168782,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5856.8,
            "unit": "ns/DU",
            "extra": "median=732100ns  p95=792900ns  stddev=18648.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 43.2,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 170742,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5890.4,
            "unit": "ns/DU",
            "extra": "median=736300ns  p95=766800ns  stddev=12984.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 76.8,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 169768,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5814.4,
            "unit": "ns/DU",
            "extra": "median=726800ns  p95=757000ns  stddev=12774.2ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 0.8,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 171987,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5825.6,
            "unit": "ns/DU",
            "extra": "median=728200ns  p95=765800ns  stddev=13071.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 171656,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5762.4,
            "unit": "ns/DU",
            "extra": "median=720300ns  p95=762800ns  stddev=14262.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -51.2,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 173539,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5800,
            "unit": "ns/DU",
            "extra": "median=725000ns  p95=748400ns  stddev=9415.3ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -13.6,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 172414,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5758.4,
            "unit": "ns/DU",
            "extra": "median=719800ns  p95=747600ns  stddev=10410.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -55.2,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 173659,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=764000ns  stddev=14988.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -45.6,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5979.2,
            "unit": "ns/DU",
            "extra": "median=747400ns  p95=773800ns  stddev=12599.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 165.6,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 167246,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5831.2,
            "unit": "ns/DU",
            "extra": "median=728900ns  p95=749800ns  stddev=9044.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 17.6,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 171491,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5882.4,
            "unit": "ns/DU",
            "extra": "median=735300ns  p95=771900ns  stddev=14252.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 68.8,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 169999,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5825.6,
            "unit": "ns/DU",
            "extra": "median=728200ns  p95=752900ns  stddev=9695.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 171656,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5829.6,
            "unit": "ns/DU",
            "extra": "median=728700ns  p95=746000ns  stddev=9652.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 16,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 171538,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5824,
            "unit": "ns/DU",
            "extra": "median=728000ns  p95=751900ns  stddev=11433.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 10.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 171703,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5872.8,
            "unit": "ns/DU",
            "extra": "median=734100ns  p95=752800ns  stddev=9645.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 59.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 170277,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5806.4,
            "unit": "ns/DU",
            "extra": "median=725800ns  p95=748600ns  stddev=11329.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 172224,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5816.8,
            "unit": "ns/DU",
            "extra": "median=727100ns  p95=766400ns  stddev=15001.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 171916,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5809.6,
            "unit": "ns/DU",
            "extra": "median=726200ns  p95=775100ns  stddev=16518.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -4,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 172129,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5801.6,
            "unit": "ns/DU",
            "extra": "median=725200ns  p95=774900ns  stddev=15853.2ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -12,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 172366,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5825.6,
            "unit": "ns/DU",
            "extra": "median=728200ns  p95=754700ns  stddev=10839.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 171656,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5758.4,
            "unit": "ns/DU",
            "extra": "median=719800ns  p95=747600ns  stddev=12347.9ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -55.2,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 173659,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5812.8,
            "unit": "ns/DU",
            "extra": "median=726600ns  p95=750200ns  stddev=11495.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -0.8,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 172034,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5837.6,
            "unit": "ns/DU",
            "extra": "median=729700ns  p95=798000ns  stddev=22687.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 24,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 171303,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5816.8,
            "unit": "ns/DU",
            "extra": "median=727100ns  p95=781900ns  stddev=17473.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 171916,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5804,
            "unit": "ns/DU",
            "extra": "median=725500ns  p95=743300ns  stddev=9953.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 172295,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5770.4,
            "unit": "ns/DU",
            "extra": "median=721300ns  p95=741600ns  stddev=9166.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -43.2,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 173298,
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
        "date": 1775235181109,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5703.2,
            "unit": "ns/DU",
            "extra": "median=712900ns  p95=736100ns  stddev=11934.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 175340,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5756,
            "unit": "ns/DU",
            "extra": "median=719500ns  p95=749400ns  stddev=13054.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 52.8,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 173732,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5721.6,
            "unit": "ns/DU",
            "extra": "median=715200ns  p95=745800ns  stddev=13967.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 18.4,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 174776,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5724.8,
            "unit": "ns/DU",
            "extra": "median=715600ns  p95=743500ns  stddev=12249.3ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 21.6,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 174679,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5731.2,
            "unit": "ns/DU",
            "extra": "median=716400ns  p95=747900ns  stddev=14399.7ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 28,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 174484,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5786.4,
            "unit": "ns/DU",
            "extra": "median=723300ns  p95=819800ns  stddev=28323.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 83.2,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 172819,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5700.8,
            "unit": "ns/DU",
            "extra": "median=712600ns  p95=999200ns  stddev=80425.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -2.4,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 175414,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5768.8,
            "unit": "ns/DU",
            "extra": "median=721100ns  p95=745200ns  stddev=11182.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 65.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 173346,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5723.2,
            "unit": "ns/DU",
            "extra": "median=715400ns  p95=742500ns  stddev=12671.3ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 20,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 174727,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5776.8,
            "unit": "ns/DU",
            "extra": "median=722100ns  p95=773700ns  stddev=17179.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 73.6,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 173106,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5845.6,
            "unit": "ns/DU",
            "extra": "median=730700ns  p95=788600ns  stddev=20106.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 142.4,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 171069,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5766.4,
            "unit": "ns/DU",
            "extra": "median=720800ns  p95=746700ns  stddev=12476.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 63.2,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 173418,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5818.4,
            "unit": "ns/DU",
            "extra": "median=727300ns  p95=777600ns  stddev=17112.5ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 115.2,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 171869,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5808,
            "unit": "ns/DU",
            "extra": "median=726000ns  p95=810500ns  stddev=25803.4ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 104.8,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 172176,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5761.6,
            "unit": "ns/DU",
            "extra": "median=720200ns  p95=749800ns  stddev=12798.8ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 58.4,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 173563,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5738.4,
            "unit": "ns/DU",
            "extra": "median=717300ns  p95=797200ns  stddev=24146.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 35.2,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 174265,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5776,
            "unit": "ns/DU",
            "extra": "median=722000ns  p95=773000ns  stddev=17949.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 72.8,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 173130,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5852,
            "unit": "ns/DU",
            "extra": "median=731500ns  p95=745600ns  stddev=10891.7ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 148.8,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 170882,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5753.6,
            "unit": "ns/DU",
            "extra": "median=719200ns  p95=766400ns  stddev=15727.6ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 50.4,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 173804,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5851.2,
            "unit": "ns/DU",
            "extra": "median=731400ns  p95=745500ns  stddev=7717.2ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 148,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 170905,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5744.8,
            "unit": "ns/DU",
            "extra": "median=718100ns  p95=744500ns  stddev=12738.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 41.6,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 174070,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5754.4,
            "unit": "ns/DU",
            "extra": "median=719300ns  p95=747700ns  stddev=12988.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 51.2,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 173780,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5800,
            "unit": "ns/DU",
            "extra": "median=725000ns  p95=763000ns  stddev=15347.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 96.8,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 172414,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5769.6,
            "unit": "ns/DU",
            "extra": "median=721200ns  p95=745700ns  stddev=10260.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 173322,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=768200ns  stddev=16150.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 64.8,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5772,
            "unit": "ns/DU",
            "extra": "median=721500ns  p95=763800ns  stddev=15949.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 68.8,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 173250,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5792,
            "unit": "ns/DU",
            "extra": "median=724000ns  p95=749600ns  stddev=12604.2ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 88.8,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 172652,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5800.8,
            "unit": "ns/DU",
            "extra": "median=725100ns  p95=759100ns  stddev=15531.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 97.6,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 172390,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5713.6,
            "unit": "ns/DU",
            "extra": "median=714200ns  p95=798300ns  stddev=25580.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 10.4,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 175021,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5743.2,
            "unit": "ns/DU",
            "extra": "median=717900ns  p95=743800ns  stddev=11300.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 40,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 174119,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5713.6,
            "unit": "ns/DU",
            "extra": "median=714200ns  p95=743800ns  stddev=12345.1ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 10.4,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 175021,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5870.4,
            "unit": "ns/DU",
            "extra": "median=733800ns  p95=876700ns  stddev=45277.8ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 167.2,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 170346,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5867.2,
            "unit": "ns/DU",
            "extra": "median=733400ns  p95=766300ns  stddev=12608.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 164,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 170439,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=752800ns  stddev=14299.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 53.6,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5736,
            "unit": "ns/DU",
            "extra": "median=717000ns  p95=758900ns  stddev=15755.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 32.8,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 174338,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5728.8,
            "unit": "ns/DU",
            "extra": "median=716100ns  p95=741200ns  stddev=11035.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 25.6,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 174557,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5742.4,
            "unit": "ns/DU",
            "extra": "median=717800ns  p95=975700ns  stddev=90845.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 39.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 174143,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5759.2,
            "unit": "ns/DU",
            "extra": "median=719900ns  p95=750500ns  stddev=13842.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 56,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 173635,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5747.2,
            "unit": "ns/DU",
            "extra": "median=718400ns  p95=760800ns  stddev=16459.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 44,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 173998,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5729.6,
            "unit": "ns/DU",
            "extra": "median=716200ns  p95=744100ns  stddev=12490.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 26.4,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 174532,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5736.8,
            "unit": "ns/DU",
            "extra": "median=717100ns  p95=741800ns  stddev=12278.1ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 33.6,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 174313,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5720.8,
            "unit": "ns/DU",
            "extra": "median=715100ns  p95=762100ns  stddev=16515.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 17.6,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 174801,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5923.2,
            "unit": "ns/DU",
            "extra": "median=740400ns  p95=758000ns  stddev=15089.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 220,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 168828,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5720.8,
            "unit": "ns/DU",
            "extra": "median=715100ns  p95=789600ns  stddev=21984.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 17.6,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 174801,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5721.6,
            "unit": "ns/DU",
            "extra": "median=715200ns  p95=743700ns  stddev=14264.0ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 18.4,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 174776,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5720.8,
            "unit": "ns/DU",
            "extra": "median=715100ns  p95=744400ns  stddev=12761.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 17.6,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 174801,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5690.4,
            "unit": "ns/DU",
            "extra": "median=711300ns  p95=748500ns  stddev=14040.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -12.8,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 175735,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5702.4,
            "unit": "ns/DU",
            "extra": "median=712800ns  p95=783200ns  stddev=24590.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -0.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 175365,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5693.6,
            "unit": "ns/DU",
            "extra": "median=711700ns  p95=739700ns  stddev=12435.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 175636,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5705.6,
            "unit": "ns/DU",
            "extra": "median=713200ns  p95=759000ns  stddev=15452.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 2.4,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 175266,
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
        "date": 1775238186086,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5762.4,
            "unit": "ns/DU",
            "extra": "median=720300ns  p95=948100ns  stddev=66041.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 173539,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 7748,
            "unit": "ns/DU",
            "extra": "median=968500ns  p95=1087300ns  stddev=40535.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 1985.6,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 129066,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5795.2,
            "unit": "ns/DU",
            "extra": "median=724400ns  p95=733800ns  stddev=6667.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 32.8,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 172557,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5718.4,
            "unit": "ns/DU",
            "extra": "median=714800ns  p95=739000ns  stddev=9972.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -44,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 174874,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5728,
            "unit": "ns/DU",
            "extra": "median=716000ns  p95=811600ns  stddev=27795.7ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -34.4,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 174581,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5774.4,
            "unit": "ns/DU",
            "extra": "median=721800ns  p95=752100ns  stddev=11438.5ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 173178,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5772,
            "unit": "ns/DU",
            "extra": "median=721500ns  p95=746300ns  stddev=13320.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 9.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 173250,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5776,
            "unit": "ns/DU",
            "extra": "median=722000ns  p95=752800ns  stddev=14300.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 13.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 173130,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5801.6,
            "unit": "ns/DU",
            "extra": "median=725200ns  p95=745000ns  stddev=10558.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 39.2,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 172366,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5840.8,
            "unit": "ns/DU",
            "extra": "median=730100ns  p95=756200ns  stddev=13400.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 78.4,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 171209,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5844,
            "unit": "ns/DU",
            "extra": "median=730500ns  p95=750500ns  stddev=9480.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 81.6,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 171116,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5751.2,
            "unit": "ns/DU",
            "extra": "median=718900ns  p95=746000ns  stddev=13281.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": -11.2,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 173877,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5765.6,
            "unit": "ns/DU",
            "extra": "median=720700ns  p95=753800ns  stddev=14525.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 173442,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5770.4,
            "unit": "ns/DU",
            "extra": "median=721300ns  p95=754200ns  stddev=14179.2ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 8,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 173298,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5753.6,
            "unit": "ns/DU",
            "extra": "median=719200ns  p95=761900ns  stddev=15565.0ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -8.8,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 173804,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5738.4,
            "unit": "ns/DU",
            "extra": "median=717300ns  p95=756500ns  stddev=16947.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -24,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 174265,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5758.4,
            "unit": "ns/DU",
            "extra": "median=719800ns  p95=750900ns  stddev=12963.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -4,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 173659,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5759.2,
            "unit": "ns/DU",
            "extra": "median=719900ns  p95=749400ns  stddev=13628.4ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": -3.2,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 173635,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5757.6,
            "unit": "ns/DU",
            "extra": "median=719700ns  p95=746200ns  stddev=10881.2ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -4.8,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 173683,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5828,
            "unit": "ns/DU",
            "extra": "median=728500ns  p95=748100ns  stddev=11011.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 65.6,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 171585,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5748.8,
            "unit": "ns/DU",
            "extra": "median=718600ns  p95=744000ns  stddev=10369.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": -13.6,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 173949,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5780.8,
            "unit": "ns/DU",
            "extra": "median=722600ns  p95=749300ns  stddev=12702.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 18.4,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 172986,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5766.4,
            "unit": "ns/DU",
            "extra": "median=720800ns  p95=754200ns  stddev=14833.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 4,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 173418,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5794.4,
            "unit": "ns/DU",
            "extra": "median=724300ns  p95=750200ns  stddev=9964.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 32,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 172580,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=747500ns  stddev=13063.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 5.6,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5753.6,
            "unit": "ns/DU",
            "extra": "median=719200ns  p95=751000ns  stddev=12344.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": -8.8,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 173804,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5754.4,
            "unit": "ns/DU",
            "extra": "median=719300ns  p95=794000ns  stddev=23294.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -8,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 173780,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5749.6,
            "unit": "ns/DU",
            "extra": "median=718700ns  p95=760000ns  stddev=17978.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": -12.8,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 173925,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5688,
            "unit": "ns/DU",
            "extra": "median=711000ns  p95=744800ns  stddev=13805.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -74.4,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 175809,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5708.8,
            "unit": "ns/DU",
            "extra": "median=713600ns  p95=799700ns  stddev=25981.8ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -53.6,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 175168,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5684,
            "unit": "ns/DU",
            "extra": "median=710500ns  p95=736300ns  stddev=11653.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -78.4,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 175932,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5879.2,
            "unit": "ns/DU",
            "extra": "median=734900ns  p95=818200ns  stddev=31296.8ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 116.8,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 170091,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5899.2,
            "unit": "ns/DU",
            "extra": "median=737400ns  p95=848800ns  stddev=32426.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 136.8,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 169515,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5736,
            "unit": "ns/DU",
            "extra": "median=717000ns  p95=742600ns  stddev=10337.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -26.4,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 174338,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5738.4,
            "unit": "ns/DU",
            "extra": "median=717300ns  p95=743300ns  stddev=11245.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -24,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 174265,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5736,
            "unit": "ns/DU",
            "extra": "median=717000ns  p95=930900ns  stddev=60532.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -26.4,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 174338,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5747.2,
            "unit": "ns/DU",
            "extra": "median=718400ns  p95=747000ns  stddev=12861.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -15.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 173998,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5732.8,
            "unit": "ns/DU",
            "extra": "median=716600ns  p95=739900ns  stddev=10469.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -29.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 174435,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5714.4,
            "unit": "ns/DU",
            "extra": "median=714300ns  p95=744300ns  stddev=12107.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -48,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 174997,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5712,
            "unit": "ns/DU",
            "extra": "median=714000ns  p95=740700ns  stddev=12091.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -50.4,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 175070,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=792600ns  stddev=22716.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -28,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=760400ns  stddev=15258.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -28,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5766.4,
            "unit": "ns/DU",
            "extra": "median=720800ns  p95=917800ns  stddev=62252.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 4,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 173418,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5910.4,
            "unit": "ns/DU",
            "extra": "median=738800ns  p95=922200ns  stddev=59404.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 148,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 169193,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5689.6,
            "unit": "ns/DU",
            "extra": "median=711200ns  p95=759300ns  stddev=16857.6ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -72.8,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 175759,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5891.2,
            "unit": "ns/DU",
            "extra": "median=736400ns  p95=750000ns  stddev=13981.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 128.8,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 169745,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5752.8,
            "unit": "ns/DU",
            "extra": "median=719100ns  p95=732800ns  stddev=7922.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 173828,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5759.2,
            "unit": "ns/DU",
            "extra": "median=719900ns  p95=763700ns  stddev=15057.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -3.2,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 173635,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5757.6,
            "unit": "ns/DU",
            "extra": "median=719700ns  p95=735500ns  stddev=8118.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -4.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 173683,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5764.8,
            "unit": "ns/DU",
            "extra": "median=720600ns  p95=733300ns  stddev=7235.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 2.4,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 173467,
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
        "date": 1775254139774,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5120.8,
            "unit": "ns/DU",
            "extra": "median=640100ns  p95=713100ns  stddev=31551.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 195282,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5179.2,
            "unit": "ns/DU",
            "extra": "median=647400ns  p95=694200ns  stddev=24652.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 58.4,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 193080,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5232,
            "unit": "ns/DU",
            "extra": "median=654000ns  p95=692900ns  stddev=21795.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 111.2,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 191131,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5198.4,
            "unit": "ns/DU",
            "extra": "median=649800ns  p95=674900ns  stddev=20207.3ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 77.6,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 192367,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5196.8,
            "unit": "ns/DU",
            "extra": "median=649600ns  p95=703700ns  stddev=33307.4ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 76,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 192426,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5383.2,
            "unit": "ns/DU",
            "extra": "median=672900ns  p95=700600ns  stddev=19797.3ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 262.4,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 185763,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5168,
            "unit": "ns/DU",
            "extra": "median=646000ns  p95=686700ns  stddev=25498.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 47.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 193498,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5051.2,
            "unit": "ns/DU",
            "extra": "median=631400ns  p95=691500ns  stddev=26688.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -69.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 197973,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5261.6,
            "unit": "ns/DU",
            "extra": "median=657700ns  p95=689700ns  stddev=24011.0ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 140.8,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 190056,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5242.4,
            "unit": "ns/DU",
            "extra": "median=655300ns  p95=701400ns  stddev=29169.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 121.6,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 190752,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5172.8,
            "unit": "ns/DU",
            "extra": "median=646600ns  p95=688100ns  stddev=26816.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 52,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 193319,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5214.4,
            "unit": "ns/DU",
            "extra": "median=651800ns  p95=675600ns  stddev=19356.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 93.6,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 191777,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5199.2,
            "unit": "ns/DU",
            "extra": "median=649900ns  p95=699400ns  stddev=23470.4ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 78.4,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 192337,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5207.2,
            "unit": "ns/DU",
            "extra": "median=650900ns  p95=694500ns  stddev=22568.2ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 86.4,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 192042,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5187.2,
            "unit": "ns/DU",
            "extra": "median=648400ns  p95=688100ns  stddev=23681.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 192782,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5195.2,
            "unit": "ns/DU",
            "extra": "median=649400ns  p95=698400ns  stddev=24335.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 74.4,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 192485,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5232.8,
            "unit": "ns/DU",
            "extra": "median=654100ns  p95=729600ns  stddev=34180.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 112,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 191102,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5071.2,
            "unit": "ns/DU",
            "extra": "median=633900ns  p95=687600ns  stddev=22865.5ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": -49.6,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 197192,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5111.2,
            "unit": "ns/DU",
            "extra": "median=638900ns  p95=682900ns  stddev=22801.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 195649,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 7010.4,
            "unit": "ns/DU",
            "extra": "median=876300ns  p95=960200ns  stddev=39866.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 1889.6,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 142645,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 6883.2,
            "unit": "ns/DU",
            "extra": "median=860400ns  p95=935600ns  stddev=108466.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 1762.4,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 145281,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5088.8,
            "unit": "ns/DU",
            "extra": "median=636100ns  p95=685100ns  stddev=24974.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": -32,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 196510,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5268,
            "unit": "ns/DU",
            "extra": "median=658500ns  p95=855800ns  stddev=84572.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 147.2,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 189825,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5225.6,
            "unit": "ns/DU",
            "extra": "median=653200ns  p95=709900ns  stddev=26395.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 104.8,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 191366,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5224.8,
            "unit": "ns/DU",
            "extra": "median=653100ns  p95=681800ns  stddev=21610.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 104,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 191395,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5168.8,
            "unit": "ns/DU",
            "extra": "median=646100ns  p95=831900ns  stddev=65969.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 48,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 193469,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5184,
            "unit": "ns/DU",
            "extra": "median=648000ns  p95=687500ns  stddev=21432.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 63.2,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 192901,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5160,
            "unit": "ns/DU",
            "extra": "median=645000ns  p95=740100ns  stddev=36560.1ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 39.2,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 193798,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5124.8,
            "unit": "ns/DU",
            "extra": "median=640600ns  p95=696400ns  stddev=25054.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 4,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 195130,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5117.6,
            "unit": "ns/DU",
            "extra": "median=639700ns  p95=684200ns  stddev=22817.6ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -3.2,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 195404,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5116.8,
            "unit": "ns/DU",
            "extra": "median=639600ns  p95=716900ns  stddev=35457.0ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -4,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 195435,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 4929.6,
            "unit": "ns/DU",
            "extra": "median=616200ns  p95=683500ns  stddev=23266.8ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -191.2,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 202856,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5192.8,
            "unit": "ns/DU",
            "extra": "median=649100ns  p95=688100ns  stddev=23827.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 72,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 192574,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5209.6,
            "unit": "ns/DU",
            "extra": "median=651200ns  p95=690200ns  stddev=24086.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 88.8,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 191953,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5110.4,
            "unit": "ns/DU",
            "extra": "median=638800ns  p95=690500ns  stddev=25572.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -10.4,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 195679,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5103.2,
            "unit": "ns/DU",
            "extra": "median=637900ns  p95=688200ns  stddev=27781.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -17.6,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 195955,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5154.4,
            "unit": "ns/DU",
            "extra": "median=644300ns  p95=661500ns  stddev=17832.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 33.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 194009,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5180.8,
            "unit": "ns/DU",
            "extra": "median=647600ns  p95=710500ns  stddev=30026.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 60,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 193020,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5036.8,
            "unit": "ns/DU",
            "extra": "median=629600ns  p95=686900ns  stddev=22233.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -84,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 198539,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5251.2,
            "unit": "ns/DU",
            "extra": "median=656400ns  p95=701300ns  stddev=26700.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 130.4,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 190433,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5285.6,
            "unit": "ns/DU",
            "extra": "median=660700ns  p95=876400ns  stddev=74647.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 164.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 189193,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5132.8,
            "unit": "ns/DU",
            "extra": "median=641600ns  p95=685300ns  stddev=22712.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 194825,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5104.8,
            "unit": "ns/DU",
            "extra": "median=638100ns  p95=681900ns  stddev=22608.8ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -16,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 195894,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5320,
            "unit": "ns/DU",
            "extra": "median=665000ns  p95=788400ns  stddev=56069.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 199.2,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 187970,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5135.2,
            "unit": "ns/DU",
            "extra": "median=641900ns  p95=690400ns  stddev=28274.0ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 14.4,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 194734,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5117.6,
            "unit": "ns/DU",
            "extra": "median=639700ns  p95=689800ns  stddev=24342.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -3.2,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 195404,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5252.8,
            "unit": "ns/DU",
            "extra": "median=656600ns  p95=691600ns  stddev=25552.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 132,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 190375,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5004.8,
            "unit": "ns/DU",
            "extra": "median=625600ns  p95=679900ns  stddev=25192.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -116,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 199808,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5202.4,
            "unit": "ns/DU",
            "extra": "median=650300ns  p95=920100ns  stddev=87961.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 81.6,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 192219,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5091.2,
            "unit": "ns/DU",
            "extra": "median=636400ns  p95=690700ns  stddev=25715.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -29.6,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 196417,
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
        "date": 1775265252387,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5684,
            "unit": "ns/DU",
            "extra": "median=710500ns  p95=759600ns  stddev=15489.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 175932,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5763.2,
            "unit": "ns/DU",
            "extra": "median=720400ns  p95=760900ns  stddev=14814.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 79.2,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 173515,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5728,
            "unit": "ns/DU",
            "extra": "median=716000ns  p95=740100ns  stddev=11558.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 44,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 174581,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5718.4,
            "unit": "ns/DU",
            "extra": "median=714800ns  p95=742000ns  stddev=11615.4ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 34.4,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 174874,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5715.2,
            "unit": "ns/DU",
            "extra": "median=714400ns  p95=745400ns  stddev=12942.9ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 31.2,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 174972,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5776,
            "unit": "ns/DU",
            "extra": "median=722000ns  p95=746200ns  stddev=9361.6ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 92,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 173130,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5727.2,
            "unit": "ns/DU",
            "extra": "median=715900ns  p95=738900ns  stddev=12577.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 43.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 174605,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5714.4,
            "unit": "ns/DU",
            "extra": "median=714300ns  p95=740800ns  stddev=11533.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 174997,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5724,
            "unit": "ns/DU",
            "extra": "median=715500ns  p95=760800ns  stddev=15122.7ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 40,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 174703,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5761.6,
            "unit": "ns/DU",
            "extra": "median=720200ns  p95=752100ns  stddev=12940.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 77.6,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 173563,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5766.4,
            "unit": "ns/DU",
            "extra": "median=720800ns  p95=746400ns  stddev=10768.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 82.4,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 173418,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5825.6,
            "unit": "ns/DU",
            "extra": "median=728200ns  p95=746100ns  stddev=9922.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 141.6,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 171656,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5762.4,
            "unit": "ns/DU",
            "extra": "median=720300ns  p95=764700ns  stddev=16648.3ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 78.4,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 173539,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5758.4,
            "unit": "ns/DU",
            "extra": "median=719800ns  p95=745400ns  stddev=9813.8ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 74.4,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 173659,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5828,
            "unit": "ns/DU",
            "extra": "median=728500ns  p95=750300ns  stddev=10123.5ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 144,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 171585,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5812.8,
            "unit": "ns/DU",
            "extra": "median=726600ns  p95=761100ns  stddev=14866.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 128.8,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 172034,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5911.2,
            "unit": "ns/DU",
            "extra": "median=738900ns  p95=983100ns  stddev=84619.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 227.2,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 169170,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5845.6,
            "unit": "ns/DU",
            "extra": "median=730700ns  p95=902700ns  stddev=56968.9ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 161.6,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 171069,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5776.8,
            "unit": "ns/DU",
            "extra": "median=722100ns  p95=748600ns  stddev=12178.1ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 92.8,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 173106,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5767.2,
            "unit": "ns/DU",
            "extra": "median=720900ns  p95=766600ns  stddev=15840.5ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 83.2,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 173394,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5744.8,
            "unit": "ns/DU",
            "extra": "median=718100ns  p95=744600ns  stddev=12268.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 60.8,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 174070,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5768.8,
            "unit": "ns/DU",
            "extra": "median=721100ns  p95=798000ns  stddev=26405.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 84.8,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 173346,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5972,
            "unit": "ns/DU",
            "extra": "median=746500ns  p95=865200ns  stddev=46057.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 288,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 167448,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5889.6,
            "unit": "ns/DU",
            "extra": "median=736200ns  p95=833100ns  stddev=32652.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 205.6,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 169791,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5765.6,
            "unit": "ns/DU",
            "extra": "median=720700ns  p95=746800ns  stddev=10396.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 81.6,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 173442,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5757.6,
            "unit": "ns/DU",
            "extra": "median=719700ns  p95=786200ns  stddev=20992.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 73.6,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 173683,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5755.2,
            "unit": "ns/DU",
            "extra": "median=719400ns  p95=761400ns  stddev=15941.2ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 71.2,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 173756,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=771800ns  stddev=16770.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 84,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5727.2,
            "unit": "ns/DU",
            "extra": "median=715900ns  p95=793500ns  stddev=23402.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 43.2,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 174605,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5783.2,
            "unit": "ns/DU",
            "extra": "median=722900ns  p95=782000ns  stddev=18988.4ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 99.2,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 172915,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5688.8,
            "unit": "ns/DU",
            "extra": "median=711100ns  p95=736400ns  stddev=11285.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 4.8,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 175784,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5774.4,
            "unit": "ns/DU",
            "extra": "median=721800ns  p95=765100ns  stddev=16502.0ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 90.4,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 173178,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5936.8,
            "unit": "ns/DU",
            "extra": "median=742100ns  p95=777800ns  stddev=14077.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 252.8,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 168441,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5732,
            "unit": "ns/DU",
            "extra": "median=716500ns  p95=737900ns  stddev=9704.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 48,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 174459,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5790.4,
            "unit": "ns/DU",
            "extra": "median=723800ns  p95=751700ns  stddev=12605.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 106.4,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 172700,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5730.4,
            "unit": "ns/DU",
            "extra": "median=716300ns  p95=763900ns  stddev=17026.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 46.4,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 174508,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5728.8,
            "unit": "ns/DU",
            "extra": "median=716100ns  p95=742300ns  stddev=11104.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 44.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 174557,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5739.2,
            "unit": "ns/DU",
            "extra": "median=717400ns  p95=753600ns  stddev=13467.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 55.2,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 174240,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5728.8,
            "unit": "ns/DU",
            "extra": "median=716100ns  p95=759800ns  stddev=14814.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 44.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 174557,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5738.4,
            "unit": "ns/DU",
            "extra": "median=717300ns  p95=737800ns  stddev=9637.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 54.4,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 174265,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5732.8,
            "unit": "ns/DU",
            "extra": "median=716600ns  p95=739600ns  stddev=11038.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 48.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 174435,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5711.2,
            "unit": "ns/DU",
            "extra": "median=713900ns  p95=756700ns  stddev=15021.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 27.2,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 175095,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5720,
            "unit": "ns/DU",
            "extra": "median=715000ns  p95=739300ns  stddev=11424.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 36,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 174825,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5720,
            "unit": "ns/DU",
            "extra": "median=715000ns  p95=739500ns  stddev=10885.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 36,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 174825,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5692.8,
            "unit": "ns/DU",
            "extra": "median=711600ns  p95=805200ns  stddev=26868.3ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 8.8,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 175660,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5800,
            "unit": "ns/DU",
            "extra": "median=725000ns  p95=743500ns  stddev=11024.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 116,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 172414,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=746800ns  stddev=11535.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 84,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5750.4,
            "unit": "ns/DU",
            "extra": "median=718800ns  p95=738100ns  stddev=10364.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 173901,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5688.8,
            "unit": "ns/DU",
            "extra": "median=711100ns  p95=738700ns  stddev=11906.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 4.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 175784,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5682.4,
            "unit": "ns/DU",
            "extra": "median=710300ns  p95=733300ns  stddev=9899.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -1.6,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 175982,
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
        "date": 1775271785829,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5257.6,
            "unit": "ns/DU",
            "extra": "median=657200ns  p95=712900ns  stddev=28463.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 190201,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5461.6,
            "unit": "ns/DU",
            "extra": "median=682700ns  p95=767100ns  stddev=45426.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 204,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 183097,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5340,
            "unit": "ns/DU",
            "extra": "median=667500ns  p95=684700ns  stddev=25550.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 82.4,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 187266,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5257.6,
            "unit": "ns/DU",
            "extra": "median=657200ns  p95=702800ns  stddev=26420.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 190201,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5202.4,
            "unit": "ns/DU",
            "extra": "median=650300ns  p95=734700ns  stddev=38701.5ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -55.2,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 192219,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5710.4,
            "unit": "ns/DU",
            "extra": "median=713800ns  p95=746600ns  stddev=26612.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 452.8,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 175119,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5142.4,
            "unit": "ns/DU",
            "extra": "median=642800ns  p95=712300ns  stddev=30220.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -115.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 194462,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5198.4,
            "unit": "ns/DU",
            "extra": "median=649800ns  p95=707500ns  stddev=31576.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -59.2,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 192367,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5064.8,
            "unit": "ns/DU",
            "extra": "median=633100ns  p95=795000ns  stddev=56267.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -192.8,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 197441,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5168,
            "unit": "ns/DU",
            "extra": "median=646000ns  p95=670400ns  stddev=15549.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -89.6,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 193498,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5132,
            "unit": "ns/DU",
            "extra": "median=641500ns  p95=699800ns  stddev=27003.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -125.6,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 194856,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5246.4,
            "unit": "ns/DU",
            "extra": "median=655800ns  p95=709200ns  stddev=31531.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": -11.2,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 190607,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5182.4,
            "unit": "ns/DU",
            "extra": "median=647800ns  p95=700000ns  stddev=30520.0ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": -75.2,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 192961,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5219.2,
            "unit": "ns/DU",
            "extra": "median=652400ns  p95=692200ns  stddev=23065.3ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -38.4,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 191600,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5200,
            "unit": "ns/DU",
            "extra": "median=650000ns  p95=709100ns  stddev=30222.0ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -57.6,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 192308,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5136.8,
            "unit": "ns/DU",
            "extra": "median=642100ns  p95=698600ns  stddev=23267.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -120.8,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 194674,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5260,
            "unit": "ns/DU",
            "extra": "median=657500ns  p95=705100ns  stddev=29226.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 2.4,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 190114,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5017.6,
            "unit": "ns/DU",
            "extra": "median=627200ns  p95=779700ns  stddev=47528.1ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": -240,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 199298,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5301.6,
            "unit": "ns/DU",
            "extra": "median=662700ns  p95=814800ns  stddev=52166.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 44,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 188622,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5288,
            "unit": "ns/DU",
            "extra": "median=661000ns  p95=728300ns  stddev=32414.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 189107,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5279.2,
            "unit": "ns/DU",
            "extra": "median=659900ns  p95=692300ns  stddev=26339.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 21.6,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 189423,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5307.2,
            "unit": "ns/DU",
            "extra": "median=663400ns  p95=754500ns  stddev=41648.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 49.6,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 188423,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5252.8,
            "unit": "ns/DU",
            "extra": "median=656600ns  p95=686500ns  stddev=21802.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": -4.8,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 190375,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5246.4,
            "unit": "ns/DU",
            "extra": "median=655800ns  p95=707300ns  stddev=27639.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -11.2,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 190607,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5290.4,
            "unit": "ns/DU",
            "extra": "median=661300ns  p95=690700ns  stddev=20769.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 32.8,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 189022,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5112,
            "unit": "ns/DU",
            "extra": "median=639000ns  p95=685800ns  stddev=20237.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": -145.6,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 195618,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5225.6,
            "unit": "ns/DU",
            "extra": "median=653200ns  p95=678100ns  stddev=15466.2ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -32,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 191366,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5361.6,
            "unit": "ns/DU",
            "extra": "median=670200ns  p95=893800ns  stddev=78360.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 104,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 186511,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5160,
            "unit": "ns/DU",
            "extra": "median=645000ns  p95=695200ns  stddev=28356.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -97.6,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 193798,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5100,
            "unit": "ns/DU",
            "extra": "median=637500ns  p95=704600ns  stddev=27366.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -157.6,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 196078,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5380.8,
            "unit": "ns/DU",
            "extra": "median=672600ns  p95=690400ns  stddev=27452.5ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 123.2,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 185846,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5147.2,
            "unit": "ns/DU",
            "extra": "median=643400ns  p95=690000ns  stddev=28799.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -110.4,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 194280,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5239.2,
            "unit": "ns/DU",
            "extra": "median=654900ns  p95=724800ns  stddev=31649.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -18.4,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 190869,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5120,
            "unit": "ns/DU",
            "extra": "median=640000ns  p95=693700ns  stddev=22009.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -137.6,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 195312,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5142.4,
            "unit": "ns/DU",
            "extra": "median=642800ns  p95=694700ns  stddev=23053.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -115.2,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 194462,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5145.6,
            "unit": "ns/DU",
            "extra": "median=643200ns  p95=758300ns  stddev=41357.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -112,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 194341,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5116.8,
            "unit": "ns/DU",
            "extra": "median=639600ns  p95=714400ns  stddev=31736.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -140.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 195435,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5180,
            "unit": "ns/DU",
            "extra": "median=647500ns  p95=761000ns  stddev=40344.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -77.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 193050,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5187.2,
            "unit": "ns/DU",
            "extra": "median=648400ns  p95=758700ns  stddev=40925.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -70.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 192782,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5157.6,
            "unit": "ns/DU",
            "extra": "median=644700ns  p95=734500ns  stddev=34383.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -100,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 193889,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5040.8,
            "unit": "ns/DU",
            "extra": "median=630100ns  p95=698200ns  stddev=27377.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -216.8,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 198381,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5331.2,
            "unit": "ns/DU",
            "extra": "median=666400ns  p95=702700ns  stddev=27800.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 73.6,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 187575,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5324,
            "unit": "ns/DU",
            "extra": "median=665500ns  p95=691500ns  stddev=23595.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 187829,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5192,
            "unit": "ns/DU",
            "extra": "median=649000ns  p95=709800ns  stddev=27967.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -65.6,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 192604,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5062.4,
            "unit": "ns/DU",
            "extra": "median=632800ns  p95=689400ns  stddev=27355.1ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -195.2,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 197535,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5315.2,
            "unit": "ns/DU",
            "extra": "median=664400ns  p95=696000ns  stddev=23438.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 57.6,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 188140,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5349.6,
            "unit": "ns/DU",
            "extra": "median=668700ns  p95=730600ns  stddev=41783.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 92,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 186930,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5217.6,
            "unit": "ns/DU",
            "extra": "median=652200ns  p95=701900ns  stddev=29450.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -40,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 191659,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5137.6,
            "unit": "ns/DU",
            "extra": "median=642200ns  p95=707300ns  stddev=27474.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -120,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 194643,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5108.8,
            "unit": "ns/DU",
            "extra": "median=638600ns  p95=672900ns  stddev=20681.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -148.8,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 195741,
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
        "date": 1775272253591,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5693.6,
            "unit": "ns/DU",
            "extra": "median=711700ns  p95=738300ns  stddev=10934.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 175636,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5751.2,
            "unit": "ns/DU",
            "extra": "median=718900ns  p95=802300ns  stddev=25227.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 57.6,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 173877,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5738.4,
            "unit": "ns/DU",
            "extra": "median=717300ns  p95=739700ns  stddev=11332.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 44.8,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 174265,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5827.2,
            "unit": "ns/DU",
            "extra": "median=728400ns  p95=906600ns  stddev=53400.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 133.6,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 171609,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5700,
            "unit": "ns/DU",
            "extra": "median=712500ns  p95=744300ns  stddev=13280.1ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 6.4,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 175439,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5828.8,
            "unit": "ns/DU",
            "extra": "median=728600ns  p95=755100ns  stddev=13215.8ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 135.2,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 171562,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5719.2,
            "unit": "ns/DU",
            "extra": "median=714900ns  p95=735800ns  stddev=10280.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 25.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 174850,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5707.2,
            "unit": "ns/DU",
            "extra": "median=713400ns  p95=745800ns  stddev=13828.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 13.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 175217,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5700.8,
            "unit": "ns/DU",
            "extra": "median=712600ns  p95=740200ns  stddev=11961.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 7.2,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 175414,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=761700ns  stddev=16284.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 40.8,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 7578.4,
            "unit": "ns/DU",
            "extra": "median=947300ns  p95=998600ns  stddev=113868.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 1884.8,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 131954,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 7566.4,
            "unit": "ns/DU",
            "extra": "median=945800ns  p95=998200ns  stddev=119063.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 1872.8,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 132163,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5740,
            "unit": "ns/DU",
            "extra": "median=717500ns  p95=744200ns  stddev=12736.0ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 46.4,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 174216,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5843.2,
            "unit": "ns/DU",
            "extra": "median=730400ns  p95=795200ns  stddev=22410.2ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 149.6,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 171139,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5746.4,
            "unit": "ns/DU",
            "extra": "median=718300ns  p95=748900ns  stddev=10913.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 52.8,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 174022,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5705.6,
            "unit": "ns/DU",
            "extra": "median=713200ns  p95=748800ns  stddev=13666.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 175266,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5743.2,
            "unit": "ns/DU",
            "extra": "median=717900ns  p95=750500ns  stddev=13678.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 49.6,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 174119,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5730.4,
            "unit": "ns/DU",
            "extra": "median=716300ns  p95=750900ns  stddev=13196.2ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 36.8,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 174508,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5731.2,
            "unit": "ns/DU",
            "extra": "median=716400ns  p95=744100ns  stddev=12734.6ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 37.6,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 174484,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5746.4,
            "unit": "ns/DU",
            "extra": "median=718300ns  p95=775400ns  stddev=19440.6ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 52.8,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 174022,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5706.4,
            "unit": "ns/DU",
            "extra": "median=713300ns  p95=740800ns  stddev=12396.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 12.8,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 175242,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=746300ns  stddev=13224.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 40.8,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5833.6,
            "unit": "ns/DU",
            "extra": "median=729200ns  p95=786000ns  stddev=20567.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 140,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 171421,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5834.4,
            "unit": "ns/DU",
            "extra": "median=729300ns  p95=744200ns  stddev=10548.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 140.8,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 171397,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5833.6,
            "unit": "ns/DU",
            "extra": "median=729200ns  p95=733000ns  stddev=6350.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 140,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 171421,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5829.6,
            "unit": "ns/DU",
            "extra": "median=728700ns  p95=743000ns  stddev=9836.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 136,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 171538,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5740,
            "unit": "ns/DU",
            "extra": "median=717500ns  p95=754200ns  stddev=15165.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 46.4,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 174216,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5747.2,
            "unit": "ns/DU",
            "extra": "median=718400ns  p95=743200ns  stddev=10001.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 53.6,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 173998,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5693.6,
            "unit": "ns/DU",
            "extra": "median=711700ns  p95=741800ns  stddev=13868.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 175636,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5709.6,
            "unit": "ns/DU",
            "extra": "median=713700ns  p95=756300ns  stddev=15420.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 16,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 175144,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5680.8,
            "unit": "ns/DU",
            "extra": "median=710100ns  p95=755300ns  stddev=15415.9ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -12.8,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 176032,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5686.4,
            "unit": "ns/DU",
            "extra": "median=710800ns  p95=742400ns  stddev=14694.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 175858,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5848,
            "unit": "ns/DU",
            "extra": "median=731000ns  p95=808600ns  stddev=23579.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 154.4,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 170999,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 6028,
            "unit": "ns/DU",
            "extra": "median=753500ns  p95=916900ns  stddev=53852.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 334.4,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 165893,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5984,
            "unit": "ns/DU",
            "extra": "median=748000ns  p95=876600ns  stddev=39716.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 290.4,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 167112,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5813.6,
            "unit": "ns/DU",
            "extra": "median=726700ns  p95=758200ns  stddev=14285.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 120,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 172010,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5707.2,
            "unit": "ns/DU",
            "extra": "median=713400ns  p95=740100ns  stddev=12586.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 13.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 175217,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5724,
            "unit": "ns/DU",
            "extra": "median=715500ns  p95=830500ns  stddev=33475.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 174703,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5713.6,
            "unit": "ns/DU",
            "extra": "median=714200ns  p95=779800ns  stddev=20384.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 20,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 175021,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5704.8,
            "unit": "ns/DU",
            "extra": "median=713100ns  p95=744700ns  stddev=13644.4ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 11.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 175291,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5708.8,
            "unit": "ns/DU",
            "extra": "median=713600ns  p95=745900ns  stddev=14361.7ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 15.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 175168,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 5723.2,
            "unit": "ns/DU",
            "extra": "median=715400ns  p95=822600ns  stddev=31757.2ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 29.6,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 174727,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5724,
            "unit": "ns/DU",
            "extra": "median=715500ns  p95=741000ns  stddev=10719.6ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 174703,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5714.4,
            "unit": "ns/DU",
            "extra": "median=714300ns  p95=741400ns  stddev=12014.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 20.8,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 174997,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5683.2,
            "unit": "ns/DU",
            "extra": "median=710400ns  p95=761000ns  stddev=18774.0ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -10.4,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 175957,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5763.2,
            "unit": "ns/DU",
            "extra": "median=720400ns  p95=749500ns  stddev=14391.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 69.6,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 173515,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5693.6,
            "unit": "ns/DU",
            "extra": "median=711700ns  p95=735100ns  stddev=11213.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 175636,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5679.2,
            "unit": "ns/DU",
            "extra": "median=709900ns  p95=752100ns  stddev=15976.2ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 176081,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5678.4,
            "unit": "ns/DU",
            "extra": "median=709800ns  p95=743300ns  stddev=14020.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -15.2,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 176106,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5688,
            "unit": "ns/DU",
            "extra": "median=711000ns  p95=739700ns  stddev=13387.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -5.6,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 175809,
            "unit": "DU/s"
          }
        ]
      }
    ]
  }
}