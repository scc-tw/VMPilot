window.BENCHMARK_DATA = {
  "lastUpdate": 1775162468355,
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
      }
    ]
  }
}