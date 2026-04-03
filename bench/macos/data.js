window.BENCHMARK_DATA = {
  "lastUpdate": 1775238135489,
  "repoUrl": "https://github.com/scc-tw/VMPilot",
  "entries": {
    "macos": [
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
        "date": 1775233494847,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 20494,
            "unit": "ns/DU",
            "extra": "median=2561750ns  p95=2714500ns  stddev=86894.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 48795,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 25715.67,
            "unit": "ns/DU",
            "extra": "median=3214459ns  p95=4546291ns  stddev=685235.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 5221.67,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 38887,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 27670.33,
            "unit": "ns/DU",
            "extra": "median=3458791ns  p95=7429625ns  stddev=1289939.1ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 7176.33,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 36140,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 25493.66,
            "unit": "ns/DU",
            "extra": "median=3186708ns  p95=6299666ns  stddev=1127017.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 4999.66,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 39225,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 21496,
            "unit": "ns/DU",
            "extra": "median=2687000ns  p95=6658125ns  stddev=1160333.7ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 1002,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 46520,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 20482,
            "unit": "ns/DU",
            "extra": "median=2560250ns  p95=3825667ns  stddev=486750.3ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": -12,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 48823,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 20361.34,
            "unit": "ns/DU",
            "extra": "median=2545167ns  p95=2720125ns  stddev=77826.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -132.66,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 49113,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 21763.66,
            "unit": "ns/DU",
            "extra": "median=2720458ns  p95=5593584ns  stddev=1088380.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 1269.66,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 45948,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 20182.67,
            "unit": "ns/DU",
            "extra": "median=2522834ns  p95=6579708ns  stddev=1187385.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -311.33,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 49547,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 20145,
            "unit": "ns/DU",
            "extra": "median=2518125ns  p95=6181000ns  stddev=1047742.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -349,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 49640,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 20424,
            "unit": "ns/DU",
            "extra": "median=2553000ns  p95=3236541ns  stddev=232049.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -70,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 48962,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 21250.66,
            "unit": "ns/DU",
            "extra": "median=2656333ns  p95=7120250ns  stddev=1300915.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 756.66,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 47057,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 20146,
            "unit": "ns/DU",
            "extra": "median=2518250ns  p95=3404208ns  stddev=269377.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": -348,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 49638,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 22510.34,
            "unit": "ns/DU",
            "extra": "median=2813792ns  p95=4003791ns  stddev=510668.7ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 2016.34,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 44424,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 23110.67,
            "unit": "ns/DU",
            "extra": "median=2888834ns  p95=3895375ns  stddev=411669.7ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 2616.67,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 43270,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 22531.34,
            "unit": "ns/DU",
            "extra": "median=2816417ns  p95=3789583ns  stddev=361792.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 2037.34,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 44383,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 21626.33,
            "unit": "ns/DU",
            "extra": "median=2703291ns  p95=4377791ns  stddev=615017.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 1132.33,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 46240,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 20512.34,
            "unit": "ns/DU",
            "extra": "median=2564042ns  p95=3245791ns  stddev=238590.7ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 18.34,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 48751,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 22583.66,
            "unit": "ns/DU",
            "extra": "median=2822958ns  p95=3503625ns  stddev=332908.6ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 2089.66,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 44280,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 20709.67,
            "unit": "ns/DU",
            "extra": "median=2588709ns  p95=6601666ns  stddev=1155840.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 215.67,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 48287,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 21456,
            "unit": "ns/DU",
            "extra": "median=2682000ns  p95=3580708ns  stddev=407988.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 962,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 46607,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 21966.33,
            "unit": "ns/DU",
            "extra": "median=2745791ns  p95=3047375ns  stddev=163603.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 1472.33,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 45524,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 21575.66,
            "unit": "ns/DU",
            "extra": "median=2696958ns  p95=3852583ns  stddev=464564.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 1081.66,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 46349,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 22266.34,
            "unit": "ns/DU",
            "extra": "median=2783292ns  p95=3650917ns  stddev=420864.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 1772.34,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 44911,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 21000.33,
            "unit": "ns/DU",
            "extra": "median=2625041ns  p95=4224583ns  stddev=471872.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 506.33,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 47618,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 20497.67,
            "unit": "ns/DU",
            "extra": "median=2562209ns  p95=3122417ns  stddev=187346.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 3.67,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 48786,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 20537.33,
            "unit": "ns/DU",
            "extra": "median=2567166ns  p95=4218167ns  stddev=506709.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 43.33,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 48692,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 19756.34,
            "unit": "ns/DU",
            "extra": "median=2469542ns  p95=2622250ns  stddev=58998.7ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": -737.66,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 50617,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 20555.66,
            "unit": "ns/DU",
            "extra": "median=2569458ns  p95=3276000ns  stddev=294960.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 61.66,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 48648,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 19632.34,
            "unit": "ns/DU",
            "extra": "median=2454042ns  p95=2855917ns  stddev=115754.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -861.66,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 50936,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 19774.34,
            "unit": "ns/DU",
            "extra": "median=2471792ns  p95=2551250ns  stddev=42114.9ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -719.66,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 50571,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 26692.34,
            "unit": "ns/DU",
            "extra": "median=3336542ns  p95=4683750ns  stddev=689887.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 6198.34,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 37464,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 24513.34,
            "unit": "ns/DU",
            "extra": "median=3064167ns  p95=4380083ns  stddev=553721.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 4019.34,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 40794,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 20476.66,
            "unit": "ns/DU",
            "extra": "median=2559583ns  p95=4294375ns  stddev=565679.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -17.34,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 48836,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 19665,
            "unit": "ns/DU",
            "extra": "median=2458125ns  p95=2511250ns  stddev=17832.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -829,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 50852,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 19642.66,
            "unit": "ns/DU",
            "extra": "median=2455333ns  p95=2472209ns  stddev=9568.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -851.34,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 50910,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 20017,
            "unit": "ns/DU",
            "extra": "median=2502125ns  p95=3054667ns  stddev=222886.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -477,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 49958,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 20427,
            "unit": "ns/DU",
            "extra": "median=2553375ns  p95=4125834ns  stddev=456412.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -67,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 48955,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 18554.34,
            "unit": "ns/DU",
            "extra": "median=2319292ns  p95=2516917ns  stddev=76174.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -1939.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 53896,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 20345.34,
            "unit": "ns/DU",
            "extra": "median=2543167ns  p95=6164167ns  stddev=1074646.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -148.66,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 49151,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 19683.33,
            "unit": "ns/DU",
            "extra": "median=2460416ns  p95=2567000ns  stddev=64073.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -810.67,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 50804,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 19624,
            "unit": "ns/DU",
            "extra": "median=2453000ns  p95=2711000ns  stddev=123690.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -870,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 50958,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 20494.34,
            "unit": "ns/DU",
            "extra": "median=2561792ns  p95=2730917ns  stddev=80208.7ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 0.34,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 48794,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 20146.66,
            "unit": "ns/DU",
            "extra": "median=2518333ns  p95=2645000ns  stddev=119734.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -347.34,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 49636,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 19742,
            "unit": "ns/DU",
            "extra": "median=2467750ns  p95=2481875ns  stddev=11060.8ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -752,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 50653,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 19647.34,
            "unit": "ns/DU",
            "extra": "median=2455917ns  p95=3170750ns  stddev=205717.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -846.66,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 50897,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 20550.33,
            "unit": "ns/DU",
            "extra": "median=2568791ns  p95=4513000ns  stddev=711443.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 56.33,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 48661,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 20318.66,
            "unit": "ns/DU",
            "extra": "median=2539833ns  p95=7419583ns  stddev=1630536.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -175.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 49216,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 19823.34,
            "unit": "ns/DU",
            "extra": "median=2477917ns  p95=6826417ns  stddev=1241689.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -670.66,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 50446,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 23432.67,
            "unit": "ns/DU",
            "extra": "median=2929084ns  p95=4323458ns  stddev=559668.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 2938.67,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 42675,
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
        "date": 1775233511830,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 20387.34,
            "unit": "ns/DU",
            "extra": "median=2548417ns  p95=7281291ns  stddev=1397741.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 49050,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 21590.67,
            "unit": "ns/DU",
            "extra": "median=2698834ns  p95=3885083ns  stddev=439939.2ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 1203.34,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 46316,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 20646.34,
            "unit": "ns/DU",
            "extra": "median=2580792ns  p95=5378833ns  stddev=887879.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 259,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 48435,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 20437,
            "unit": "ns/DU",
            "extra": "median=2554625ns  p95=4450458ns  stddev=566064.4ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 49.66,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 48931,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 21115.34,
            "unit": "ns/DU",
            "extra": "median=2639417ns  p95=5833833ns  stddev=1365893.5ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 728,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 47359,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 26264.34,
            "unit": "ns/DU",
            "extra": "median=3283042ns  p95=9058084ns  stddev=1844218.2ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 5877,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 38074,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 28253.66,
            "unit": "ns/DU",
            "extra": "median=3531708ns  p95=7234250ns  stddev=1775082.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 7866.33,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 35394,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 21526,
            "unit": "ns/DU",
            "extra": "median=2690750ns  p95=6478584ns  stddev=1356332.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 1138.66,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 46455,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 20483.34,
            "unit": "ns/DU",
            "extra": "median=2560417ns  p95=6397917ns  stddev=1101505.4ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 96,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 48820,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 20517.34,
            "unit": "ns/DU",
            "extra": "median=2564667ns  p95=7844917ns  stddev=1513870.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 130,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 48739,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 20991.67,
            "unit": "ns/DU",
            "extra": "median=2623959ns  p95=8713833ns  stddev=2078343.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 604.34,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 47638,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 20764.33,
            "unit": "ns/DU",
            "extra": "median=2595541ns  p95=5692083ns  stddev=962772.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 376.99,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 48160,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 21076.66,
            "unit": "ns/DU",
            "extra": "median=2634583ns  p95=7674250ns  stddev=1439213.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 689.33,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 47446,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 20266.34,
            "unit": "ns/DU",
            "extra": "median=2533292ns  p95=11283709ns  stddev=2862276.5ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -121,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 49343,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 20803,
            "unit": "ns/DU",
            "extra": "median=2600375ns  p95=7179833ns  stddev=1429935.5ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 415.66,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 48070,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 21412,
            "unit": "ns/DU",
            "extra": "median=2676500ns  p95=6317833ns  stddev=1089477.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 1024.66,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 46703,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 20387,
            "unit": "ns/DU",
            "extra": "median=2548375ns  p95=7837000ns  stddev=1632872.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -0.34,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 49051,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 20190.67,
            "unit": "ns/DU",
            "extra": "median=2523834ns  p95=7418166ns  stddev=1399491.4ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": -196.66,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 49528,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 20054.33,
            "unit": "ns/DU",
            "extra": "median=2506791ns  p95=7395917ns  stddev=1400587.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -333.01,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 49865,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 20179.34,
            "unit": "ns/DU",
            "extra": "median=2522417ns  p95=7187000ns  stddev=1592982.3ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": -208,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 49556,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 20160.66,
            "unit": "ns/DU",
            "extra": "median=2520083ns  p95=2683416ns  stddev=53790.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": -226.67,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 49602,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 20450.33,
            "unit": "ns/DU",
            "extra": "median=2556291ns  p95=4419208ns  stddev=691106.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 62.99,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 48899,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 27886.67,
            "unit": "ns/DU",
            "extra": "median=3485834ns  p95=7105500ns  stddev=1455491.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 7499.34,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 35859,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 21787.34,
            "unit": "ns/DU",
            "extra": "median=2723417ns  p95=7204916ns  stddev=1734386.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 1400,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 45898,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 20130.66,
            "unit": "ns/DU",
            "extra": "median=2516333ns  p95=3662083ns  stddev=331308.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -256.67,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 49675,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 28921.66,
            "unit": "ns/DU",
            "extra": "median=3615208ns  p95=6088791ns  stddev=1212047.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 8534.33,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 34576,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 29295,
            "unit": "ns/DU",
            "extra": "median=3661875ns  p95=5752333ns  stddev=1045257.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 8907.66,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 34136,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 24973.33,
            "unit": "ns/DU",
            "extra": "median=3121666ns  p95=9458875ns  stddev=2104967.6ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 4585.99,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 40043,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 30305,
            "unit": "ns/DU",
            "extra": "median=3788125ns  p95=7650584ns  stddev=1848625.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 9917.66,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 32998,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 23104.66,
            "unit": "ns/DU",
            "extra": "median=2888083ns  p95=3713083ns  stddev=361990.7ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 2717.33,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 43281,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 22251,
            "unit": "ns/DU",
            "extra": "median=2781375ns  p95=3727833ns  stddev=375915.3ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 1863.66,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 44942,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 21128,
            "unit": "ns/DU",
            "extra": "median=2641000ns  p95=6689375ns  stddev=1342056.6ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 740.66,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 47331,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 20381,
            "unit": "ns/DU",
            "extra": "median=2547625ns  p95=13418291ns  stddev=3205006.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -6.34,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 49065,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 20717.67,
            "unit": "ns/DU",
            "extra": "median=2589709ns  p95=7204667ns  stddev=1414924.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 330.34,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 48268,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 21859.66,
            "unit": "ns/DU",
            "extra": "median=2732458ns  p95=7015875ns  stddev=1525385.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 1472.33,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 45746,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 26777.66,
            "unit": "ns/DU",
            "extra": "median=3347208ns  p95=8062750ns  stddev=1786537.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 6390.33,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 37345,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 20191,
            "unit": "ns/DU",
            "extra": "median=2523875ns  p95=8321167ns  stddev=2520979.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -196.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 49527,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 20422.34,
            "unit": "ns/DU",
            "extra": "median=2552792ns  p95=3205417ns  stddev=283865.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 35,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 48966,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 20560,
            "unit": "ns/DU",
            "extra": "median=2570000ns  p95=10200500ns  stddev=2224657.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 172.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 48638,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 20660.66,
            "unit": "ns/DU",
            "extra": "median=2582583ns  p95=7401292ns  stddev=1386105.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 273.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 48401,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 28554.66,
            "unit": "ns/DU",
            "extra": "median=3569333ns  p95=8288083ns  stddev=2237779.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 8167.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 35021,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 20533.33,
            "unit": "ns/DU",
            "extra": "median=2566666ns  p95=7726875ns  stddev=1851117.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 145.99,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 48701,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 21084.33,
            "unit": "ns/DU",
            "extra": "median=2635541ns  p95=6815333ns  stddev=1362343.5ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 696.99,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 47429,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 25705.34,
            "unit": "ns/DU",
            "extra": "median=3213167ns  p95=7202583ns  stddev=1463583.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 5318,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 38902,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 26973,
            "unit": "ns/DU",
            "extra": "median=3371625ns  p95=7131541ns  stddev=1215283.2ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 6585.66,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 37074,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 20393.34,
            "unit": "ns/DU",
            "extra": "median=2549167ns  p95=7919625ns  stddev=1749663.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 6,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 49036,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 19856.66,
            "unit": "ns/DU",
            "extra": "median=2482083ns  p95=7397250ns  stddev=1418102.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -530.67,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 50361,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 20076,
            "unit": "ns/DU",
            "extra": "median=2509500ns  p95=4018458ns  stddev=433373.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -311.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 49811,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 20929,
            "unit": "ns/DU",
            "extra": "median=2616125ns  p95=21227791ns  stddev=5454692.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 541.66,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 47781,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 33567.67,
            "unit": "ns/DU",
            "extra": "median=4195959ns  p95=8615292ns  stddev=2044944.5ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 13180.34,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 29791,
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
        "date": 1775235118552,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 18041.33,
            "unit": "ns/DU",
            "extra": "median=2255166ns  p95=2358708ns  stddev=39819.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 55428,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 18623.34,
            "unit": "ns/DU",
            "extra": "median=2327917ns  p95=2643041ns  stddev=118536.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 582.01,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 53696,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 19877.66,
            "unit": "ns/DU",
            "extra": "median=2484708ns  p95=2551750ns  stddev=108986.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 1836.34,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 50308,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 18116.66,
            "unit": "ns/DU",
            "extra": "median=2264583ns  p95=2397166ns  stddev=61281.9ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 75.34,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 55198,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 20173.67,
            "unit": "ns/DU",
            "extra": "median=2521709ns  p95=2719042ns  stddev=136838.8ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 2132.34,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 49570,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 20901,
            "unit": "ns/DU",
            "extra": "median=2612625ns  p95=4444292ns  stddev=535522.3ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 2859.67,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 47845,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 20007.33,
            "unit": "ns/DU",
            "extra": "median=2500916ns  p95=2603208ns  stddev=52838.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 1966,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 49982,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 19924.66,
            "unit": "ns/DU",
            "extra": "median=2490583ns  p95=2694209ns  stddev=64559.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 1883.34,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 50189,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 19767,
            "unit": "ns/DU",
            "extra": "median=2470875ns  p95=2603084ns  stddev=55363.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 1725.67,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 50589,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 19462.67,
            "unit": "ns/DU",
            "extra": "median=2432834ns  p95=2480417ns  stddev=52159.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 1421.34,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 51380,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 18276,
            "unit": "ns/DU",
            "extra": "median=2284500ns  p95=2779459ns  stddev=169050.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 234.67,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 54717,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 20533,
            "unit": "ns/DU",
            "extra": "median=2566625ns  p95=4195625ns  stddev=492373.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 2491.67,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 48702,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 20502.33,
            "unit": "ns/DU",
            "extra": "median=2562791ns  p95=2791958ns  stddev=81699.8ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 2461,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 48775,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 20659.34,
            "unit": "ns/DU",
            "extra": "median=2582417ns  p95=2918875ns  stddev=129217.4ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 2618.01,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 48404,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 20907.66,
            "unit": "ns/DU",
            "extra": "median=2613458ns  p95=2957334ns  stddev=136605.8ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 2866.34,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 47829,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 20141,
            "unit": "ns/DU",
            "extra": "median=2517625ns  p95=2609208ns  stddev=48690.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 2099.67,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 49650,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 32282.67,
            "unit": "ns/DU",
            "extra": "median=4035334ns  p95=7620625ns  stddev=1883223.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 14241.34,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 30976,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 20047.34,
            "unit": "ns/DU",
            "extra": "median=2505917ns  p95=2645833ns  stddev=53512.6ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 2006.01,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 49882,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 20028.34,
            "unit": "ns/DU",
            "extra": "median=2503542ns  p95=3110708ns  stddev=187471.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 1987.01,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 49929,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 20168.66,
            "unit": "ns/DU",
            "extra": "median=2521083ns  p95=2747500ns  stddev=106123.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 2127.34,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 49582,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 18749,
            "unit": "ns/DU",
            "extra": "median=2343625ns  p95=2431750ns  stddev=49946.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 707.67,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 53336,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 18712.66,
            "unit": "ns/DU",
            "extra": "median=2339083ns  p95=2649584ns  stddev=120141.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 671.34,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 53440,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 18141,
            "unit": "ns/DU",
            "extra": "median=2267625ns  p95=2432167ns  stddev=50271.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 99.67,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 55124,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 18117.33,
            "unit": "ns/DU",
            "extra": "median=2264666ns  p95=2302375ns  stddev=14130.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 76,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 55196,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 20573.66,
            "unit": "ns/DU",
            "extra": "median=2571708ns  p95=2719875ns  stddev=68489.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 2532.34,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 48606,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 20254.66,
            "unit": "ns/DU",
            "extra": "median=2531833ns  p95=2578292ns  stddev=22091.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 2213.34,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 49371,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 19939.33,
            "unit": "ns/DU",
            "extra": "median=2492416ns  p95=2703375ns  stddev=121154.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 1898,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 50152,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 19853.66,
            "unit": "ns/DU",
            "extra": "median=2481708ns  p95=3501000ns  stddev=413701.6ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 1812.34,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 50369,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 20646.67,
            "unit": "ns/DU",
            "extra": "median=2580834ns  p95=6995458ns  stddev=1256920.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 2605.34,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 48434,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 20347.34,
            "unit": "ns/DU",
            "extra": "median=2543417ns  p95=2746666ns  stddev=94710.9ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 2306.01,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 49146,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 20383.34,
            "unit": "ns/DU",
            "extra": "median=2547917ns  p95=2952042ns  stddev=134659.9ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 2342.01,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 49060,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 20556,
            "unit": "ns/DU",
            "extra": "median=2569500ns  p95=2890125ns  stddev=157350.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 2514.67,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 48648,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 18451.66,
            "unit": "ns/DU",
            "extra": "median=2306458ns  p95=2363500ns  stddev=35610.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 410.34,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 54196,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 20931.66,
            "unit": "ns/DU",
            "extra": "median=2616458ns  p95=14174042ns  stddev=4347331.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 2890.34,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 47775,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 20937.66,
            "unit": "ns/DU",
            "extra": "median=2617208ns  p95=6600459ns  stddev=1187429.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 2896.34,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 47761,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 20064.34,
            "unit": "ns/DU",
            "extra": "median=2508042ns  p95=2774541ns  stddev=89248.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 2023.01,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 49840,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 20741.67,
            "unit": "ns/DU",
            "extra": "median=2592709ns  p95=2703208ns  stddev=78106.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 2700.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 48212,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 20723.34,
            "unit": "ns/DU",
            "extra": "median=2590417ns  p95=2960542ns  stddev=154037.4ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 2682.01,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 48255,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 20461.67,
            "unit": "ns/DU",
            "extra": "median=2557709ns  p95=2866208ns  stddev=117088.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 2420.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 48872,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 20669,
            "unit": "ns/DU",
            "extra": "median=2583625ns  p95=2730583ns  stddev=70131.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 2627.67,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 48382,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 20544.34,
            "unit": "ns/DU",
            "extra": "median=2568042ns  p95=2648833ns  stddev=52743.1ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 2503.01,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 48675,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 23466.67,
            "unit": "ns/DU",
            "extra": "median=2933334ns  p95=4932667ns  stddev=688390.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 5425.34,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 42614,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 20370.34,
            "unit": "ns/DU",
            "extra": "median=2546292ns  p95=3174000ns  stddev=273176.1ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 2329.01,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 49091,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 21424.33,
            "unit": "ns/DU",
            "extra": "median=2678041ns  p95=3009875ns  stddev=134089.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 3383,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 46676,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 41921.67,
            "unit": "ns/DU",
            "extra": "median=5240209ns  p95=8263375ns  stddev=1952363.9ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 23880.34,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 23854,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 21119.66,
            "unit": "ns/DU",
            "extra": "median=2639958ns  p95=3374625ns  stddev=225827.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 3078.34,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 47349,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 21762.33,
            "unit": "ns/DU",
            "extra": "median=2720291ns  p95=2869875ns  stddev=92154.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 3721,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 45951,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 21610.67,
            "unit": "ns/DU",
            "extra": "median=2701334ns  p95=3306833ns  stddev=200355.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 3569.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 46273,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 21356.34,
            "unit": "ns/DU",
            "extra": "median=2669542ns  p95=2828208ns  stddev=113780.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 3315.01,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 46825,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 20010,
            "unit": "ns/DU",
            "extra": "median=2501250ns  p95=2646917ns  stddev=54928.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 1968.67,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 49975,
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
        "date": 1775238133580,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 19114.33,
            "unit": "ns/DU",
            "extra": "median=2389291ns  p95=2425833ns  stddev=48718.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 52317,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 29733.34,
            "unit": "ns/DU",
            "extra": "median=3716667ns  p95=8458667ns  stddev=1875843.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 10619.01,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 33632,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 19780,
            "unit": "ns/DU",
            "extra": "median=2472500ns  p95=6338208ns  stddev=1286957.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 665.67,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 50556,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 20097.33,
            "unit": "ns/DU",
            "extra": "median=2512166ns  p95=4453208ns  stddev=582737.6ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 983,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 49758,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 22441.33,
            "unit": "ns/DU",
            "extra": "median=2805166ns  p95=5394708ns  stddev=992194.4ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 3327,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 44561,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 21038.67,
            "unit": "ns/DU",
            "extra": "median=2629834ns  p95=5411458ns  stddev=953170.9ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 1924.34,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 47532,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 19701,
            "unit": "ns/DU",
            "extra": "median=2462625ns  p95=4126041ns  stddev=596168.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 586.67,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 50759,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 23136.34,
            "unit": "ns/DU",
            "extra": "median=2892042ns  p95=4518208ns  stddev=563582.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 4022.01,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 43222,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 23911.67,
            "unit": "ns/DU",
            "extra": "median=2988959ns  p95=19379500ns  stddev=4687342.7ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 4797.34,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 41821,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 19581,
            "unit": "ns/DU",
            "extra": "median=2447625ns  p95=3000792ns  stddev=159403.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 466.67,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 51070,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 19746,
            "unit": "ns/DU",
            "extra": "median=2468250ns  p95=2523958ns  stddev=27229.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 631.67,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 50643,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 23131,
            "unit": "ns/DU",
            "extra": "median=2891375ns  p95=7182583ns  stddev=1391347.9ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 4016.67,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 43232,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 25098.34,
            "unit": "ns/DU",
            "extra": "median=3137292ns  p95=4751250ns  stddev=646940.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 5984.01,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 39843,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 22004,
            "unit": "ns/DU",
            "extra": "median=2750500ns  p95=4958542ns  stddev=675198.4ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 2889.67,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 45446,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 19762.67,
            "unit": "ns/DU",
            "extra": "median=2470334ns  p95=3023083ns  stddev=160869.5ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 648.34,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 50600,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 19668.33,
            "unit": "ns/DU",
            "extra": "median=2458541ns  p95=3840208ns  stddev=400601.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 554,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 50843,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 20373,
            "unit": "ns/DU",
            "extra": "median=2546625ns  p95=4078083ns  stddev=516578.0ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 1258.67,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 49085,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 23778.34,
            "unit": "ns/DU",
            "extra": "median=2972292ns  p95=5655875ns  stddev=861685.7ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 4664.01,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 42055,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 19565.34,
            "unit": "ns/DU",
            "extra": "median=2445667ns  p95=3333042ns  stddev=253834.8ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 451.01,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 51111,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 24100.33,
            "unit": "ns/DU",
            "extra": "median=3012541ns  p95=5112000ns  stddev=952030.0ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 4986,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 41493,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 19755.66,
            "unit": "ns/DU",
            "extra": "median=2469458ns  p95=2571667ns  stddev=48006.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 641.34,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 50618,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 31465.33,
            "unit": "ns/DU",
            "extra": "median=3933166ns  p95=9311917ns  stddev=1925084.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 12351,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 31781,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 19902.33,
            "unit": "ns/DU",
            "extra": "median=2487791ns  p95=2864167ns  stddev=115798.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 788,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 50245,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 19607.66,
            "unit": "ns/DU",
            "extra": "median=2450958ns  p95=2545417ns  stddev=38063.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 493.34,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 51000,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 20144.67,
            "unit": "ns/DU",
            "extra": "median=2518084ns  p95=3594750ns  stddev=342196.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 1030.34,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 49641,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 21290.67,
            "unit": "ns/DU",
            "extra": "median=2661334ns  p95=6898917ns  stddev=1729960.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 2176.34,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 46969,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 19548.34,
            "unit": "ns/DU",
            "extra": "median=2443542ns  p95=2696209ns  stddev=72873.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 434.01,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 51155,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 19661.34,
            "unit": "ns/DU",
            "extra": "median=2457667ns  p95=4865041ns  stddev=692516.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 547.01,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 50861,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 20390,
            "unit": "ns/DU",
            "extra": "median=2548750ns  p95=4045125ns  stddev=501508.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 1275.67,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 49044,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 29261,
            "unit": "ns/DU",
            "extra": "median=3657625ns  p95=6855750ns  stddev=1155616.1ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 10146.67,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 34175,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 25856.66,
            "unit": "ns/DU",
            "extra": "median=3232083ns  p95=11806459ns  stddev=3043647.0ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 6742.34,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 38675,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 19908.66,
            "unit": "ns/DU",
            "extra": "median=2488583ns  p95=2698958ns  stddev=98323.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 794.34,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 50229,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 19625.34,
            "unit": "ns/DU",
            "extra": "median=2453167ns  p95=4408000ns  stddev=558991.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 511.01,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 50955,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 19519.34,
            "unit": "ns/DU",
            "extra": "median=2439917ns  p95=2620084ns  stddev=56788.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 405.01,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 51231,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 19071.67,
            "unit": "ns/DU",
            "extra": "median=2383959ns  p95=2618541ns  stddev=97167.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -42.66,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 52434,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 19806.66,
            "unit": "ns/DU",
            "extra": "median=2475833ns  p95=7877250ns  stddev=1547128.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 692.34,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 50488,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 19747.34,
            "unit": "ns/DU",
            "extra": "median=2468417ns  p95=5665333ns  stddev=917564.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 633.01,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 50640,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 20010.33,
            "unit": "ns/DU",
            "extra": "median=2501291ns  p95=5472750ns  stddev=848413.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 896,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 49974,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 19701,
            "unit": "ns/DU",
            "extra": "median=2462625ns  p95=5518084ns  stddev=948256.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 586.67,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 50759,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 19970.33,
            "unit": "ns/DU",
            "extra": "median=2496291ns  p95=4270750ns  stddev=655739.2ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 856,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 50074,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 25953.66,
            "unit": "ns/DU",
            "extra": "median=3244208ns  p95=7435666ns  stddev=1746307.2ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 6839.34,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 38530,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 24190.34,
            "unit": "ns/DU",
            "extra": "median=3023792ns  p95=4183417ns  stddev=496811.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 5076.01,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 41339,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 25206,
            "unit": "ns/DU",
            "extra": "median=3150750ns  p95=6307625ns  stddev=1210522.5ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 6091.67,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 39673,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 31100.33,
            "unit": "ns/DU",
            "extra": "median=3887541ns  p95=5764458ns  stddev=1041986.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 11986,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 32154,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 25648.66,
            "unit": "ns/DU",
            "extra": "median=3206083ns  p95=5876833ns  stddev=1071036.7ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 6534.34,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 38988,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 19817.66,
            "unit": "ns/DU",
            "extra": "median=2477208ns  p95=3526083ns  stddev=305878.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 703.34,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 50460,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 19273.67,
            "unit": "ns/DU",
            "extra": "median=2409209ns  p95=2548333ns  stddev=51926.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 159.34,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 51884,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 19879,
            "unit": "ns/DU",
            "extra": "median=2484875ns  p95=3312167ns  stddev=248158.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 764.67,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 50304,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 20017.67,
            "unit": "ns/DU",
            "extra": "median=2502209ns  p95=5636125ns  stddev=899891.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 903.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 49956,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 19920.33,
            "unit": "ns/DU",
            "extra": "median=2490041ns  p95=7776625ns  stddev=1593736.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 806,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 50200,
            "unit": "DU/s"
          }
        ]
      }
    ]
  }
}