window.BENCHMARK_DATA = {
  "lastUpdate": 1775385299671,
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
        "date": 1775254069212,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 18045.67,
            "unit": "ns/DU",
            "extra": "median=2255709ns  p95=2379083ns  stddev=51731.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 55415,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 18093,
            "unit": "ns/DU",
            "extra": "median=2261625ns  p95=2351959ns  stddev=32316.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 47.33,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 55270,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 19700.67,
            "unit": "ns/DU",
            "extra": "median=2462584ns  p95=2611667ns  stddev=52103.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 1655,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 50760,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 19847.33,
            "unit": "ns/DU",
            "extra": "median=2480916ns  p95=2507417ns  stddev=17289.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 1801.66,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 50385,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 19912.66,
            "unit": "ns/DU",
            "extra": "median=2489083ns  p95=2519667ns  stddev=14586.4ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 1866.99,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 50219,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 19899.34,
            "unit": "ns/DU",
            "extra": "median=2487417ns  p95=2608625ns  stddev=42080.4ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 1853.66,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 50253,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 20020,
            "unit": "ns/DU",
            "extra": "median=2502500ns  p95=2531500ns  stddev=18537.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 1974.33,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 49950,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 19963.34,
            "unit": "ns/DU",
            "extra": "median=2495417ns  p95=2524375ns  stddev=15394.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 1917.66,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 50092,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 19778.34,
            "unit": "ns/DU",
            "extra": "median=2472292ns  p95=2518292ns  stddev=20044.6ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 1732.66,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 50560,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 19862.34,
            "unit": "ns/DU",
            "extra": "median=2482792ns  p95=2600750ns  stddev=37895.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 1816.66,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 50347,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 19885.67,
            "unit": "ns/DU",
            "extra": "median=2485709ns  p95=2540542ns  stddev=24083.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 1840,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 50287,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 19852,
            "unit": "ns/DU",
            "extra": "median=2481500ns  p95=2581750ns  stddev=37225.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 1806.33,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 50373,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 20141.66,
            "unit": "ns/DU",
            "extra": "median=2517708ns  p95=2781834ns  stddev=82532.7ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 2095.99,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 49648,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 19796.34,
            "unit": "ns/DU",
            "extra": "median=2474542ns  p95=2497958ns  stddev=22250.6ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 1750.66,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 50514,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 19539,
            "unit": "ns/DU",
            "extra": "median=2442375ns  p95=2483250ns  stddev=17902.1ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 1493.33,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 51180,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 19555.66,
            "unit": "ns/DU",
            "extra": "median=2444458ns  p95=2541667ns  stddev=31682.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 1509.99,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 51136,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 19649,
            "unit": "ns/DU",
            "extra": "median=2456125ns  p95=2504041ns  stddev=22332.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 1603.33,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 50893,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 19909,
            "unit": "ns/DU",
            "extra": "median=2488625ns  p95=2603667ns  stddev=45539.3ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 1863.33,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 50229,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 20402.34,
            "unit": "ns/DU",
            "extra": "median=2550292ns  p95=2665917ns  stddev=55090.8ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 2356.66,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 49014,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 19943.66,
            "unit": "ns/DU",
            "extra": "median=2492958ns  p95=2553959ns  stddev=27633.5ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 1897.99,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 50141,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 20329.66,
            "unit": "ns/DU",
            "extra": "median=2541208ns  p95=2627166ns  stddev=40442.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 2283.99,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 49189,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 20339,
            "unit": "ns/DU",
            "extra": "median=2542375ns  p95=2617292ns  stddev=40633.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 2293.33,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 49167,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 20158,
            "unit": "ns/DU",
            "extra": "median=2519750ns  p95=2551208ns  stddev=29855.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 2112.33,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 49608,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 20718,
            "unit": "ns/DU",
            "extra": "median=2589750ns  p95=3088125ns  stddev=179103.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 2672.33,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 48267,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 19995,
            "unit": "ns/DU",
            "extra": "median=2499375ns  p95=2683500ns  stddev=64537.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 1949.33,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 50013,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 20508.33,
            "unit": "ns/DU",
            "extra": "median=2563541ns  p95=2703417ns  stddev=84320.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 2462.66,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 48761,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 19247,
            "unit": "ns/DU",
            "extra": "median=2405875ns  p95=2454125ns  stddev=46796.8ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 1201.33,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 51956,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 19412,
            "unit": "ns/DU",
            "extra": "median=2426500ns  p95=2440667ns  stddev=7764.5ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 1366.33,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 51515,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 19378.66,
            "unit": "ns/DU",
            "extra": "median=2422333ns  p95=2447542ns  stddev=9257.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 1332.99,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 51603,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 19409,
            "unit": "ns/DU",
            "extra": "median=2426125ns  p95=2466083ns  stddev=14679.4ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 1363.33,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 51522,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 19346.66,
            "unit": "ns/DU",
            "extra": "median=2418333ns  p95=2524375ns  stddev=30729.8ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 1300.99,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 51688,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 19343,
            "unit": "ns/DU",
            "extra": "median=2417875ns  p95=2453125ns  stddev=17225.6ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 1297.33,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 51698,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 19537.67,
            "unit": "ns/DU",
            "extra": "median=2442209ns  p95=2477500ns  stddev=20951.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 1492,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 51183,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 19475,
            "unit": "ns/DU",
            "extra": "median=2434375ns  p95=2539917ns  stddev=38140.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 1429.33,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 51348,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 19446.66,
            "unit": "ns/DU",
            "extra": "median=2430833ns  p95=2477625ns  stddev=18432.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 1400.99,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 51423,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 19396.34,
            "unit": "ns/DU",
            "extra": "median=2424542ns  p95=2467083ns  stddev=15548.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 1350.66,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 51556,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 19417,
            "unit": "ns/DU",
            "extra": "median=2427125ns  p95=2543791ns  stddev=40824.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 1371.33,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 51501,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 19405.66,
            "unit": "ns/DU",
            "extra": "median=2425708ns  p95=2436416ns  stddev=13287.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 1359.99,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 51531,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 19676,
            "unit": "ns/DU",
            "extra": "median=2459500ns  p95=9125041ns  stddev=1899761.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 1630.33,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 50823,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 20411,
            "unit": "ns/DU",
            "extra": "median=2551375ns  p95=2635416ns  stddev=58165.2ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 2365.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 48993,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 20079.66,
            "unit": "ns/DU",
            "extra": "median=2509958ns  p95=2855417ns  stddev=103748.3ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 2033.99,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 49802,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 19792.67,
            "unit": "ns/DU",
            "extra": "median=2474084ns  p95=3452666ns  stddev=284834.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 1747,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 50524,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 18096.34,
            "unit": "ns/DU",
            "extra": "median=2262042ns  p95=2288666ns  stddev=11613.5ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 50.66,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 55260,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 18057.66,
            "unit": "ns/DU",
            "extra": "median=2257208ns  p95=2334167ns  stddev=22644.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 11.99,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 55378,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 19479.33,
            "unit": "ns/DU",
            "extra": "median=2434916ns  p95=2481459ns  stddev=92908.9ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 1433.66,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 51336,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 19705.33,
            "unit": "ns/DU",
            "extra": "median=2463166ns  p95=2515042ns  stddev=25841.3ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 1659.66,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 50748,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 19784,
            "unit": "ns/DU",
            "extra": "median=2473000ns  p95=2508459ns  stddev=13246.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 1738.33,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 50546,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 19729.34,
            "unit": "ns/DU",
            "extra": "median=2466167ns  p95=2544166ns  stddev=40431.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 1683.66,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 50686,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 19924.34,
            "unit": "ns/DU",
            "extra": "median=2490542ns  p95=2530375ns  stddev=25518.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 1878.66,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 50190,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 19703.66,
            "unit": "ns/DU",
            "extra": "median=2462958ns  p95=2542083ns  stddev=33508.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 1657.99,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 50752,
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
        "date": 1775265232736,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 28773.34,
            "unit": "ns/DU",
            "extra": "median=3596667ns  p95=7197458ns  stddev=1398058.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 34754,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 30929,
            "unit": "ns/DU",
            "extra": "median=3866125ns  p95=11954042ns  stddev=2825250.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 2155.66,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 32332,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 23911,
            "unit": "ns/DU",
            "extra": "median=2988875ns  p95=10757208ns  stddev=2692765.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": -4862.34,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 41822,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 28501.33,
            "unit": "ns/DU",
            "extra": "median=3562666ns  p95=7342583ns  stddev=1802893.7ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -272.01,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 35086,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 24511,
            "unit": "ns/DU",
            "extra": "median=3063875ns  p95=8427958ns  stddev=1909244.6ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": -4262.34,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 40798,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 35221.34,
            "unit": "ns/DU",
            "extra": "median=4402667ns  p95=8713666ns  stddev=2439704.4ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 6448,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 28392,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 20280.34,
            "unit": "ns/DU",
            "extra": "median=2535042ns  p95=3112625ns  stddev=170741.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -8493,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 49309,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 37726,
            "unit": "ns/DU",
            "extra": "median=4715750ns  p95=7084667ns  stddev=1282128.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 8952.66,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 26507,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 20697,
            "unit": "ns/DU",
            "extra": "median=2587125ns  p95=5413833ns  stddev=850669.0ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -8076.34,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 48316,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 19875.34,
            "unit": "ns/DU",
            "extra": "median=2484417ns  p95=2533583ns  stddev=22730.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -8898,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 50314,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 20337.34,
            "unit": "ns/DU",
            "extra": "median=2542167ns  p95=5090916ns  stddev=966110.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -8436,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 49171,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 20297.33,
            "unit": "ns/DU",
            "extra": "median=2537166ns  p95=6082084ns  stddev=1014568.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": -8476.01,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 49268,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 35260.67,
            "unit": "ns/DU",
            "extra": "median=4407584ns  p95=9644083ns  stddev=1988522.9ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 6487.34,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 28360,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 20602.34,
            "unit": "ns/DU",
            "extra": "median=2575292ns  p95=9242792ns  stddev=2239585.1ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": -8171,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 48538,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 20954.33,
            "unit": "ns/DU",
            "extra": "median=2619291ns  p95=5821500ns  stddev=968834.7ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -7819.01,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 47723,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 38423,
            "unit": "ns/DU",
            "extra": "median=4802875ns  p95=27176583ns  stddev=7238117.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 9649.66,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 26026,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 28405.34,
            "unit": "ns/DU",
            "extra": "median=3550667ns  p95=4496792ns  stddev=747056.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -368,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 35205,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 36553.66,
            "unit": "ns/DU",
            "extra": "median=4569208ns  p95=7591125ns  stddev=1465373.9ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 7780.33,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 27357,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 23165.67,
            "unit": "ns/DU",
            "extra": "median=2895709ns  p95=4872500ns  stddev=797529.3ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -5607.66,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 43167,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 34659,
            "unit": "ns/DU",
            "extra": "median=4332375ns  p95=5026209ns  stddev=926354.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 5885.66,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 28853,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 21065.33,
            "unit": "ns/DU",
            "extra": "median=2633166ns  p95=7010792ns  stddev=1550824.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": -7708.01,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 47471,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 23564.34,
            "unit": "ns/DU",
            "extra": "median=2945542ns  p95=7616583ns  stddev=1892019.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": -5209,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 42437,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 32373.66,
            "unit": "ns/DU",
            "extra": "median=4046708ns  p95=7235333ns  stddev=1257448.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 3600.33,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 30889,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 27799.67,
            "unit": "ns/DU",
            "extra": "median=3474959ns  p95=6446459ns  stddev=1008355.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -973.66,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 35972,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 27929.34,
            "unit": "ns/DU",
            "extra": "median=3491167ns  p95=5505042ns  stddev=961628.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -844,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 35805,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 25088.34,
            "unit": "ns/DU",
            "extra": "median=3136042ns  p95=6401083ns  stddev=1390482.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": -3685,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 39859,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 20867,
            "unit": "ns/DU",
            "extra": "median=2608375ns  p95=7865167ns  stddev=2090010.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -7906.34,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 47923,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 39858.66,
            "unit": "ns/DU",
            "extra": "median=4982333ns  p95=7392833ns  stddev=1821496.6ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 11085.33,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 25089,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 28168,
            "unit": "ns/DU",
            "extra": "median=3521000ns  p95=7681833ns  stddev=1535555.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -605.34,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 35501,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 38124.33,
            "unit": "ns/DU",
            "extra": "median=4765541ns  p95=9492167ns  stddev=2215192.7ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 9350.99,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 26230,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 28778.67,
            "unit": "ns/DU",
            "extra": "median=3597334ns  p95=6580708ns  stddev=1081316.7ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 5.34,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 34748,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 32740,
            "unit": "ns/DU",
            "extra": "median=4092500ns  p95=11671833ns  stddev=3054891.5ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 3966.66,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 30544,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 22457.67,
            "unit": "ns/DU",
            "extra": "median=2807209ns  p95=5672292ns  stddev=921263.0ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -6315.66,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 44528,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 20887.66,
            "unit": "ns/DU",
            "extra": "median=2610958ns  p95=6585125ns  stddev=1466478.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -7885.67,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 47875,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 20980.34,
            "unit": "ns/DU",
            "extra": "median=2622542ns  p95=7262875ns  stddev=1475984.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -7793,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 47664,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 20694.34,
            "unit": "ns/DU",
            "extra": "median=2586792ns  p95=6862542ns  stddev=1542839.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -8079,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 48322,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 21575.67,
            "unit": "ns/DU",
            "extra": "median=2696959ns  p95=7809084ns  stddev=2074496.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -7197.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 46348,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 20582.67,
            "unit": "ns/DU",
            "extra": "median=2572834ns  p95=6196875ns  stddev=1108447.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -8190.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 48585,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 31225.66,
            "unit": "ns/DU",
            "extra": "median=3903208ns  p95=7712667ns  stddev=1625510.6ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 2452.33,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 32025,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 40869,
            "unit": "ns/DU",
            "extra": "median=5108625ns  p95=10887916ns  stddev=2355272.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 12095.66,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 24468,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 27332.33,
            "unit": "ns/DU",
            "extra": "median=3416541ns  p95=5917000ns  stddev=1309121.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -1441.01,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 36587,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 20521.66,
            "unit": "ns/DU",
            "extra": "median=2565208ns  p95=8413917ns  stddev=2207576.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -8251.67,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 48729,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 26861.67,
            "unit": "ns/DU",
            "extra": "median=3357709ns  p95=5946417ns  stddev=1185703.8ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -1911.66,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 37228,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 50129.34,
            "unit": "ns/DU",
            "extra": "median=6266167ns  p95=8613083ns  stddev=1983455.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 21356,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 19948,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 23025.34,
            "unit": "ns/DU",
            "extra": "median=2878167ns  p95=5672708ns  stddev=975863.5ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -5748,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 43430,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 31735.34,
            "unit": "ns/DU",
            "extra": "median=3966917ns  p95=9120458ns  stddev=1984717.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 2962,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 31511,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 20184.34,
            "unit": "ns/DU",
            "extra": "median=2523042ns  p95=5435708ns  stddev=838392.7ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -8589,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 49543,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 25180,
            "unit": "ns/DU",
            "extra": "median=3147500ns  p95=15064292ns  stddev=3963441.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -3593.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 39714,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 30018.33,
            "unit": "ns/DU",
            "extra": "median=3752291ns  p95=8216583ns  stddev=1737170.0ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 1244.99,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 33313,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 24939.66,
            "unit": "ns/DU",
            "extra": "median=3117458ns  p95=5156500ns  stddev=779519.4ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -3833.67,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 40097,
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
        "date": 1775271749363,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 19938,
            "unit": "ns/DU",
            "extra": "median=2492250ns  p95=2686125ns  stddev=63553.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 50155,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 20083,
            "unit": "ns/DU",
            "extra": "median=2510375ns  p95=2584125ns  stddev=42606.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 145,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 49793,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 20205.66,
            "unit": "ns/DU",
            "extra": "median=2525708ns  p95=2763709ns  stddev=76834.3ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 267.66,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 49491,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 19810.66,
            "unit": "ns/DU",
            "extra": "median=2476333ns  p95=2667375ns  stddev=69271.8ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": -127.34,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 50478,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 20379,
            "unit": "ns/DU",
            "extra": "median=2547375ns  p95=3028750ns  stddev=145761.2ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 441,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 49070,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 20015,
            "unit": "ns/DU",
            "extra": "median=2501875ns  p95=2582541ns  stddev=42663.8ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 77,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 49963,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 19276.66,
            "unit": "ns/DU",
            "extra": "median=2409583ns  p95=2426666ns  stddev=13004.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -661.34,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 51876,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 18790,
            "unit": "ns/DU",
            "extra": "median=2348750ns  p95=2398167ns  stddev=27416.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -1148,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 53220,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 19046.34,
            "unit": "ns/DU",
            "extra": "median=2380792ns  p95=2424334ns  stddev=34712.2ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -891.66,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 52504,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 19677,
            "unit": "ns/DU",
            "extra": "median=2459625ns  p95=2625292ns  stddev=63347.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": -261,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 50821,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 19686.67,
            "unit": "ns/DU",
            "extra": "median=2460834ns  p95=2628250ns  stddev=65599.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": -251.33,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 50796,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 19929.67,
            "unit": "ns/DU",
            "extra": "median=2491209ns  p95=2569458ns  stddev=53863.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": -8.33,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 50176,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 20018.66,
            "unit": "ns/DU",
            "extra": "median=2502333ns  p95=2551292ns  stddev=20844.4ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 80.66,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 49953,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 20242.33,
            "unit": "ns/DU",
            "extra": "median=2530291ns  p95=2604042ns  stddev=34892.0ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 304.33,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 49401,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 19495.67,
            "unit": "ns/DU",
            "extra": "median=2436959ns  p95=2532708ns  stddev=30850.9ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": -442.33,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 51293,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 19585.67,
            "unit": "ns/DU",
            "extra": "median=2448209ns  p95=2481875ns  stddev=15948.6ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": -352.33,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 51058,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 19677,
            "unit": "ns/DU",
            "extra": "median=2459625ns  p95=2608250ns  stddev=59872.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": -261,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 50821,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 20165.34,
            "unit": "ns/DU",
            "extra": "median=2520667ns  p95=2600333ns  stddev=35895.8ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 227.34,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 49590,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 19879,
            "unit": "ns/DU",
            "extra": "median=2484875ns  p95=2570750ns  stddev=34315.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": -59,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 50304,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 19708,
            "unit": "ns/DU",
            "extra": "median=2463500ns  p95=2522709ns  stddev=33258.9ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": -230,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 50741,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 19974,
            "unit": "ns/DU",
            "extra": "median=2496750ns  p95=2653625ns  stddev=48140.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 36,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 50065,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 20142.33,
            "unit": "ns/DU",
            "extra": "median=2517791ns  p95=2546208ns  stddev=21889.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 204.33,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 49647,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 19391.34,
            "unit": "ns/DU",
            "extra": "median=2423917ns  p95=2451625ns  stddev=9340.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": -546.66,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 51569,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 19396,
            "unit": "ns/DU",
            "extra": "median=2424500ns  p95=2451541ns  stddev=18336.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": -542,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 51557,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 18850.66,
            "unit": "ns/DU",
            "extra": "median=2356333ns  p95=2578708ns  stddev=72826.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": -1087.34,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 53049,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 20727.66,
            "unit": "ns/DU",
            "extra": "median=2590958ns  p95=2640834ns  stddev=87865.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 789.66,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 48245,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 19735.33,
            "unit": "ns/DU",
            "extra": "median=2466916ns  p95=2629458ns  stddev=67234.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": -202.67,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 50671,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 20092,
            "unit": "ns/DU",
            "extra": "median=2511500ns  p95=2798291ns  stddev=106637.0ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 154,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 49771,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 19548.67,
            "unit": "ns/DU",
            "extra": "median=2443584ns  p95=2515125ns  stddev=29820.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -389.33,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 51154,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 19587,
            "unit": "ns/DU",
            "extra": "median=2448375ns  p95=2507958ns  stddev=24391.7ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -351,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 51054,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 19529.66,
            "unit": "ns/DU",
            "extra": "median=2441208ns  p95=2524000ns  stddev=28176.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -408.34,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 51204,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 20120,
            "unit": "ns/DU",
            "extra": "median=2515000ns  p95=2577584ns  stddev=35681.9ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 182,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 49702,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 20468.66,
            "unit": "ns/DU",
            "extra": "median=2558583ns  p95=2736708ns  stddev=66664.8ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 530.66,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 48855,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 19656,
            "unit": "ns/DU",
            "extra": "median=2457000ns  p95=2531208ns  stddev=33490.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -282,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 50875,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 19493,
            "unit": "ns/DU",
            "extra": "median=2436625ns  p95=2466750ns  stddev=13965.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -445,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 51300,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 19470.34,
            "unit": "ns/DU",
            "extra": "median=2433792ns  p95=2477625ns  stddev=16883.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -467.66,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 51360,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 19428.34,
            "unit": "ns/DU",
            "extra": "median=2428542ns  p95=2516042ns  stddev=28173.5ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -509.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 51471,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 19602.34,
            "unit": "ns/DU",
            "extra": "median=2450292ns  p95=2487833ns  stddev=19952.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -335.66,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 51014,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 19957.34,
            "unit": "ns/DU",
            "extra": "median=2494667ns  p95=2532291ns  stddev=31242.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 19.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 50107,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 19589.67,
            "unit": "ns/DU",
            "extra": "median=2448709ns  p95=2600250ns  stddev=48838.2ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -348.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 51047,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 19389,
            "unit": "ns/DU",
            "extra": "median=2423625ns  p95=2495208ns  stddev=23978.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -549,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 51576,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 19933.34,
            "unit": "ns/DU",
            "extra": "median=2491667ns  p95=2546959ns  stddev=33428.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -4.66,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 50167,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 19397.66,
            "unit": "ns/DU",
            "extra": "median=2424708ns  p95=2506792ns  stddev=25513.3ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": -540.34,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 51553,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 19362.66,
            "unit": "ns/DU",
            "extra": "median=2420333ns  p95=2469541ns  stddev=17069.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -575.34,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 51646,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 19408.66,
            "unit": "ns/DU",
            "extra": "median=2426083ns  p95=2445292ns  stddev=12669.0ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": -529.34,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 51523,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 19574.67,
            "unit": "ns/DU",
            "extra": "median=2446834ns  p95=2519000ns  stddev=24663.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -363.33,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 51086,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 19437.66,
            "unit": "ns/DU",
            "extra": "median=2429708ns  p95=2466250ns  stddev=16897.5ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": -500.34,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 51447,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 19427.66,
            "unit": "ns/DU",
            "extra": "median=2428458ns  p95=2478792ns  stddev=17069.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -510.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 51473,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 20301.34,
            "unit": "ns/DU",
            "extra": "median=2537667ns  p95=2574333ns  stddev=39310.5ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 363.34,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 49258,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 20139,
            "unit": "ns/DU",
            "extra": "median=2517375ns  p95=2611916ns  stddev=38590.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 201,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 49655,
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
        "date": 1775272195262,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 19341,
            "unit": "ns/DU",
            "extra": "median=2417625ns  p95=2441333ns  stddev=10441.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 51704,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 19425.66,
            "unit": "ns/DU",
            "extra": "median=2428208ns  p95=4183208ns  stddev=498628.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 84.66,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 51478,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 19538,
            "unit": "ns/DU",
            "extra": "median=2442250ns  p95=2550208ns  stddev=47352.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 197,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 51182,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 19364,
            "unit": "ns/DU",
            "extra": "median=2420500ns  p95=2546334ns  stddev=36777.1ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 23,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 51642,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 19592.67,
            "unit": "ns/DU",
            "extra": "median=2449084ns  p95=6348000ns  stddev=1114122.9ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 251.67,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 51039,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 20465,
            "unit": "ns/DU",
            "extra": "median=2558125ns  p95=8145541ns  stddev=1630617.3ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 1124,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 48864,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 20417,
            "unit": "ns/DU",
            "extra": "median=2552125ns  p95=7514584ns  stddev=1407063.4ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 1076,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 48979,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 22551.34,
            "unit": "ns/DU",
            "extra": "median=2818917ns  p95=3430125ns  stddev=312752.9ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 3210.34,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 44343,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 20000.34,
            "unit": "ns/DU",
            "extra": "median=2500042ns  p95=5418167ns  stddev=837360.8ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 659.34,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 49999,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 20198.34,
            "unit": "ns/DU",
            "extra": "median=2524792ns  p95=2544125ns  stddev=22024.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 857.34,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 49509,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 20244.34,
            "unit": "ns/DU",
            "extra": "median=2530542ns  p95=7467916ns  stddev=1476328.8ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 903.34,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 49397,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 20306,
            "unit": "ns/DU",
            "extra": "median=2538250ns  p95=6292583ns  stddev=1075027.1ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 965,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 49247,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 20529,
            "unit": "ns/DU",
            "extra": "median=2566125ns  p95=3220625ns  stddev=199280.2ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 1188,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 48712,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 21545.34,
            "unit": "ns/DU",
            "extra": "median=2693167ns  p95=5381750ns  stddev=1010956.4ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 2204.34,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 46414,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 19994,
            "unit": "ns/DU",
            "extra": "median=2499250ns  p95=2571875ns  stddev=35695.0ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 653,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 50015,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 19831.34,
            "unit": "ns/DU",
            "extra": "median=2478917ns  p95=3468125ns  stddev=294388.1ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 490.34,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 50425,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 19626.67,
            "unit": "ns/DU",
            "extra": "median=2453334ns  p95=2577875ns  stddev=38316.3ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 285.67,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 50951,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 19782.66,
            "unit": "ns/DU",
            "extra": "median=2472833ns  p95=2676375ns  stddev=59766.2ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 441.66,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 50549,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 20487,
            "unit": "ns/DU",
            "extra": "median=2560875ns  p95=6458208ns  stddev=1221837.1ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 1146,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 48811,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 20130,
            "unit": "ns/DU",
            "extra": "median=2516250ns  p95=5052583ns  stddev=955038.4ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 789,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 49677,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 19736.34,
            "unit": "ns/DU",
            "extra": "median=2467042ns  p95=3506250ns  stddev=297898.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 395.34,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 50668,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 20027.67,
            "unit": "ns/DU",
            "extra": "median=2503459ns  p95=2816292ns  stddev=93562.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 686.67,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 49931,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 20007.34,
            "unit": "ns/DU",
            "extra": "median=2500917ns  p95=2761500ns  stddev=82763.4ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 666.34,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 49982,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 20416.34,
            "unit": "ns/DU",
            "extra": "median=2552042ns  p95=17871125ns  stddev=4380677.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 1075.34,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 48980,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 19396.67,
            "unit": "ns/DU",
            "extra": "median=2424584ns  p95=2438834ns  stddev=7325.2ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 55.67,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 51555,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 19477.67,
            "unit": "ns/DU",
            "extra": "median=2434709ns  p95=2538459ns  stddev=37162.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 136.67,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 51341,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 19899.66,
            "unit": "ns/DU",
            "extra": "median=2487458ns  p95=5194667ns  stddev=769973.7ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 558.66,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 50252,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 25436.33,
            "unit": "ns/DU",
            "extra": "median=3179541ns  p95=9257417ns  stddev=2017178.7ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 6095.33,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 39314,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 19959.34,
            "unit": "ns/DU",
            "extra": "median=2494917ns  p95=4152042ns  stddev=616096.7ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 618.34,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 50102,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 19448.66,
            "unit": "ns/DU",
            "extra": "median=2431083ns  p95=2472458ns  stddev=16493.8ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 107.66,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 51417,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 19438.34,
            "unit": "ns/DU",
            "extra": "median=2429792ns  p95=2636875ns  stddev=65908.5ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": 97.34,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 51445,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 19741.67,
            "unit": "ns/DU",
            "extra": "median=2467709ns  p95=5223250ns  stddev=853747.0ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 400.67,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 50654,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 19443.34,
            "unit": "ns/DU",
            "extra": "median=2430417ns  p95=2522042ns  stddev=27111.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 102.34,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 51432,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 19491,
            "unit": "ns/DU",
            "extra": "median=2436375ns  p95=6431625ns  stddev=1153828.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 150,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 51306,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 20186.67,
            "unit": "ns/DU",
            "extra": "median=2523334ns  p95=3706209ns  stddev=386257.5ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 845.67,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 49538,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 20015.34,
            "unit": "ns/DU",
            "extra": "median=2501917ns  p95=5415916ns  stddev=840461.0ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 674.34,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 49962,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 20898,
            "unit": "ns/DU",
            "extra": "median=2612250ns  p95=6651792ns  stddev=1540062.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 1557,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 47851,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 20079.67,
            "unit": "ns/DU",
            "extra": "median=2509959ns  p95=3458583ns  stddev=293203.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 738.67,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 49802,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 20289.34,
            "unit": "ns/DU",
            "extra": "median=2536167ns  p95=4072916ns  stddev=448114.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 948.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 49287,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 19376.33,
            "unit": "ns/DU",
            "extra": "median=2422041ns  p95=17247000ns  stddev=4238924.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 35.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 51609,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 19568,
            "unit": "ns/DU",
            "extra": "median=2446000ns  p95=2506500ns  stddev=26924.7ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 227,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 51104,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 20784,
            "unit": "ns/DU",
            "extra": "median=2598000ns  p95=3669166ns  stddev=404823.0ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 1443,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 48114,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 20115.66,
            "unit": "ns/DU",
            "extra": "median=2514458ns  p95=3617250ns  stddev=319251.8ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 774.66,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 49713,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 20263.66,
            "unit": "ns/DU",
            "extra": "median=2532958ns  p95=4639917ns  stddev=605856.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 922.66,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 49349,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 20025.33,
            "unit": "ns/DU",
            "extra": "median=2503166ns  p95=3774417ns  stddev=364655.9ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 684.33,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 49937,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 20029.66,
            "unit": "ns/DU",
            "extra": "median=2503708ns  p95=2520916ns  stddev=18877.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 688.66,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 49926,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 20234.33,
            "unit": "ns/DU",
            "extra": "median=2529291ns  p95=6230083ns  stddev=1262277.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 893.33,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 49421,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 19630,
            "unit": "ns/DU",
            "extra": "median=2453750ns  p95=2529458ns  stddev=39658.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 289,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 50942,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 19559.66,
            "unit": "ns/DU",
            "extra": "median=2444958ns  p95=3153792ns  stddev=210518.1ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 218.66,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 51126,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 19473,
            "unit": "ns/DU",
            "extra": "median=2434125ns  p95=6163625ns  stddev=1060124.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 132,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 51353,
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
        "date": 1775272374392,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 18030.66,
            "unit": "ns/DU",
            "extra": "median=2253833ns  p95=2344750ns  stddev=27413.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 55461,
            "unit": "DU/s"
          },
          {
            "name": "MOVE (total)",
            "value": 18077,
            "unit": "ns/DU",
            "extra": "median=2259625ns  p95=2305542ns  stddev=15139.7ns  iterations=11  DUs=125"
          },
          {
            "name": "MOVE (handler)",
            "value": 46.34,
            "unit": "ns"
          },
          {
            "name": "MOVE (DU/s)",
            "value": 55319,
            "unit": "DU/s"
          },
          {
            "name": "LOAD (total)",
            "value": 18058.34,
            "unit": "ns/DU",
            "extra": "median=2257292ns  p95=2264250ns  stddev=4166.8ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD (handler)",
            "value": 27.67,
            "unit": "ns"
          },
          {
            "name": "LOAD (DU/s)",
            "value": 55376,
            "unit": "DU/s"
          },
          {
            "name": "STORE (total)",
            "value": 18067,
            "unit": "ns/DU",
            "extra": "median=2258375ns  p95=2351417ns  stddev=27397.9ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE (handler)",
            "value": 36.34,
            "unit": "ns"
          },
          {
            "name": "STORE (DU/s)",
            "value": 55350,
            "unit": "DU/s"
          },
          {
            "name": "PUSH (total)",
            "value": 18061.66,
            "unit": "ns/DU",
            "extra": "median=2257708ns  p95=2267042ns  stddev=5129.3ns  iterations=11  DUs=125"
          },
          {
            "name": "PUSH (handler)",
            "value": 31,
            "unit": "ns"
          },
          {
            "name": "PUSH (DU/s)",
            "value": 55366,
            "unit": "DU/s"
          },
          {
            "name": "POP (total)",
            "value": 18275.34,
            "unit": "ns/DU",
            "extra": "median=2284417ns  p95=2356334ns  stddev=30609.6ns  iterations=11  DUs=125"
          },
          {
            "name": "POP (handler)",
            "value": 244.67,
            "unit": "ns"
          },
          {
            "name": "POP (DU/s)",
            "value": 54719,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 18919.66,
            "unit": "ns/DU",
            "extra": "median=2364958ns  p95=2413167ns  stddev=62044.6ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 889,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (DU/s)",
            "value": 52855,
            "unit": "DU/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 18035.33,
            "unit": "ns/DU",
            "extra": "median=2254416ns  p95=2304166ns  stddev=14683.7ns  iterations=11  DUs=125"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 4.66,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (DU/s)",
            "value": 55447,
            "unit": "DU/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 18080.66,
            "unit": "ns/DU",
            "extra": "median=2260083ns  p95=2412292ns  stddev=46826.5ns  iterations=11  DUs=125"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 50,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (DU/s)",
            "value": 55308,
            "unit": "DU/s"
          },
          {
            "name": "ADD (total)",
            "value": 18056.34,
            "unit": "ns/DU",
            "extra": "median=2257042ns  p95=2273583ns  stddev=6094.1ns  iterations=11  DUs=125"
          },
          {
            "name": "ADD (handler)",
            "value": 25.67,
            "unit": "ns"
          },
          {
            "name": "ADD (DU/s)",
            "value": 55382,
            "unit": "DU/s"
          },
          {
            "name": "SUB (total)",
            "value": 18123.66,
            "unit": "ns/DU",
            "extra": "median=2265458ns  p95=2280375ns  stddev=7471.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SUB (handler)",
            "value": 93,
            "unit": "ns"
          },
          {
            "name": "SUB (DU/s)",
            "value": 55176,
            "unit": "DU/s"
          },
          {
            "name": "MUL (total)",
            "value": 18908.67,
            "unit": "ns/DU",
            "extra": "median=2363584ns  p95=2403458ns  stddev=36012.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUL (handler)",
            "value": 878.01,
            "unit": "ns"
          },
          {
            "name": "MUL (DU/s)",
            "value": 52886,
            "unit": "DU/s"
          },
          {
            "name": "IMUL (total)",
            "value": 19473,
            "unit": "ns/DU",
            "extra": "median=2434125ns  p95=2484583ns  stddev=37367.9ns  iterations=11  DUs=125"
          },
          {
            "name": "IMUL (handler)",
            "value": 1442.34,
            "unit": "ns"
          },
          {
            "name": "IMUL (DU/s)",
            "value": 51353,
            "unit": "DU/s"
          },
          {
            "name": "DIV (total)",
            "value": 18120.34,
            "unit": "ns/DU",
            "extra": "median=2265042ns  p95=2561833ns  stddev=99136.8ns  iterations=11  DUs=125"
          },
          {
            "name": "DIV (handler)",
            "value": 89.67,
            "unit": "ns"
          },
          {
            "name": "DIV (DU/s)",
            "value": 55187,
            "unit": "DU/s"
          },
          {
            "name": "IDIV (total)",
            "value": 18917,
            "unit": "ns/DU",
            "extra": "median=2364625ns  p95=2569208ns  stddev=102208.6ns  iterations=11  DUs=125"
          },
          {
            "name": "IDIV (handler)",
            "value": 886.34,
            "unit": "ns"
          },
          {
            "name": "IDIV (DU/s)",
            "value": 52863,
            "unit": "DU/s"
          },
          {
            "name": "NEG (total)",
            "value": 18059,
            "unit": "ns/DU",
            "extra": "median=2257375ns  p95=2302625ns  stddev=14036.2ns  iterations=11  DUs=125"
          },
          {
            "name": "NEG (handler)",
            "value": 28.34,
            "unit": "ns"
          },
          {
            "name": "NEG (DU/s)",
            "value": 55374,
            "unit": "DU/s"
          },
          {
            "name": "MOD (total)",
            "value": 18056.66,
            "unit": "ns/DU",
            "extra": "median=2257083ns  p95=2270167ns  stddev=5642.8ns  iterations=11  DUs=125"
          },
          {
            "name": "MOD (handler)",
            "value": 26,
            "unit": "ns"
          },
          {
            "name": "MOD (DU/s)",
            "value": 55381,
            "unit": "DU/s"
          },
          {
            "name": "AND (total)",
            "value": 18053.34,
            "unit": "ns/DU",
            "extra": "median=2256667ns  p95=2335416ns  stddev=22827.4ns  iterations=11  DUs=125"
          },
          {
            "name": "AND (handler)",
            "value": 22.67,
            "unit": "ns"
          },
          {
            "name": "AND (DU/s)",
            "value": 55391,
            "unit": "DU/s"
          },
          {
            "name": "OR (total)",
            "value": 18267.66,
            "unit": "ns/DU",
            "extra": "median=2283458ns  p95=2471083ns  stddev=68407.5ns  iterations=11  DUs=125"
          },
          {
            "name": "OR (handler)",
            "value": 237,
            "unit": "ns"
          },
          {
            "name": "OR (DU/s)",
            "value": 54742,
            "unit": "DU/s"
          },
          {
            "name": "XOR (total)",
            "value": 18132.66,
            "unit": "ns/DU",
            "extra": "median=2266583ns  p95=3168667ns  stddev=271139.9ns  iterations=11  DUs=125"
          },
          {
            "name": "XOR (handler)",
            "value": 102,
            "unit": "ns"
          },
          {
            "name": "XOR (DU/s)",
            "value": 55149,
            "unit": "DU/s"
          },
          {
            "name": "NOT (total)",
            "value": 18078.33,
            "unit": "ns/DU",
            "extra": "median=2259791ns  p95=2308958ns  stddev=16708.4ns  iterations=11  DUs=125"
          },
          {
            "name": "NOT (handler)",
            "value": 47.66,
            "unit": "ns"
          },
          {
            "name": "NOT (DU/s)",
            "value": 55315,
            "unit": "DU/s"
          },
          {
            "name": "SHL (total)",
            "value": 18078.67,
            "unit": "ns/DU",
            "extra": "median=2259834ns  p95=2296500ns  stddev=13947.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SHL (handler)",
            "value": 48.01,
            "unit": "ns"
          },
          {
            "name": "SHL (DU/s)",
            "value": 55314,
            "unit": "DU/s"
          },
          {
            "name": "SHR (total)",
            "value": 18074,
            "unit": "ns/DU",
            "extra": "median=2259250ns  p95=2345250ns  stddev=31424.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SHR (handler)",
            "value": 43.34,
            "unit": "ns"
          },
          {
            "name": "SHR (DU/s)",
            "value": 55328,
            "unit": "DU/s"
          },
          {
            "name": "SAR (total)",
            "value": 18081.34,
            "unit": "ns/DU",
            "extra": "median=2260167ns  p95=2335291ns  stddev=22651.1ns  iterations=11  DUs=125"
          },
          {
            "name": "SAR (handler)",
            "value": 50.67,
            "unit": "ns"
          },
          {
            "name": "SAR (DU/s)",
            "value": 55306,
            "unit": "DU/s"
          },
          {
            "name": "ROL (total)",
            "value": 18756,
            "unit": "ns/DU",
            "extra": "median=2344500ns  p95=2379042ns  stddev=13209.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ROL (handler)",
            "value": 725.34,
            "unit": "ns"
          },
          {
            "name": "ROL (DU/s)",
            "value": 53316,
            "unit": "DU/s"
          },
          {
            "name": "ROR (total)",
            "value": 18686,
            "unit": "ns/DU",
            "extra": "median=2335750ns  p95=2349791ns  stddev=32245.7ns  iterations=11  DUs=125"
          },
          {
            "name": "ROR (handler)",
            "value": 655.34,
            "unit": "ns"
          },
          {
            "name": "ROR (DU/s)",
            "value": 53516,
            "unit": "DU/s"
          },
          {
            "name": "CMP (total)",
            "value": 18072.67,
            "unit": "ns/DU",
            "extra": "median=2259084ns  p95=2289291ns  stddev=10684.4ns  iterations=11  DUs=125"
          },
          {
            "name": "CMP (handler)",
            "value": 42.01,
            "unit": "ns"
          },
          {
            "name": "CMP (DU/s)",
            "value": 55332,
            "unit": "DU/s"
          },
          {
            "name": "TEST (total)",
            "value": 18082.66,
            "unit": "ns/DU",
            "extra": "median=2260333ns  p95=2439834ns  stddev=55668.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TEST (handler)",
            "value": 52,
            "unit": "ns"
          },
          {
            "name": "TEST (DU/s)",
            "value": 55302,
            "unit": "DU/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 18010.34,
            "unit": "ns/DU",
            "extra": "median=2251292ns  p95=2273667ns  stddev=8710.2ns  iterations=11  DUs=125"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -20.33,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (DU/s)",
            "value": 55524,
            "unit": "DU/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 18064,
            "unit": "ns/DU",
            "extra": "median=2258000ns  p95=2282000ns  stddev=8971.9ns  iterations=11  DUs=125"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 33.34,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (DU/s)",
            "value": 55359,
            "unit": "DU/s"
          },
          {
            "name": "JMP (total)",
            "value": 18007.66,
            "unit": "ns/DU",
            "extra": "median=2250958ns  p95=2287083ns  stddev=11776.2ns  iterations=11  DUs=125"
          },
          {
            "name": "JMP (handler)",
            "value": -23,
            "unit": "ns"
          },
          {
            "name": "JMP (DU/s)",
            "value": 55532,
            "unit": "DU/s"
          },
          {
            "name": "JCC (total)",
            "value": 18153,
            "unit": "ns/DU",
            "extra": "median=2269125ns  p95=2413250ns  stddev=45292.4ns  iterations=11  DUs=125"
          },
          {
            "name": "JCC (handler)",
            "value": 122.34,
            "unit": "ns"
          },
          {
            "name": "JCC (DU/s)",
            "value": 55087,
            "unit": "DU/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 18091.66,
            "unit": "ns/DU",
            "extra": "median=2261458ns  p95=2266750ns  stddev=2994.3ns  iterations=11  DUs=125"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 61,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (DU/s)",
            "value": 55274,
            "unit": "DU/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 19396.66,
            "unit": "ns/DU",
            "extra": "median=2424583ns  p95=5316250ns  stddev=829802.6ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 1366,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (DU/s)",
            "value": 51555,
            "unit": "DU/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 18041.66,
            "unit": "ns/DU",
            "extra": "median=2255208ns  p95=2281750ns  stddev=9225.3ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 11,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (DU/s)",
            "value": 55427,
            "unit": "DU/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 18047.33,
            "unit": "ns/DU",
            "extra": "median=2255916ns  p95=2261084ns  stddev=4007.9ns  iterations=11  DUs=125"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 16.66,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (DU/s)",
            "value": 55410,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 18059,
            "unit": "ns/DU",
            "extra": "median=2257375ns  p95=2265000ns  stddev=4207.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 28.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (DU/s)",
            "value": 55374,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 18053,
            "unit": "ns/DU",
            "extra": "median=2256625ns  p95=2303041ns  stddev=14364.9ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 22.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (DU/s)",
            "value": 55392,
            "unit": "DU/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 18083,
            "unit": "ns/DU",
            "extra": "median=2260375ns  p95=2271917ns  stddev=7021.8ns  iterations=11  DUs=125"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 52.34,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (DU/s)",
            "value": 55301,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 18070.34,
            "unit": "ns/DU",
            "extra": "median=2258792ns  p95=2323542ns  stddev=21903.8ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 39.67,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (DU/s)",
            "value": 55339,
            "unit": "DU/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 18084.67,
            "unit": "ns/DU",
            "extra": "median=2260584ns  p95=2547958ns  stddev=111761.9ns  iterations=11  DUs=125"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 54.01,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (DU/s)",
            "value": 55295,
            "unit": "DU/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 19079,
            "unit": "ns/DU",
            "extra": "median=2384875ns  p95=2454125ns  stddev=39655.5ns  iterations=11  DUs=125"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 1048.34,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (DU/s)",
            "value": 52414,
            "unit": "DU/s"
          },
          {
            "name": "XCHG (total)",
            "value": 18032.33,
            "unit": "ns/DU",
            "extra": "median=2254041ns  p95=2260250ns  stddev=3630.0ns  iterations=11  DUs=125"
          },
          {
            "name": "XCHG (handler)",
            "value": 1.66,
            "unit": "ns"
          },
          {
            "name": "XCHG (DU/s)",
            "value": 55456,
            "unit": "DU/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 18035.66,
            "unit": "ns/DU",
            "extra": "median=2254458ns  p95=2303750ns  stddev=14694.9ns  iterations=11  DUs=125"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 5,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (DU/s)",
            "value": 55446,
            "unit": "DU/s"
          },
          {
            "name": "FENCE (total)",
            "value": 18048.34,
            "unit": "ns/DU",
            "extra": "median=2256042ns  p95=2409000ns  stddev=44648.9ns  iterations=11  DUs=125"
          },
          {
            "name": "FENCE (handler)",
            "value": 17.67,
            "unit": "ns"
          },
          {
            "name": "FENCE (DU/s)",
            "value": 55407,
            "unit": "DU/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 18045.66,
            "unit": "ns/DU",
            "extra": "median=2255708ns  p95=2271042ns  stddev=6235.0ns  iterations=11  DUs=125"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 15,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (DU/s)",
            "value": 55415,
            "unit": "DU/s"
          },
          {
            "name": "NOP (total)",
            "value": 19216.33,
            "unit": "ns/DU",
            "extra": "median=2402041ns  p95=2429000ns  stddev=44973.9ns  iterations=11  DUs=125"
          },
          {
            "name": "NOP (handler)",
            "value": 1185.66,
            "unit": "ns"
          },
          {
            "name": "NOP (DU/s)",
            "value": 52039,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 19293.34,
            "unit": "ns/DU",
            "extra": "median=2411667ns  p95=2579750ns  stddev=52105.6ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 1262.67,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (DU/s)",
            "value": 51831,
            "unit": "DU/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 19305.34,
            "unit": "ns/DU",
            "extra": "median=2413167ns  p95=2467250ns  stddev=24557.3ns  iterations=11  DUs=125"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 1274.67,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (DU/s)",
            "value": 51799,
            "unit": "DU/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 19230,
            "unit": "ns/DU",
            "extra": "median=2403750ns  p95=2613167ns  stddev=61835.6ns  iterations=11  DUs=125"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 1199.34,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (DU/s)",
            "value": 52002,
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
        "date": 1775379876396,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 20017.66,
            "unit": "ns/DU",
            "extra": "median=2502208ns  p95=2634959ns  stddev=75864.9ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 49956,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOVE (total)",
            "value": 19932.66,
            "unit": "ns/DU",
            "extra": "median=2491583ns  p95=2647166ns  stddev=71002.1ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOVE (handler)",
            "value": -85,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOVE (DU/s)",
            "value": 50169,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD (total)",
            "value": 19996.34,
            "unit": "ns/DU",
            "extra": "median=2499542ns  p95=2716500ns  stddev=91992.9ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD (handler)",
            "value": -21.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD (DU/s)",
            "value": 50009,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE (total)",
            "value": 19962.34,
            "unit": "ns/DU",
            "extra": "median=2495292ns  p95=2607792ns  stddev=71510.1ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE (handler)",
            "value": -55.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE (DU/s)",
            "value": 50094,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] PUSH (total)",
            "value": 20136,
            "unit": "ns/DU",
            "extra": "median=2517000ns  p95=2652000ns  stddev=76277.2ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] PUSH (handler)",
            "value": 118.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] PUSH (DU/s)",
            "value": 49662,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] POP (total)",
            "value": 19949.33,
            "unit": "ns/DU",
            "extra": "median=2493666ns  p95=2624708ns  stddev=74234.4ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] POP (handler)",
            "value": -68.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] POP (DU/s)",
            "value": 50127,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (total)",
            "value": 19998.34,
            "unit": "ns/DU",
            "extra": "median=2499792ns  p95=2622167ns  stddev=68585.4ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (handler)",
            "value": -19.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (DU/s)",
            "value": 50004,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (total)",
            "value": 19986.66,
            "unit": "ns/DU",
            "extra": "median=2498333ns  p95=2692916ns  stddev=81565.9ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (handler)",
            "value": -31,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (DU/s)",
            "value": 50033,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (total)",
            "value": 19993.67,
            "unit": "ns/DU",
            "extra": "median=2499209ns  p95=2623958ns  stddev=72707.5ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (handler)",
            "value": -23.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (DU/s)",
            "value": 50016,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ADD (total)",
            "value": 20045.34,
            "unit": "ns/DU",
            "extra": "median=2505667ns  p95=2653417ns  stddev=79835.5ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] ADD (handler)",
            "value": 27.67,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ADD (DU/s)",
            "value": 49887,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SUB (total)",
            "value": 20079.67,
            "unit": "ns/DU",
            "extra": "median=2509959ns  p95=2668083ns  stddev=76800.5ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] SUB (handler)",
            "value": 62.01,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SUB (DU/s)",
            "value": 49802,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUL (total)",
            "value": 19914.66,
            "unit": "ns/DU",
            "extra": "median=2489333ns  p95=2615166ns  stddev=67924.1ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUL (handler)",
            "value": -103,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUL (DU/s)",
            "value": 50214,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IMUL (total)",
            "value": 20104.66,
            "unit": "ns/DU",
            "extra": "median=2513083ns  p95=2603000ns  stddev=66101.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] IMUL (handler)",
            "value": 87,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IMUL (DU/s)",
            "value": 49740,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] DIV (total)",
            "value": 19919,
            "unit": "ns/DU",
            "extra": "median=2489875ns  p95=2590666ns  stddev=65191.9ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] DIV (handler)",
            "value": -98.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] DIV (DU/s)",
            "value": 50203,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IDIV (total)",
            "value": 19884,
            "unit": "ns/DU",
            "extra": "median=2485500ns  p95=2655375ns  stddev=76905.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] IDIV (handler)",
            "value": -133.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IDIV (DU/s)",
            "value": 50292,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NEG (total)",
            "value": 20038,
            "unit": "ns/DU",
            "extra": "median=2504750ns  p95=2652000ns  stddev=77460.6ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] NEG (handler)",
            "value": 20.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NEG (DU/s)",
            "value": 49905,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOD (total)",
            "value": 20039,
            "unit": "ns/DU",
            "extra": "median=2504875ns  p95=2674750ns  stddev=72979.7ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOD (handler)",
            "value": 21.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOD (DU/s)",
            "value": 49903,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] AND (total)",
            "value": 19992.33,
            "unit": "ns/DU",
            "extra": "median=2499041ns  p95=2670958ns  stddev=80460.4ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] AND (handler)",
            "value": -25.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] AND (DU/s)",
            "value": 50019,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] OR (total)",
            "value": 19894,
            "unit": "ns/DU",
            "extra": "median=2486750ns  p95=2625083ns  stddev=62893.5ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] OR (handler)",
            "value": -123.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] OR (DU/s)",
            "value": 50266,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XOR (total)",
            "value": 19981.67,
            "unit": "ns/DU",
            "extra": "median=2497709ns  p95=2605375ns  stddev=58028.2ns  iterations=97  DUs=125"
          },
          {
            "name": "[DebugPolicy] XOR (handler)",
            "value": -35.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XOR (DU/s)",
            "value": 50046,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOT (total)",
            "value": 20085.66,
            "unit": "ns/DU",
            "extra": "median=2510708ns  p95=2635667ns  stddev=67775.7ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOT (handler)",
            "value": 68,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOT (DU/s)",
            "value": 49787,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHL (total)",
            "value": 20006.66,
            "unit": "ns/DU",
            "extra": "median=2500833ns  p95=2640209ns  stddev=74573.7ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHL (handler)",
            "value": -11,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHL (DU/s)",
            "value": 49983,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHR (total)",
            "value": 19867.66,
            "unit": "ns/DU",
            "extra": "median=2483458ns  p95=2590583ns  stddev=59603.0ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHR (handler)",
            "value": -150,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHR (DU/s)",
            "value": 50333,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SAR (total)",
            "value": 20056,
            "unit": "ns/DU",
            "extra": "median=2507000ns  p95=2690125ns  stddev=76058.5ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] SAR (handler)",
            "value": 38.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SAR (DU/s)",
            "value": 49860,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROL (total)",
            "value": 19989.34,
            "unit": "ns/DU",
            "extra": "median=2498667ns  p95=2640708ns  stddev=64663.7ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROL (handler)",
            "value": -28.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROL (DU/s)",
            "value": 50027,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROR (total)",
            "value": 20117,
            "unit": "ns/DU",
            "extra": "median=2514625ns  p95=2610875ns  stddev=63947.3ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROR (handler)",
            "value": 99.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROR (DU/s)",
            "value": 49709,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMP (total)",
            "value": 19969.33,
            "unit": "ns/DU",
            "extra": "median=2496166ns  p95=2731000ns  stddev=78296.2ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMP (handler)",
            "value": -48.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMP (DU/s)",
            "value": 50077,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TEST (total)",
            "value": 19939.34,
            "unit": "ns/DU",
            "extra": "median=2492417ns  p95=2620167ns  stddev=60613.5ns  iterations=106  DUs=125"
          },
          {
            "name": "[DebugPolicy] TEST (handler)",
            "value": -78.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TEST (DU/s)",
            "value": 50152,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (total)",
            "value": 19925.67,
            "unit": "ns/DU",
            "extra": "median=2490709ns  p95=2627958ns  stddev=73502.6ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (handler)",
            "value": -91.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (DU/s)",
            "value": 50187,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (total)",
            "value": 19952.33,
            "unit": "ns/DU",
            "extra": "median=2494041ns  p95=2645041ns  stddev=76559.1ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (handler)",
            "value": -65.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (DU/s)",
            "value": 50119,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JMP (total)",
            "value": 19985,
            "unit": "ns/DU",
            "extra": "median=2498125ns  p95=2615209ns  stddev=69255.0ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] JMP (handler)",
            "value": -32.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JMP (DU/s)",
            "value": 50038,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JCC (total)",
            "value": 20047,
            "unit": "ns/DU",
            "extra": "median=2505875ns  p95=2646833ns  stddev=74767.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] JCC (handler)",
            "value": 29.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JCC (DU/s)",
            "value": 49883,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (total)",
            "value": 20101,
            "unit": "ns/DU",
            "extra": "median=2512625ns  p95=2653125ns  stddev=72105.5ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (handler)",
            "value": 83.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (DU/s)",
            "value": 49749,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT8 (total)",
            "value": 20065.66,
            "unit": "ns/DU",
            "extra": "median=2508208ns  p95=2613959ns  stddev=64724.6ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT8 (handler)",
            "value": 48,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT8 (DU/s)",
            "value": 49836,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT16 (total)",
            "value": 19987.34,
            "unit": "ns/DU",
            "extra": "median=2498417ns  p95=2629625ns  stddev=66204.6ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT16 (handler)",
            "value": -30.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT16 (DU/s)",
            "value": 50032,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT32 (total)",
            "value": 20055,
            "unit": "ns/DU",
            "extra": "median=2506875ns  p95=2657541ns  stddev=72577.0ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT32 (handler)",
            "value": 37.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT32 (DU/s)",
            "value": 49863,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (total)",
            "value": 19879.34,
            "unit": "ns/DU",
            "extra": "median=2484917ns  p95=2641959ns  stddev=67898.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (handler)",
            "value": -138.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (DU/s)",
            "value": 50303,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (total)",
            "value": 19921.34,
            "unit": "ns/DU",
            "extra": "median=2490167ns  p95=2601250ns  stddev=66783.1ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (handler)",
            "value": -96.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (DU/s)",
            "value": 50197,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (total)",
            "value": 20060.33,
            "unit": "ns/DU",
            "extra": "median=2507541ns  p95=2667375ns  stddev=72802.2ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (handler)",
            "value": 42.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (DU/s)",
            "value": 49850,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (total)",
            "value": 19867.34,
            "unit": "ns/DU",
            "extra": "median=2483417ns  p95=2593291ns  stddev=55448.0ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (handler)",
            "value": -150.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (DU/s)",
            "value": 50334,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (total)",
            "value": 20034.33,
            "unit": "ns/DU",
            "extra": "median=2504291ns  p95=2654708ns  stddev=78236.6ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (handler)",
            "value": 16.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (DU/s)",
            "value": 49914,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (total)",
            "value": 19964.67,
            "unit": "ns/DU",
            "extra": "median=2495584ns  p95=2606458ns  stddev=67404.3ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (handler)",
            "value": -52.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (DU/s)",
            "value": 50088,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XCHG (total)",
            "value": 19927,
            "unit": "ns/DU",
            "extra": "median=2490875ns  p95=2619542ns  stddev=63162.5ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] XCHG (handler)",
            "value": -90.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XCHG (DU/s)",
            "value": 50183,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (total)",
            "value": 20003,
            "unit": "ns/DU",
            "extra": "median=2500375ns  p95=2633416ns  stddev=67140.1ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (handler)",
            "value": -14.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (DU/s)",
            "value": 49993,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] FENCE (total)",
            "value": 19948,
            "unit": "ns/DU",
            "extra": "median=2493500ns  p95=2685875ns  stddev=76192.2ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] FENCE (handler)",
            "value": -69.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] FENCE (DU/s)",
            "value": 50130,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (total)",
            "value": 19916,
            "unit": "ns/DU",
            "extra": "median=2489500ns  p95=2679500ns  stddev=70736.2ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (handler)",
            "value": -101.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (DU/s)",
            "value": 50211,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 19937.66,
            "unit": "ns/DU",
            "extra": "median=2492208ns  p95=2605625ns  stddev=66216.3ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": -80,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 50156,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (total)",
            "value": 19924,
            "unit": "ns/DU",
            "extra": "median=2490500ns  p95=2620500ns  stddev=71665.3ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (handler)",
            "value": -93.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 50191,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (total)",
            "value": 19930.33,
            "unit": "ns/DU",
            "extra": "median=2491291ns  p95=2615834ns  stddev=69833.8ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (handler)",
            "value": -87.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (DU/s)",
            "value": 50175,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (total)",
            "value": 20119.34,
            "unit": "ns/DU",
            "extra": "median=2514917ns  p95=2699625ns  stddev=81319.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (handler)",
            "value": 101.67,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (DU/s)",
            "value": 49703,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 62796,
            "unit": "ns/DU",
            "extra": "median=7849500ns  p95=8156916ns  stddev=141730.8ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 15925,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOVE (total)",
            "value": 62778,
            "unit": "ns/DU",
            "extra": "median=7847250ns  p95=8073292ns  stddev=113096.1ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOVE (handler)",
            "value": -18,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOVE (DU/s)",
            "value": 15929,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD (total)",
            "value": 62763,
            "unit": "ns/DU",
            "extra": "median=7845375ns  p95=8037875ns  stddev=105353.5ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD (handler)",
            "value": -33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD (DU/s)",
            "value": 15933,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE (total)",
            "value": 62878,
            "unit": "ns/DU",
            "extra": "median=7859750ns  p95=8083125ns  stddev=109404.1ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE (handler)",
            "value": 82,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE (DU/s)",
            "value": 15904,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] PUSH (total)",
            "value": 62947.34,
            "unit": "ns/DU",
            "extra": "median=7868417ns  p95=8058583ns  stddev=111556.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] PUSH (handler)",
            "value": 151.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] PUSH (DU/s)",
            "value": 15886,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] POP (total)",
            "value": 62942.66,
            "unit": "ns/DU",
            "extra": "median=7867833ns  p95=8166875ns  stddev=140986.1ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] POP (handler)",
            "value": 146.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] POP (DU/s)",
            "value": 15887,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (total)",
            "value": 62893,
            "unit": "ns/DU",
            "extra": "median=7861625ns  p95=8274084ns  stddev=154497.7ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (handler)",
            "value": 97,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (DU/s)",
            "value": 15900,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (total)",
            "value": 63012,
            "unit": "ns/DU",
            "extra": "median=7876500ns  p95=8103917ns  stddev=121120.6ns  iterations=102  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (handler)",
            "value": 216,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (DU/s)",
            "value": 15870,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (total)",
            "value": 62889.66,
            "unit": "ns/DU",
            "extra": "median=7861208ns  p95=8089958ns  stddev=115707.5ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (handler)",
            "value": 93.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (DU/s)",
            "value": 15901,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ADD (total)",
            "value": 62836,
            "unit": "ns/DU",
            "extra": "median=7854500ns  p95=8191375ns  stddev=155522.7ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] ADD (handler)",
            "value": 40,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ADD (DU/s)",
            "value": 15914,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SUB (total)",
            "value": 62718,
            "unit": "ns/DU",
            "extra": "median=7839750ns  p95=8005875ns  stddev=99769.5ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] SUB (handler)",
            "value": -78,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SUB (DU/s)",
            "value": 15944,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUL (total)",
            "value": 63002,
            "unit": "ns/DU",
            "extra": "median=7875250ns  p95=8155667ns  stddev=127440.1ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUL (handler)",
            "value": 206,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUL (DU/s)",
            "value": 15873,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IMUL (total)",
            "value": 63006.67,
            "unit": "ns/DU",
            "extra": "median=7875834ns  p95=8290542ns  stddev=172340.4ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] IMUL (handler)",
            "value": 210.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IMUL (DU/s)",
            "value": 15871,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] DIV (total)",
            "value": 62803.66,
            "unit": "ns/DU",
            "extra": "median=7850458ns  p95=8105166ns  stddev=133797.4ns  iterations=103  DUs=125"
          },
          {
            "name": "[StandardPolicy] DIV (handler)",
            "value": 7.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] DIV (DU/s)",
            "value": 15923,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IDIV (total)",
            "value": 63015.66,
            "unit": "ns/DU",
            "extra": "median=7876958ns  p95=8093166ns  stddev=118905.9ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] IDIV (handler)",
            "value": 219.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IDIV (DU/s)",
            "value": 15869,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NEG (total)",
            "value": 62828.33,
            "unit": "ns/DU",
            "extra": "median=7853541ns  p95=8135792ns  stddev=141491.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] NEG (handler)",
            "value": 32.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NEG (DU/s)",
            "value": 15916,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOD (total)",
            "value": 62757.34,
            "unit": "ns/DU",
            "extra": "median=7844667ns  p95=8053459ns  stddev=121424.5ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOD (handler)",
            "value": -38.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOD (DU/s)",
            "value": 15934,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] AND (total)",
            "value": 62766.67,
            "unit": "ns/DU",
            "extra": "median=7845834ns  p95=8036750ns  stddev=111089.2ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] AND (handler)",
            "value": -29.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] AND (DU/s)",
            "value": 15932,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] OR (total)",
            "value": 62809.66,
            "unit": "ns/DU",
            "extra": "median=7851208ns  p95=8189542ns  stddev=137971.7ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] OR (handler)",
            "value": 13.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] OR (DU/s)",
            "value": 15921,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XOR (total)",
            "value": 62650,
            "unit": "ns/DU",
            "extra": "median=7831250ns  p95=8101209ns  stddev=133675.2ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] XOR (handler)",
            "value": -146,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XOR (DU/s)",
            "value": 15962,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOT (total)",
            "value": 62892,
            "unit": "ns/DU",
            "extra": "median=7861500ns  p95=8043333ns  stddev=107666.3ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOT (handler)",
            "value": 96,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOT (DU/s)",
            "value": 15900,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHL (total)",
            "value": 62835.34,
            "unit": "ns/DU",
            "extra": "median=7854417ns  p95=8048041ns  stddev=115456.0ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHL (handler)",
            "value": 39.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHL (DU/s)",
            "value": 15915,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHR (total)",
            "value": 62879.66,
            "unit": "ns/DU",
            "extra": "median=7859958ns  p95=8148958ns  stddev=128917.4ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHR (handler)",
            "value": 83.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHR (DU/s)",
            "value": 15903,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SAR (total)",
            "value": 62770.34,
            "unit": "ns/DU",
            "extra": "median=7846292ns  p95=8069958ns  stddev=112416.0ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] SAR (handler)",
            "value": -25.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SAR (DU/s)",
            "value": 15931,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROL (total)",
            "value": 62771.33,
            "unit": "ns/DU",
            "extra": "median=7846416ns  p95=8084375ns  stddev=127265.0ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROL (handler)",
            "value": -24.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROL (DU/s)",
            "value": 15931,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROR (total)",
            "value": 62848,
            "unit": "ns/DU",
            "extra": "median=7856000ns  p95=8283708ns  stddev=183702.6ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROR (handler)",
            "value": 52,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROR (DU/s)",
            "value": 15911,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMP (total)",
            "value": 62676.66,
            "unit": "ns/DU",
            "extra": "median=7834583ns  p95=7995583ns  stddev=99052.0ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMP (handler)",
            "value": -119.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMP (DU/s)",
            "value": 15955,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TEST (total)",
            "value": 62703.33,
            "unit": "ns/DU",
            "extra": "median=7837916ns  p95=8194333ns  stddev=138637.1ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] TEST (handler)",
            "value": -92.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TEST (DU/s)",
            "value": 15948,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (total)",
            "value": 62862,
            "unit": "ns/DU",
            "extra": "median=7857750ns  p95=8158417ns  stddev=118278.8ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (handler)",
            "value": 66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (DU/s)",
            "value": 15908,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (total)",
            "value": 62970,
            "unit": "ns/DU",
            "extra": "median=7871250ns  p95=8231583ns  stddev=146981.9ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (handler)",
            "value": 174,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (DU/s)",
            "value": 15881,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JMP (total)",
            "value": 63047.67,
            "unit": "ns/DU",
            "extra": "median=7880959ns  p95=8474541ns  stddev=236202.0ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] JMP (handler)",
            "value": 251.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JMP (DU/s)",
            "value": 15861,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JCC (total)",
            "value": 63078.34,
            "unit": "ns/DU",
            "extra": "median=7884792ns  p95=8235500ns  stddev=157207.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] JCC (handler)",
            "value": 282.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JCC (DU/s)",
            "value": 15853,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (total)",
            "value": 63116.66,
            "unit": "ns/DU",
            "extra": "median=7889583ns  p95=8141875ns  stddev=119835.3ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (handler)",
            "value": 320.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (DU/s)",
            "value": 15844,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT8 (total)",
            "value": 62857.33,
            "unit": "ns/DU",
            "extra": "median=7857166ns  p95=8124125ns  stddev=139478.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT8 (handler)",
            "value": 61.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT8 (DU/s)",
            "value": 15909,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT16 (total)",
            "value": 62902.67,
            "unit": "ns/DU",
            "extra": "median=7862834ns  p95=8062291ns  stddev=100288.3ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT16 (handler)",
            "value": 106.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT16 (DU/s)",
            "value": 15898,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT32 (total)",
            "value": 62765,
            "unit": "ns/DU",
            "extra": "median=7845625ns  p95=8136459ns  stddev=124186.6ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT32 (handler)",
            "value": -31,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT32 (DU/s)",
            "value": 15932,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (total)",
            "value": 62833,
            "unit": "ns/DU",
            "extra": "median=7854125ns  p95=8135500ns  stddev=129406.3ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (handler)",
            "value": 37,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (DU/s)",
            "value": 15915,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (total)",
            "value": 63076.34,
            "unit": "ns/DU",
            "extra": "median=7884542ns  p95=8177583ns  stddev=151700.7ns  iterations=99  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (handler)",
            "value": 280.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (DU/s)",
            "value": 15854,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (total)",
            "value": 62969.67,
            "unit": "ns/DU",
            "extra": "median=7871209ns  p95=8062916ns  stddev=112609.1ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (handler)",
            "value": 173.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (DU/s)",
            "value": 15881,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (total)",
            "value": 62893,
            "unit": "ns/DU",
            "extra": "median=7861625ns  p95=8011292ns  stddev=102347.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (handler)",
            "value": 97,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (DU/s)",
            "value": 15900,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (total)",
            "value": 62977.33,
            "unit": "ns/DU",
            "extra": "median=7872166ns  p95=8219917ns  stddev=152536.6ns  iterations=101  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (handler)",
            "value": 181.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (DU/s)",
            "value": 15879,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (total)",
            "value": 62901,
            "unit": "ns/DU",
            "extra": "median=7862625ns  p95=8042875ns  stddev=100790.0ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (handler)",
            "value": 105,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (DU/s)",
            "value": 15898,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XCHG (total)",
            "value": 62860.67,
            "unit": "ns/DU",
            "extra": "median=7857584ns  p95=8112417ns  stddev=115887.0ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] XCHG (handler)",
            "value": 64.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XCHG (DU/s)",
            "value": 15908,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (total)",
            "value": 62822,
            "unit": "ns/DU",
            "extra": "median=7852750ns  p95=8174209ns  stddev=139171.5ns  iterations=100  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (handler)",
            "value": 26,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (DU/s)",
            "value": 15918,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] FENCE (total)",
            "value": 62908.66,
            "unit": "ns/DU",
            "extra": "median=7863583ns  p95=8280958ns  stddev=151642.8ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] FENCE (handler)",
            "value": 112.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] FENCE (DU/s)",
            "value": 15896,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (total)",
            "value": 62873.66,
            "unit": "ns/DU",
            "extra": "median=7859208ns  p95=8196875ns  stddev=140061.6ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (handler)",
            "value": 77.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (DU/s)",
            "value": 15905,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 62981,
            "unit": "ns/DU",
            "extra": "median=7872625ns  p95=8167334ns  stddev=138872.9ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 185,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 15878,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (total)",
            "value": 62836.34,
            "unit": "ns/DU",
            "extra": "median=7854542ns  p95=8046459ns  stddev=119181.2ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (handler)",
            "value": 40.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 15914,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (total)",
            "value": 62983,
            "unit": "ns/DU",
            "extra": "median=7872875ns  p95=8072708ns  stddev=125163.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (handler)",
            "value": 187,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (DU/s)",
            "value": 15877,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (total)",
            "value": 62852.33,
            "unit": "ns/DU",
            "extra": "median=7856541ns  p95=8020334ns  stddev=104956.9ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (handler)",
            "value": 56.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (DU/s)",
            "value": 15910,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 110302,
            "unit": "ns/DU",
            "extra": "median=13787750ns  p95=14134500ns  stddev=176751.4ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 9066,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOVE (total)",
            "value": 110369,
            "unit": "ns/DU",
            "extra": "median=13796125ns  p95=14205333ns  stddev=219174.8ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOVE (handler)",
            "value": 67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOVE (DU/s)",
            "value": 9061,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD (total)",
            "value": 110341.67,
            "unit": "ns/DU",
            "extra": "median=13792709ns  p95=14356416ns  stddev=263936.2ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD (handler)",
            "value": 39.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD (DU/s)",
            "value": 9063,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE (total)",
            "value": 110413.66,
            "unit": "ns/DU",
            "extra": "median=13801708ns  p95=14245125ns  stddev=209438.1ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE (handler)",
            "value": 111.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE (DU/s)",
            "value": 9057,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] PUSH (total)",
            "value": 110501.34,
            "unit": "ns/DU",
            "extra": "median=13812667ns  p95=14271000ns  stddev=202856.7ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] PUSH (handler)",
            "value": 199.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] PUSH (DU/s)",
            "value": 9050,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] POP (total)",
            "value": 110668.34,
            "unit": "ns/DU",
            "extra": "median=13833542ns  p95=14314042ns  stddev=256433.8ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] POP (handler)",
            "value": 366.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] POP (DU/s)",
            "value": 9036,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (total)",
            "value": 110513.34,
            "unit": "ns/DU",
            "extra": "median=13814167ns  p95=14039416ns  stddev=168253.1ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (handler)",
            "value": 211.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (DU/s)",
            "value": 9049,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (total)",
            "value": 110589.33,
            "unit": "ns/DU",
            "extra": "median=13823666ns  p95=14255750ns  stddev=195873.2ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (handler)",
            "value": 287.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (DU/s)",
            "value": 9042,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (total)",
            "value": 110392.34,
            "unit": "ns/DU",
            "extra": "median=13799042ns  p95=14287417ns  stddev=225646.7ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (handler)",
            "value": 90.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (DU/s)",
            "value": 9059,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ADD (total)",
            "value": 110491.66,
            "unit": "ns/DU",
            "extra": "median=13811458ns  p95=14189875ns  stddev=217200.0ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ADD (handler)",
            "value": 189.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ADD (DU/s)",
            "value": 9050,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SUB (total)",
            "value": 110772.67,
            "unit": "ns/DU",
            "extra": "median=13846584ns  p95=14254042ns  stddev=226748.1ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SUB (handler)",
            "value": 470.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SUB (DU/s)",
            "value": 9027,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUL (total)",
            "value": 110610.66,
            "unit": "ns/DU",
            "extra": "median=13826333ns  p95=14212125ns  stddev=236411.8ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUL (handler)",
            "value": 308.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUL (DU/s)",
            "value": 9041,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IMUL (total)",
            "value": 110514.67,
            "unit": "ns/DU",
            "extra": "median=13814334ns  p95=14153917ns  stddev=190144.9ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IMUL (handler)",
            "value": 212.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IMUL (DU/s)",
            "value": 9049,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] DIV (total)",
            "value": 110347,
            "unit": "ns/DU",
            "extra": "median=13793375ns  p95=14657792ns  stddev=344840.4ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] DIV (handler)",
            "value": 45,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] DIV (DU/s)",
            "value": 9062,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IDIV (total)",
            "value": 110716.66,
            "unit": "ns/DU",
            "extra": "median=13839583ns  p95=14202750ns  stddev=199871.3ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IDIV (handler)",
            "value": 414.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IDIV (DU/s)",
            "value": 9032,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NEG (total)",
            "value": 110483.66,
            "unit": "ns/DU",
            "extra": "median=13810458ns  p95=14198500ns  stddev=193479.2ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NEG (handler)",
            "value": 181.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NEG (DU/s)",
            "value": 9051,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOD (total)",
            "value": 110377.67,
            "unit": "ns/DU",
            "extra": "median=13797209ns  p95=14128916ns  stddev=198615.1ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOD (handler)",
            "value": 75.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOD (DU/s)",
            "value": 9060,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] AND (total)",
            "value": 110357.66,
            "unit": "ns/DU",
            "extra": "median=13794708ns  p95=14269792ns  stddev=233970.0ns  iterations=94  DUs=125"
          },
          {
            "name": "[HighSecPolicy] AND (handler)",
            "value": 55.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] AND (DU/s)",
            "value": 9061,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] OR (total)",
            "value": 110675.66,
            "unit": "ns/DU",
            "extra": "median=13834458ns  p95=14750000ns  stddev=339029.8ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] OR (handler)",
            "value": 373.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] OR (DU/s)",
            "value": 9035,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XOR (total)",
            "value": 110523.34,
            "unit": "ns/DU",
            "extra": "median=13815417ns  p95=14390833ns  stddev=262642.4ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XOR (handler)",
            "value": 221.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XOR (DU/s)",
            "value": 9048,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOT (total)",
            "value": 110598.34,
            "unit": "ns/DU",
            "extra": "median=13824792ns  p95=14417792ns  stddev=291305.9ns  iterations=102  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOT (handler)",
            "value": 296.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOT (DU/s)",
            "value": 9042,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHL (total)",
            "value": 110404.34,
            "unit": "ns/DU",
            "extra": "median=13800542ns  p95=14296333ns  stddev=229148.1ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHL (handler)",
            "value": 102.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHL (DU/s)",
            "value": 9058,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHR (total)",
            "value": 110357,
            "unit": "ns/DU",
            "extra": "median=13794625ns  p95=14165166ns  stddev=199315.5ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHR (handler)",
            "value": 55,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHR (DU/s)",
            "value": 9062,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SAR (total)",
            "value": 110543.66,
            "unit": "ns/DU",
            "extra": "median=13817958ns  p95=14113083ns  stddev=206841.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SAR (handler)",
            "value": 241.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SAR (DU/s)",
            "value": 9046,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROL (total)",
            "value": 110623.34,
            "unit": "ns/DU",
            "extra": "median=13827917ns  p95=14139625ns  stddev=203354.2ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROL (handler)",
            "value": 321.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROL (DU/s)",
            "value": 9040,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROR (total)",
            "value": 110217.34,
            "unit": "ns/DU",
            "extra": "median=13777167ns  p95=14162750ns  stddev=196327.3ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROR (handler)",
            "value": -84.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROR (DU/s)",
            "value": 9073,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMP (total)",
            "value": 110330,
            "unit": "ns/DU",
            "extra": "median=13791250ns  p95=14357125ns  stddev=260752.4ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMP (handler)",
            "value": 28,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMP (DU/s)",
            "value": 9064,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TEST (total)",
            "value": 110680,
            "unit": "ns/DU",
            "extra": "median=13835000ns  p95=14453250ns  stddev=266850.9ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TEST (handler)",
            "value": 378,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TEST (DU/s)",
            "value": 9035,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (total)",
            "value": 110500.66,
            "unit": "ns/DU",
            "extra": "median=13812583ns  p95=14332625ns  stddev=238299.2ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (handler)",
            "value": 198.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (DU/s)",
            "value": 9050,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (total)",
            "value": 110334,
            "unit": "ns/DU",
            "extra": "median=13791750ns  p95=14302709ns  stddev=207523.8ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (handler)",
            "value": 32,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (DU/s)",
            "value": 9063,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JMP (total)",
            "value": 110380.33,
            "unit": "ns/DU",
            "extra": "median=13797541ns  p95=14312208ns  stddev=229000.9ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JMP (handler)",
            "value": 78.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JMP (DU/s)",
            "value": 9060,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JCC (total)",
            "value": 110361.67,
            "unit": "ns/DU",
            "extra": "median=13795209ns  p95=14092792ns  stddev=172821.7ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JCC (handler)",
            "value": 59.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JCC (DU/s)",
            "value": 9061,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (total)",
            "value": 110625.66,
            "unit": "ns/DU",
            "extra": "median=13828208ns  p95=14171917ns  stddev=204309.2ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (handler)",
            "value": 323.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (DU/s)",
            "value": 9039,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (total)",
            "value": 110217.67,
            "unit": "ns/DU",
            "extra": "median=13777209ns  p95=14155084ns  stddev=242188.9ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (handler)",
            "value": -84.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (DU/s)",
            "value": 9073,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (total)",
            "value": 110465.34,
            "unit": "ns/DU",
            "extra": "median=13808167ns  p95=14206792ns  stddev=216984.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (handler)",
            "value": 163.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (DU/s)",
            "value": 9053,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (total)",
            "value": 110131.34,
            "unit": "ns/DU",
            "extra": "median=13766417ns  p95=14325625ns  stddev=247342.8ns  iterations=94  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (handler)",
            "value": -170.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (DU/s)",
            "value": 9080,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (total)",
            "value": 110575,
            "unit": "ns/DU",
            "extra": "median=13821875ns  p95=14103708ns  stddev=205157.7ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (handler)",
            "value": 273,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (DU/s)",
            "value": 9044,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (total)",
            "value": 110485.34,
            "unit": "ns/DU",
            "extra": "median=13810667ns  p95=14403333ns  stddev=267645.1ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (handler)",
            "value": 183.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (DU/s)",
            "value": 9051,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (total)",
            "value": 110085,
            "unit": "ns/DU",
            "extra": "median=13760625ns  p95=14069625ns  stddev=179142.7ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (handler)",
            "value": -217,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (DU/s)",
            "value": 9084,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (total)",
            "value": 110271.34,
            "unit": "ns/DU",
            "extra": "median=13783917ns  p95=14148041ns  stddev=195814.2ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (handler)",
            "value": -30.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (DU/s)",
            "value": 9069,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (total)",
            "value": 110471.33,
            "unit": "ns/DU",
            "extra": "median=13808916ns  p95=14424292ns  stddev=256203.7ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (handler)",
            "value": 169.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (DU/s)",
            "value": 9052,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (total)",
            "value": 110617.34,
            "unit": "ns/DU",
            "extra": "median=13827167ns  p95=14392083ns  stddev=265848.8ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (handler)",
            "value": 315.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (DU/s)",
            "value": 9040,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XCHG (total)",
            "value": 110286,
            "unit": "ns/DU",
            "extra": "median=13785750ns  p95=14251209ns  stddev=218521.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XCHG (handler)",
            "value": -16,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XCHG (DU/s)",
            "value": 9067,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (total)",
            "value": 110291.33,
            "unit": "ns/DU",
            "extra": "median=13786416ns  p95=14250541ns  stddev=223478.6ns  iterations=98  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (handler)",
            "value": -10.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (DU/s)",
            "value": 9067,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] FENCE (total)",
            "value": 110489.66,
            "unit": "ns/DU",
            "extra": "median=13811208ns  p95=14407417ns  stddev=277716.8ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] FENCE (handler)",
            "value": 187.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] FENCE (DU/s)",
            "value": 9051,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (total)",
            "value": 110649.33,
            "unit": "ns/DU",
            "extra": "median=13831166ns  p95=14287291ns  stddev=200607.2ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (handler)",
            "value": 347.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (DU/s)",
            "value": 9038,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 110542.67,
            "unit": "ns/DU",
            "extra": "median=13817834ns  p95=14193542ns  stddev=183632.4ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 240.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 9046,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (total)",
            "value": 110264.66,
            "unit": "ns/DU",
            "extra": "median=13783083ns  p95=14088709ns  stddev=168559.9ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (handler)",
            "value": -37.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 9069,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (total)",
            "value": 110412.33,
            "unit": "ns/DU",
            "extra": "median=13801541ns  p95=14089750ns  stddev=205459.9ns  iterations=95  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (handler)",
            "value": 110.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (DU/s)",
            "value": 9057,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (total)",
            "value": 110451.66,
            "unit": "ns/DU",
            "extra": "median=13806458ns  p95=14414291ns  stddev=251577.8ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (handler)",
            "value": 149.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (DU/s)",
            "value": 9054,
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
          "id": "a0885e3df859cf861d9055b0d58af718671b3e6f",
          "message": "test(security): add isolated verify_bb_mac coverage for enc_state evolution\n\nFill three coverage gaps identified in verify_bb_mac testing:\n\n1. No existing test directly calls pipeline::verify_bb_mac() — all prior\n   tests exercised it indirectly through engine execution.\n\n2. REKEY at non-zero position within a BB with padding iterations was\n   untested. The enc_state mutation from REKEY must be replayed correctly\n   during MAC re-derivation, or subsequent keystream values diverge.\n\n3. The \"1 real instruction + max padding\" boundary (Doc 19 §4.2 Fix #2)\n   had no dedicated test.\n\nNew tests (9):\n- DirectCallSucceeds: isolated pipeline::verify_bb_mac() call\n- RekeyMidBbProducesCorrectMac: REKEY at position 1 of 4\n- MultipleRekeyInOneBb: two REKEY with different counters\n- SingleInsnBbWithMaxPadding: 1 real + 5 dummy iterations\n- RekeyInShortBbWithLargePadding: REKEY in 2-insn BB padded to 10\n- DirectCallWithRekeyMidBb: isolated call with REKEY at position 1\n- TamperAfterRekeyDetected: bit flip after REKEY → cascade failure\n- HighSecFusionWithRekeyMidBb: N=4 fusion + REKEY interaction\n- CrossBbRekeyIndependence: per-BB enc_state isolation with REKEY",
          "timestamp": "2026-04-05T18:31:36+08:00",
          "tree_id": "5c4aaff38c815911dacd0e29482ca1ff82a78848",
          "url": "https://github.com/scc-tw/VMPilot/commit/a0885e3df859cf861d9055b0d58af718671b3e6f"
        },
        "date": 1775385298644,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 19724.34,
            "unit": "ns/DU",
            "extra": "median=2465542ns  p95=2569834ns  stddev=53579.2ns  iterations=95  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 50699,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOVE (total)",
            "value": 19670,
            "unit": "ns/DU",
            "extra": "median=2458750ns  p95=2621000ns  stddev=64589.3ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOVE (handler)",
            "value": -54.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOVE (DU/s)",
            "value": 50839,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD (total)",
            "value": 19656.33,
            "unit": "ns/DU",
            "extra": "median=2457041ns  p95=2580625ns  stddev=59322.5ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD (handler)",
            "value": -68.01,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD (DU/s)",
            "value": 50874,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE (total)",
            "value": 19679.34,
            "unit": "ns/DU",
            "extra": "median=2459917ns  p95=2621042ns  stddev=71509.1ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE (handler)",
            "value": -45,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE (DU/s)",
            "value": 50815,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] PUSH (total)",
            "value": 19887.66,
            "unit": "ns/DU",
            "extra": "median=2485958ns  p95=2670333ns  stddev=78496.7ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] PUSH (handler)",
            "value": 163.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] PUSH (DU/s)",
            "value": 50282,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] POP (total)",
            "value": 19799.33,
            "unit": "ns/DU",
            "extra": "median=2474916ns  p95=2632500ns  stddev=77938.5ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] POP (handler)",
            "value": 74.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] POP (DU/s)",
            "value": 50507,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (total)",
            "value": 19858.33,
            "unit": "ns/DU",
            "extra": "median=2482291ns  p95=2683666ns  stddev=83389.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (handler)",
            "value": 133.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CONST (DU/s)",
            "value": 50357,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (total)",
            "value": 19740,
            "unit": "ns/DU",
            "extra": "median=2467500ns  p95=2615291ns  stddev=67326.5ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (handler)",
            "value": 15.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOAD_CTX (DU/s)",
            "value": 50659,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (total)",
            "value": 19821.33,
            "unit": "ns/DU",
            "extra": "median=2477666ns  p95=2599333ns  stddev=60193.5ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (handler)",
            "value": 96.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] STORE_CTX (DU/s)",
            "value": 50451,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ADD (total)",
            "value": 19643.33,
            "unit": "ns/DU",
            "extra": "median=2455416ns  p95=2590958ns  stddev=63454.8ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] ADD (handler)",
            "value": -81.01,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ADD (DU/s)",
            "value": 50908,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SUB (total)",
            "value": 19798.34,
            "unit": "ns/DU",
            "extra": "median=2474792ns  p95=2616625ns  stddev=63704.9ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] SUB (handler)",
            "value": 74,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SUB (DU/s)",
            "value": 50509,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUL (total)",
            "value": 19730,
            "unit": "ns/DU",
            "extra": "median=2466250ns  p95=2652542ns  stddev=69374.3ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUL (handler)",
            "value": 5.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUL (DU/s)",
            "value": 50684,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IMUL (total)",
            "value": 19785,
            "unit": "ns/DU",
            "extra": "median=2473125ns  p95=2606750ns  stddev=61037.3ns  iterations=97  DUs=125"
          },
          {
            "name": "[DebugPolicy] IMUL (handler)",
            "value": 60.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IMUL (DU/s)",
            "value": 50543,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] DIV (total)",
            "value": 19743.66,
            "unit": "ns/DU",
            "extra": "median=2467958ns  p95=2604542ns  stddev=65701.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] DIV (handler)",
            "value": 19.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] DIV (DU/s)",
            "value": 50649,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] IDIV (total)",
            "value": 19882.33,
            "unit": "ns/DU",
            "extra": "median=2485291ns  p95=2612333ns  stddev=67934.1ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] IDIV (handler)",
            "value": 157.99,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] IDIV (DU/s)",
            "value": 50296,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NEG (total)",
            "value": 19763.34,
            "unit": "ns/DU",
            "extra": "median=2470417ns  p95=2683125ns  stddev=81444.9ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] NEG (handler)",
            "value": 39,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NEG (DU/s)",
            "value": 50599,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MOD (total)",
            "value": 19792.67,
            "unit": "ns/DU",
            "extra": "median=2474084ns  p95=2590833ns  stddev=61273.8ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] MOD (handler)",
            "value": 68.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MOD (DU/s)",
            "value": 50524,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] AND (total)",
            "value": 19729,
            "unit": "ns/DU",
            "extra": "median=2466125ns  p95=2589459ns  stddev=59549.8ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] AND (handler)",
            "value": 4.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] AND (DU/s)",
            "value": 50687,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] OR (total)",
            "value": 19690.66,
            "unit": "ns/DU",
            "extra": "median=2461333ns  p95=2618292ns  stddev=71221.3ns  iterations=96  DUs=125"
          },
          {
            "name": "[DebugPolicy] OR (handler)",
            "value": -33.67,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] OR (DU/s)",
            "value": 50785,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XOR (total)",
            "value": 19711,
            "unit": "ns/DU",
            "extra": "median=2463875ns  p95=2575625ns  stddev=58651.8ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] XOR (handler)",
            "value": -13.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XOR (DU/s)",
            "value": 50733,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOT (total)",
            "value": 19755.34,
            "unit": "ns/DU",
            "extra": "median=2469417ns  p95=2612250ns  stddev=69290.1ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOT (handler)",
            "value": 31,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOT (DU/s)",
            "value": 50619,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHL (total)",
            "value": 19816.34,
            "unit": "ns/DU",
            "extra": "median=2477042ns  p95=2689250ns  stddev=76141.5ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHL (handler)",
            "value": 92,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHL (DU/s)",
            "value": 50463,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SHR (total)",
            "value": 19710,
            "unit": "ns/DU",
            "extra": "median=2463750ns  p95=2615708ns  stddev=69960.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] SHR (handler)",
            "value": -14.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SHR (DU/s)",
            "value": 50736,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SAR (total)",
            "value": 19765,
            "unit": "ns/DU",
            "extra": "median=2470625ns  p95=2589250ns  stddev=65598.1ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] SAR (handler)",
            "value": 40.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SAR (DU/s)",
            "value": 50594,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROL (total)",
            "value": 19700.67,
            "unit": "ns/DU",
            "extra": "median=2462584ns  p95=2594875ns  stddev=63734.6ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROL (handler)",
            "value": -23.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROL (DU/s)",
            "value": 50760,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ROR (total)",
            "value": 19962,
            "unit": "ns/DU",
            "extra": "median=2495250ns  p95=2675375ns  stddev=79521.3ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] ROR (handler)",
            "value": 237.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ROR (DU/s)",
            "value": 50095,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMP (total)",
            "value": 19680,
            "unit": "ns/DU",
            "extra": "median=2460000ns  p95=2612583ns  stddev=63838.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMP (handler)",
            "value": -44.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMP (DU/s)",
            "value": 50813,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TEST (total)",
            "value": 19829.66,
            "unit": "ns/DU",
            "extra": "median=2478708ns  p95=2629708ns  stddev=68016.5ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] TEST (handler)",
            "value": 105.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TEST (DU/s)",
            "value": 50429,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (total)",
            "value": 19790.34,
            "unit": "ns/DU",
            "extra": "median=2473792ns  p95=2638417ns  stddev=70028.2ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (handler)",
            "value": 66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SET_FLAG (DU/s)",
            "value": 50530,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (total)",
            "value": 19844.67,
            "unit": "ns/DU",
            "extra": "median=2480584ns  p95=2653458ns  stddev=73773.3ns  iterations=104  DUs=125"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (handler)",
            "value": 120.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] GET_FLAG (DU/s)",
            "value": 50391,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JMP (total)",
            "value": 19805.67,
            "unit": "ns/DU",
            "extra": "median=2475709ns  p95=2610583ns  stddev=78268.6ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] JMP (handler)",
            "value": 81.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JMP (DU/s)",
            "value": 50491,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] JCC (total)",
            "value": 19700.66,
            "unit": "ns/DU",
            "extra": "median=2462583ns  p95=2548458ns  stddev=45324.6ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] JCC (handler)",
            "value": -23.67,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] JCC (DU/s)",
            "value": 50760,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (total)",
            "value": 19799.67,
            "unit": "ns/DU",
            "extra": "median=2474959ns  p95=2589250ns  stddev=61199.0ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (handler)",
            "value": 75.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NATIVE_CALL (DU/s)",
            "value": 50506,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT8 (total)",
            "value": 19737,
            "unit": "ns/DU",
            "extra": "median=2467125ns  p95=2610958ns  stddev=64797.1ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT8 (handler)",
            "value": 12.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT8 (DU/s)",
            "value": 50666,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT16 (total)",
            "value": 19733.66,
            "unit": "ns/DU",
            "extra": "median=2466708ns  p95=2593125ns  stddev=59649.3ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT16 (handler)",
            "value": 9.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT16 (DU/s)",
            "value": 50675,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] SEXT32 (total)",
            "value": 19744,
            "unit": "ns/DU",
            "extra": "median=2468000ns  p95=2615917ns  stddev=68201.8ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] SEXT32 (handler)",
            "value": 19.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] SEXT32 (DU/s)",
            "value": 50648,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (total)",
            "value": 19795,
            "unit": "ns/DU",
            "extra": "median=2474375ns  p95=2644084ns  stddev=74944.8ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (handler)",
            "value": 70.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT8 (DU/s)",
            "value": 50518,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (total)",
            "value": 19678,
            "unit": "ns/DU",
            "extra": "median=2459750ns  p95=2620500ns  stddev=63817.9ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (handler)",
            "value": -46.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT16 (DU/s)",
            "value": 50818,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (total)",
            "value": 19700.66,
            "unit": "ns/DU",
            "extra": "median=2462583ns  p95=2620625ns  stddev=69748.9ns  iterations=103  DUs=125"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (handler)",
            "value": -23.67,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ZEXT32 (DU/s)",
            "value": 50760,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (total)",
            "value": 19743.34,
            "unit": "ns/DU",
            "extra": "median=2467917ns  p95=2624000ns  stddev=75904.9ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (handler)",
            "value": 19,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC8 (DU/s)",
            "value": 50650,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (total)",
            "value": 19787.66,
            "unit": "ns/DU",
            "extra": "median=2473458ns  p95=2642042ns  stddev=69664.3ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (handler)",
            "value": 63.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] TRUNC16 (DU/s)",
            "value": 50537,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (total)",
            "value": 19670,
            "unit": "ns/DU",
            "extra": "median=2458750ns  p95=2593792ns  stddev=65464.9ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (handler)",
            "value": -54.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] LOCK_ADD (DU/s)",
            "value": 50839,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] XCHG (total)",
            "value": 19791,
            "unit": "ns/DU",
            "extra": "median=2473875ns  p95=2594708ns  stddev=57576.7ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] XCHG (handler)",
            "value": 66.66,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] XCHG (DU/s)",
            "value": 50528,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (total)",
            "value": 19722.66,
            "unit": "ns/DU",
            "extra": "median=2465333ns  p95=2577208ns  stddev=62187.2ns  iterations=99  DUs=125"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (handler)",
            "value": -1.67,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CMPXCHG (DU/s)",
            "value": 50703,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] FENCE (total)",
            "value": 19677.34,
            "unit": "ns/DU",
            "extra": "median=2459667ns  p95=2593125ns  stddev=60053.5ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] FENCE (handler)",
            "value": -47,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] FENCE (DU/s)",
            "value": 50820,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (total)",
            "value": 19761.34,
            "unit": "ns/DU",
            "extra": "median=2470167ns  p95=2605292ns  stddev=56192.9ns  iterations=100  DUs=125"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (handler)",
            "value": 37,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] ATOMIC_LOAD (DU/s)",
            "value": 50604,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] NOP (total)",
            "value": 19584,
            "unit": "ns/DU",
            "extra": "median=2448000ns  p95=2588667ns  stddev=56811.9ns  iterations=98  DUs=125"
          },
          {
            "name": "[DebugPolicy] NOP (handler)",
            "value": -140.34,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] NOP (DU/s)",
            "value": 51062,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (total)",
            "value": 19756.66,
            "unit": "ns/DU",
            "extra": "median=2469583ns  p95=2595958ns  stddev=62449.0ns  iterations=105  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (handler)",
            "value": 32.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 50616,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (total)",
            "value": 19763.66,
            "unit": "ns/DU",
            "extra": "median=2470458ns  p95=2591000ns  stddev=64853.2ns  iterations=102  DUs=125"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (handler)",
            "value": 39.33,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] CHECK_DEBUG (DU/s)",
            "value": 50598,
            "unit": "DU/s"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (total)",
            "value": 19688.34,
            "unit": "ns/DU",
            "extra": "median=2461042ns  p95=2574167ns  stddev=58817.4ns  iterations=101  DUs=125"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (handler)",
            "value": -36,
            "unit": "ns"
          },
          {
            "name": "[DebugPolicy] MUTATE_ISA (DU/s)",
            "value": 50791,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 61795.34,
            "unit": "ns/DU",
            "extra": "median=7724417ns  p95=7817208ns  stddev=50003.4ns  iterations=91  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 16182,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOVE (total)",
            "value": 61672.33,
            "unit": "ns/DU",
            "extra": "median=7709041ns  p95=7809208ns  stddev=48019.0ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOVE (handler)",
            "value": -123.01,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOVE (DU/s)",
            "value": 16215,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD (total)",
            "value": 61734,
            "unit": "ns/DU",
            "extra": "median=7716750ns  p95=7852584ns  stddev=63120.8ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD (handler)",
            "value": -61.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD (DU/s)",
            "value": 16199,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE (total)",
            "value": 61657.67,
            "unit": "ns/DU",
            "extra": "median=7707209ns  p95=7792333ns  stddev=37941.9ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE (handler)",
            "value": -137.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE (DU/s)",
            "value": 16219,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] PUSH (total)",
            "value": 61827.34,
            "unit": "ns/DU",
            "extra": "median=7728417ns  p95=7843167ns  stddev=70841.2ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] PUSH (handler)",
            "value": 32,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] PUSH (DU/s)",
            "value": 16174,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] POP (total)",
            "value": 61804,
            "unit": "ns/DU",
            "extra": "median=7725500ns  p95=7846959ns  stddev=65984.6ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] POP (handler)",
            "value": 8.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] POP (DU/s)",
            "value": 16180,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (total)",
            "value": 61816,
            "unit": "ns/DU",
            "extra": "median=7727000ns  p95=7822417ns  stddev=46179.5ns  iterations=89  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (handler)",
            "value": 20.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CONST (DU/s)",
            "value": 16177,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (total)",
            "value": 61834.66,
            "unit": "ns/DU",
            "extra": "median=7729333ns  p95=7826875ns  stddev=78596.6ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (handler)",
            "value": 39.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOAD_CTX (DU/s)",
            "value": 16172,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (total)",
            "value": 61784,
            "unit": "ns/DU",
            "extra": "median=7723000ns  p95=7833375ns  stddev=63065.2ns  iterations=91  DUs=125"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (handler)",
            "value": -11.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] STORE_CTX (DU/s)",
            "value": 16185,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ADD (total)",
            "value": 61663.34,
            "unit": "ns/DU",
            "extra": "median=7707917ns  p95=7816416ns  stddev=77589.9ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] ADD (handler)",
            "value": -132,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ADD (DU/s)",
            "value": 16217,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SUB (total)",
            "value": 61736,
            "unit": "ns/DU",
            "extra": "median=7717000ns  p95=7771125ns  stddev=39399.6ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] SUB (handler)",
            "value": -59.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SUB (DU/s)",
            "value": 16198,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUL (total)",
            "value": 61700.66,
            "unit": "ns/DU",
            "extra": "median=7712583ns  p95=7799500ns  stddev=49270.7ns  iterations=90  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUL (handler)",
            "value": -94.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUL (DU/s)",
            "value": 16207,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IMUL (total)",
            "value": 61713.66,
            "unit": "ns/DU",
            "extra": "median=7714208ns  p95=7796417ns  stddev=34506.8ns  iterations=89  DUs=125"
          },
          {
            "name": "[StandardPolicy] IMUL (handler)",
            "value": -81.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IMUL (DU/s)",
            "value": 16204,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] DIV (total)",
            "value": 61757,
            "unit": "ns/DU",
            "extra": "median=7719625ns  p95=7808083ns  stddev=44947.6ns  iterations=91  DUs=125"
          },
          {
            "name": "[StandardPolicy] DIV (handler)",
            "value": -38.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] DIV (DU/s)",
            "value": 16192,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] IDIV (total)",
            "value": 61717,
            "unit": "ns/DU",
            "extra": "median=7714625ns  p95=7791208ns  stddev=53731.3ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] IDIV (handler)",
            "value": -78.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] IDIV (DU/s)",
            "value": 16203,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NEG (total)",
            "value": 61823.66,
            "unit": "ns/DU",
            "extra": "median=7727958ns  p95=7821708ns  stddev=59875.0ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] NEG (handler)",
            "value": 28.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NEG (DU/s)",
            "value": 16175,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MOD (total)",
            "value": 61686.66,
            "unit": "ns/DU",
            "extra": "median=7710833ns  p95=7775792ns  stddev=52929.1ns  iterations=90  DUs=125"
          },
          {
            "name": "[StandardPolicy] MOD (handler)",
            "value": -108.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MOD (DU/s)",
            "value": 16211,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] AND (total)",
            "value": 61756,
            "unit": "ns/DU",
            "extra": "median=7719500ns  p95=7785083ns  stddev=39410.4ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] AND (handler)",
            "value": -39.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] AND (DU/s)",
            "value": 16193,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] OR (total)",
            "value": 61642,
            "unit": "ns/DU",
            "extra": "median=7705250ns  p95=7824542ns  stddev=65207.4ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] OR (handler)",
            "value": -153.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] OR (DU/s)",
            "value": 16223,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XOR (total)",
            "value": 61748.66,
            "unit": "ns/DU",
            "extra": "median=7718583ns  p95=7795500ns  stddev=39146.1ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] XOR (handler)",
            "value": -46.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XOR (DU/s)",
            "value": 16195,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOT (total)",
            "value": 61817.33,
            "unit": "ns/DU",
            "extra": "median=7727166ns  p95=7794083ns  stddev=48146.2ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOT (handler)",
            "value": 21.99,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOT (DU/s)",
            "value": 16177,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHL (total)",
            "value": 61760.66,
            "unit": "ns/DU",
            "extra": "median=7720083ns  p95=7800208ns  stddev=40773.1ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHL (handler)",
            "value": -34.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHL (DU/s)",
            "value": 16192,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SHR (total)",
            "value": 61714.66,
            "unit": "ns/DU",
            "extra": "median=7714333ns  p95=7799916ns  stddev=43520.6ns  iterations=89  DUs=125"
          },
          {
            "name": "[StandardPolicy] SHR (handler)",
            "value": -80.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SHR (DU/s)",
            "value": 16204,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SAR (total)",
            "value": 61669.67,
            "unit": "ns/DU",
            "extra": "median=7708709ns  p95=7780709ns  stddev=45450.6ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] SAR (handler)",
            "value": -125.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SAR (DU/s)",
            "value": 16215,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROL (total)",
            "value": 61693,
            "unit": "ns/DU",
            "extra": "median=7711625ns  p95=7798375ns  stddev=64421.1ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROL (handler)",
            "value": -102.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROL (DU/s)",
            "value": 16209,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ROR (total)",
            "value": 61760.67,
            "unit": "ns/DU",
            "extra": "median=7720084ns  p95=7774167ns  stddev=35914.0ns  iterations=91  DUs=125"
          },
          {
            "name": "[StandardPolicy] ROR (handler)",
            "value": -34.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ROR (DU/s)",
            "value": 16192,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMP (total)",
            "value": 61641,
            "unit": "ns/DU",
            "extra": "median=7705125ns  p95=7782750ns  stddev=48326.7ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMP (handler)",
            "value": -154.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMP (DU/s)",
            "value": 16223,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TEST (total)",
            "value": 61688.66,
            "unit": "ns/DU",
            "extra": "median=7711083ns  p95=7788917ns  stddev=51227.1ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] TEST (handler)",
            "value": -106.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TEST (DU/s)",
            "value": 16210,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (total)",
            "value": 61880.66,
            "unit": "ns/DU",
            "extra": "median=7735083ns  p95=7797417ns  stddev=36882.5ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (handler)",
            "value": 85.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SET_FLAG (DU/s)",
            "value": 16160,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (total)",
            "value": 61836.34,
            "unit": "ns/DU",
            "extra": "median=7729542ns  p95=7820833ns  stddev=50866.3ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (handler)",
            "value": 41,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] GET_FLAG (DU/s)",
            "value": 16172,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JMP (total)",
            "value": 61770.67,
            "unit": "ns/DU",
            "extra": "median=7721334ns  p95=7820125ns  stddev=89369.4ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] JMP (handler)",
            "value": -24.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JMP (DU/s)",
            "value": 16189,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] JCC (total)",
            "value": 61825.66,
            "unit": "ns/DU",
            "extra": "median=7728208ns  p95=7815417ns  stddev=38725.5ns  iterations=96  DUs=125"
          },
          {
            "name": "[StandardPolicy] JCC (handler)",
            "value": 30.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] JCC (DU/s)",
            "value": 16175,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (total)",
            "value": 61949.34,
            "unit": "ns/DU",
            "extra": "median=7743667ns  p95=7845292ns  stddev=46217.7ns  iterations=93  DUs=125"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (handler)",
            "value": 154,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NATIVE_CALL (DU/s)",
            "value": 16142,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT8 (total)",
            "value": 61836.33,
            "unit": "ns/DU",
            "extra": "median=7729541ns  p95=7815666ns  stddev=45313.3ns  iterations=95  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT8 (handler)",
            "value": 40.99,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT8 (DU/s)",
            "value": 16172,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT16 (total)",
            "value": 61787,
            "unit": "ns/DU",
            "extra": "median=7723375ns  p95=7801333ns  stddev=45302.9ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT16 (handler)",
            "value": -8.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT16 (DU/s)",
            "value": 16185,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] SEXT32 (total)",
            "value": 61805,
            "unit": "ns/DU",
            "extra": "median=7725625ns  p95=7820959ns  stddev=60096.1ns  iterations=98  DUs=125"
          },
          {
            "name": "[StandardPolicy] SEXT32 (handler)",
            "value": 9.66,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] SEXT32 (DU/s)",
            "value": 16180,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (total)",
            "value": 61769.66,
            "unit": "ns/DU",
            "extra": "median=7721208ns  p95=7806625ns  stddev=55157.8ns  iterations=90  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (handler)",
            "value": -25.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT8 (DU/s)",
            "value": 16189,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (total)",
            "value": 61835.66,
            "unit": "ns/DU",
            "extra": "median=7729458ns  p95=7821500ns  stddev=46574.9ns  iterations=90  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (handler)",
            "value": 40.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT16 (DU/s)",
            "value": 16172,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (total)",
            "value": 61818.33,
            "unit": "ns/DU",
            "extra": "median=7727291ns  p95=7815208ns  stddev=65673.5ns  iterations=90  DUs=125"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (handler)",
            "value": 22.99,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ZEXT32 (DU/s)",
            "value": 16176,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (total)",
            "value": 61840.66,
            "unit": "ns/DU",
            "extra": "median=7730083ns  p95=7803666ns  stddev=46478.9ns  iterations=89  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (handler)",
            "value": 45.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC8 (DU/s)",
            "value": 16171,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (total)",
            "value": 61865.33,
            "unit": "ns/DU",
            "extra": "median=7733166ns  p95=7797334ns  stddev=39614.6ns  iterations=88  DUs=125"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (handler)",
            "value": 69.99,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] TRUNC16 (DU/s)",
            "value": 16164,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (total)",
            "value": 61640.66,
            "unit": "ns/DU",
            "extra": "median=7705083ns  p95=7767667ns  stddev=42396.0ns  iterations=90  DUs=125"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (handler)",
            "value": -154.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] LOCK_ADD (DU/s)",
            "value": 16223,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] XCHG (total)",
            "value": 61754,
            "unit": "ns/DU",
            "extra": "median=7719250ns  p95=7799542ns  stddev=42825.6ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] XCHG (handler)",
            "value": -41.34,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] XCHG (DU/s)",
            "value": 16193,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (total)",
            "value": 61751.33,
            "unit": "ns/DU",
            "extra": "median=7718916ns  p95=7773625ns  stddev=37163.6ns  iterations=91  DUs=125"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (handler)",
            "value": -44.01,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CMPXCHG (DU/s)",
            "value": 16194,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] FENCE (total)",
            "value": 61810.33,
            "unit": "ns/DU",
            "extra": "median=7726291ns  p95=7822334ns  stddev=63706.3ns  iterations=87  DUs=125"
          },
          {
            "name": "[StandardPolicy] FENCE (handler)",
            "value": 14.99,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] FENCE (DU/s)",
            "value": 16179,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (total)",
            "value": 61697.66,
            "unit": "ns/DU",
            "extra": "median=7712208ns  p95=7782000ns  stddev=41480.4ns  iterations=92  DUs=125"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (handler)",
            "value": -97.67,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] ATOMIC_LOAD (DU/s)",
            "value": 16208,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] NOP (total)",
            "value": 61867.66,
            "unit": "ns/DU",
            "extra": "median=7733458ns  p95=7838625ns  stddev=53258.7ns  iterations=94  DUs=125"
          },
          {
            "name": "[StandardPolicy] NOP (handler)",
            "value": 72.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] NOP (DU/s)",
            "value": 16164,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (total)",
            "value": 61756.33,
            "unit": "ns/DU",
            "extra": "median=7719541ns  p95=7830000ns  stddev=78581.9ns  iterations=97  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (handler)",
            "value": -39.01,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 16193,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (total)",
            "value": 61818.66,
            "unit": "ns/DU",
            "extra": "median=7727333ns  p95=7842583ns  stddev=51583.3ns  iterations=91  DUs=125"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (handler)",
            "value": 23.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] CHECK_DEBUG (DU/s)",
            "value": 16176,
            "unit": "DU/s"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (total)",
            "value": 61856.66,
            "unit": "ns/DU",
            "extra": "median=7732083ns  p95=7810333ns  stddev=38724.8ns  iterations=89  DUs=125"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (handler)",
            "value": 61.33,
            "unit": "ns"
          },
          {
            "name": "[StandardPolicy] MUTATE_ISA (DU/s)",
            "value": 16166,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 108708.67,
            "unit": "ns/DU",
            "extra": "median=13588584ns  p95=13724542ns  stddev=64392.3ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 9199,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOVE (total)",
            "value": 108730,
            "unit": "ns/DU",
            "extra": "median=13591250ns  p95=13710125ns  stddev=54797.0ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOVE (handler)",
            "value": 21.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOVE (DU/s)",
            "value": 9197,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD (total)",
            "value": 108726.33,
            "unit": "ns/DU",
            "extra": "median=13590791ns  p95=13744084ns  stddev=79068.0ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD (handler)",
            "value": 17.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD (DU/s)",
            "value": 9197,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE (total)",
            "value": 108791.34,
            "unit": "ns/DU",
            "extra": "median=13598917ns  p95=13716500ns  stddev=74034.9ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE (handler)",
            "value": 82.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE (DU/s)",
            "value": 9192,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] PUSH (total)",
            "value": 108756.33,
            "unit": "ns/DU",
            "extra": "median=13594541ns  p95=13700250ns  stddev=62433.8ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] PUSH (handler)",
            "value": 47.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] PUSH (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] POP (total)",
            "value": 108726.34,
            "unit": "ns/DU",
            "extra": "median=13590792ns  p95=13737875ns  stddev=74915.6ns  iterations=97  DUs=125"
          },
          {
            "name": "[HighSecPolicy] POP (handler)",
            "value": 17.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] POP (DU/s)",
            "value": 9197,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (total)",
            "value": 108693.34,
            "unit": "ns/DU",
            "extra": "median=13586667ns  p95=13691333ns  stddev=63352.0ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (handler)",
            "value": -15.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CONST (DU/s)",
            "value": 9200,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (total)",
            "value": 108669.34,
            "unit": "ns/DU",
            "extra": "median=13583667ns  p95=13707666ns  stddev=110811.7ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (handler)",
            "value": -39.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOAD_CTX (DU/s)",
            "value": 9202,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (total)",
            "value": 108788.67,
            "unit": "ns/DU",
            "extra": "median=13598584ns  p95=13755084ns  stddev=82831.1ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (handler)",
            "value": 80,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] STORE_CTX (DU/s)",
            "value": 9192,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ADD (total)",
            "value": 108697,
            "unit": "ns/DU",
            "extra": "median=13587125ns  p95=13741042ns  stddev=58610.1ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ADD (handler)",
            "value": -11.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ADD (DU/s)",
            "value": 9200,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SUB (total)",
            "value": 108753.34,
            "unit": "ns/DU",
            "extra": "median=13594167ns  p95=13726625ns  stddev=75786.5ns  iterations=84  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SUB (handler)",
            "value": 44.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SUB (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUL (total)",
            "value": 108714.34,
            "unit": "ns/DU",
            "extra": "median=13589292ns  p95=13708375ns  stddev=67186.6ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUL (handler)",
            "value": 5.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUL (DU/s)",
            "value": 9198,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IMUL (total)",
            "value": 108666.67,
            "unit": "ns/DU",
            "extra": "median=13583334ns  p95=13681000ns  stddev=66374.3ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IMUL (handler)",
            "value": -42,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IMUL (DU/s)",
            "value": 9202,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] DIV (total)",
            "value": 108744.34,
            "unit": "ns/DU",
            "extra": "median=13593042ns  p95=13726708ns  stddev=77521.4ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] DIV (handler)",
            "value": 35.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] DIV (DU/s)",
            "value": 9196,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] IDIV (total)",
            "value": 108679.66,
            "unit": "ns/DU",
            "extra": "median=13584958ns  p95=13725542ns  stddev=68010.9ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] IDIV (handler)",
            "value": -29.01,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] IDIV (DU/s)",
            "value": 9201,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NEG (total)",
            "value": 108792,
            "unit": "ns/DU",
            "extra": "median=13599000ns  p95=13695708ns  stddev=64409.9ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NEG (handler)",
            "value": 83.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NEG (DU/s)",
            "value": 9192,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MOD (total)",
            "value": 108750,
            "unit": "ns/DU",
            "extra": "median=13593750ns  p95=13744417ns  stddev=114471.9ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MOD (handler)",
            "value": 41.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MOD (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] AND (total)",
            "value": 108681.34,
            "unit": "ns/DU",
            "extra": "median=13585167ns  p95=13740167ns  stddev=85877.1ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] AND (handler)",
            "value": -27.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] AND (DU/s)",
            "value": 9201,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] OR (total)",
            "value": 108742.66,
            "unit": "ns/DU",
            "extra": "median=13592833ns  p95=13705625ns  stddev=87437.2ns  iterations=99  DUs=125"
          },
          {
            "name": "[HighSecPolicy] OR (handler)",
            "value": 33.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] OR (DU/s)",
            "value": 9196,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XOR (total)",
            "value": 108769.66,
            "unit": "ns/DU",
            "extra": "median=13596208ns  p95=13746000ns  stddev=89781.8ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XOR (handler)",
            "value": 60.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XOR (DU/s)",
            "value": 9194,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOT (total)",
            "value": 108731.34,
            "unit": "ns/DU",
            "extra": "median=13591417ns  p95=13748667ns  stddev=78336.2ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOT (handler)",
            "value": 22.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOT (DU/s)",
            "value": 9197,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHL (total)",
            "value": 108785,
            "unit": "ns/DU",
            "extra": "median=13598125ns  p95=13786708ns  stddev=82999.9ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHL (handler)",
            "value": 76.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHL (DU/s)",
            "value": 9192,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SHR (total)",
            "value": 108741.66,
            "unit": "ns/DU",
            "extra": "median=13592708ns  p95=13788500ns  stddev=102899.0ns  iterations=100  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SHR (handler)",
            "value": 32.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SHR (DU/s)",
            "value": 9196,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SAR (total)",
            "value": 108771,
            "unit": "ns/DU",
            "extra": "median=13596375ns  p95=13712292ns  stddev=79662.1ns  iterations=96  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SAR (handler)",
            "value": 62.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SAR (DU/s)",
            "value": 9194,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROL (total)",
            "value": 108796.66,
            "unit": "ns/DU",
            "extra": "median=13599583ns  p95=13750959ns  stddev=73644.1ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROL (handler)",
            "value": 87.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROL (DU/s)",
            "value": 9191,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ROR (total)",
            "value": 108753.66,
            "unit": "ns/DU",
            "extra": "median=13594208ns  p95=13717542ns  stddev=57259.9ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ROR (handler)",
            "value": 44.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ROR (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMP (total)",
            "value": 108699,
            "unit": "ns/DU",
            "extra": "median=13587375ns  p95=13730959ns  stddev=91429.4ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMP (handler)",
            "value": -9.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMP (DU/s)",
            "value": 9200,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TEST (total)",
            "value": 108703.66,
            "unit": "ns/DU",
            "extra": "median=13587958ns  p95=13723792ns  stddev=68667.7ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TEST (handler)",
            "value": -5.01,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TEST (DU/s)",
            "value": 9199,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (total)",
            "value": 108776.66,
            "unit": "ns/DU",
            "extra": "median=13597083ns  p95=13736458ns  stddev=92253.9ns  iterations=94  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (handler)",
            "value": 67.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SET_FLAG (DU/s)",
            "value": 9193,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (total)",
            "value": 108706.66,
            "unit": "ns/DU",
            "extra": "median=13588333ns  p95=13650084ns  stddev=54856.5ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (handler)",
            "value": -2.01,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] GET_FLAG (DU/s)",
            "value": 9199,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JMP (total)",
            "value": 108678.34,
            "unit": "ns/DU",
            "extra": "median=13584792ns  p95=13728667ns  stddev=78054.1ns  iterations=94  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JMP (handler)",
            "value": -30.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JMP (DU/s)",
            "value": 9201,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] JCC (total)",
            "value": 108701.67,
            "unit": "ns/DU",
            "extra": "median=13587709ns  p95=13726916ns  stddev=83820.4ns  iterations=89  DUs=125"
          },
          {
            "name": "[HighSecPolicy] JCC (handler)",
            "value": -7,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] JCC (DU/s)",
            "value": 9199,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (total)",
            "value": 108756.66,
            "unit": "ns/DU",
            "extra": "median=13594583ns  p95=13736125ns  stddev=103740.9ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (handler)",
            "value": 47.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NATIVE_CALL (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (total)",
            "value": 108698,
            "unit": "ns/DU",
            "extra": "median=13587250ns  p95=13709000ns  stddev=62020.0ns  iterations=86  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (handler)",
            "value": -10.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT8 (DU/s)",
            "value": 9200,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (total)",
            "value": 108758.66,
            "unit": "ns/DU",
            "extra": "median=13594833ns  p95=13722250ns  stddev=76410.0ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (handler)",
            "value": 49.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT16 (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (total)",
            "value": 108715,
            "unit": "ns/DU",
            "extra": "median=13589375ns  p95=13717167ns  stddev=75582.8ns  iterations=90  DUs=125"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (handler)",
            "value": 6.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] SEXT32 (DU/s)",
            "value": 9198,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (total)",
            "value": 108808.33,
            "unit": "ns/DU",
            "extra": "median=13601041ns  p95=13715458ns  stddev=70659.0ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (handler)",
            "value": 99.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT8 (DU/s)",
            "value": 9190,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (total)",
            "value": 108750.66,
            "unit": "ns/DU",
            "extra": "median=13593833ns  p95=13713667ns  stddev=60726.0ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (handler)",
            "value": 41.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT16 (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (total)",
            "value": 108673,
            "unit": "ns/DU",
            "extra": "median=13584125ns  p95=13713500ns  stddev=90501.1ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (handler)",
            "value": -35.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ZEXT32 (DU/s)",
            "value": 9202,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (total)",
            "value": 108745.66,
            "unit": "ns/DU",
            "extra": "median=13593208ns  p95=13712333ns  stddev=74981.3ns  iterations=91  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (handler)",
            "value": 36.99,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC8 (DU/s)",
            "value": 9196,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (total)",
            "value": 108675.34,
            "unit": "ns/DU",
            "extra": "median=13584417ns  p95=13739958ns  stddev=94746.8ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (handler)",
            "value": -33.34,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] TRUNC16 (DU/s)",
            "value": 9202,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (total)",
            "value": 108752.67,
            "unit": "ns/DU",
            "extra": "median=13594084ns  p95=13734000ns  stddev=74308.0ns  iterations=92  DUs=125"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (handler)",
            "value": 44,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] LOCK_ADD (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] XCHG (total)",
            "value": 108745,
            "unit": "ns/DU",
            "extra": "median=13593125ns  p95=13705375ns  stddev=62229.5ns  iterations=81  DUs=125"
          },
          {
            "name": "[HighSecPolicy] XCHG (handler)",
            "value": 36.33,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] XCHG (DU/s)",
            "value": 9196,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (total)",
            "value": 108700,
            "unit": "ns/DU",
            "extra": "median=13587500ns  p95=13711625ns  stddev=75265.3ns  iterations=88  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (handler)",
            "value": -8.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CMPXCHG (DU/s)",
            "value": 9200,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] FENCE (total)",
            "value": 108742.34,
            "unit": "ns/DU",
            "extra": "median=13592792ns  p95=13736958ns  stddev=85599.0ns  iterations=87  DUs=125"
          },
          {
            "name": "[HighSecPolicy] FENCE (handler)",
            "value": 33.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] FENCE (DU/s)",
            "value": 9196,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (total)",
            "value": 108770.34,
            "unit": "ns/DU",
            "extra": "median=13596292ns  p95=13719084ns  stddev=89612.5ns  iterations=84  DUs=125"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (handler)",
            "value": 61.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] ATOMIC_LOAD (DU/s)",
            "value": 9194,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] NOP (total)",
            "value": 108752.34,
            "unit": "ns/DU",
            "extra": "median=13594042ns  p95=13738000ns  stddev=77637.8ns  iterations=94  DUs=125"
          },
          {
            "name": "[HighSecPolicy] NOP (handler)",
            "value": 43.66,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] NOP (DU/s)",
            "value": 9195,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (total)",
            "value": 108681,
            "unit": "ns/DU",
            "extra": "median=13585125ns  p95=13703042ns  stddev=58778.1ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (handler)",
            "value": -27.67,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_INTEGRITY (DU/s)",
            "value": 9201,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (total)",
            "value": 108683.66,
            "unit": "ns/DU",
            "extra": "median=13585458ns  p95=13728250ns  stddev=87001.9ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (handler)",
            "value": -25.01,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] CHECK_DEBUG (DU/s)",
            "value": 9201,
            "unit": "DU/s"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (total)",
            "value": 108704.66,
            "unit": "ns/DU",
            "extra": "median=13588083ns  p95=13687000ns  stddev=85390.3ns  iterations=93  DUs=125"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (handler)",
            "value": -4.01,
            "unit": "ns"
          },
          {
            "name": "[HighSecPolicy] MUTATE_ISA (DU/s)",
            "value": 9199,
            "unit": "DU/s"
          }
        ]
      }
    ]
  }
}