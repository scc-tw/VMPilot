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
    baseline = raw.get("baseline_ns_per_insn", 0)

    for r in raw.get("results", []):
        name = r["opcode"]

        # ns_per_insn — total pipeline cost (smaller is better)
        entries.append({
            "name": f"{name} (total)",
            "unit": "ns/insn",
            "value": round(r["ns_per_insn"], 2),
            "extra": f"median={r['median_ns']}ns  p95={r['p95_ns']}ns  "
                     f"stddev={r['stddev_ns']:.1f}ns  "
                     f"iterations={r['iterations']}  insns={r['insn_count']}",
        })

        # handler_ns — handler overhead above baseline (smaller is better)
        entries.append({
            "name": f"{name} (handler)",
            "unit": "ns",
            "value": round(r.get("handler_ns", r["ns_per_insn"] - baseline), 2),
        })

        # IPS — throughput (bigger is better, separate chart group)
        if r.get("ips", 0) > 0:
            entries.append({
                "name": f"{name} (IPS)",
                "unit": "insn/s",
                "value": round(r["ips"]),
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
