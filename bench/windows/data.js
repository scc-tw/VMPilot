window.BENCHMARK_DATA = {
  "lastUpdate": 1775384415970,
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
        "date": 1775272456057,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5773.6,
            "unit": "ns/DU",
            "extra": "median=721700ns  p95=775300ns  stddev=19968.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 173202,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=763400ns  stddev=15601.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": -16.8,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 5728,
            "unit": "ns/DU",
            "extra": "median=716000ns  p95=778400ns  stddev=19685.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -45.6,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 174581,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 5720.8,
            "unit": "ns/DU",
            "extra": "median=715100ns  p95=760300ns  stddev=16189.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -52.8,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 174801,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=767900ns  stddev=17547.4ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -5.6,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 5796,
            "unit": "ns/DU",
            "extra": "median=724500ns  p95=751900ns  stddev=11010.9ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 22.4,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 172533,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5704,
            "unit": "ns/DU",
            "extra": "median=713000ns  p95=755700ns  stddev=15823.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -69.6,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 175316,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 5728.8,
            "unit": "ns/DU",
            "extra": "median=716100ns  p95=785500ns  stddev=21243.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -44.8,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 174557,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=760000ns  stddev=16844.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -39.2,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 5748,
            "unit": "ns/DU",
            "extra": "median=718500ns  p95=761800ns  stddev=17147.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -25.6,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 173974,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 5890.4,
            "unit": "ns/DU",
            "extra": "median=736300ns  p95=814900ns  stddev=26905.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 116.8,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 169768,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 5851.2,
            "unit": "ns/DU",
            "extra": "median=731400ns  p95=773400ns  stddev=15808.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 77.6,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 170905,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 5842.4,
            "unit": "ns/DU",
            "extra": "median=730300ns  p95=792800ns  stddev=20962.1ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 68.8,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 171163,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 5770.4,
            "unit": "ns/DU",
            "extra": "median=721300ns  p95=747500ns  stddev=11007.5ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -3.2,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 173298,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 5888,
            "unit": "ns/DU",
            "extra": "median=736000ns  p95=757600ns  stddev=10243.0ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 114.4,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 169837,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 5831.2,
            "unit": "ns/DU",
            "extra": "median=728900ns  p95=760800ns  stddev=12940.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 57.6,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 171491,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 5839.2,
            "unit": "ns/DU",
            "extra": "median=729900ns  p95=784600ns  stddev=19336.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 65.6,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 171256,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 5777.6,
            "unit": "ns/DU",
            "extra": "median=722200ns  p95=808600ns  stddev=30054.5ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 4,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 173082,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 5768,
            "unit": "ns/DU",
            "extra": "median=721000ns  p95=751700ns  stddev=15393.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -5.6,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 173370,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 5748,
            "unit": "ns/DU",
            "extra": "median=718500ns  p95=750300ns  stddev=13608.6ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": -25.6,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 173974,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 5729.6,
            "unit": "ns/DU",
            "extra": "median=716200ns  p95=743100ns  stddev=12345.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": -44,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 174532,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 5767.2,
            "unit": "ns/DU",
            "extra": "median=720900ns  p95=769800ns  stddev=17369.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": -6.4,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 173394,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 5766.4,
            "unit": "ns/DU",
            "extra": "median=720800ns  p95=752500ns  stddev=14703.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 173418,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=755700ns  stddev=14420.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -16.8,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 5860,
            "unit": "ns/DU",
            "extra": "median=732500ns  p95=753100ns  stddev=12790.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 86.4,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 170648,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 5851.2,
            "unit": "ns/DU",
            "extra": "median=731400ns  p95=743000ns  stddev=8021.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 77.6,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 170905,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 5794.4,
            "unit": "ns/DU",
            "extra": "median=724300ns  p95=746900ns  stddev=11585.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 20.8,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 172580,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 5764.8,
            "unit": "ns/DU",
            "extra": "median=720600ns  p95=813000ns  stddev=26741.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": -8.8,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 173467,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5782.4,
            "unit": "ns/DU",
            "extra": "median=722800ns  p95=770100ns  stddev=17318.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 8.8,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 172939,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 5723.2,
            "unit": "ns/DU",
            "extra": "median=715400ns  p95=793600ns  stddev=23358.5ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -50.4,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 174727,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 5703.2,
            "unit": "ns/DU",
            "extra": "median=712900ns  p95=744600ns  stddev=12967.7ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -70.4,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 175340,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 5795.2,
            "unit": "ns/DU",
            "extra": "median=724400ns  p95=744200ns  stddev=11919.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 21.6,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 172557,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 5896.8,
            "unit": "ns/DU",
            "extra": "median=737100ns  p95=792900ns  stddev=17734.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 123.2,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 169584,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 5726.4,
            "unit": "ns/DU",
            "extra": "median=715800ns  p95=746100ns  stddev=12242.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -47.2,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 174630,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5744,
            "unit": "ns/DU",
            "extra": "median=718000ns  p95=746800ns  stddev=14324.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -29.6,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 174095,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5711.2,
            "unit": "ns/DU",
            "extra": "median=713900ns  p95=741700ns  stddev=11420.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -62.4,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 175095,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 5720.8,
            "unit": "ns/DU",
            "extra": "median=715100ns  p95=747600ns  stddev=14600.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -52.8,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 174801,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5716,
            "unit": "ns/DU",
            "extra": "median=714500ns  p95=745800ns  stddev=12751.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -57.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 174948,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 5716,
            "unit": "ns/DU",
            "extra": "median=714500ns  p95=756500ns  stddev=15015.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -57.6,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 174948,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5726.4,
            "unit": "ns/DU",
            "extra": "median=715800ns  p95=748500ns  stddev=14572.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -47.2,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 174630,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 5734.4,
            "unit": "ns/DU",
            "extra": "median=716800ns  p95=784300ns  stddev=21983.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -39.2,
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
            "extra": "median=716800ns  p95=742500ns  stddev=10959.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -39.2,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 174386,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 5740.8,
            "unit": "ns/DU",
            "extra": "median=717600ns  p95=747700ns  stddev=12735.8ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -32.8,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 174192,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5752.8,
            "unit": "ns/DU",
            "extra": "median=719100ns  p95=842300ns  stddev=35388.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -20.8,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 173828,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5694.4,
            "unit": "ns/DU",
            "extra": "median=711800ns  p95=741800ns  stddev=12788.0ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -79.2,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 175611,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 5727.2,
            "unit": "ns/DU",
            "extra": "median=715900ns  p95=742600ns  stddev=12737.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -46.4,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 174605,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 5696,
            "unit": "ns/DU",
            "extra": "median=712000ns  p95=763000ns  stddev=17052.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -77.6,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 175562,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5700.8,
            "unit": "ns/DU",
            "extra": "median=712600ns  p95=740200ns  stddev=13285.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -72.8,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 175414,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5694.4,
            "unit": "ns/DU",
            "extra": "median=711800ns  p95=735500ns  stddev=9381.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -79.2,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 175611,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 5696.8,
            "unit": "ns/DU",
            "extra": "median=712100ns  p95=744700ns  stddev=11577.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -76.8,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 175537,
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
          "id": "96cc7ba2d3ceeccfa46c9b1da39ac2b31a44707b",
          "message": "fix(benchmark): improve measurement integrity and CI diagnostics",
          "timestamp": "2026-04-05T17:00:16+08:00",
          "tree_id": "a3b215573e745fe111d7d30253a4d4b83f3922e3",
          "url": "https://github.com/scc-tw/VMPilot/commit/96cc7ba2d3ceeccfa46c9b1da39ac2b31a44707b"
        },
        "date": 1775379904471,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 5764,
            "unit": "ns/DU",
            "extra": "median=720500ns  p95=746000ns  stddev=11019.0ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 173491,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOVE (total)",
            "value": 5752.8,
            "unit": "ns/DU",
            "extra": "median=719100ns  p95=742800ns  stddev=10136.3ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOVE (handler)",
            "value": -11.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOVE (DU/s)",
            "value": 173828,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD (total)",
            "value": 5769.6,
            "unit": "ns/DU",
            "extra": "median=721200ns  p95=760300ns  stddev=14248.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD (handler)",
            "value": 5.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD (DU/s)",
            "value": 173322,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE (total)",
            "value": 5799.2,
            "unit": "ns/DU",
            "extra": "median=724900ns  p95=751900ns  stddev=13374.3ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE (handler)",
            "value": 35.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE (DU/s)",
            "value": 172438,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] PUSH (total)",
            "value": 5762.4,
            "unit": "ns/DU",
            "extra": "median=720300ns  p95=745600ns  stddev=11518.1ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] PUSH (handler)",
            "value": -1.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] PUSH (DU/s)",
            "value": 173539,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] POP (total)",
            "value": 5812,
            "unit": "ns/DU",
            "extra": "median=726500ns  p95=754900ns  stddev=13272.1ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] POP (handler)",
            "value": 48,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] POP (DU/s)",
            "value": 172058,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (total)",
            "value": 5752.8,
            "unit": "ns/DU",
            "extra": "median=719100ns  p95=742900ns  stddev=10289.5ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (handler)",
            "value": -11.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (DU/s)",
            "value": 173828,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (total)",
            "value": 5762.4,
            "unit": "ns/DU",
            "extra": "median=720300ns  p95=748100ns  stddev=12169.3ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (handler)",
            "value": -1.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (DU/s)",
            "value": 173539,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (total)",
            "value": 5749.6,
            "unit": "ns/DU",
            "extra": "median=718700ns  p95=743900ns  stddev=10931.8ns  iterations=93  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (DU/s)",
            "value": 173925,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ADD (total)",
            "value": 5753.6,
            "unit": "ns/DU",
            "extra": "median=719200ns  p95=743800ns  stddev=11009.6ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] ADD (handler)",
            "value": -10.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ADD (DU/s)",
            "value": 173804,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SUB (total)",
            "value": 5756,
            "unit": "ns/DU",
            "extra": "median=719500ns  p95=744700ns  stddev=10830.2ns  iterations=90  DUs=125"
          },
          {
            "name": "[DebugPolicy] SUB (handler)",
            "value": -8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SUB (DU/s)",
            "value": 173732,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUL (total)",
            "value": 5748.8,
            "unit": "ns/DU",
            "extra": "median=718600ns  p95=731700ns  stddev=4600.7ns  iterations=74  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUL (handler)",
            "value": -15.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUL (DU/s)",
            "value": 173949,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IMUL (total)",
            "value": 5761.6,
            "unit": "ns/DU",
            "extra": "median=720200ns  p95=747200ns  stddev=11213.8ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] IMUL (handler)",
            "value": -2.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IMUL (DU/s)",
            "value": 173563,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] DIV (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=745300ns  stddev=11225.4ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] DIV (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] DIV (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IDIV (total)",
            "value": 5758.4,
            "unit": "ns/DU",
            "extra": "median=719800ns  p95=747600ns  stddev=11744.7ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] IDIV (handler)",
            "value": -5.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IDIV (DU/s)",
            "value": 173659,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NEG (total)",
            "value": 5792.8,
            "unit": "ns/DU",
            "extra": "median=724100ns  p95=751300ns  stddev=13212.7ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] NEG (handler)",
            "value": 28.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NEG (DU/s)",
            "value": 172628,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOD (total)",
            "value": 5752,
            "unit": "ns/DU",
            "extra": "median=719000ns  p95=740000ns  stddev=7842.3ns  iterations=84  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOD (handler)",
            "value": -12,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOD (DU/s)",
            "value": 173853,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] AND (total)",
            "value": 5777.6,
            "unit": "ns/DU",
            "extra": "median=722200ns  p95=753500ns  stddev=14320.7ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] AND (handler)",
            "value": 13.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] AND (DU/s)",
            "value": 173082,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] OR (total)",
            "value": 5767.2,
            "unit": "ns/DU",
            "extra": "median=720900ns  p95=756300ns  stddev=13870.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] OR (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] OR (DU/s)",
            "value": 173394,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XOR (total)",
            "value": 5786.4,
            "unit": "ns/DU",
            "extra": "median=723300ns  p95=750700ns  stddev=14104.7ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] XOR (handler)",
            "value": 22.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XOR (DU/s)",
            "value": 172819,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOT (total)",
            "value": 5809.6,
            "unit": "ns/DU",
            "extra": "median=726200ns  p95=757700ns  stddev=17583.7ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOT (handler)",
            "value": 45.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOT (DU/s)",
            "value": 172129,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHL (total)",
            "value": 5768.8,
            "unit": "ns/DU",
            "extra": "median=721100ns  p95=747400ns  stddev=12000.9ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHL (handler)",
            "value": 4.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHL (DU/s)",
            "value": 173346,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHR (total)",
            "value": 5764,
            "unit": "ns/DU",
            "extra": "median=720500ns  p95=749000ns  stddev=11600.4ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHR (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHR (DU/s)",
            "value": 173491,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SAR (total)",
            "value": 5745.6,
            "unit": "ns/DU",
            "extra": "median=718200ns  p95=740000ns  stddev=7318.1ns  iterations=79  DUs=125"
          },
          {
            "name": "[DebugPolicy] SAR (handler)",
            "value": -18.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SAR (DU/s)",
            "value": 174046,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROL (total)",
            "value": 5764.8,
            "unit": "ns/DU",
            "extra": "median=720600ns  p95=754300ns  stddev=14288.4ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROL (handler)",
            "value": 0.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROL (DU/s)",
            "value": 173467,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROR (total)",
            "value": 5765.6,
            "unit": "ns/DU",
            "extra": "median=720700ns  p95=752900ns  stddev=14665.5ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROR (handler)",
            "value": 1.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROR (DU/s)",
            "value": 173442,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMP (total)",
            "value": 5767.2,
            "unit": "ns/DU",
            "extra": "median=720900ns  p95=747600ns  stddev=11953.6ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMP (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMP (DU/s)",
            "value": 173394,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TEST (total)",
            "value": 5750.4,
            "unit": "ns/DU",
            "extra": "median=718800ns  p95=745400ns  stddev=11294.6ns  iterations=90  DUs=125"
          },
          {
            "name": "[DebugPolicy] TEST (handler)",
            "value": -13.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TEST (DU/s)",
            "value": 173901,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=746200ns  stddev=11111.1ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (total)",
            "value": 5748.8,
            "unit": "ns/DU",
            "extra": "median=718600ns  p95=733300ns  stddev=4913.4ns  iterations=77  DUs=125"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (handler)",
            "value": -15.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (DU/s)",
            "value": 173949,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JMP (total)",
            "value": 5760,
            "unit": "ns/DU",
            "extra": "median=720000ns  p95=747600ns  stddev=11407.4ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] JMP (handler)",
            "value": -4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JMP (DU/s)",
            "value": 173611,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JCC (total)",
            "value": 5758.4,
            "unit": "ns/DU",
            "extra": "median=719800ns  p95=742900ns  stddev=9160.9ns  iterations=88  DUs=125"
          },
          {
            "name": "[DebugPolicy] JCC (handler)",
            "value": -5.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JCC (DU/s)",
            "value": 173659,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (total)",
            "value": 5953.6,
            "unit": "ns/DU",
            "extra": "median=744200ns  p95=765900ns  stddev=7705.4ns  iterations=83  DUs=125"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (handler)",
            "value": 189.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (DU/s)",
            "value": 167966,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT8 (total)",
            "value": 5757.6,
            "unit": "ns/DU",
            "extra": "median=719700ns  p95=745400ns  stddev=11462.9ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT8 (handler)",
            "value": -6.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT8 (DU/s)",
            "value": 173683,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT16 (total)",
            "value": 5766.4,
            "unit": "ns/DU",
            "extra": "median=720800ns  p95=748100ns  stddev=12613.4ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT16 (handler)",
            "value": 2.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT16 (DU/s)",
            "value": 173418,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT32 (total)",
            "value": 5749.6,
            "unit": "ns/DU",
            "extra": "median=718700ns  p95=741900ns  stddev=9123.6ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT32 (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT32 (DU/s)",
            "value": 173925,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (total)",
            "value": 5752,
            "unit": "ns/DU",
            "extra": "median=719000ns  p95=746400ns  stddev=11967.0ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (handler)",
            "value": -12,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (DU/s)",
            "value": 173853,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (total)",
            "value": 5756,
            "unit": "ns/DU",
            "extra": "median=719500ns  p95=744700ns  stddev=10927.4ns  iterations=97  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (handler)",
            "value": -8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (DU/s)",
            "value": 173732,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (total)",
            "value": 5751.2,
            "unit": "ns/DU",
            "extra": "median=718900ns  p95=742400ns  stddev=9133.1ns  iterations=84  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (handler)",
            "value": -12.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (DU/s)",
            "value": 173877,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (total)",
            "value": 5765.6,
            "unit": "ns/DU",
            "extra": "median=720700ns  p95=751800ns  stddev=12631.2ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (handler)",
            "value": 1.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (DU/s)",
            "value": 173442,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (total)",
            "value": 5769.6,
            "unit": "ns/DU",
            "extra": "median=721200ns  p95=752100ns  stddev=13535.7ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (handler)",
            "value": 5.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (DU/s)",
            "value": 173322,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (total)",
            "value": 5800.8,
            "unit": "ns/DU",
            "extra": "median=725100ns  p95=751200ns  stddev=13851.0ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (handler)",
            "value": 36.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (DU/s)",
            "value": 172390,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XCHG (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=744200ns  stddev=10969.5ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] XCHG (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XCHG (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (total)",
            "value": 5752.8,
            "unit": "ns/DU",
            "extra": "median=719100ns  p95=730800ns  stddev=3983.7ns  iterations=73  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (handler)",
            "value": -11.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (DU/s)",
            "value": 173828,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] FENCE (total)",
            "value": 5824.8,
            "unit": "ns/DU",
            "extra": "median=728100ns  p95=753600ns  stddev=15046.3ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] FENCE (handler)",
            "value": 60.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] FENCE (DU/s)",
            "value": 171680,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=742600ns  stddev=9634.3ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 5749.6,
            "unit": "ns/DU",
            "extra": "median=718700ns  p95=742600ns  stddev=9581.6ns  iterations=85  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 173925,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (total)",
            "value": 5756.8,
            "unit": "ns/DU",
            "extra": "median=719600ns  p95=750200ns  stddev=12597.5ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (handler)",
            "value": -7.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 173708,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (total)",
            "value": 5757.6,
            "unit": "ns/DU",
            "extra": "median=719700ns  p95=747800ns  stddev=12164.9ns  iterations=97  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (handler)",
            "value": -6.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (DU/s)",
            "value": 173683,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (total)",
            "value": 5763.2,
            "unit": "ns/DU",
            "extra": "median=720400ns  p95=748900ns  stddev=12448.2ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (handler)",
            "value": -0.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (DU/s)",
            "value": 173515,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 44578.4,
            "unit": "ns/DU",
            "extra": "median=5572300ns  p95=5716400ns  stddev=57732.0ns  iterations=81  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 22432,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOVE (total)",
            "value": 44460.8,
            "unit": "ns/DU",
            "extra": "median=5557600ns  p95=5666100ns  stddev=50695.5ns  iterations=79  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOVE (handler)",
            "value": -117.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOVE (DU/s)",
            "value": 22492,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD (total)",
            "value": 44534.4,
            "unit": "ns/DU",
            "extra": "median=5566800ns  p95=5821800ns  stddev=94989.4ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD (handler)",
            "value": -44,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD (DU/s)",
            "value": 22455,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE (total)",
            "value": 44484,
            "unit": "ns/DU",
            "extra": "median=5560500ns  p95=5731900ns  stddev=61937.3ns  iterations=72  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE (handler)",
            "value": -94.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE (DU/s)",
            "value": 22480,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] PUSH (total)",
            "value": 44595.2,
            "unit": "ns/DU",
            "extra": "median=5574400ns  p95=5768100ns  stddev=60876.9ns  iterations=77  DUs=125"
          },
          {
            "name": "[StandardPolicy] PUSH (handler)",
            "value": 16.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] PUSH (DU/s)",
            "value": 22424,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] POP (total)",
            "value": 44754.4,
            "unit": "ns/DU",
            "extra": "median=5594300ns  p95=5790300ns  stddev=84703.3ns  iterations=75  DUs=125"
          },
          {
            "name": "[StandardPolicy] POP (handler)",
            "value": 176,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] POP (DU/s)",
            "value": 22344,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (total)",
            "value": 44648.8,
            "unit": "ns/DU",
            "extra": "median=5581100ns  p95=5763400ns  stddev=65477.1ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (handler)",
            "value": 70.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (DU/s)",
            "value": 22397,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (total)",
            "value": 44643.2,
            "unit": "ns/DU",
            "extra": "median=5580400ns  p95=5766000ns  stddev=67097.9ns  iterations=80  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (handler)",
            "value": 64.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (DU/s)",
            "value": 22400,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (total)",
            "value": 44569.6,
            "unit": "ns/DU",
            "extra": "median=5571200ns  p95=5848000ns  stddev=85020.8ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (handler)",
            "value": -8.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (DU/s)",
            "value": 22437,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ADD (total)",
            "value": 44478.4,
            "unit": "ns/DU",
            "extra": "median=5559800ns  p95=5786800ns  stddev=84958.9ns  iterations=74  DUs=125"
          },
          {
            "name": "[StandardPolicy] ADD (handler)",
            "value": -100,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ADD (DU/s)",
            "value": 22483,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SUB (total)",
            "value": 44536,
            "unit": "ns/DU",
            "extra": "median=5567000ns  p95=5785600ns  stddev=84399.8ns  iterations=83  DUs=125"
          },
          {
            "name": "[StandardPolicy] SUB (handler)",
            "value": -42.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SUB (DU/s)",
            "value": 22454,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUL (total)",
            "value": 44474.4,
            "unit": "ns/DU",
            "extra": "median=5559300ns  p95=5727300ns  stddev=54219.0ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUL (handler)",
            "value": -104,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUL (DU/s)",
            "value": 22485,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IMUL (total)",
            "value": 44504.8,
            "unit": "ns/DU",
            "extra": "median=5563100ns  p95=5761900ns  stddev=60870.6ns  iterations=79  DUs=125"
          },
          {
            "name": "[StandardPolicy] IMUL (handler)",
            "value": -73.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IMUL (DU/s)",
            "value": 22469,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] DIV (total)",
            "value": 44541.6,
            "unit": "ns/DU",
            "extra": "median=5567700ns  p95=5781000ns  stddev=76433.2ns  iterations=77  DUs=125"
          },
          {
            "name": "[StandardPolicy] DIV (handler)",
            "value": -36.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] DIV (DU/s)",
            "value": 22451,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IDIV (total)",
            "value": 44529.6,
            "unit": "ns/DU",
            "extra": "median=5566200ns  p95=5727900ns  stddev=59052.3ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] IDIV (handler)",
            "value": -48.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IDIV (DU/s)",
            "value": 22457,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NEG (total)",
            "value": 44593.6,
            "unit": "ns/DU",
            "extra": "median=5574200ns  p95=5764600ns  stddev=69118.2ns  iterations=71  DUs=125"
          },
          {
            "name": "[StandardPolicy] NEG (handler)",
            "value": 15.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NEG (DU/s)",
            "value": 22425,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOD (total)",
            "value": 44502.4,
            "unit": "ns/DU",
            "extra": "median=5562800ns  p95=5724600ns  stddev=60716.5ns  iterations=83  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOD (handler)",
            "value": -76,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOD (DU/s)",
            "value": 22471,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] AND (total)",
            "value": 44530.4,
            "unit": "ns/DU",
            "extra": "median=5566300ns  p95=5733200ns  stddev=62976.3ns  iterations=69  DUs=125"
          },
          {
            "name": "[StandardPolicy] AND (handler)",
            "value": -48,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] AND (DU/s)",
            "value": 22457,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] OR (total)",
            "value": 44476.8,
            "unit": "ns/DU",
            "extra": "median=5559600ns  p95=5749900ns  stddev=60818.4ns  iterations=77  DUs=125"
          },
          {
            "name": "[StandardPolicy] OR (handler)",
            "value": -101.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] OR (DU/s)",
            "value": 22484,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XOR (total)",
            "value": 44440,
            "unit": "ns/DU",
            "extra": "median=5555000ns  p95=5721800ns  stddev=54809.3ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] XOR (handler)",
            "value": -138.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XOR (DU/s)",
            "value": 22502,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOT (total)",
            "value": 44648.8,
            "unit": "ns/DU",
            "extra": "median=5581100ns  p95=5804300ns  stddev=80955.2ns  iterations=75  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOT (handler)",
            "value": 70.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOT (DU/s)",
            "value": 22397,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHL (total)",
            "value": 44478.4,
            "unit": "ns/DU",
            "extra": "median=5559800ns  p95=5738700ns  stddev=61576.1ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHL (handler)",
            "value": -100,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHL (DU/s)",
            "value": 22483,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHR (total)",
            "value": 44463.2,
            "unit": "ns/DU",
            "extra": "median=5557900ns  p95=5749600ns  stddev=60140.7ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHR (handler)",
            "value": -115.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHR (DU/s)",
            "value": 22491,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SAR (total)",
            "value": 44529.6,
            "unit": "ns/DU",
            "extra": "median=5566200ns  p95=5739200ns  stddev=68379.0ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] SAR (handler)",
            "value": -48.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SAR (DU/s)",
            "value": 22457,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROL (total)",
            "value": 44496,
            "unit": "ns/DU",
            "extra": "median=5562000ns  p95=5782600ns  stddev=91618.4ns  iterations=72  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROL (handler)",
            "value": -82.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROL (DU/s)",
            "value": 22474,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROR (total)",
            "value": 44534.4,
            "unit": "ns/DU",
            "extra": "median=5566800ns  p95=5738600ns  stddev=61751.4ns  iterations=74  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROR (handler)",
            "value": -44,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROR (DU/s)",
            "value": 22455,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMP (total)",
            "value": 44476,
            "unit": "ns/DU",
            "extra": "median=5559500ns  p95=5836800ns  stddev=95345.0ns  iterations=73  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMP (handler)",
            "value": -102.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMP (DU/s)",
            "value": 22484,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TEST (total)",
            "value": 44508,
            "unit": "ns/DU",
            "extra": "median=5563500ns  p95=5772800ns  stddev=68843.0ns  iterations=79  DUs=125"
          },
          {
            "name": "[StandardPolicy] TEST (handler)",
            "value": -70.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TEST (DU/s)",
            "value": 22468,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (total)",
            "value": 44570.4,
            "unit": "ns/DU",
            "extra": "median=5571300ns  p95=5674200ns  stddev=43497.0ns  iterations=74  DUs=125"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (handler)",
            "value": -8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (DU/s)",
            "value": 22436,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (total)",
            "value": 44600.8,
            "unit": "ns/DU",
            "extra": "median=5575100ns  p95=5842200ns  stddev=82993.7ns  iterations=77  DUs=125"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (handler)",
            "value": 22.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (DU/s)",
            "value": 22421,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JMP (total)",
            "value": 44616,
            "unit": "ns/DU",
            "extra": "median=5577000ns  p95=5778500ns  stddev=65845.7ns  iterations=74  DUs=125"
          },
          {
            "name": "[StandardPolicy] JMP (handler)",
            "value": 37.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JMP (DU/s)",
            "value": 22413,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JCC (total)",
            "value": 44640.8,
            "unit": "ns/DU",
            "extra": "median=5580100ns  p95=5768600ns  stddev=65101.8ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] JCC (handler)",
            "value": 62.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JCC (DU/s)",
            "value": 22401,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (total)",
            "value": 44798.4,
            "unit": "ns/DU",
            "extra": "median=5599800ns  p95=5716900ns  stddev=47686.7ns  iterations=75  DUs=125"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (handler)",
            "value": 220,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (DU/s)",
            "value": 22322,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT8 (total)",
            "value": 44650.4,
            "unit": "ns/DU",
            "extra": "median=5581300ns  p95=5800600ns  stddev=82872.0ns  iterations=80  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT8 (handler)",
            "value": 72,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT8 (DU/s)",
            "value": 22396,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT16 (total)",
            "value": 44593.6,
            "unit": "ns/DU",
            "extra": "median=5574200ns  p95=5748700ns  stddev=69952.8ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT16 (handler)",
            "value": 15.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT16 (DU/s)",
            "value": 22425,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT32 (total)",
            "value": 44628.8,
            "unit": "ns/DU",
            "extra": "median=5578600ns  p95=5779500ns  stddev=74115.6ns  iterations=77  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT32 (handler)",
            "value": 50.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT32 (DU/s)",
            "value": 22407,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (total)",
            "value": 44689.6,
            "unit": "ns/DU",
            "extra": "median=5586200ns  p95=5746700ns  stddev=64869.3ns  iterations=81  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (handler)",
            "value": 111.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (DU/s)",
            "value": 22377,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (total)",
            "value": 44624.8,
            "unit": "ns/DU",
            "extra": "median=5578100ns  p95=5781600ns  stddev=83608.2ns  iterations=73  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (handler)",
            "value": 46.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (DU/s)",
            "value": 22409,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (total)",
            "value": 44628.8,
            "unit": "ns/DU",
            "extra": "median=5578600ns  p95=5734000ns  stddev=54275.3ns  iterations=75  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (handler)",
            "value": 50.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (DU/s)",
            "value": 22407,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (total)",
            "value": 44578.4,
            "unit": "ns/DU",
            "extra": "median=5572300ns  p95=5682800ns  stddev=44548.8ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (DU/s)",
            "value": 22432,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (total)",
            "value": 44623.2,
            "unit": "ns/DU",
            "extra": "median=5577900ns  p95=5713700ns  stddev=65122.8ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (handler)",
            "value": 44.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (DU/s)",
            "value": 22410,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (total)",
            "value": 44488,
            "unit": "ns/DU",
            "extra": "median=5561000ns  p95=5678500ns  stddev=43061.9ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (handler)",
            "value": -90.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (DU/s)",
            "value": 22478,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XCHG (total)",
            "value": 44511.2,
            "unit": "ns/DU",
            "extra": "median=5563900ns  p95=5721200ns  stddev=54994.3ns  iterations=71  DUs=125"
          },
          {
            "name": "[StandardPolicy] XCHG (handler)",
            "value": -67.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XCHG (DU/s)",
            "value": 22466,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (total)",
            "value": 44475.2,
            "unit": "ns/DU",
            "extra": "median=5559400ns  p95=5744400ns  stddev=62919.0ns  iterations=74  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (handler)",
            "value": -103.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (DU/s)",
            "value": 22484,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] FENCE (total)",
            "value": 44625.6,
            "unit": "ns/DU",
            "extra": "median=5578200ns  p95=5761700ns  stddev=73378.0ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] FENCE (handler)",
            "value": 47.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] FENCE (DU/s)",
            "value": 22409,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (total)",
            "value": 44516,
            "unit": "ns/DU",
            "extra": "median=5564500ns  p95=5728500ns  stddev=51982.3ns  iterations=78  DUs=125"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (handler)",
            "value": -62.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (DU/s)",
            "value": 22464,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 44587.2,
            "unit": "ns/DU",
            "extra": "median=5573400ns  p95=5875900ns  stddev=104678.7ns  iterations=76  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 8.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 22428,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (total)",
            "value": 44604.8,
            "unit": "ns/DU",
            "extra": "median=5575600ns  p95=5742400ns  stddev=54204.5ns  iterations=73  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (handler)",
            "value": 26.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 22419,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (total)",
            "value": 44640,
            "unit": "ns/DU",
            "extra": "median=5580000ns  p95=5842200ns  stddev=82227.4ns  iterations=75  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (handler)",
            "value": 61.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (DU/s)",
            "value": 22401,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (total)",
            "value": 44634.4,
            "unit": "ns/DU",
            "extra": "median=5579300ns  p95=5759800ns  stddev=65599.4ns  iterations=81  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (handler)",
            "value": 56,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (DU/s)",
            "value": 22404,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 85135.2,
            "unit": "ns/DU",
            "extra": "median=10641900ns  p95=10759300ns  stddev=50487.1ns  iterations=84  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 11746,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOVE (total)",
            "value": 85208.8,
            "unit": "ns/DU",
            "extra": "median=10651100ns  p95=10815800ns  stddev=57279.8ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOVE (handler)",
            "value": 73.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOVE (DU/s)",
            "value": 11736,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD (total)",
            "value": 85156.8,
            "unit": "ns/DU",
            "extra": "median=10644600ns  p95=10804700ns  stddev=58419.6ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD (handler)",
            "value": 21.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD (DU/s)",
            "value": 11743,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE (total)",
            "value": 85138.4,
            "unit": "ns/DU",
            "extra": "median=10642300ns  p95=10764500ns  stddev=55561.5ns  iterations=85  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE (DU/s)",
            "value": 11746,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] PUSH (total)",
            "value": 85100,
            "unit": "ns/DU",
            "extra": "median=10637500ns  p95=10764600ns  stddev=46530.5ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] PUSH (handler)",
            "value": -35.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] PUSH (DU/s)",
            "value": 11751,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] POP (total)",
            "value": 85218.4,
            "unit": "ns/DU",
            "extra": "median=10652300ns  p95=10767100ns  stddev=48536.8ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] POP (handler)",
            "value": 83.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] POP (DU/s)",
            "value": 11735,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (total)",
            "value": 85172.8,
            "unit": "ns/DU",
            "extra": "median=10646600ns  p95=10776100ns  stddev=50584.1ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (handler)",
            "value": 37.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (DU/s)",
            "value": 11741,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (total)",
            "value": 85195.2,
            "unit": "ns/DU",
            "extra": "median=10649400ns  p95=10754500ns  stddev=46512.9ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (handler)",
            "value": 60,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (DU/s)",
            "value": 11738,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (total)",
            "value": 85161.6,
            "unit": "ns/DU",
            "extra": "median=10645200ns  p95=10817200ns  stddev=71674.6ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (handler)",
            "value": 26.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (DU/s)",
            "value": 11742,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ADD (total)",
            "value": 85131.2,
            "unit": "ns/DU",
            "extra": "median=10641400ns  p95=10804700ns  stddev=63688.1ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ADD (handler)",
            "value": -4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ADD (DU/s)",
            "value": 11747,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SUB (total)",
            "value": 85176,
            "unit": "ns/DU",
            "extra": "median=10647000ns  p95=10788800ns  stddev=58889.4ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SUB (handler)",
            "value": 40.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SUB (DU/s)",
            "value": 11740,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUL (total)",
            "value": 85170.4,
            "unit": "ns/DU",
            "extra": "median=10646300ns  p95=10727500ns  stddev=38720.8ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUL (handler)",
            "value": 35.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUL (DU/s)",
            "value": 11741,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IMUL (total)",
            "value": 85181.6,
            "unit": "ns/DU",
            "extra": "median=10647700ns  p95=10761800ns  stddev=50634.3ns  iterations=85  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IMUL (handler)",
            "value": 46.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IMUL (DU/s)",
            "value": 11740,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] DIV (total)",
            "value": 85184,
            "unit": "ns/DU",
            "extra": "median=10648000ns  p95=10766200ns  stddev=44665.2ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] DIV (handler)",
            "value": 48.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] DIV (DU/s)",
            "value": 11739,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IDIV (total)",
            "value": 85168.8,
            "unit": "ns/DU",
            "extra": "median=10646100ns  p95=10796100ns  stddev=64524.0ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IDIV (handler)",
            "value": 33.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IDIV (DU/s)",
            "value": 11741,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NEG (total)",
            "value": 85209.6,
            "unit": "ns/DU",
            "extra": "median=10651200ns  p95=10755300ns  stddev=46787.7ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NEG (handler)",
            "value": 74.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NEG (DU/s)",
            "value": 11736,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOD (total)",
            "value": 85146.4,
            "unit": "ns/DU",
            "extra": "median=10643300ns  p95=10788100ns  stddev=59052.5ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOD (handler)",
            "value": 11.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOD (DU/s)",
            "value": 11744,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] AND (total)",
            "value": 85148,
            "unit": "ns/DU",
            "extra": "median=10643500ns  p95=10775100ns  stddev=51106.7ns  iterations=83  DUs=125"
          },
          {
            "name": "[HighSecPolicy] AND (handler)",
            "value": 12.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] AND (DU/s)",
            "value": 11744,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] OR (total)",
            "value": 85160,
            "unit": "ns/DU",
            "extra": "median=10645000ns  p95=10743400ns  stddev=46575.0ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] OR (handler)",
            "value": 24.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] OR (DU/s)",
            "value": 11743,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XOR (total)",
            "value": 85125.6,
            "unit": "ns/DU",
            "extra": "median=10640700ns  p95=10762700ns  stddev=45681.6ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XOR (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XOR (DU/s)",
            "value": 11747,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOT (total)",
            "value": 85180.8,
            "unit": "ns/DU",
            "extra": "median=10647600ns  p95=10771300ns  stddev=49400.1ns  iterations=83  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOT (handler)",
            "value": 45.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOT (DU/s)",
            "value": 11740,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHL (total)",
            "value": 85183.2,
            "unit": "ns/DU",
            "extra": "median=10647900ns  p95=10759000ns  stddev=46736.1ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHL (handler)",
            "value": 48,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHL (DU/s)",
            "value": 11739,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHR (total)",
            "value": 85200,
            "unit": "ns/DU",
            "extra": "median=10650000ns  p95=10793600ns  stddev=55113.8ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHR (handler)",
            "value": 64.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHR (DU/s)",
            "value": 11737,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SAR (total)",
            "value": 85278.4,
            "unit": "ns/DU",
            "extra": "median=10659800ns  p95=10809300ns  stddev=58090.6ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SAR (handler)",
            "value": 143.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SAR (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROL (total)",
            "value": 85154.4,
            "unit": "ns/DU",
            "extra": "median=10644300ns  p95=10723900ns  stddev=36185.3ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROL (handler)",
            "value": 19.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROL (DU/s)",
            "value": 11743,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROR (total)",
            "value": 85264,
            "unit": "ns/DU",
            "extra": "median=10658000ns  p95=10868900ns  stddev=75329.0ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROR (handler)",
            "value": 128.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROR (DU/s)",
            "value": 11728,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMP (total)",
            "value": 85114.4,
            "unit": "ns/DU",
            "extra": "median=10639300ns  p95=10749500ns  stddev=48538.3ns  iterations=85  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMP (handler)",
            "value": -20.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMP (DU/s)",
            "value": 11749,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TEST (total)",
            "value": 85168.8,
            "unit": "ns/DU",
            "extra": "median=10646100ns  p95=10747300ns  stddev=52381.3ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TEST (handler)",
            "value": 33.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TEST (DU/s)",
            "value": 11741,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (total)",
            "value": 85125.6,
            "unit": "ns/DU",
            "extra": "median=10640700ns  p95=10733200ns  stddev=40371.7ns  iterations=83  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (handler)",
            "value": -9.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (DU/s)",
            "value": 11747,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (total)",
            "value": 85149.6,
            "unit": "ns/DU",
            "extra": "median=10643700ns  p95=10820100ns  stddev=66126.1ns  iterations=82  DUs=125"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (handler)",
            "value": 14.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (DU/s)",
            "value": 11744,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JMP (total)",
            "value": 85165.6,
            "unit": "ns/DU",
            "extra": "median=10645700ns  p95=10748700ns  stddev=42024.6ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JMP (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JMP (DU/s)",
            "value": 11742,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JCC (total)",
            "value": 85136,
            "unit": "ns/DU",
            "extra": "median=10642000ns  p95=10804500ns  stddev=58899.5ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JCC (handler)",
            "value": 0.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JCC (DU/s)",
            "value": 11746,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (total)",
            "value": 85389.6,
            "unit": "ns/DU",
            "extra": "median=10673700ns  p95=10822200ns  stddev=63214.4ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (handler)",
            "value": 254.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (DU/s)",
            "value": 11711,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (total)",
            "value": 85183.2,
            "unit": "ns/DU",
            "extra": "median=10647900ns  p95=10757000ns  stddev=46395.1ns  iterations=83  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (handler)",
            "value": 48,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (DU/s)",
            "value": 11739,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (total)",
            "value": 85247.2,
            "unit": "ns/DU",
            "extra": "median=10655900ns  p95=10823900ns  stddev=63299.4ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (handler)",
            "value": 112,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (DU/s)",
            "value": 11731,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (total)",
            "value": 85217.6,
            "unit": "ns/DU",
            "extra": "median=10652200ns  p95=10783900ns  stddev=53569.2ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (handler)",
            "value": 82.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (DU/s)",
            "value": 11735,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (total)",
            "value": 85168,
            "unit": "ns/DU",
            "extra": "median=10646000ns  p95=10760000ns  stddev=49828.0ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (handler)",
            "value": 32.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (DU/s)",
            "value": 11741,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (total)",
            "value": 85164.8,
            "unit": "ns/DU",
            "extra": "median=10645600ns  p95=10764300ns  stddev=52148.5ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (handler)",
            "value": 29.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (DU/s)",
            "value": 11742,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (total)",
            "value": 85208.8,
            "unit": "ns/DU",
            "extra": "median=10651100ns  p95=10785500ns  stddev=55767.1ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (handler)",
            "value": 73.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (DU/s)",
            "value": 11736,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (total)",
            "value": 85212.8,
            "unit": "ns/DU",
            "extra": "median=10651600ns  p95=10801800ns  stddev=53126.0ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (handler)",
            "value": 77.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (DU/s)",
            "value": 11735,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (total)",
            "value": 85220.8,
            "unit": "ns/DU",
            "extra": "median=10652600ns  p95=10771100ns  stddev=52274.2ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (handler)",
            "value": 85.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (DU/s)",
            "value": 11734,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (total)",
            "value": 85194.4,
            "unit": "ns/DU",
            "extra": "median=10649300ns  p95=10777800ns  stddev=51772.2ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (handler)",
            "value": 59.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (DU/s)",
            "value": 11738,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XCHG (total)",
            "value": 85188.8,
            "unit": "ns/DU",
            "extra": "median=10648600ns  p95=10827400ns  stddev=62883.8ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XCHG (handler)",
            "value": 53.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XCHG (DU/s)",
            "value": 11739,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (total)",
            "value": 85120.8,
            "unit": "ns/DU",
            "extra": "median=10640100ns  p95=10728800ns  stddev=39164.4ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (handler)",
            "value": -14.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (DU/s)",
            "value": 11748,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] FENCE (total)",
            "value": 85180.8,
            "unit": "ns/DU",
            "extra": "median=10647600ns  p95=10813400ns  stddev=62242.4ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] FENCE (handler)",
            "value": 45.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] FENCE (DU/s)",
            "value": 11740,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (total)",
            "value": 85213.6,
            "unit": "ns/DU",
            "extra": "median=10651700ns  p95=10775400ns  stddev=51371.5ns  iterations=85  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (handler)",
            "value": 78.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (DU/s)",
            "value": 11735,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 85176.8,
            "unit": "ns/DU",
            "extra": "median=10647100ns  p95=10795200ns  stddev=57526.8ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 41.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 11740,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (total)",
            "value": 85205.6,
            "unit": "ns/DU",
            "extra": "median=10650700ns  p95=10743900ns  stddev=43015.6ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (handler)",
            "value": 70.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 11736,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (total)",
            "value": 85162.4,
            "unit": "ns/DU",
            "extra": "median=10645300ns  p95=10779600ns  stddev=53726.6ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (handler)",
            "value": 27.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (DU/s)",
            "value": 11742,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (total)",
            "value": 85108.8,
            "unit": "ns/DU",
            "extra": "median=10638600ns  p95=10794300ns  stddev=59454.4ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (handler)",
            "value": -26.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (DU/s)",
            "value": 11750,
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
          "id": "fa6035ad3761b4eb6c8ce62310bea3145487b21f",
          "message": "perf(runtime): reduce fixed overhead without changing security semantics (P1-P8)\n\nRemove waste around mandatory crypto work while preserving all security\ninvariants from doc 16, 17, and 19. No phases removed, no branch shapes\nchanged, no crypto semantics altered.\n\n- P1: replace heap alloc in verify_bb_mac with stack buffer (VM_MAX_BB_INSN_CAP)\n- P2: replace O(n) find_bb_index with O(1) dense vector lookup\n- P3: hash exec.regs[] directly, eliminating 128-byte copy per instruction\n- P4: preexpand BLAKE3 [K||K] key once for Phase F + Phase G\n- P5: precompute bb_end_ip at load time\n- P6: pre-decode bb_enc_seed as uint64_t at load time\n- P7: build siphash_expand message prefix once outside 8-iteration loop\n- P8: hoist ORAM temp buffers out of 64-line loop, reuse keystream buffer",
          "timestamp": "2026-04-05T18:11:33+08:00",
          "tree_id": "c0b5fdef24f7de5a7ce8c7e2f6db9c19f53dfbff",
          "url": "https://github.com/scc-tw/VMPilot/commit/fa6035ad3761b4eb6c8ce62310bea3145487b21f"
        },
        "date": 1775384414033,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 5688.8,
            "unit": "ns/DU",
            "extra": "median=711100ns  p95=732100ns  stddev=12615.0ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 175784,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOVE (total)",
            "value": 5623.2,
            "unit": "ns/DU",
            "extra": "median=702900ns  p95=715000ns  stddev=4511.8ns  iterations=77  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOVE (handler)",
            "value": -65.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOVE (DU/s)",
            "value": 177835,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD (total)",
            "value": 5640.8,
            "unit": "ns/DU",
            "extra": "median=705100ns  p95=729300ns  stddev=11120.3ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD (handler)",
            "value": -48,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD (DU/s)",
            "value": 177280,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE (total)",
            "value": 5628.8,
            "unit": "ns/DU",
            "extra": "median=703600ns  p95=719900ns  stddev=5828.5ns  iterations=80  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE (handler)",
            "value": -60,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE (DU/s)",
            "value": 177658,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] PUSH (total)",
            "value": 5628,
            "unit": "ns/DU",
            "extra": "median=703500ns  p95=726900ns  stddev=9521.3ns  iterations=86  DUs=125"
          },
          {
            "name": "[DebugPolicy] PUSH (handler)",
            "value": -60.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] PUSH (DU/s)",
            "value": 177683,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] POP (total)",
            "value": 5633.6,
            "unit": "ns/DU",
            "extra": "median=704200ns  p95=728500ns  stddev=10460.2ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] POP (handler)",
            "value": -55.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] POP (DU/s)",
            "value": 177506,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (total)",
            "value": 5622.4,
            "unit": "ns/DU",
            "extra": "median=702800ns  p95=725400ns  stddev=9794.9ns  iterations=92  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (handler)",
            "value": -66.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (DU/s)",
            "value": 177860,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (total)",
            "value": 5700.8,
            "unit": "ns/DU",
            "extra": "median=712600ns  p95=734600ns  stddev=13714.4ns  iterations=110  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (handler)",
            "value": 12,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (DU/s)",
            "value": 175414,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (total)",
            "value": 5638.4,
            "unit": "ns/DU",
            "extra": "median=704800ns  p95=731600ns  stddev=11894.8ns  iterations=108  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (handler)",
            "value": -50.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (DU/s)",
            "value": 177355,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ADD (total)",
            "value": 5628,
            "unit": "ns/DU",
            "extra": "median=703500ns  p95=722800ns  stddev=6696.7ns  iterations=82  DUs=125"
          },
          {
            "name": "[DebugPolicy] ADD (handler)",
            "value": -60.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ADD (DU/s)",
            "value": 177683,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SUB (total)",
            "value": 5650.4,
            "unit": "ns/DU",
            "extra": "median=706300ns  p95=732900ns  stddev=12520.0ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] SUB (handler)",
            "value": -38.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SUB (DU/s)",
            "value": 176979,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUL (total)",
            "value": 5652.8,
            "unit": "ns/DU",
            "extra": "median=706600ns  p95=731700ns  stddev=12136.6ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUL (handler)",
            "value": -36,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUL (DU/s)",
            "value": 176903,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IMUL (total)",
            "value": 5625.6,
            "unit": "ns/DU",
            "extra": "median=703200ns  p95=725400ns  stddev=7591.6ns  iterations=86  DUs=125"
          },
          {
            "name": "[DebugPolicy] IMUL (handler)",
            "value": -63.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IMUL (DU/s)",
            "value": 177759,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] DIV (total)",
            "value": 5632,
            "unit": "ns/DU",
            "extra": "median=704000ns  p95=729600ns  stddev=11038.8ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] DIV (handler)",
            "value": -56.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] DIV (DU/s)",
            "value": 177557,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IDIV (total)",
            "value": 5625.6,
            "unit": "ns/DU",
            "extra": "median=703200ns  p95=726300ns  stddev=8280.5ns  iterations=85  DUs=125"
          },
          {
            "name": "[DebugPolicy] IDIV (handler)",
            "value": -63.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IDIV (DU/s)",
            "value": 177759,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NEG (total)",
            "value": 5623.2,
            "unit": "ns/DU",
            "extra": "median=702900ns  p95=718800ns  stddev=6777.6ns  iterations=85  DUs=125"
          },
          {
            "name": "[DebugPolicy] NEG (handler)",
            "value": -65.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NEG (DU/s)",
            "value": 177835,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOD (total)",
            "value": 5628,
            "unit": "ns/DU",
            "extra": "median=703500ns  p95=727300ns  stddev=9880.3ns  iterations=92  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOD (handler)",
            "value": -60.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOD (DU/s)",
            "value": 177683,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] AND (total)",
            "value": 5646.4,
            "unit": "ns/DU",
            "extra": "median=705800ns  p95=730700ns  stddev=10929.1ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] AND (handler)",
            "value": -42.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] AND (DU/s)",
            "value": 177104,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] OR (total)",
            "value": 5636,
            "unit": "ns/DU",
            "extra": "median=704500ns  p95=733500ns  stddev=12112.4ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] OR (handler)",
            "value": -52.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] OR (DU/s)",
            "value": 177431,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XOR (total)",
            "value": 5625.6,
            "unit": "ns/DU",
            "extra": "median=703200ns  p95=724300ns  stddev=7272.4ns  iterations=86  DUs=125"
          },
          {
            "name": "[DebugPolicy] XOR (handler)",
            "value": -63.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XOR (DU/s)",
            "value": 177759,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOT (total)",
            "value": 5633.6,
            "unit": "ns/DU",
            "extra": "median=704200ns  p95=729300ns  stddev=11018.6ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOT (handler)",
            "value": -55.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOT (DU/s)",
            "value": 177506,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHL (total)",
            "value": 5631.2,
            "unit": "ns/DU",
            "extra": "median=703900ns  p95=727600ns  stddev=10108.8ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHL (handler)",
            "value": -57.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHL (DU/s)",
            "value": 177582,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHR (total)",
            "value": 5620.8,
            "unit": "ns/DU",
            "extra": "median=702600ns  p95=715000ns  stddev=5108.9ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHR (handler)",
            "value": -68,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHR (DU/s)",
            "value": 177911,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SAR (total)",
            "value": 5633.6,
            "unit": "ns/DU",
            "extra": "median=704200ns  p95=728000ns  stddev=10009.3ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] SAR (handler)",
            "value": -55.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SAR (DU/s)",
            "value": 177506,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROL (total)",
            "value": 5622.4,
            "unit": "ns/DU",
            "extra": "median=702800ns  p95=717600ns  stddev=5165.9ns  iterations=76  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROL (handler)",
            "value": -66.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROL (DU/s)",
            "value": 177860,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROR (total)",
            "value": 5652.8,
            "unit": "ns/DU",
            "extra": "median=706600ns  p95=744000ns  stddev=14918.4ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROR (handler)",
            "value": -36,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROR (DU/s)",
            "value": 176903,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMP (total)",
            "value": 5626.4,
            "unit": "ns/DU",
            "extra": "median=703300ns  p95=723200ns  stddev=6688.2ns  iterations=83  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMP (handler)",
            "value": -62.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMP (DU/s)",
            "value": 177734,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TEST (total)",
            "value": 5625.6,
            "unit": "ns/DU",
            "extra": "median=703200ns  p95=726800ns  stddev=9622.5ns  iterations=92  DUs=125"
          },
          {
            "name": "[DebugPolicy] TEST (handler)",
            "value": -63.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TEST (DU/s)",
            "value": 177759,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (total)",
            "value": 5636.8,
            "unit": "ns/DU",
            "extra": "median=704600ns  p95=731100ns  stddev=11453.2ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (handler)",
            "value": -52,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (DU/s)",
            "value": 177406,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (total)",
            "value": 5620.8,
            "unit": "ns/DU",
            "extra": "median=702600ns  p95=720200ns  stddev=6528.5ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (handler)",
            "value": -68,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (DU/s)",
            "value": 177911,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JMP (total)",
            "value": 5622.4,
            "unit": "ns/DU",
            "extra": "median=702800ns  p95=716300ns  stddev=5015.7ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] JMP (handler)",
            "value": -66.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JMP (DU/s)",
            "value": 177860,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JCC (total)",
            "value": 5632,
            "unit": "ns/DU",
            "extra": "median=704000ns  p95=727200ns  stddev=8758.1ns  iterations=94  DUs=125"
          },
          {
            "name": "[DebugPolicy] JCC (handler)",
            "value": -56.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JCC (DU/s)",
            "value": 177557,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (total)",
            "value": 5808,
            "unit": "ns/DU",
            "extra": "median=726000ns  p95=750700ns  stddev=11073.1ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (handler)",
            "value": 119.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (DU/s)",
            "value": 172176,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT8 (total)",
            "value": 5620,
            "unit": "ns/DU",
            "extra": "median=702500ns  p95=720800ns  stddev=6459.9ns  iterations=80  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT8 (handler)",
            "value": -68.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT8 (DU/s)",
            "value": 177936,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT16 (total)",
            "value": 5710.4,
            "unit": "ns/DU",
            "extra": "median=713800ns  p95=740300ns  stddev=13748.9ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT16 (handler)",
            "value": 21.6,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT16 (DU/s)",
            "value": 175119,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT32 (total)",
            "value": 5621.6,
            "unit": "ns/DU",
            "extra": "median=702700ns  p95=716500ns  stddev=5029.0ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT32 (handler)",
            "value": -67.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT32 (DU/s)",
            "value": 177885,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (total)",
            "value": 5630.4,
            "unit": "ns/DU",
            "extra": "median=703800ns  p95=728100ns  stddev=9664.4ns  iterations=97  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (handler)",
            "value": -58.4,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (DU/s)",
            "value": 177607,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (total)",
            "value": 5632.8,
            "unit": "ns/DU",
            "extra": "median=704100ns  p95=733300ns  stddev=11824.5ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (handler)",
            "value": -56,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (DU/s)",
            "value": 177532,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (total)",
            "value": 5621.6,
            "unit": "ns/DU",
            "extra": "median=702700ns  p95=717300ns  stddev=5077.5ns  iterations=76  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (handler)",
            "value": -67.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (DU/s)",
            "value": 177885,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (total)",
            "value": 5632.8,
            "unit": "ns/DU",
            "extra": "median=704100ns  p95=728700ns  stddev=10761.1ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (handler)",
            "value": -56,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (DU/s)",
            "value": 177532,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (total)",
            "value": 5628,
            "unit": "ns/DU",
            "extra": "median=703500ns  p95=726700ns  stddev=9188.7ns  iterations=92  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (handler)",
            "value": -60.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (DU/s)",
            "value": 177683,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (total)",
            "value": 5632.8,
            "unit": "ns/DU",
            "extra": "median=704100ns  p95=728000ns  stddev=9576.0ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (handler)",
            "value": -56,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (DU/s)",
            "value": 177532,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XCHG (total)",
            "value": 5620,
            "unit": "ns/DU",
            "extra": "median=702500ns  p95=715800ns  stddev=5297.8ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] XCHG (handler)",
            "value": -68.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XCHG (DU/s)",
            "value": 177936,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (total)",
            "value": 5709.6,
            "unit": "ns/DU",
            "extra": "median=713700ns  p95=736300ns  stddev=13990.2ns  iterations=109  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (handler)",
            "value": 20.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (DU/s)",
            "value": 175144,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] FENCE (total)",
            "value": 5624,
            "unit": "ns/DU",
            "extra": "median=703000ns  p95=717000ns  stddev=5508.7ns  iterations=78  DUs=125"
          },
          {
            "name": "[DebugPolicy] FENCE (handler)",
            "value": -64.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] FENCE (DU/s)",
            "value": 177809,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (total)",
            "value": 5640,
            "unit": "ns/DU",
            "extra": "median=705000ns  p95=729100ns  stddev=11080.0ns  iterations=107  DUs=125"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (handler)",
            "value": -48.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (DU/s)",
            "value": 177305,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 5621.6,
            "unit": "ns/DU",
            "extra": "median=702700ns  p95=715200ns  stddev=4460.4ns  iterations=82  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": -67.2,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 177885,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (total)",
            "value": 5620,
            "unit": "ns/DU",
            "extra": "median=702500ns  p95=719700ns  stddev=5600.2ns  iterations=73  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (handler)",
            "value": -68.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 177936,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (total)",
            "value": 5701.6,
            "unit": "ns/DU",
            "extra": "median=712700ns  p95=735600ns  stddev=13113.0ns  iterations=110  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (handler)",
            "value": 12.8,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (DU/s)",
            "value": 175389,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (total)",
            "value": 5620.8,
            "unit": "ns/DU",
            "extra": "median=702600ns  p95=717200ns  stddev=5833.1ns  iterations=81  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (handler)",
            "value": -68,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (DU/s)",
            "value": 177911,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 44487.2,
            "unit": "ns/DU",
            "extra": "median=5560900ns  p95=5590900ns  stddev=15618.6ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 22478,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOVE (total)",
            "value": 44392,
            "unit": "ns/DU",
            "extra": "median=5549000ns  p95=5577600ns  stddev=14768.9ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOVE (handler)",
            "value": -95.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOVE (DU/s)",
            "value": 22527,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD (total)",
            "value": 44405.6,
            "unit": "ns/DU",
            "extra": "median=5550700ns  p95=5574900ns  stddev=15254.3ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD (handler)",
            "value": -81.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD (DU/s)",
            "value": 22520,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE (total)",
            "value": 44371.2,
            "unit": "ns/DU",
            "extra": "median=5546400ns  p95=5576200ns  stddev=16597.5ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE (handler)",
            "value": -116,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE (DU/s)",
            "value": 22537,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] PUSH (total)",
            "value": 44486.4,
            "unit": "ns/DU",
            "extra": "median=5560800ns  p95=5590200ns  stddev=16173.0ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] PUSH (handler)",
            "value": -0.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] PUSH (DU/s)",
            "value": 22479,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] POP (total)",
            "value": 44489.6,
            "unit": "ns/DU",
            "extra": "median=5561200ns  p95=5585000ns  stddev=15295.8ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] POP (handler)",
            "value": 2.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] POP (DU/s)",
            "value": 22477,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (total)",
            "value": 44523.2,
            "unit": "ns/DU",
            "extra": "median=5565400ns  p95=5603400ns  stddev=18892.1ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (handler)",
            "value": 36,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (DU/s)",
            "value": 22460,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (total)",
            "value": 44498.4,
            "unit": "ns/DU",
            "extra": "median=5562300ns  p95=5596900ns  stddev=18643.0ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (handler)",
            "value": 11.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (DU/s)",
            "value": 22473,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (total)",
            "value": 44488,
            "unit": "ns/DU",
            "extra": "median=5561000ns  p95=5589000ns  stddev=15584.2ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (handler)",
            "value": 0.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (DU/s)",
            "value": 22478,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ADD (total)",
            "value": 44370.4,
            "unit": "ns/DU",
            "extra": "median=5546300ns  p95=5576500ns  stddev=16549.6ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] ADD (handler)",
            "value": -116.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ADD (DU/s)",
            "value": 22538,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SUB (total)",
            "value": 44392.8,
            "unit": "ns/DU",
            "extra": "median=5549100ns  p95=5576600ns  stddev=16588.0ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] SUB (handler)",
            "value": -94.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SUB (DU/s)",
            "value": 22526,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUL (total)",
            "value": 44387.2,
            "unit": "ns/DU",
            "extra": "median=5548400ns  p95=5570900ns  stddev=13404.3ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUL (handler)",
            "value": -100,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUL (DU/s)",
            "value": 22529,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IMUL (total)",
            "value": 44393.6,
            "unit": "ns/DU",
            "extra": "median=5549200ns  p95=5588900ns  stddev=16993.0ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] IMUL (handler)",
            "value": -93.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IMUL (DU/s)",
            "value": 22526,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] DIV (total)",
            "value": 44397.6,
            "unit": "ns/DU",
            "extra": "median=5549700ns  p95=5586100ns  stddev=19859.0ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] DIV (handler)",
            "value": -89.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] DIV (DU/s)",
            "value": 22524,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IDIV (total)",
            "value": 44380,
            "unit": "ns/DU",
            "extra": "median=5547500ns  p95=5577700ns  stddev=16721.3ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] IDIV (handler)",
            "value": -107.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IDIV (DU/s)",
            "value": 22533,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NEG (total)",
            "value": 44556.8,
            "unit": "ns/DU",
            "extra": "median=5569600ns  p95=5600400ns  stddev=17491.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] NEG (handler)",
            "value": 69.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NEG (DU/s)",
            "value": 22443,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOD (total)",
            "value": 44391.2,
            "unit": "ns/DU",
            "extra": "median=5548900ns  p95=5573500ns  stddev=15712.0ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOD (handler)",
            "value": -96,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOD (DU/s)",
            "value": 22527,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] AND (total)",
            "value": 44397.6,
            "unit": "ns/DU",
            "extra": "median=5549700ns  p95=5585100ns  stddev=20569.0ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] AND (handler)",
            "value": -89.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] AND (DU/s)",
            "value": 22524,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] OR (total)",
            "value": 44375.2,
            "unit": "ns/DU",
            "extra": "median=5546900ns  p95=5579300ns  stddev=16229.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] OR (handler)",
            "value": -112,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] OR (DU/s)",
            "value": 22535,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XOR (total)",
            "value": 44380.8,
            "unit": "ns/DU",
            "extra": "median=5547600ns  p95=5577900ns  stddev=17126.3ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] XOR (handler)",
            "value": -106.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XOR (DU/s)",
            "value": 22532,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOT (total)",
            "value": 44522.4,
            "unit": "ns/DU",
            "extra": "median=5565300ns  p95=5598700ns  stddev=17091.2ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOT (handler)",
            "value": 35.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOT (DU/s)",
            "value": 22461,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHL (total)",
            "value": 44380.8,
            "unit": "ns/DU",
            "extra": "median=5547600ns  p95=5589300ns  stddev=18496.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHL (handler)",
            "value": -106.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHL (DU/s)",
            "value": 22532,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHR (total)",
            "value": 44383.2,
            "unit": "ns/DU",
            "extra": "median=5547900ns  p95=5583600ns  stddev=16453.2ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHR (handler)",
            "value": -104,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHR (DU/s)",
            "value": 22531,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SAR (total)",
            "value": 44392,
            "unit": "ns/DU",
            "extra": "median=5549000ns  p95=5573600ns  stddev=14713.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] SAR (handler)",
            "value": -95.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SAR (DU/s)",
            "value": 22527,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROL (total)",
            "value": 44375.2,
            "unit": "ns/DU",
            "extra": "median=5546900ns  p95=5574900ns  stddev=16346.1ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROL (handler)",
            "value": -112,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROL (DU/s)",
            "value": 22535,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROR (total)",
            "value": 44392.8,
            "unit": "ns/DU",
            "extra": "median=5549100ns  p95=5590300ns  stddev=18267.3ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROR (handler)",
            "value": -94.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROR (DU/s)",
            "value": 22526,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMP (total)",
            "value": 44385.6,
            "unit": "ns/DU",
            "extra": "median=5548200ns  p95=5580000ns  stddev=17946.0ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMP (handler)",
            "value": -101.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMP (DU/s)",
            "value": 22530,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TEST (total)",
            "value": 44360,
            "unit": "ns/DU",
            "extra": "median=5545000ns  p95=5604600ns  stddev=23663.5ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] TEST (handler)",
            "value": -127.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TEST (DU/s)",
            "value": 22543,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (total)",
            "value": 44491.2,
            "unit": "ns/DU",
            "extra": "median=5561400ns  p95=5605900ns  stddev=18102.0ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (handler)",
            "value": 4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (DU/s)",
            "value": 22476,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (total)",
            "value": 44528,
            "unit": "ns/DU",
            "extra": "median=5566000ns  p95=5600500ns  stddev=19097.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (handler)",
            "value": 40.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (DU/s)",
            "value": 22458,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JMP (total)",
            "value": 44507.2,
            "unit": "ns/DU",
            "extra": "median=5563400ns  p95=5591500ns  stddev=15894.6ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] JMP (handler)",
            "value": 20,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JMP (DU/s)",
            "value": 22468,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JCC (total)",
            "value": 44507.2,
            "unit": "ns/DU",
            "extra": "median=5563400ns  p95=5602500ns  stddev=17807.7ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] JCC (handler)",
            "value": 20,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JCC (DU/s)",
            "value": 22468,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (total)",
            "value": 44663.2,
            "unit": "ns/DU",
            "extra": "median=5582900ns  p95=5621400ns  stddev=17908.2ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (handler)",
            "value": 176,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (DU/s)",
            "value": 22390,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT8 (total)",
            "value": 44519.2,
            "unit": "ns/DU",
            "extra": "median=5564900ns  p95=5592100ns  stddev=17825.9ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT8 (handler)",
            "value": 32,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT8 (DU/s)",
            "value": 22462,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT16 (total)",
            "value": 44536,
            "unit": "ns/DU",
            "extra": "median=5567000ns  p95=5595800ns  stddev=18337.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT16 (handler)",
            "value": 48.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT16 (DU/s)",
            "value": 22454,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT32 (total)",
            "value": 44509.6,
            "unit": "ns/DU",
            "extra": "median=5563700ns  p95=5606900ns  stddev=19496.6ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT32 (handler)",
            "value": 22.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT32 (DU/s)",
            "value": 22467,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (total)",
            "value": 44532,
            "unit": "ns/DU",
            "extra": "median=5566500ns  p95=5603300ns  stddev=18269.9ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (handler)",
            "value": 44.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (DU/s)",
            "value": 22456,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (total)",
            "value": 44512,
            "unit": "ns/DU",
            "extra": "median=5564000ns  p95=5607000ns  stddev=19573.6ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (handler)",
            "value": 24.8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (DU/s)",
            "value": 22466,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (total)",
            "value": 44493.6,
            "unit": "ns/DU",
            "extra": "median=5561700ns  p95=5587600ns  stddev=16141.3ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (handler)",
            "value": 6.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (DU/s)",
            "value": 22475,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (total)",
            "value": 44490.4,
            "unit": "ns/DU",
            "extra": "median=5561300ns  p95=5594900ns  stddev=18795.6ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (handler)",
            "value": 3.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (DU/s)",
            "value": 22477,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (total)",
            "value": 44518.4,
            "unit": "ns/DU",
            "extra": "median=5564800ns  p95=5607100ns  stddev=19429.9ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (handler)",
            "value": 31.2,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (DU/s)",
            "value": 22463,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (total)",
            "value": 44371.2,
            "unit": "ns/DU",
            "extra": "median=5546400ns  p95=5595200ns  stddev=21321.3ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (handler)",
            "value": -116,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (DU/s)",
            "value": 22537,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XCHG (total)",
            "value": 44376.8,
            "unit": "ns/DU",
            "extra": "median=5547100ns  p95=5574400ns  stddev=13764.9ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] XCHG (handler)",
            "value": -110.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XCHG (DU/s)",
            "value": 22534,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (total)",
            "value": 44385.6,
            "unit": "ns/DU",
            "extra": "median=5548200ns  p95=5579700ns  stddev=16482.6ns  iterations=104  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (handler)",
            "value": -101.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (DU/s)",
            "value": 22530,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] FENCE (total)",
            "value": 44501.6,
            "unit": "ns/DU",
            "extra": "median=5562700ns  p95=5592600ns  stddev=16113.3ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] FENCE (handler)",
            "value": 14.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] FENCE (DU/s)",
            "value": 22471,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (total)",
            "value": 44388.8,
            "unit": "ns/DU",
            "extra": "median=5548600ns  p95=5587600ns  stddev=21817.6ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (handler)",
            "value": -98.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (DU/s)",
            "value": 22528,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 44495.2,
            "unit": "ns/DU",
            "extra": "median=5561900ns  p95=5586100ns  stddev=14229.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 8,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 22474,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (total)",
            "value": 44507.2,
            "unit": "ns/DU",
            "extra": "median=5563400ns  p95=5589100ns  stddev=15398.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (handler)",
            "value": 20,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 22468,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (total)",
            "value": 44517.6,
            "unit": "ns/DU",
            "extra": "median=5564700ns  p95=5584000ns  stddev=16561.1ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (handler)",
            "value": 30.4,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (DU/s)",
            "value": 22463,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (total)",
            "value": 44504.8,
            "unit": "ns/DU",
            "extra": "median=5563100ns  p95=5585000ns  stddev=13910.1ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (handler)",
            "value": 17.6,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (DU/s)",
            "value": 22469,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 85232,
            "unit": "ns/DU",
            "extra": "median=10654000ns  p95=10718700ns  stddev=27628.7ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 11733,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOVE (total)",
            "value": 85296.8,
            "unit": "ns/DU",
            "extra": "median=10662100ns  p95=10723900ns  stddev=27968.5ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOVE (handler)",
            "value": 64.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOVE (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD (total)",
            "value": 85265.6,
            "unit": "ns/DU",
            "extra": "median=10658200ns  p95=10717200ns  stddev=30319.1ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD (handler)",
            "value": 33.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD (DU/s)",
            "value": 11728,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE (total)",
            "value": 85247.2,
            "unit": "ns/DU",
            "extra": "median=10655900ns  p95=10705400ns  stddev=25661.9ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE (handler)",
            "value": 15.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE (DU/s)",
            "value": 11731,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] PUSH (total)",
            "value": 85290.4,
            "unit": "ns/DU",
            "extra": "median=10661300ns  p95=10716700ns  stddev=24115.0ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] PUSH (handler)",
            "value": 58.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] PUSH (DU/s)",
            "value": 11725,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] POP (total)",
            "value": 85199.2,
            "unit": "ns/DU",
            "extra": "median=10649900ns  p95=10705700ns  stddev=21260.3ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] POP (handler)",
            "value": -32.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] POP (DU/s)",
            "value": 11737,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (total)",
            "value": 85252.8,
            "unit": "ns/DU",
            "extra": "median=10656600ns  p95=10738600ns  stddev=30387.1ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (handler)",
            "value": 20.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (DU/s)",
            "value": 11730,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (total)",
            "value": 85258.4,
            "unit": "ns/DU",
            "extra": "median=10657300ns  p95=10707600ns  stddev=24781.8ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (handler)",
            "value": 26.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (DU/s)",
            "value": 11729,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (total)",
            "value": 85260,
            "unit": "ns/DU",
            "extra": "median=10657500ns  p95=10704600ns  stddev=24142.3ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (handler)",
            "value": 28,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (DU/s)",
            "value": 11729,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ADD (total)",
            "value": 85281.6,
            "unit": "ns/DU",
            "extra": "median=10660200ns  p95=10716900ns  stddev=26301.2ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ADD (handler)",
            "value": 49.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ADD (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SUB (total)",
            "value": 85312.8,
            "unit": "ns/DU",
            "extra": "median=10664100ns  p95=10699600ns  stddev=24411.4ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SUB (handler)",
            "value": 80.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SUB (DU/s)",
            "value": 11722,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUL (total)",
            "value": 85273.6,
            "unit": "ns/DU",
            "extra": "median=10659200ns  p95=10723800ns  stddev=25553.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUL (handler)",
            "value": 41.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUL (DU/s)",
            "value": 11727,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IMUL (total)",
            "value": 85314.4,
            "unit": "ns/DU",
            "extra": "median=10664300ns  p95=10717900ns  stddev=24991.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IMUL (handler)",
            "value": 82.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IMUL (DU/s)",
            "value": 11721,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] DIV (total)",
            "value": 85275.2,
            "unit": "ns/DU",
            "extra": "median=10659400ns  p95=10705500ns  stddev=24912.7ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] DIV (handler)",
            "value": 43.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] DIV (DU/s)",
            "value": 11727,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IDIV (total)",
            "value": 85308,
            "unit": "ns/DU",
            "extra": "median=10663500ns  p95=10719400ns  stddev=26382.5ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IDIV (handler)",
            "value": 76,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IDIV (DU/s)",
            "value": 11722,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NEG (total)",
            "value": 85304.8,
            "unit": "ns/DU",
            "extra": "median=10663100ns  p95=10715100ns  stddev=26693.3ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NEG (handler)",
            "value": 72.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NEG (DU/s)",
            "value": 11723,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOD (total)",
            "value": 85298.4,
            "unit": "ns/DU",
            "extra": "median=10662300ns  p95=10710200ns  stddev=25094.5ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOD (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOD (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] AND (total)",
            "value": 85250.4,
            "unit": "ns/DU",
            "extra": "median=10656300ns  p95=10724100ns  stddev=25246.9ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] AND (handler)",
            "value": 18.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] AND (DU/s)",
            "value": 11730,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] OR (total)",
            "value": 85326.4,
            "unit": "ns/DU",
            "extra": "median=10665800ns  p95=10709700ns  stddev=24911.4ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] OR (handler)",
            "value": 94.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] OR (DU/s)",
            "value": 11720,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XOR (total)",
            "value": 85234.4,
            "unit": "ns/DU",
            "extra": "median=10654300ns  p95=10717400ns  stddev=25484.2ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XOR (handler)",
            "value": 2.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XOR (DU/s)",
            "value": 11732,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOT (total)",
            "value": 85298.4,
            "unit": "ns/DU",
            "extra": "median=10662300ns  p95=10721500ns  stddev=26268.0ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOT (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOT (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHL (total)",
            "value": 85294.4,
            "unit": "ns/DU",
            "extra": "median=10661800ns  p95=10707800ns  stddev=26325.3ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHL (handler)",
            "value": 62.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHL (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHR (total)",
            "value": 85281.6,
            "unit": "ns/DU",
            "extra": "median=10660200ns  p95=10722400ns  stddev=27254.3ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHR (handler)",
            "value": 49.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHR (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SAR (total)",
            "value": 85282.4,
            "unit": "ns/DU",
            "extra": "median=10660300ns  p95=10697600ns  stddev=20962.6ns  iterations=106  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SAR (handler)",
            "value": 50.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SAR (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROL (total)",
            "value": 85336,
            "unit": "ns/DU",
            "extra": "median=10667000ns  p95=10723000ns  stddev=26376.2ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROL (handler)",
            "value": 104,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROL (DU/s)",
            "value": 11718,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROR (total)",
            "value": 85323.2,
            "unit": "ns/DU",
            "extra": "median=10665400ns  p95=10753400ns  stddev=32917.5ns  iterations=105  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROR (handler)",
            "value": 91.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROR (DU/s)",
            "value": 11720,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMP (total)",
            "value": 85296,
            "unit": "ns/DU",
            "extra": "median=10662000ns  p95=10713000ns  stddev=26280.5ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMP (handler)",
            "value": 64,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMP (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TEST (total)",
            "value": 85240,
            "unit": "ns/DU",
            "extra": "median=10655000ns  p95=10719300ns  stddev=25583.6ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TEST (handler)",
            "value": 8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TEST (DU/s)",
            "value": 11732,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (total)",
            "value": 85233.6,
            "unit": "ns/DU",
            "extra": "median=10654200ns  p95=10726000ns  stddev=25875.4ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (handler)",
            "value": 1.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (DU/s)",
            "value": 11732,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (total)",
            "value": 85266.4,
            "unit": "ns/DU",
            "extra": "median=10658300ns  p95=10726400ns  stddev=28583.5ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (handler)",
            "value": 34.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (DU/s)",
            "value": 11728,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JMP (total)",
            "value": 85290.4,
            "unit": "ns/DU",
            "extra": "median=10661300ns  p95=10715300ns  stddev=25065.3ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JMP (handler)",
            "value": 58.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JMP (DU/s)",
            "value": 11725,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JCC (total)",
            "value": 85282.4,
            "unit": "ns/DU",
            "extra": "median=10660300ns  p95=10711400ns  stddev=26436.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JCC (handler)",
            "value": 50.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JCC (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (total)",
            "value": 85472.8,
            "unit": "ns/DU",
            "extra": "median=10684100ns  p95=10717300ns  stddev=22593.1ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (handler)",
            "value": 240.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (DU/s)",
            "value": 11700,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (total)",
            "value": 85282.4,
            "unit": "ns/DU",
            "extra": "median=10660300ns  p95=10702500ns  stddev=21211.9ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (handler)",
            "value": 50.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (total)",
            "value": 85265.6,
            "unit": "ns/DU",
            "extra": "median=10658200ns  p95=10716200ns  stddev=24583.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (handler)",
            "value": 33.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (DU/s)",
            "value": 11728,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (total)",
            "value": 85299.2,
            "unit": "ns/DU",
            "extra": "median=10662400ns  p95=10714100ns  stddev=25264.3ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (handler)",
            "value": 67.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (DU/s)",
            "value": 11723,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (total)",
            "value": 85259.2,
            "unit": "ns/DU",
            "extra": "median=10657400ns  p95=10731000ns  stddev=30268.1ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (handler)",
            "value": 27.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (DU/s)",
            "value": 11729,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (total)",
            "value": 85296.8,
            "unit": "ns/DU",
            "extra": "median=10662100ns  p95=10707500ns  stddev=25836.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (handler)",
            "value": 64.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (total)",
            "value": 85261.6,
            "unit": "ns/DU",
            "extra": "median=10657700ns  p95=10711200ns  stddev=22444.4ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (handler)",
            "value": 29.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (DU/s)",
            "value": 11729,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (total)",
            "value": 85239.2,
            "unit": "ns/DU",
            "extra": "median=10654900ns  p95=10709400ns  stddev=25219.0ns  iterations=107  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (handler)",
            "value": 7.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (DU/s)",
            "value": 11732,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (total)",
            "value": 85275.2,
            "unit": "ns/DU",
            "extra": "median=10659400ns  p95=10709500ns  stddev=22495.5ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (handler)",
            "value": 43.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (DU/s)",
            "value": 11727,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (total)",
            "value": 85298.4,
            "unit": "ns/DU",
            "extra": "median=10662300ns  p95=10730500ns  stddev=29600.2ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (handler)",
            "value": 66.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XCHG (total)",
            "value": 85272.8,
            "unit": "ns/DU",
            "extra": "median=10659100ns  p95=10708100ns  stddev=29739.1ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XCHG (handler)",
            "value": 40.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XCHG (DU/s)",
            "value": 11727,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (total)",
            "value": 85256.8,
            "unit": "ns/DU",
            "extra": "median=10657100ns  p95=10717800ns  stddev=25910.9ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (handler)",
            "value": 24.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (DU/s)",
            "value": 11729,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] FENCE (total)",
            "value": 85266.4,
            "unit": "ns/DU",
            "extra": "median=10658300ns  p95=10712600ns  stddev=23707.8ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] FENCE (handler)",
            "value": 34.4,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] FENCE (DU/s)",
            "value": 11728,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (total)",
            "value": 85288.8,
            "unit": "ns/DU",
            "extra": "median=10661100ns  p95=10698200ns  stddev=22911.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (handler)",
            "value": 56.8,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (DU/s)",
            "value": 11725,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 85279.2,
            "unit": "ns/DU",
            "extra": "median=10659900ns  p95=10745100ns  stddev=32333.0ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 47.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (total)",
            "value": 85243.2,
            "unit": "ns/DU",
            "extra": "median=10655400ns  p95=10696700ns  stddev=23153.4ns  iterations=103  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (handler)",
            "value": 11.2,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 11731,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (total)",
            "value": 85277.6,
            "unit": "ns/DU",
            "extra": "median=10659700ns  p95=10716900ns  stddev=26586.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (handler)",
            "value": 45.6,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (DU/s)",
            "value": 11726,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (total)",
            "value": 85296,
            "unit": "ns/DU",
            "extra": "median=10662000ns  p95=10706500ns  stddev=26873.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (handler)",
            "value": 64,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (DU/s)",
            "value": 11724,
            "unit": "DU/s"
          }
        ]
      }
    ]
  }
}