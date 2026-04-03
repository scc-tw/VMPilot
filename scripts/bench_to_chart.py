#!/usr/bin/env python3
"""Convert vmpilot_bench JSON output → github-action-benchmark format.

Usage:
    python bench_to_chart.py raw.json > chart.json

Input:  vmpilot_bench JSON (with metadata + results array)
Output: github-action-benchmark customSmallerIsBetter JSON array
"""

import json
import sys


def convert(raw: dict) -> list:
    """Convert our benchmark JSON to github-action-benchmark format."""
    entries = []
    baseline = raw.get("baseline_ns_per_du",
                       raw.get("baseline_ns_per_insn", 0))

    for r in raw.get("results", []):
        name = r["opcode"]

        # ns_per_du — total pipeline cost per dispatch unit (smaller is better)
        ns_per = r.get("ns_per_du", r.get("ns_per_insn", 0))
        du_count = r.get("du_count", r.get("insn_count", 0))
        entries.append({
            "name": f"{name} (total)",
            "unit": "ns/DU",
            "value": round(ns_per, 2),
            "extra": f"median={r['median_ns']}ns  p95={r['p95_ns']}ns  "
                     f"stddev={r['stddev_ns']:.1f}ns  "
                     f"iterations={r['iterations']}  DUs={du_count}",
        })

        # delta_ns — handler overhead above baseline (smaller is better)
        delta = r.get("delta_ns", r.get("handler_ns", ns_per - baseline))
        entries.append({
            "name": f"{name} (handler)",
            "unit": "ns",
            "value": round(delta, 2),
        })

        # du_per_sec — throughput (bigger is better, separate chart group)
        throughput = r.get("du_per_sec", r.get("ips", 0))
        if throughput > 0:
            entries.append({
                "name": f"{name} (DU/s)",
                "unit": "DU/s",
                "value": round(throughput),
            })

    return entries


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input.json>", file=sys.stderr)
        sys.exit(1)

    with open(sys.argv[1], encoding="utf-8") as f:
        raw = json.load(f)

    entries = convert(raw)
    json.dump(entries, sys.stdout, indent=2)
    print()


if __name__ == "__main__":
    main()
