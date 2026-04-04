#!/usr/bin/env python3
"""Compare benchmark results against a baseline → markdown PR comment.

Usage:
    python bench_compare.py --current pr.json --baseline base.json > comment.md
    python bench_compare.py --current pr.json   # no baseline → just show current
"""

import argparse
import json
import sys


def load(path: str) -> dict:
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def by_opcode(raw: dict) -> dict:
    """Index results by opcode name."""
    return {r["opcode"]: r for r in raw.get("results", [])}


CAT_NAMES = {
    0: "Data Movement", 1: "Arithmetic", 2: "Logic",
    3: "Comparison", 4: "Control Flow", 5: "Width/Extension",
    6: "Atomic", 7: "VM Internal",
}


def delta_str(cur: float, base: float) -> str:
    if base == 0:
        return ""
    pct = (cur - base) / base * 100
    if abs(pct) < 1.0:
        return "~"
    arrow = "+" if pct > 0 else "-"
    return f"{arrow}{abs(pct):.1f}%"


def render_markdown_for_policy(current: dict, baseline: dict | None, platform: str) -> str:
    lines = []
    cur = by_opcode(current)
    base = by_opcode(baseline) if baseline else {}
    meta = current.get("metadata", {})
    policy = meta.get("policy", "Unknown")

    lines.append(f"### {platform} - {policy}")
    lines.append(f"")
    lines.append(f"| Opcode | Cat | ns/DU | delta ns | vs base |")
    lines.append(f"|--------|-----|------:|--------:|--------:|")

    # Group by category
    from collections import defaultdict
    cats = defaultdict(list)
    for name, r in sorted(cur.items(), key=lambda x: (x[1]["category"], x[0])):
        cats[r["category"]].append((name, r))

    for cat_id in sorted(cats.keys()):
        for name, r in cats[cat_id]:
            ns = r.get("ns_per_du", r.get("ns_per_insn", 0))
            hns = r.get("delta_ns", r.get("handler_ns", 0))
            d = ""
            if name in base:
                base_ns = base[name].get("ns_per_du", base[name].get("ns_per_insn", 0))
                d = delta_str(ns, base_ns)
            cat = CAT_NAMES.get(cat_id, "?")
            lines.append(f"| `{name}` | {cat} | {ns:.1f} | {hns:+.1f} | {d} |")

    bl = current.get("baseline_ns_per_du", current.get("baseline_ns_per_insn", 0))
    lines.append(f"")
    
    sec = current.get("security_metrics", {})
    sec_str = ""
    if sec:
        p_val = sec.get("anova_p_value")
        f_stat = sec.get("anova_f_stat")
        bits = sec.get("leakage_bits")
        if p_val is not None and bits is not None:
            if p_val == 0.0 and f_stat is not None:
                p_str = f"~0 (F={f_stat:.1f})"
            elif p_val < 0.0001:
                p_str = f"{p_val:.2e}"
            else:
                p_str = f"{p_val:.4f}"
            sec_str = f" | ANOVA $p$: {p_str} | Leakage: {bits:.4f} bits"
            
    lines.append(f"_Pipeline baseline (NOP): {bl:.1f} ns/DU "
                 f"| Policy: {meta.get('policy','?')} "
                 f"| Build: {meta.get('build_type','?')}{sec_str}_")
    return "\n".join(lines)


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


def load_multiple(path: str) -> list:
    with open(path, encoding="utf-8") as f:
        content = f.read()
    if not content.strip():
        return []
    if content.lstrip().startswith('['):
        return json.loads(content)
    return parse_multiple_jsons(content)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--current", required=True, help="Current benchmark JSON")
    ap.add_argument("--baseline", default=None, help="Baseline JSON (optional)")
    ap.add_argument("--platform", default="", help="Platform label")
    args = ap.parse_args()

    current_list = load_multiple(args.current)
    baseline_list = load_multiple(args.baseline) if args.baseline else []
    
    baseline_by_policy = {
        b.get("metadata", {}).get("policy", "Unknown"): b 
        for b in baseline_list
    }

    for current in current_list:
        policy = current.get("metadata", {}).get("policy", "Unknown")
        baseline = baseline_by_policy.get(policy)
        print(render_markdown_for_policy(current, baseline, args.platform))
        print("\n---\n")


if __name__ == "__main__":
    main()
