window.BENCHMARK_DATA = {
  "lastUpdate": 1775162696330,
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
      }
    ]
  }
}