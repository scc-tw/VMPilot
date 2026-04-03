window.BENCHMARK_DATA = {
  "lastUpdate": 1775222478841,
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
          "id": "cff327330048790ca49fb668f3d9d86b30c0cf98",
          "message": "fix: Add missing cstddef include for size_t in opcode_spec.hpp",
          "timestamp": "2026-04-03T04:40:04+08:00",
          "tree_id": "18ea829370e32f46e76a98f5b28c75b7ca90f5bd",
          "url": "https://github.com/scc-tw/VMPilot/commit/cff327330048790ca49fb668f3d9d86b30c0cf98"
        },
        "date": 1775162466962,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 4674,
            "unit": "ns/insn",
            "extra": "median=2337000ns  p95=2439500ns  stddev=31918.0ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 213950,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 6829.42,
            "unit": "ns/insn",
            "extra": "median=3414708ns  p95=6465208ns  stddev=1079241.0ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 2155.42,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 146425,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 6297.83,
            "unit": "ns/insn",
            "extra": "median=3148916ns  p95=4141500ns  stddev=622441.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 1623.83,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 158785,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 4731.58,
            "unit": "ns/insn",
            "extra": "median=2365791ns  p95=2402625ns  stddev=20500.3ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 57.58,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 211346,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 4728.17,
            "unit": "ns/insn",
            "extra": "median=2364084ns  p95=2431875ns  stddev=30497.3ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 54.17,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 211498,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 13377,
            "unit": "ns/insn",
            "extra": "median=6688500ns  p95=26496375ns  stddev=5893317.2ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 8703,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 74755,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 5723.83,
            "unit": "ns/insn",
            "extra": "median=2861917ns  p95=10933500ns  stddev=3134326.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 1049.83,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 174708,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 7964.83,
            "unit": "ns/insn",
            "extra": "median=3982416ns  p95=7205917ns  stddev=1368886.3ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 3290.83,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 125552,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5735.58,
            "unit": "ns/insn",
            "extra": "median=2867792ns  p95=3922958ns  stddev=428108.2ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 1061.58,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 174350,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 6877.83,
            "unit": "ns/insn",
            "extra": "median=3438916ns  p95=7297541ns  stddev=1303217.1ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 2203.83,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 145395,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 5847.33,
            "unit": "ns/insn",
            "extra": "median=2923666ns  p95=3844542ns  stddev=358063.2ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 1173.33,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 171018,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 5093.33,
            "unit": "ns/insn",
            "extra": "median=2546667ns  p95=3750292ns  stddev=382684.7ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 419.33,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 196335,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 7388.33,
            "unit": "ns/insn",
            "extra": "median=3694166ns  p95=5091916ns  stddev=780964.7ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 2714.33,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 135349,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 8505.25,
            "unit": "ns/insn",
            "extra": "median=4252625ns  p95=15635417ns  stddev=4313227.6ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 3831.25,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 117574,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 10021.5,
            "unit": "ns/insn",
            "extra": "median=5010750ns  p95=14673584ns  stddev=3307635.8ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 5347.5,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 99785,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 6343.83,
            "unit": "ns/insn",
            "extra": "median=3171917ns  p95=5541125ns  stddev=890993.4ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 1669.83,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 157633,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 6104.33,
            "unit": "ns/insn",
            "extra": "median=3052167ns  p95=4688084ns  stddev=727804.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 1430.33,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 163818,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 8888.33,
            "unit": "ns/insn",
            "extra": "median=4444167ns  p95=5397375ns  stddev=841513.3ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 4214.33,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 112507,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 7717.75,
            "unit": "ns/insn",
            "extra": "median=3858875ns  p95=4407167ns  stddev=516847.7ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 3043.75,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 129571,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 8129.08,
            "unit": "ns/insn",
            "extra": "median=4064542ns  p95=7123625ns  stddev=1125348.5ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 3455.08,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 123015,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 7522,
            "unit": "ns/insn",
            "extra": "median=3761000ns  p95=5770333ns  stddev=886270.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 2848,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 132943,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 9731.33,
            "unit": "ns/insn",
            "extra": "median=4865667ns  p95=6931750ns  stddev=1164617.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 5057.33,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 102761,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 5251,
            "unit": "ns/insn",
            "extra": "median=2625500ns  p95=17213500ns  stddev=4120820.7ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 577,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 190440,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 7760.92,
            "unit": "ns/insn",
            "extra": "median=3880459ns  p95=7184666ns  stddev=1484594.4ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 3086.92,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 128851,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 7275.75,
            "unit": "ns/insn",
            "extra": "median=3637875ns  p95=8908208ns  stddev=2037526.4ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 2601.75,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 137443,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 5156.5,
            "unit": "ns/insn",
            "extra": "median=2578250ns  p95=3101667ns  stddev=260989.4ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 482.5,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 193930,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 5287.08,
            "unit": "ns/insn",
            "extra": "median=2643542ns  p95=4339583ns  stddev=640708.0ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 613.08,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 189140,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 5677.42,
            "unit": "ns/insn",
            "extra": "median=2838708ns  p95=3268542ns  stddev=201280.6ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 1003.42,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 176136,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 6456.42,
            "unit": "ns/insn",
            "extra": "median=3228208ns  p95=6459792ns  stddev=1135291.8ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 1782.42,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 154885,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 9467.33,
            "unit": "ns/insn",
            "extra": "median=4733666ns  p95=28885083ns  stddev=7813940.0ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 4793.33,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 105626,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 31036.5,
            "unit": "ns/insn",
            "extra": "median=15518250ns  p95=37151375ns  stddev=6495706.5ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 26362.5,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 32220,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 33340.33,
            "unit": "ns/insn",
            "extra": "median=16670166ns  p95=20904583ns  stddev=2004001.1ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 28666.33,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 29994,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 7743.42,
            "unit": "ns/insn",
            "extra": "median=3871709ns  p95=5479583ns  stddev=928886.5ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 3069.42,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 129142,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 7154,
            "unit": "ns/insn",
            "extra": "median=3577000ns  p95=4971083ns  stddev=700675.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 2480,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 139782,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 5111.58,
            "unit": "ns/insn",
            "extra": "median=2555792ns  p95=3477292ns  stddev=290400.7ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 437.58,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 195634,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 5024.5,
            "unit": "ns/insn",
            "extra": "median=2512250ns  p95=3495125ns  stddev=286302.2ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 350.5,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 199025,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 8220,
            "unit": "ns/insn",
            "extra": "median=4110000ns  p95=8685792ns  stddev=1790557.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 3546,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 121655,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 5643.58,
            "unit": "ns/insn",
            "extra": "median=2821791ns  p95=7222875ns  stddev=1327210.9ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 969.58,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 177192,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 6240.92,
            "unit": "ns/insn",
            "extra": "median=3120459ns  p95=7113625ns  stddev=1324146.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 1566.92,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 160233,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 5613.08,
            "unit": "ns/insn",
            "extra": "median=2806542ns  p95=6050708ns  stddev=1168840.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 939.08,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 178155,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 6859.92,
            "unit": "ns/insn",
            "extra": "median=3429959ns  p95=15066333ns  stddev=3514365.2ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 2185.92,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 145774,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 8740.5,
            "unit": "ns/insn",
            "extra": "median=4370250ns  p95=8323583ns  stddev=1944160.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 4066.5,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 114410,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 7520.08,
            "unit": "ns/insn",
            "extra": "median=3760041ns  p95=10166542ns  stddev=2399959.2ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 2846.08,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 132977,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 5364.08,
            "unit": "ns/insn",
            "extra": "median=2682041ns  p95=7946791ns  stddev=2015872.5ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 690.08,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 186425,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5432.33,
            "unit": "ns/insn",
            "extra": "median=2716167ns  p95=6756500ns  stddev=1591257.3ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": 758.33,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 184083,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 7862.25,
            "unit": "ns/insn",
            "extra": "median=3931125ns  p95=8674000ns  stddev=1973358.2ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 3188.25,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 127190,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 6256.75,
            "unit": "ns/insn",
            "extra": "median=3128375ns  p95=6392292ns  stddev=1494176.1ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 1582.75,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 159827,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 5994.5,
            "unit": "ns/insn",
            "extra": "median=2997250ns  p95=5268041ns  stddev=976436.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 1320.5,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 166820,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 5531,
            "unit": "ns/insn",
            "extra": "median=2765500ns  p95=6469750ns  stddev=1130508.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 857,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 180799,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 4870.58,
            "unit": "ns/insn",
            "extra": "median=2435292ns  p95=2957666ns  stddev=170243.3ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 196.58,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 205314,
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
        "date": 1775162695051,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 5505.92,
            "unit": "ns/insn",
            "extra": "median=2752958ns  p95=5007500ns  stddev=821317.1ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 181623,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 5263.08,
            "unit": "ns/insn",
            "extra": "median=2631541ns  p95=3966792ns  stddev=423955.7ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": -242.83,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 190003,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 6151.5,
            "unit": "ns/insn",
            "extra": "median=3075750ns  p95=5260291ns  stddev=898301.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 645.58,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 162562,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 6344.17,
            "unit": "ns/insn",
            "extra": "median=3172083ns  p95=4427917ns  stddev=627292.0ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 838.25,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 157625,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 5991.5,
            "unit": "ns/insn",
            "extra": "median=2995750ns  p95=3331875ns  stddev=271026.7ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 485.58,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 166903,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 15502.75,
            "unit": "ns/insn",
            "extra": "median=7751375ns  p95=10988792ns  stddev=1622273.1ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 9996.83,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 64505,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 6521.25,
            "unit": "ns/insn",
            "extra": "median=3260625ns  p95=4981625ns  stddev=821092.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 1015.33,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 153345,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 6721.92,
            "unit": "ns/insn",
            "extra": "median=3360958ns  p95=5823167ns  stddev=947256.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 1216,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 148767,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 6224.75,
            "unit": "ns/insn",
            "extra": "median=3112375ns  p95=4660959ns  stddev=556242.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 718.83,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 160649,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 6908.67,
            "unit": "ns/insn",
            "extra": "median=3454333ns  p95=4614833ns  stddev=728744.3ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 1402.75,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 144746,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 5319.33,
            "unit": "ns/insn",
            "extra": "median=2659667ns  p95=8289750ns  stddev=2097221.4ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": -186.58,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 187993,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 5023.5,
            "unit": "ns/insn",
            "extra": "median=2511750ns  p95=7793875ns  stddev=1545471.5ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": -482.42,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 199064,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 4888.42,
            "unit": "ns/insn",
            "extra": "median=2444209ns  p95=7320708ns  stddev=1982864.2ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": -617.5,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 204565,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 4812.25,
            "unit": "ns/insn",
            "extra": "median=2406125ns  p95=2459583ns  stddev=25013.6ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": -693.67,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 207803,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 4788.5,
            "unit": "ns/insn",
            "extra": "median=2394250ns  p95=2777917ns  stddev=110245.0ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": -717.42,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 208834,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 5059.25,
            "unit": "ns/insn",
            "extra": "median=2529625ns  p95=5457667ns  stddev=879531.7ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": -446.67,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 197658,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 4900.58,
            "unit": "ns/insn",
            "extra": "median=2450292ns  p95=7386542ns  stddev=1461853.1ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": -605.33,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 204057,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 5217.42,
            "unit": "ns/insn",
            "extra": "median=2608708ns  p95=31455625ns  stddev=8166124.4ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": -288.5,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 191666,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 8009.17,
            "unit": "ns/insn",
            "extra": "median=4004583ns  p95=15120333ns  stddev=3835948.9ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 2503.25,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 124857,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 7186.5,
            "unit": "ns/insn",
            "extra": "median=3593250ns  p95=17685000ns  stddev=4712889.5ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 1680.58,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 139150,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 6998.17,
            "unit": "ns/insn",
            "extra": "median=3499083ns  p95=5277209ns  stddev=808737.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 1492.25,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 142895,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 5752.42,
            "unit": "ns/insn",
            "extra": "median=2876208ns  p95=4765708ns  stddev=713957.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 246.5,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 173840,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 6101.25,
            "unit": "ns/insn",
            "extra": "median=3050625ns  p95=4714667ns  stddev=659563.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 595.33,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 163901,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 6043.67,
            "unit": "ns/insn",
            "extra": "median=3021833ns  p95=5198292ns  stddev=779825.7ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 537.75,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 165462,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 7460.58,
            "unit": "ns/insn",
            "extra": "median=3730292ns  p95=5377208ns  stddev=837740.5ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 1954.67,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 134038,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 7387.67,
            "unit": "ns/insn",
            "extra": "median=3693834ns  p95=6754417ns  stddev=1130187.6ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 1881.75,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 135361,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 6314.58,
            "unit": "ns/insn",
            "extra": "median=3157291ns  p95=4895917ns  stddev=752929.2ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 808.67,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 158364,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 6379,
            "unit": "ns/insn",
            "extra": "median=3189500ns  p95=5633750ns  stddev=1050993.9ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 873.08,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 156764,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 5969.25,
            "unit": "ns/insn",
            "extra": "median=2984625ns  p95=3588083ns  stddev=317280.0ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 463.33,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 167525,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 6018.92,
            "unit": "ns/insn",
            "extra": "median=3009458ns  p95=4789792ns  stddev=670502.9ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 513,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 166143,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 38997.33,
            "unit": "ns/insn",
            "extra": "median=19498666ns  p95=27776292ns  stddev=3535054.7ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 33491.42,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 25643,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 39995,
            "unit": "ns/insn",
            "extra": "median=19997500ns  p95=24537250ns  stddev=3111793.0ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 34489.08,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 25003,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 4924.17,
            "unit": "ns/insn",
            "extra": "median=2462084ns  p95=5014500ns  stddev=869784.6ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -581.75,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 203080,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 4936.67,
            "unit": "ns/insn",
            "extra": "median=2468333ns  p95=7493708ns  stddev=1432754.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -569.25,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 202566,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 4871.42,
            "unit": "ns/insn",
            "extra": "median=2435708ns  p95=5279333ns  stddev=831187.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -634.5,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 205279,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 4879.42,
            "unit": "ns/insn",
            "extra": "median=2439708ns  p95=7544333ns  stddev=1467422.3ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -626.5,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 204943,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 7051.08,
            "unit": "ns/insn",
            "extra": "median=3525542ns  p95=4201208ns  stddev=486972.4ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 1545.17,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 141822,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 8693.17,
            "unit": "ns/insn",
            "extra": "median=4346583ns  p95=36951375ns  stddev=9562062.8ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 3187.25,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 115033,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 7191.92,
            "unit": "ns/insn",
            "extra": "median=3595958ns  p95=25781417ns  stddev=6466144.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 1686,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 139045,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 6555.33,
            "unit": "ns/insn",
            "extra": "median=3277666ns  p95=3989666ns  stddev=596645.2ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 1049.42,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 152548,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 6103.25,
            "unit": "ns/insn",
            "extra": "median=3051625ns  p95=4957000ns  stddev=671743.3ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 597.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 163847,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 8556.83,
            "unit": "ns/insn",
            "extra": "median=4278417ns  p95=5025125ns  stddev=959341.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 3050.92,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 116866,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 7556.58,
            "unit": "ns/insn",
            "extra": "median=3778291ns  p95=7016542ns  stddev=1056047.8ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 2050.67,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 132335,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 7395.08,
            "unit": "ns/insn",
            "extra": "median=3697542ns  p95=20654209ns  stddev=5784711.6ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 1889.17,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 135225,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 5831,
            "unit": "ns/insn",
            "extra": "median=2915500ns  p95=4427834ns  stddev=720696.9ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": 325.08,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 171497,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 7238.75,
            "unit": "ns/insn",
            "extra": "median=3619375ns  p95=17676375ns  stddev=4067682.4ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 1732.83,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 138145,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 5106.75,
            "unit": "ns/insn",
            "extra": "median=2553375ns  p95=3997458ns  stddev=548503.7ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -399.17,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 195819,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 6786.17,
            "unit": "ns/insn",
            "extra": "median=3393083ns  p95=4195958ns  stddev=554471.0ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 1280.25,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 147359,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 6983.33,
            "unit": "ns/insn",
            "extra": "median=3491666ns  p95=4176917ns  stddev=697091.6ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 1477.42,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 143198,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 6960.42,
            "unit": "ns/insn",
            "extra": "median=3480209ns  p95=4722417ns  stddev=602077.2ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 1454.5,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 143670,
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
        "date": 1775163825902,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 4708.5,
            "unit": "ns/insn",
            "extra": "median=2354250ns  p95=2403708ns  stddev=24760.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 212382,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 4769.5,
            "unit": "ns/insn",
            "extra": "median=2384750ns  p95=2461250ns  stddev=32564.9ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 61,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 209666,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 4853.92,
            "unit": "ns/insn",
            "extra": "median=2426958ns  p95=3089792ns  stddev=214524.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 145.42,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 206019,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 4901.92,
            "unit": "ns/insn",
            "extra": "median=2450959ns  p95=2700750ns  stddev=85103.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 193.42,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 204002,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 4835,
            "unit": "ns/insn",
            "extra": "median=2417500ns  p95=2656250ns  stddev=82905.2ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 126.5,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 206825,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 10135.42,
            "unit": "ns/insn",
            "extra": "median=5067708ns  p95=5771375ns  stddev=251095.3ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 5426.92,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 98664,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 4937.33,
            "unit": "ns/insn",
            "extra": "median=2468667ns  p95=2787583ns  stddev=153044.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 228.83,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 202538,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 4875.17,
            "unit": "ns/insn",
            "extra": "median=2437583ns  p95=2690834ns  stddev=92097.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 166.67,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 205121,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 5915.83,
            "unit": "ns/insn",
            "extra": "median=2957916ns  p95=6814250ns  stddev=1372420.2ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 1207.33,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 169038,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 5467.75,
            "unit": "ns/insn",
            "extra": "median=2733875ns  p95=3887250ns  stddev=384161.1ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 759.25,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 182891,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 5067,
            "unit": "ns/insn",
            "extra": "median=2533500ns  p95=3141833ns  stddev=201902.7ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 358.5,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 197355,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 4878.42,
            "unit": "ns/insn",
            "extra": "median=2439208ns  p95=2820042ns  stddev=149203.6ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 169.92,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 204985,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 4979.67,
            "unit": "ns/insn",
            "extra": "median=2489833ns  p95=4283209ns  stddev=572379.6ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 271.17,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 200817,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 5199.5,
            "unit": "ns/insn",
            "extra": "median=2599750ns  p95=3716375ns  stddev=434080.0ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 491,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 192326,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 4877.42,
            "unit": "ns/insn",
            "extra": "median=2438708ns  p95=2482458ns  stddev=28506.0ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 168.92,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 205027,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 4884.67,
            "unit": "ns/insn",
            "extra": "median=2442333ns  p95=2724500ns  stddev=116388.7ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 176.17,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 204722,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 4942.33,
            "unit": "ns/insn",
            "extra": "median=2471167ns  p95=3671958ns  stddev=371838.0ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 233.83,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 202334,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 4889.5,
            "unit": "ns/insn",
            "extra": "median=2444750ns  p95=2553834ns  stddev=45277.8ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 181,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 204520,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 4779.92,
            "unit": "ns/insn",
            "extra": "median=2389959ns  p95=2442833ns  stddev=19449.3ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 71.42,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 209209,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 4855.17,
            "unit": "ns/insn",
            "extra": "median=2427583ns  p95=2643958ns  stddev=68610.3ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 146.67,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 205966,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 4799.08,
            "unit": "ns/insn",
            "extra": "median=2399542ns  p95=2439083ns  stddev=16440.8ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": 90.58,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 208373,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 4850.25,
            "unit": "ns/insn",
            "extra": "median=2425125ns  p95=3312292ns  stddev=258201.5ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": 141.75,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 206175,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 5414.67,
            "unit": "ns/insn",
            "extra": "median=2707333ns  p95=17050208ns  stddev=4415363.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": 706.17,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 184684,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 4797.25,
            "unit": "ns/insn",
            "extra": "median=2398625ns  p95=17247750ns  stddev=4266269.0ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": 88.75,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 208453,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 4823.75,
            "unit": "ns/insn",
            "extra": "median=2411875ns  p95=2428667ns  stddev=12200.7ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 115.25,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 207308,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 4843.75,
            "unit": "ns/insn",
            "extra": "median=2421875ns  p95=2468167ns  stddev=21232.7ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 135.25,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 206452,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 4853.75,
            "unit": "ns/insn",
            "extra": "median=2426875ns  p95=2470875ns  stddev=25654.4ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 145.25,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 206026,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 4819.33,
            "unit": "ns/insn",
            "extra": "median=2409666ns  p95=2511041ns  stddev=38749.2ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 110.83,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 207498,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 4736.92,
            "unit": "ns/insn",
            "extra": "median=2368459ns  p95=2427750ns  stddev=26099.7ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 28.42,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 211108,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 4820.67,
            "unit": "ns/insn",
            "extra": "median=2410334ns  p95=2438542ns  stddev=15594.8ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 112.17,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 207440,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 29302.83,
            "unit": "ns/insn",
            "extra": "median=14651417ns  p95=14848917ns  stddev=128319.9ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": 24594.33,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 34126,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 29480.5,
            "unit": "ns/insn",
            "extra": "median=14740250ns  p95=17217958ns  stddev=786933.0ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": 24772,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 33921,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 4959.58,
            "unit": "ns/insn",
            "extra": "median=2479791ns  p95=2760000ns  stddev=88102.7ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 251.08,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 201630,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 4969.25,
            "unit": "ns/insn",
            "extra": "median=2484625ns  p95=2599083ns  stddev=57264.6ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 260.75,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 201238,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 4947.83,
            "unit": "ns/insn",
            "extra": "median=2473917ns  p95=2526542ns  stddev=41106.7ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 239.33,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 202109,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 4802.08,
            "unit": "ns/insn",
            "extra": "median=2401042ns  p95=2518250ns  stddev=41215.8ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 93.58,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 208243,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 4990.08,
            "unit": "ns/insn",
            "extra": "median=2495042ns  p95=3317458ns  stddev=237931.0ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 281.58,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 200397,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 4873.33,
            "unit": "ns/insn",
            "extra": "median=2436666ns  p95=2483167ns  stddev=28164.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 164.83,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 205198,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 4895.5,
            "unit": "ns/insn",
            "extra": "median=2447750ns  p95=2714625ns  stddev=81692.4ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 187,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 204269,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 4978.17,
            "unit": "ns/insn",
            "extra": "median=2489083ns  p95=2722208ns  stddev=83084.8ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": 269.67,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 200877,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 4933.67,
            "unit": "ns/insn",
            "extra": "median=2466834ns  p95=2998334ns  stddev=182295.6ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": 225.17,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 202689,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 4973.83,
            "unit": "ns/insn",
            "extra": "median=2486917ns  p95=3145250ns  stddev=198653.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": 265.33,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 201052,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 4950.58,
            "unit": "ns/insn",
            "extra": "median=2475292ns  p95=2787917ns  stddev=109598.2ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": 242.08,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 201996,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 4876.58,
            "unit": "ns/insn",
            "extra": "median=2438292ns  p95=2497250ns  stddev=25769.7ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": 168.08,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 205062,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 4770.5,
            "unit": "ns/insn",
            "extra": "median=2385250ns  p95=2483458ns  stddev=34126.5ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": 62,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 209622,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 4933.33,
            "unit": "ns/insn",
            "extra": "median=2466667ns  p95=2720625ns  stddev=82047.6ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": 224.83,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 202703,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 4811.75,
            "unit": "ns/insn",
            "extra": "median=2405875ns  p95=2619834ns  stddev=66532.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 103.25,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 207825,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 4831.42,
            "unit": "ns/insn",
            "extra": "median=2415708ns  p95=3137750ns  stddev=206826.3ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 122.92,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 206979,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 4775.83,
            "unit": "ns/insn",
            "extra": "median=2387916ns  p95=2464666ns  stddev=29386.3ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": 67.33,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 209388,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 4839,
            "unit": "ns/insn",
            "extra": "median=2419500ns  p95=2563167ns  stddev=60192.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": 130.5,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 206654,
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
        "date": 1775206137277,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 113494.08,
            "unit": "ns/insn",
            "extra": "median=56747042ns  p95=58323583ns  stddev=714045.3ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 8811,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 114851,
            "unit": "ns/insn",
            "extra": "median=57425500ns  p95=60233583ns  stddev=1059286.6ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 1356.92,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 8707,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 116858.08,
            "unit": "ns/insn",
            "extra": "median=58429042ns  p95=85102084ns  stddev=8054257.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 3364,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 8557,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 114340.17,
            "unit": "ns/insn",
            "extra": "median=57170084ns  p95=70747167ns  stddev=3883259.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 846.08,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 8746,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 117132.83,
            "unit": "ns/insn",
            "extra": "median=58566417ns  p95=80933042ns  stddev=6706506.7ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 3638.75,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 8537,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 395682.17,
            "unit": "ns/insn",
            "extra": "median=197841084ns  p95=273464000ns  stddev=23660699.4ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 282188.08,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 2527,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 115502.5,
            "unit": "ns/insn",
            "extra": "median=57751250ns  p95=66948334ns  stddev=2946488.3ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 2008.42,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 8658,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 114105.42,
            "unit": "ns/insn",
            "extra": "median=57052709ns  p95=82722792ns  stddev=9092252.3ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 611.33,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 8764,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 114228,
            "unit": "ns/insn",
            "extra": "median=57114000ns  p95=57662458ns  stddev=294525.3ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 733.92,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 8754,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 113718.42,
            "unit": "ns/insn",
            "extra": "median=56859208ns  p95=58045833ns  stddev=726005.6ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 224.33,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 8794,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 113788.08,
            "unit": "ns/insn",
            "extra": "median=56894042ns  p95=57205709ns  stddev=561693.1ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 294,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 8788,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 114419.33,
            "unit": "ns/insn",
            "extra": "median=57209666ns  p95=58141000ns  stddev=704295.9ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 925.25,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 8740,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 114206.58,
            "unit": "ns/insn",
            "extra": "median=57103291ns  p95=72610208ns  stddev=4471308.1ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 712.5,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 8756,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 114532,
            "unit": "ns/insn",
            "extra": "median=57266000ns  p95=64551583ns  stddev=2377539.0ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 1037.92,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 8731,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 114042.75,
            "unit": "ns/insn",
            "extra": "median=57021375ns  p95=69501709ns  stddev=3715536.8ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 548.67,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 8769,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 113491.25,
            "unit": "ns/insn",
            "extra": "median=56745625ns  p95=60413250ns  stddev=1402861.9ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": -2.83,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 8811,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 115460.83,
            "unit": "ns/insn",
            "extra": "median=57730416ns  p95=61502167ns  stddev=1499100.3ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 1966.75,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 8661,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 115846.75,
            "unit": "ns/insn",
            "extra": "median=57923375ns  p95=77766042ns  stddev=5725801.1ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": 2352.67,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 8632,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 112707.42,
            "unit": "ns/insn",
            "extra": "median=56353708ns  p95=66100167ns  stddev=3863322.9ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": -786.67,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 8873,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 118025.08,
            "unit": "ns/insn",
            "extra": "median=59012541ns  p95=69357375ns  stddev=3928568.2ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 4531,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 8473,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 112219,
            "unit": "ns/insn",
            "extra": "median=56109500ns  p95=63203208ns  stddev=2420406.1ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": -1275.08,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 8911,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 111875.58,
            "unit": "ns/insn",
            "extra": "median=55937792ns  p95=56565416ns  stddev=348646.4ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": -1618.5,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 8939,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 113143.58,
            "unit": "ns/insn",
            "extra": "median=56571792ns  p95=61058083ns  stddev=1655435.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": -350.5,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 8838,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 111823.17,
            "unit": "ns/insn",
            "extra": "median=55911583ns  p95=78666333ns  stddev=6505968.5ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": -1670.92,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 8943,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 115541.83,
            "unit": "ns/insn",
            "extra": "median=57770916ns  p95=62630125ns  stddev=2205604.4ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": 2047.75,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 8655,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 116047.08,
            "unit": "ns/insn",
            "extra": "median=58023542ns  p95=59001833ns  stddev=399070.0ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 2553,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 8617,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 113833.67,
            "unit": "ns/insn",
            "extra": "median=56916834ns  p95=71312792ns  stddev=5259261.5ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 339.58,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 8785,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 114301.75,
            "unit": "ns/insn",
            "extra": "median=57150875ns  p95=57698917ns  stddev=504252.9ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 807.67,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 8749,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 112814.92,
            "unit": "ns/insn",
            "extra": "median=56407458ns  p95=58340041ns  stddev=850280.6ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -679.17,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 8864,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 114327.92,
            "unit": "ns/insn",
            "extra": "median=57163958ns  p95=81420041ns  stddev=6991960.8ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": 833.83,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 8747,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 61288.83,
            "unit": "ns/insn",
            "extra": "median=30644416ns  p95=30911834ns  stddev=179390.4ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -52205.25,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 16316,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 60953.5,
            "unit": "ns/insn",
            "extra": "median=30476750ns  p95=30959375ns  stddev=154022.0ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -52540.58,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 16406,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 111339,
            "unit": "ns/insn",
            "extra": "median=55669500ns  p95=57804375ns  stddev=861887.0ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -2155.08,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 8982,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 114490.92,
            "unit": "ns/insn",
            "extra": "median=57245458ns  p95=65785792ns  stddev=3338162.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 996.83,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 8734,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 125226.42,
            "unit": "ns/insn",
            "extra": "median=62613208ns  p95=110191833ns  stddev=15865968.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 11732.33,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 7986,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 127691.58,
            "unit": "ns/insn",
            "extra": "median=63845791ns  p95=136342792ns  stddev=22039167.1ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": 14197.5,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 7831,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 114973.83,
            "unit": "ns/insn",
            "extra": "median=57486916ns  p95=88460959ns  stddev=9399725.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": 1479.75,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 8698,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 149429.25,
            "unit": "ns/insn",
            "extra": "median=74714625ns  p95=189583125ns  stddev=34641380.2ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 35935.17,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 6692,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 114193.92,
            "unit": "ns/insn",
            "extra": "median=57096958ns  p95=66255500ns  stddev=3049047.5ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": 699.83,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 8757,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 111578.67,
            "unit": "ns/insn",
            "extra": "median=55789333ns  p95=63784833ns  stddev=2296249.1ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -1915.42,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 8962,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 111865.75,
            "unit": "ns/insn",
            "extra": "median=55932875ns  p95=56344125ns  stddev=786360.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -1628.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 8939,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 112136,
            "unit": "ns/insn",
            "extra": "median=56068000ns  p95=56462167ns  stddev=891316.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -1358.08,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 8918,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 112138.5,
            "unit": "ns/insn",
            "extra": "median=56069250ns  p95=56704417ns  stddev=801023.7ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": -1355.58,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 8918,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 112073.83,
            "unit": "ns/insn",
            "extra": "median=56036917ns  p95=56440042ns  stddev=958990.8ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -1420.25,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 8923,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 111616.08,
            "unit": "ns/insn",
            "extra": "median=55808041ns  p95=56067292ns  stddev=694880.6ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -1878,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 8959,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 112148.75,
            "unit": "ns/insn",
            "extra": "median=56074375ns  p95=56485666ns  stddev=461509.7ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -1345.33,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 8917,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 112007.58,
            "unit": "ns/insn",
            "extra": "median=56003792ns  p95=57253125ns  stddev=876644.2ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -1486.5,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 8928,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 113887.33,
            "unit": "ns/insn",
            "extra": "median=56943667ns  p95=58736792ns  stddev=883842.7ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": 393.25,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 8781,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 111048.42,
            "unit": "ns/insn",
            "extra": "median=55524209ns  p95=55988667ns  stddev=477080.1ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -2445.67,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 9005,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 110891.42,
            "unit": "ns/insn",
            "extra": "median=55445709ns  p95=63044708ns  stddev=2235930.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -2602.67,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 9018,
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
        "date": 1775222075214,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 114147,
            "unit": "ns/insn",
            "extra": "median=57073500ns  p95=57945709ns  stddev=676059.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 8761,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 113941.33,
            "unit": "ns/insn",
            "extra": "median=56970667ns  p95=87474042ns  stddev=8721736.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": -205.67,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 8776,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 114262.58,
            "unit": "ns/insn",
            "extra": "median=57131291ns  p95=65708125ns  stddev=2606230.0ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 115.58,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 8752,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 114558.42,
            "unit": "ns/insn",
            "extra": "median=57279209ns  p95=81769292ns  stddev=7200409.8ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": 411.42,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 8729,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 116056.42,
            "unit": "ns/insn",
            "extra": "median=58028208ns  p95=76483334ns  stddev=5391188.8ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": 1909.42,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 8616,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 382063.08,
            "unit": "ns/insn",
            "extra": "median=191031541ns  p95=223748208ns  stddev=11176361.1ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 267916.08,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 2617,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 124296.42,
            "unit": "ns/insn",
            "extra": "median=62148208ns  p95=67425583ns  stddev=3786799.8ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 10149.42,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 8045,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 116247.75,
            "unit": "ns/insn",
            "extra": "median=58123875ns  p95=83456542ns  stddev=8136077.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": 2100.75,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 8602,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 118779.83,
            "unit": "ns/insn",
            "extra": "median=59389917ns  p95=81779584ns  stddev=7109716.8ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 4632.83,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 8419,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 115015.33,
            "unit": "ns/insn",
            "extra": "median=57507667ns  p95=60004292ns  stddev=1594306.9ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": 868.33,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 8694,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 114564.75,
            "unit": "ns/insn",
            "extra": "median=57282375ns  p95=84860667ns  stddev=7899216.3ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": 417.75,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 8729,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 114148.5,
            "unit": "ns/insn",
            "extra": "median=57074250ns  p95=57944250ns  stddev=811890.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 1.5,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 8761,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 114279.83,
            "unit": "ns/insn",
            "extra": "median=57139917ns  p95=77782958ns  stddev=5982522.7ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": 132.83,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 8750,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 113936.67,
            "unit": "ns/insn",
            "extra": "median=56968333ns  p95=66425291ns  stddev=3037467.5ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": -210.33,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 8777,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 114781.58,
            "unit": "ns/insn",
            "extra": "median=57390792ns  p95=80071416ns  stddev=6571848.8ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 634.58,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 8712,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 114469.92,
            "unit": "ns/insn",
            "extra": "median=57234958ns  p95=58722583ns  stddev=1096855.3ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 322.92,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 8736,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 114125.17,
            "unit": "ns/insn",
            "extra": "median=57062584ns  p95=77404667ns  stddev=5865720.9ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": -21.83,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 8762,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 114053.25,
            "unit": "ns/insn",
            "extra": "median=57026625ns  p95=57279875ns  stddev=164473.9ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": -93.75,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 8768,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 114254.75,
            "unit": "ns/insn",
            "extra": "median=57127375ns  p95=58401667ns  stddev=657712.2ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 107.75,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 8752,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 132410.42,
            "unit": "ns/insn",
            "extra": "median=66205209ns  p95=90597708ns  stddev=9641433.1ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": 18263.42,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 7552,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 112880.83,
            "unit": "ns/insn",
            "extra": "median=56440416ns  p95=66504750ns  stddev=3234115.3ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": -1266.17,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 8859,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 111825.92,
            "unit": "ns/insn",
            "extra": "median=55912959ns  p95=57244959ns  stddev=459941.5ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": -2321.08,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 8942,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 112151.17,
            "unit": "ns/insn",
            "extra": "median=56075584ns  p95=58212167ns  stddev=812800.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": -1995.83,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 8917,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 111364.75,
            "unit": "ns/insn",
            "extra": "median=55682375ns  p95=56107791ns  stddev=217233.8ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": -2782.25,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 8980,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 111650.08,
            "unit": "ns/insn",
            "extra": "median=55825041ns  p95=59278750ns  stddev=1103709.1ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": -2496.92,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 8957,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 125871.75,
            "unit": "ns/insn",
            "extra": "median=62935875ns  p95=101464125ns  stddev=11961853.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": 11724.75,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 7945,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 115612.83,
            "unit": "ns/insn",
            "extra": "median=57806417ns  p95=90837750ns  stddev=9482484.3ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": 1465.83,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 8650,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 114564,
            "unit": "ns/insn",
            "extra": "median=57282000ns  p95=58229375ns  stddev=518686.1ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": 417,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 8729,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 112680.92,
            "unit": "ns/insn",
            "extra": "median=56340458ns  p95=78032917ns  stddev=6277172.5ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -1466.08,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 8875,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 114110.83,
            "unit": "ns/insn",
            "extra": "median=57055417ns  p95=58542459ns  stddev=851976.5ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -36.17,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 8763,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 60973,
            "unit": "ns/insn",
            "extra": "median=30486500ns  p95=30911209ns  stddev=206326.6ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -53174,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 16401,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 61185.33,
            "unit": "ns/insn",
            "extra": "median=30592666ns  p95=30707625ns  stddev=167713.4ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -52961.67,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 16344,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 114253,
            "unit": "ns/insn",
            "extra": "median=57126500ns  p95=57986209ns  stddev=604065.8ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": 106,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 8753,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 114133,
            "unit": "ns/insn",
            "extra": "median=57066500ns  p95=57972959ns  stddev=1040512.6ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -14,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 8762,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 118803,
            "unit": "ns/insn",
            "extra": "median=59401500ns  p95=78588375ns  stddev=6417447.3ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": 4656,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 8417,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 112760.17,
            "unit": "ns/insn",
            "extra": "median=56380083ns  p95=107634459ns  stddev=14627946.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -1386.83,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 8868,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 112966.17,
            "unit": "ns/insn",
            "extra": "median=56483083ns  p95=66477375ns  stddev=2974787.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -1180.83,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 8852,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 115653.75,
            "unit": "ns/insn",
            "extra": "median=57826875ns  p95=64439333ns  stddev=2352897.3ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": 1506.75,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 8646,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 113984.17,
            "unit": "ns/insn",
            "extra": "median=56992083ns  p95=80872334ns  stddev=6968078.1ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -162.83,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 8773,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 111520.67,
            "unit": "ns/insn",
            "extra": "median=55760333ns  p95=78250959ns  stddev=6743833.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -2626.33,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 8967,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 111633.75,
            "unit": "ns/insn",
            "extra": "median=55816875ns  p95=56481250ns  stddev=861491.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -2513.25,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 8958,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 111957.25,
            "unit": "ns/insn",
            "extra": "median=55978625ns  p95=56074792ns  stddev=770722.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -2189.75,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 8932,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 111762.83,
            "unit": "ns/insn",
            "extra": "median=55881417ns  p95=56070000ns  stddev=802332.1ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": -2384.17,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 8948,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 112162.92,
            "unit": "ns/insn",
            "extra": "median=56081458ns  p95=56247750ns  stddev=862087.6ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -1984.08,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 8916,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 111336.42,
            "unit": "ns/insn",
            "extra": "median=55668209ns  p95=57623292ns  stddev=1073839.4ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -2810.58,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 8982,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 111945.75,
            "unit": "ns/insn",
            "extra": "median=55972875ns  p95=56450583ns  stddev=897920.8ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -2201.25,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 8933,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 111282.5,
            "unit": "ns/insn",
            "extra": "median=55641250ns  p95=56144584ns  stddev=781906.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -2864.5,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 8986,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 111303.67,
            "unit": "ns/insn",
            "extra": "median=55651833ns  p95=55904375ns  stddev=801775.9ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -2843.33,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 8984,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 111147.75,
            "unit": "ns/insn",
            "extra": "median=55573875ns  p95=56580333ns  stddev=876528.3ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -2999.25,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 8997,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 111506.75,
            "unit": "ns/insn",
            "extra": "median=55753375ns  p95=56514292ns  stddev=770338.0ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -2640.25,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 8968,
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
        "date": 1775222340629,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 121552.58,
            "unit": "ns/insn",
            "extra": "median=60776291ns  p95=65473000ns  stddev=2851624.9ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 8227,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 123380.17,
            "unit": "ns/insn",
            "extra": "median=61690084ns  p95=83824000ns  stddev=7279142.5ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": 1827.59,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 8105,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 124246.17,
            "unit": "ns/insn",
            "extra": "median=62123083ns  p95=73425625ns  stddev=5851236.9ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": 2693.58,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 8049,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 120469.75,
            "unit": "ns/insn",
            "extra": "median=60234875ns  p95=80273292ns  stddev=9125823.1ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": -1082.83,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 8301,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 116843.92,
            "unit": "ns/insn",
            "extra": "median=58421958ns  p95=78482917ns  stddev=8356177.2ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": -4708.67,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 8558,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 419179.5,
            "unit": "ns/insn",
            "extra": "median=209589750ns  p95=234037542ns  stddev=12278081.3ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 297626.92,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 2386,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 129922.92,
            "unit": "ns/insn",
            "extra": "median=64961459ns  p95=94047708ns  stddev=10411635.5ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": 8370.34,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 7697,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 120353.67,
            "unit": "ns/insn",
            "extra": "median=60176834ns  p95=94728792ns  stddev=10868467.3ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -1198.91,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 8309,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 129157.58,
            "unit": "ns/insn",
            "extra": "median=64578791ns  p95=116024375ns  stddev=17033965.3ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": 7605,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 7742,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 118404.5,
            "unit": "ns/insn",
            "extra": "median=59202250ns  p95=112221042ns  stddev=16393447.9ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": -3148.08,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 8446,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 116570.67,
            "unit": "ns/insn",
            "extra": "median=58285334ns  p95=94189875ns  stddev=11888575.6ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": -4981.91,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 8578,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 121821.33,
            "unit": "ns/insn",
            "extra": "median=60910667ns  p95=69547625ns  stddev=4831578.8ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": 268.75,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 8209,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 117947.08,
            "unit": "ns/insn",
            "extra": "median=58973542ns  p95=76942208ns  stddev=6770044.9ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": -3605.5,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 8478,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 127003.17,
            "unit": "ns/insn",
            "extra": "median=63501583ns  p95=88115750ns  stddev=8615888.5ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": 5450.58,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 7874,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 131072.5,
            "unit": "ns/insn",
            "extra": "median=65536250ns  p95=80206500ns  stddev=7850831.8ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": 9519.92,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 7629,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 114303.67,
            "unit": "ns/insn",
            "extra": "median=57151834ns  p95=96676666ns  stddev=11512318.8ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": -7248.91,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 8749,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 125615,
            "unit": "ns/insn",
            "extra": "median=62807500ns  p95=92948167ns  stddev=11581896.2ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": 4062.42,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 7961,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 113637.5,
            "unit": "ns/insn",
            "extra": "median=56818750ns  p95=65147875ns  stddev=2435362.5ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": -7915.08,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 8800,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 114264,
            "unit": "ns/insn",
            "extra": "median=57132000ns  p95=60133584ns  stddev=972589.9ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": -7288.58,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 8752,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 114501.42,
            "unit": "ns/insn",
            "extra": "median=57250709ns  p95=93516416ns  stddev=11824140.5ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": -7051.16,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 8734,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 114908.5,
            "unit": "ns/insn",
            "extra": "median=57454250ns  p95=74970250ns  stddev=6044442.5ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": -6644.08,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 8703,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 112847.08,
            "unit": "ns/insn",
            "extra": "median=56423542ns  p95=61460917ns  stddev=1849558.1ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": -8705.5,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 8862,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 111431.92,
            "unit": "ns/insn",
            "extra": "median=55715959ns  p95=67344333ns  stddev=4105439.2ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": -10120.66,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 8974,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 111327.08,
            "unit": "ns/insn",
            "extra": "median=55663542ns  p95=64404625ns  stddev=3025530.9ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": -10225.5,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 8983,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 114030.58,
            "unit": "ns/insn",
            "extra": "median=57015292ns  p95=75657166ns  stddev=7901663.9ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": -7522,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 8770,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 111878.5,
            "unit": "ns/insn",
            "extra": "median=55939250ns  p95=63078000ns  stddev=3046258.8ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": -9674.08,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 8938,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 114055.92,
            "unit": "ns/insn",
            "extra": "median=57027958ns  p95=88980417ns  stddev=9701119.8ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": -7496.67,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 8768,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 115038.58,
            "unit": "ns/insn",
            "extra": "median=57519292ns  p95=84329459ns  stddev=8437034.6ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": -6514,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 8693,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 130784,
            "unit": "ns/insn",
            "extra": "median=65392000ns  p95=97910583ns  stddev=11344044.9ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": 9231.42,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 7646,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 117046.5,
            "unit": "ns/insn",
            "extra": "median=58523250ns  p95=99221834ns  stddev=13961143.7ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -4506.08,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 8544,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 60137.33,
            "unit": "ns/insn",
            "extra": "median=30068667ns  p95=69627916ns  stddev=12435776.4ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -61415.25,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 16629,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 62101,
            "unit": "ns/insn",
            "extra": "median=31050500ns  p95=48656750ns  stddev=5217452.8ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -59451.58,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 16103,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 113538.58,
            "unit": "ns/insn",
            "extra": "median=56769292ns  p95=73490875ns  stddev=5328575.5ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -8014,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 8808,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 122454,
            "unit": "ns/insn",
            "extra": "median=61227000ns  p95=79236875ns  stddev=8203244.6ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": 901.42,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 8166,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 114122.67,
            "unit": "ns/insn",
            "extra": "median=57061334ns  p95=79934875ns  stddev=9295714.6ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -7429.91,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 8763,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 116309.58,
            "unit": "ns/insn",
            "extra": "median=58154792ns  p95=87575125ns  stddev=9864894.1ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -5243,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 8598,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 116289.17,
            "unit": "ns/insn",
            "extra": "median=58144583ns  p95=80132000ns  stddev=8180330.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -5263.42,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 8599,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 111095.83,
            "unit": "ns/insn",
            "extra": "median=55547916ns  p95=76915917ns  stddev=6611544.3ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -10456.75,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 9001,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 107279.75,
            "unit": "ns/insn",
            "extra": "median=53639875ns  p95=77556375ns  stddev=7369739.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -14272.83,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 9321,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 114502.08,
            "unit": "ns/insn",
            "extra": "median=57251042ns  p95=94076209ns  stddev=13367942.9ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -7050.5,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 8733,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 115003,
            "unit": "ns/insn",
            "extra": "median=57501500ns  p95=90389583ns  stddev=10665268.0ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -6549.58,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 8695,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 116473.33,
            "unit": "ns/insn",
            "extra": "median=58236667ns  p95=78602125ns  stddev=7959456.1ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -5079.25,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 8586,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 115666,
            "unit": "ns/insn",
            "extra": "median=57833000ns  p95=67068958ns  stddev=4817267.4ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": -5886.58,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 8646,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 104693.83,
            "unit": "ns/insn",
            "extra": "median=52346916ns  p95=57159417ns  stddev=1558745.1ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -16858.75,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 9552,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 103617.58,
            "unit": "ns/insn",
            "extra": "median=51808792ns  p95=55610959ns  stddev=1346125.5ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -17935,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 9651,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 105205.75,
            "unit": "ns/insn",
            "extra": "median=52602875ns  p95=59436500ns  stddev=2386457.4ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -16346.83,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 9505,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 108754.92,
            "unit": "ns/insn",
            "extra": "median=54377458ns  p95=56643500ns  stddev=1752024.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -12797.67,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 9195,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 110764,
            "unit": "ns/insn",
            "extra": "median=55382000ns  p95=58066333ns  stddev=1376558.1ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -10788.58,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 9028,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 111578.33,
            "unit": "ns/insn",
            "extra": "median=55789167ns  p95=57183958ns  stddev=1439716.4ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -9974.25,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 8962,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 111631.83,
            "unit": "ns/insn",
            "extra": "median=55815916ns  p95=65734292ns  stddev=3413493.4ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -9920.75,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 8958,
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
        "date": 1775222476935,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "NOP (total)",
            "value": 159646.67,
            "unit": "ns/insn",
            "extra": "median=79823333ns  p95=94071458ns  stddev=11727810.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": 0,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 6264,
            "unit": "insn/s"
          },
          {
            "name": "MOVE (total)",
            "value": 117733.75,
            "unit": "ns/insn",
            "extra": "median=58866875ns  p95=93925792ns  stddev=10919313.8ns  iterations=11  insns=500"
          },
          {
            "name": "MOVE (handler)",
            "value": -41912.92,
            "unit": "ns"
          },
          {
            "name": "MOVE (IPS)",
            "value": 8494,
            "unit": "insn/s"
          },
          {
            "name": "LOAD (total)",
            "value": 118468.17,
            "unit": "ns/insn",
            "extra": "median=59234084ns  p95=82416583ns  stddev=8809562.2ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD (handler)",
            "value": -41178.5,
            "unit": "ns"
          },
          {
            "name": "LOAD (IPS)",
            "value": 8441,
            "unit": "insn/s"
          },
          {
            "name": "STORE (total)",
            "value": 112762.67,
            "unit": "ns/insn",
            "extra": "median=56381334ns  p95=73624083ns  stddev=6606536.8ns  iterations=11  insns=500"
          },
          {
            "name": "STORE (handler)",
            "value": -46884,
            "unit": "ns"
          },
          {
            "name": "STORE (IPS)",
            "value": 8868,
            "unit": "insn/s"
          },
          {
            "name": "PUSH (total)",
            "value": 115664.08,
            "unit": "ns/insn",
            "extra": "median=57832041ns  p95=67822667ns  stddev=4076096.5ns  iterations=11  insns=500"
          },
          {
            "name": "PUSH (handler)",
            "value": -43982.58,
            "unit": "ns"
          },
          {
            "name": "PUSH (IPS)",
            "value": 8646,
            "unit": "insn/s"
          },
          {
            "name": "POP (total)",
            "value": 404891.67,
            "unit": "ns/insn",
            "extra": "median=202445834ns  p95=251143500ns  stddev=19751596.7ns  iterations=11  insns=500"
          },
          {
            "name": "POP (handler)",
            "value": 245245,
            "unit": "ns"
          },
          {
            "name": "POP (IPS)",
            "value": 2470,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CONST (total)",
            "value": 119965.25,
            "unit": "ns/insn",
            "extra": "median=59982625ns  p95=90497291ns  stddev=10982035.6ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CONST (handler)",
            "value": -39681.42,
            "unit": "ns"
          },
          {
            "name": "LOAD_CONST (IPS)",
            "value": 8336,
            "unit": "insn/s"
          },
          {
            "name": "LOAD_CTX (total)",
            "value": 112092.67,
            "unit": "ns/insn",
            "extra": "median=56046333ns  p95=80272250ns  stddev=7197194.7ns  iterations=11  insns=500"
          },
          {
            "name": "LOAD_CTX (handler)",
            "value": -47554,
            "unit": "ns"
          },
          {
            "name": "LOAD_CTX (IPS)",
            "value": 8921,
            "unit": "insn/s"
          },
          {
            "name": "STORE_CTX (total)",
            "value": 125799.25,
            "unit": "ns/insn",
            "extra": "median=62899625ns  p95=104760958ns  stddev=15485927.0ns  iterations=11  insns=500"
          },
          {
            "name": "STORE_CTX (handler)",
            "value": -33847.42,
            "unit": "ns"
          },
          {
            "name": "STORE_CTX (IPS)",
            "value": 7949,
            "unit": "insn/s"
          },
          {
            "name": "ADD (total)",
            "value": 146861.17,
            "unit": "ns/insn",
            "extra": "median=73430583ns  p95=127938125ns  stddev=18561629.5ns  iterations=11  insns=500"
          },
          {
            "name": "ADD (handler)",
            "value": -12785.5,
            "unit": "ns"
          },
          {
            "name": "ADD (IPS)",
            "value": 6809,
            "unit": "insn/s"
          },
          {
            "name": "SUB (total)",
            "value": 139145.67,
            "unit": "ns/insn",
            "extra": "median=69572833ns  p95=78125541ns  stddev=5920716.7ns  iterations=11  insns=500"
          },
          {
            "name": "SUB (handler)",
            "value": -20501,
            "unit": "ns"
          },
          {
            "name": "SUB (IPS)",
            "value": 7187,
            "unit": "insn/s"
          },
          {
            "name": "MUL (total)",
            "value": 127733.58,
            "unit": "ns/insn",
            "extra": "median=63866792ns  p95=80606458ns  stddev=7683952.3ns  iterations=11  insns=500"
          },
          {
            "name": "MUL (handler)",
            "value": -31913.08,
            "unit": "ns"
          },
          {
            "name": "MUL (IPS)",
            "value": 7829,
            "unit": "insn/s"
          },
          {
            "name": "IMUL (total)",
            "value": 123449.5,
            "unit": "ns/insn",
            "extra": "median=61724750ns  p95=82784584ns  stddev=8291106.4ns  iterations=11  insns=500"
          },
          {
            "name": "IMUL (handler)",
            "value": -36197.17,
            "unit": "ns"
          },
          {
            "name": "IMUL (IPS)",
            "value": 8100,
            "unit": "insn/s"
          },
          {
            "name": "DIV (total)",
            "value": 113682.75,
            "unit": "ns/insn",
            "extra": "median=56841375ns  p95=89868375ns  stddev=10435154.1ns  iterations=11  insns=500"
          },
          {
            "name": "DIV (handler)",
            "value": -45963.92,
            "unit": "ns"
          },
          {
            "name": "DIV (IPS)",
            "value": 8796,
            "unit": "insn/s"
          },
          {
            "name": "IDIV (total)",
            "value": 122418.17,
            "unit": "ns/insn",
            "extra": "median=61209084ns  p95=92162083ns  stddev=11667155.0ns  iterations=11  insns=500"
          },
          {
            "name": "IDIV (handler)",
            "value": -37228.5,
            "unit": "ns"
          },
          {
            "name": "IDIV (IPS)",
            "value": 8169,
            "unit": "insn/s"
          },
          {
            "name": "NEG (total)",
            "value": 160030,
            "unit": "ns/insn",
            "extra": "median=80015000ns  p95=91080084ns  stddev=8211520.8ns  iterations=11  insns=500"
          },
          {
            "name": "NEG (handler)",
            "value": 383.33,
            "unit": "ns"
          },
          {
            "name": "NEG (IPS)",
            "value": 6249,
            "unit": "insn/s"
          },
          {
            "name": "MOD (total)",
            "value": 137034.92,
            "unit": "ns/insn",
            "extra": "median=68517458ns  p95=84562958ns  stddev=10304786.4ns  iterations=11  insns=500"
          },
          {
            "name": "MOD (handler)",
            "value": -22611.75,
            "unit": "ns"
          },
          {
            "name": "MOD (IPS)",
            "value": 7297,
            "unit": "insn/s"
          },
          {
            "name": "AND (total)",
            "value": 125640.17,
            "unit": "ns/insn",
            "extra": "median=62820083ns  p95=82389292ns  stddev=8111423.6ns  iterations=11  insns=500"
          },
          {
            "name": "AND (handler)",
            "value": -34006.5,
            "unit": "ns"
          },
          {
            "name": "AND (IPS)",
            "value": 7959,
            "unit": "insn/s"
          },
          {
            "name": "OR (total)",
            "value": 197427,
            "unit": "ns/insn",
            "extra": "median=98713500ns  p95=119896667ns  stddev=11143361.1ns  iterations=11  insns=500"
          },
          {
            "name": "OR (handler)",
            "value": 37780.33,
            "unit": "ns"
          },
          {
            "name": "OR (IPS)",
            "value": 5065,
            "unit": "insn/s"
          },
          {
            "name": "XOR (total)",
            "value": 145230.83,
            "unit": "ns/insn",
            "extra": "median=72615416ns  p95=89028250ns  stddev=7896791.1ns  iterations=11  insns=500"
          },
          {
            "name": "XOR (handler)",
            "value": -14415.83,
            "unit": "ns"
          },
          {
            "name": "XOR (IPS)",
            "value": 6886,
            "unit": "insn/s"
          },
          {
            "name": "NOT (total)",
            "value": 145178.33,
            "unit": "ns/insn",
            "extra": "median=72589166ns  p95=95198750ns  stddev=10380156.1ns  iterations=11  insns=500"
          },
          {
            "name": "NOT (handler)",
            "value": -14468.33,
            "unit": "ns"
          },
          {
            "name": "NOT (IPS)",
            "value": 6888,
            "unit": "insn/s"
          },
          {
            "name": "SHL (total)",
            "value": 119757.75,
            "unit": "ns/insn",
            "extra": "median=59878875ns  p95=70632500ns  stddev=5710033.8ns  iterations=11  insns=500"
          },
          {
            "name": "SHL (handler)",
            "value": -39888.92,
            "unit": "ns"
          },
          {
            "name": "SHL (IPS)",
            "value": 8350,
            "unit": "insn/s"
          },
          {
            "name": "SHR (total)",
            "value": 135397.67,
            "unit": "ns/insn",
            "extra": "median=67698833ns  p95=89035375ns  stddev=9686111.0ns  iterations=11  insns=500"
          },
          {
            "name": "SHR (handler)",
            "value": -24249,
            "unit": "ns"
          },
          {
            "name": "SHR (IPS)",
            "value": 7386,
            "unit": "insn/s"
          },
          {
            "name": "SAR (total)",
            "value": 124028.58,
            "unit": "ns/insn",
            "extra": "median=62014292ns  p95=91680958ns  stddev=11638868.3ns  iterations=11  insns=500"
          },
          {
            "name": "SAR (handler)",
            "value": -35618.08,
            "unit": "ns"
          },
          {
            "name": "SAR (IPS)",
            "value": 8063,
            "unit": "insn/s"
          },
          {
            "name": "ROL (total)",
            "value": 113622.33,
            "unit": "ns/insn",
            "extra": "median=56811166ns  p95=75754917ns  stddev=6399367.0ns  iterations=11  insns=500"
          },
          {
            "name": "ROL (handler)",
            "value": -46024.33,
            "unit": "ns"
          },
          {
            "name": "ROL (IPS)",
            "value": 8801,
            "unit": "insn/s"
          },
          {
            "name": "ROR (total)",
            "value": 116207.75,
            "unit": "ns/insn",
            "extra": "median=58103875ns  p95=72865667ns  stddev=6832164.5ns  iterations=11  insns=500"
          },
          {
            "name": "ROR (handler)",
            "value": -43438.92,
            "unit": "ns"
          },
          {
            "name": "ROR (IPS)",
            "value": 8605,
            "unit": "insn/s"
          },
          {
            "name": "CMP (total)",
            "value": 112519.75,
            "unit": "ns/insn",
            "extra": "median=56259875ns  p95=80701250ns  stddev=7259867.2ns  iterations=11  insns=500"
          },
          {
            "name": "CMP (handler)",
            "value": -47126.92,
            "unit": "ns"
          },
          {
            "name": "CMP (IPS)",
            "value": 8887,
            "unit": "insn/s"
          },
          {
            "name": "TEST (total)",
            "value": 117830.42,
            "unit": "ns/insn",
            "extra": "median=58915209ns  p95=83075584ns  stddev=7774822.6ns  iterations=11  insns=500"
          },
          {
            "name": "TEST (handler)",
            "value": -41816.25,
            "unit": "ns"
          },
          {
            "name": "TEST (IPS)",
            "value": 8487,
            "unit": "insn/s"
          },
          {
            "name": "SET_FLAG (total)",
            "value": 125557.58,
            "unit": "ns/insn",
            "extra": "median=62778792ns  p95=101348167ns  stddev=12082907.5ns  iterations=11  insns=500"
          },
          {
            "name": "SET_FLAG (handler)",
            "value": -34089.08,
            "unit": "ns"
          },
          {
            "name": "SET_FLAG (IPS)",
            "value": 7964,
            "unit": "insn/s"
          },
          {
            "name": "GET_FLAG (total)",
            "value": 112746.83,
            "unit": "ns/insn",
            "extra": "median=56373417ns  p95=74318833ns  stddev=5437323.3ns  iterations=11  insns=500"
          },
          {
            "name": "GET_FLAG (handler)",
            "value": -46899.83,
            "unit": "ns"
          },
          {
            "name": "GET_FLAG (IPS)",
            "value": 8869,
            "unit": "insn/s"
          },
          {
            "name": "JMP (total)",
            "value": 73179.08,
            "unit": "ns/insn",
            "extra": "median=36589542ns  p95=46629333ns  stddev=5668721.6ns  iterations=11  insns=500"
          },
          {
            "name": "JMP (handler)",
            "value": -86467.58,
            "unit": "ns"
          },
          {
            "name": "JMP (IPS)",
            "value": 13665,
            "unit": "insn/s"
          },
          {
            "name": "JCC (total)",
            "value": 60931.67,
            "unit": "ns/insn",
            "extra": "median=30465834ns  p95=48533709ns  stddev=5893122.7ns  iterations=11  insns=500"
          },
          {
            "name": "JCC (handler)",
            "value": -98715,
            "unit": "ns"
          },
          {
            "name": "JCC (IPS)",
            "value": 16412,
            "unit": "insn/s"
          },
          {
            "name": "NATIVE_CALL (total)",
            "value": 115075,
            "unit": "ns/insn",
            "extra": "median=57537500ns  p95=74866958ns  stddev=5314831.3ns  iterations=11  insns=500"
          },
          {
            "name": "NATIVE_CALL (handler)",
            "value": -44571.67,
            "unit": "ns"
          },
          {
            "name": "NATIVE_CALL (IPS)",
            "value": 8690,
            "unit": "insn/s"
          },
          {
            "name": "SEXT8 (total)",
            "value": 118577.08,
            "unit": "ns/insn",
            "extra": "median=59288542ns  p95=89547334ns  stddev=10327824.0ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT8 (handler)",
            "value": -41069.58,
            "unit": "ns"
          },
          {
            "name": "SEXT8 (IPS)",
            "value": 8433,
            "unit": "insn/s"
          },
          {
            "name": "SEXT16 (total)",
            "value": 121944.33,
            "unit": "ns/insn",
            "extra": "median=60972167ns  p95=81072333ns  stddev=8362570.9ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT16 (handler)",
            "value": -37702.33,
            "unit": "ns"
          },
          {
            "name": "SEXT16 (IPS)",
            "value": 8200,
            "unit": "insn/s"
          },
          {
            "name": "SEXT32 (total)",
            "value": 119945.5,
            "unit": "ns/insn",
            "extra": "median=59972750ns  p95=81728375ns  stddev=8811717.4ns  iterations=11  insns=500"
          },
          {
            "name": "SEXT32 (handler)",
            "value": -39701.17,
            "unit": "ns"
          },
          {
            "name": "SEXT32 (IPS)",
            "value": 8337,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT8 (total)",
            "value": 123125.17,
            "unit": "ns/insn",
            "extra": "median=61562584ns  p95=79984250ns  stddev=8534337.6ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT8 (handler)",
            "value": -36521.5,
            "unit": "ns"
          },
          {
            "name": "ZEXT8 (IPS)",
            "value": 8122,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT16 (total)",
            "value": 114650.5,
            "unit": "ns/insn",
            "extra": "median=57325250ns  p95=75778667ns  stddev=6602515.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT16 (handler)",
            "value": -44996.17,
            "unit": "ns"
          },
          {
            "name": "ZEXT16 (IPS)",
            "value": 8722,
            "unit": "insn/s"
          },
          {
            "name": "ZEXT32 (total)",
            "value": 111463.25,
            "unit": "ns/insn",
            "extra": "median=55731625ns  p95=72154292ns  stddev=4689279.7ns  iterations=11  insns=500"
          },
          {
            "name": "ZEXT32 (handler)",
            "value": -48183.42,
            "unit": "ns"
          },
          {
            "name": "ZEXT32 (IPS)",
            "value": 8972,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC8 (total)",
            "value": 114390.75,
            "unit": "ns/insn",
            "extra": "median=57195375ns  p95=75583167ns  stddev=7229787.9ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC8 (handler)",
            "value": -45255.92,
            "unit": "ns"
          },
          {
            "name": "TRUNC8 (IPS)",
            "value": 8742,
            "unit": "insn/s"
          },
          {
            "name": "TRUNC16 (total)",
            "value": 123119.75,
            "unit": "ns/insn",
            "extra": "median=61559875ns  p95=87123875ns  stddev=10630047.8ns  iterations=11  insns=500"
          },
          {
            "name": "TRUNC16 (handler)",
            "value": -36526.92,
            "unit": "ns"
          },
          {
            "name": "TRUNC16 (IPS)",
            "value": 8122,
            "unit": "insn/s"
          },
          {
            "name": "LOCK_ADD (total)",
            "value": 133069,
            "unit": "ns/insn",
            "extra": "median=66534500ns  p95=87214083ns  stddev=8231504.4ns  iterations=11  insns=500"
          },
          {
            "name": "LOCK_ADD (handler)",
            "value": -26577.67,
            "unit": "ns"
          },
          {
            "name": "LOCK_ADD (IPS)",
            "value": 7515,
            "unit": "insn/s"
          },
          {
            "name": "XCHG (total)",
            "value": 126949.08,
            "unit": "ns/insn",
            "extra": "median=63474541ns  p95=83919542ns  stddev=9411679.8ns  iterations=11  insns=500"
          },
          {
            "name": "XCHG (handler)",
            "value": -32697.58,
            "unit": "ns"
          },
          {
            "name": "XCHG (IPS)",
            "value": 7877,
            "unit": "insn/s"
          },
          {
            "name": "CMPXCHG (total)",
            "value": 121375.5,
            "unit": "ns/insn",
            "extra": "median=60687750ns  p95=75759000ns  stddev=7332577.9ns  iterations=11  insns=500"
          },
          {
            "name": "CMPXCHG (handler)",
            "value": -38271.17,
            "unit": "ns"
          },
          {
            "name": "CMPXCHG (IPS)",
            "value": 8239,
            "unit": "insn/s"
          },
          {
            "name": "FENCE (total)",
            "value": 144773.83,
            "unit": "ns/insn",
            "extra": "median=72386917ns  p95=83328125ns  stddev=9706572.1ns  iterations=11  insns=500"
          },
          {
            "name": "FENCE (handler)",
            "value": -14872.83,
            "unit": "ns"
          },
          {
            "name": "FENCE (IPS)",
            "value": 6907,
            "unit": "insn/s"
          },
          {
            "name": "ATOMIC_LOAD (total)",
            "value": 135038.42,
            "unit": "ns/insn",
            "extra": "median=67519208ns  p95=89602291ns  stddev=10083295.1ns  iterations=11  insns=500"
          },
          {
            "name": "ATOMIC_LOAD (handler)",
            "value": -24608.25,
            "unit": "ns"
          },
          {
            "name": "ATOMIC_LOAD (IPS)",
            "value": 7405,
            "unit": "insn/s"
          },
          {
            "name": "NOP (total)",
            "value": 139397.75,
            "unit": "ns/insn",
            "extra": "median=69698875ns  p95=80917417ns  stddev=9148193.6ns  iterations=11  insns=500"
          },
          {
            "name": "NOP (handler)",
            "value": -20248.92,
            "unit": "ns"
          },
          {
            "name": "NOP (IPS)",
            "value": 7174,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_INTEGRITY (total)",
            "value": 143017.5,
            "unit": "ns/insn",
            "extra": "median=71508750ns  p95=81218334ns  stddev=6494802.4ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_INTEGRITY (handler)",
            "value": -16629.17,
            "unit": "ns"
          },
          {
            "name": "CHECK_INTEGRITY (IPS)",
            "value": 6992,
            "unit": "insn/s"
          },
          {
            "name": "CHECK_DEBUG (total)",
            "value": 146675.83,
            "unit": "ns/insn",
            "extra": "median=73337916ns  p95=92585917ns  stddev=8651725.8ns  iterations=11  insns=500"
          },
          {
            "name": "CHECK_DEBUG (handler)",
            "value": -12970.83,
            "unit": "ns"
          },
          {
            "name": "CHECK_DEBUG (IPS)",
            "value": 6818,
            "unit": "insn/s"
          },
          {
            "name": "MUTATE_ISA (total)",
            "value": 143864.08,
            "unit": "ns/insn",
            "extra": "median=71932041ns  p95=83932416ns  stddev=6311539.9ns  iterations=11  insns=500"
          },
          {
            "name": "MUTATE_ISA (handler)",
            "value": -15782.58,
            "unit": "ns"
          },
          {
            "name": "MUTATE_ISA (IPS)",
            "value": 6951,
            "unit": "insn/s"
          }
        ]
      }
    ]
  }
}