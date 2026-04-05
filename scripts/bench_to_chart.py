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
    policy = raw.get("metadata", {}).get("policy", "Unknown")

    for r in raw.get("results", []):
        name = r["opcode"]

        # ns_per_du — total pipeline cost per dispatch unit (smaller is better)
        ns_per = r.get("ns_per_du", r.get("ns_per_insn", 0))
        du_count = r.get("du_count", r.get("insn_count", 0))
        entries.append({
            "name": f"[{policy}] {name} (total)",
            "unit": "ns/DU",
            "value": round(ns_per, 2),
            "extra": f"median={r['median_ns']}ns  p95={r['p95_ns']}ns  "
                     f"stddev={r['stddev_ns']:.1f}ns  "
                     f"iterations={r['iterations']}  DUs={du_count}",
        })

        # delta_ns — handler overhead above baseline (smaller is better)
        delta = r.get("delta_ns", r.get("handler_ns", ns_per - baseline))
        entries.append({
            "name": f"[{policy}] {name} (handler)",
            "unit": "ns",
            "value": round(delta, 2),
        })

        # du_per_sec — throughput (bigger is better, separate chart group)
        throughput = r.get("du_per_sec", r.get("ips", 0))
        if throughput > 0:
            entries.append({
                "name": f"[{policy}] {name} (DU/s)",
                "unit": "DU/s",
                "value": round(throughput),
            })

    return entries


def parse_multiple_jsons(text):
    decoder = json.JSONDecoder()
    pos = 0
    results = []
    text = text.lstrip()
    while pos < len(text):
        obj, pos = decoder.raw_decode(text, pos)
        results.append(obj)
        while pos < len(text) and text[pos].isspace():
            pos += 1
    return results


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input.json>", file=sys.stderr)
        sys.exit(1)

    with open(sys.argv[1], encoding="utf-8") as f:
        content = f.read()

    stripped = content.lstrip()
    if stripped.startswith("["):
        loaded = json.loads(content)
        raw_datas = loaded if isinstance(loaded, list) else [loaded]
    else:
        raw_datas = parse_multiple_jsons(content)

    all_entries = []
    for raw in raw_datas:
        all_entries.extend(convert(raw))

    json.dump(all_entries, sys.stdout, indent=2)
    print()


if __name__ == "__main__":
    main()
