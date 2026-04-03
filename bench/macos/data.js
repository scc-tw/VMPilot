window.BENCHMARK_DATA = {
  "lastUpdate": 1775233513346,
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
      }
    ]
  }
}