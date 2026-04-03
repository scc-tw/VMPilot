window.BENCHMARK_DATA = {
  "lastUpdate": 1775235183546,
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
      }
    ]
  }
}