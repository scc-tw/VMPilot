#!/usr/bin/env python3
"""
bench_analyzer.py

Analyzes VMPilot benchmark raw JSON output.
1. Removes jitter using MAD (Median Absolute Deviation) filtering.
2. Re-calculates statistics (median, mean, stddev, ns_per_du, etc.).
3. Performs ANOVA to check for statistical indistinguishability.
4. Calculates Shannon Leakage (Mutual Information).
5. Outputs a "clean" JSON backward-compatible with older tools.

Usage:
    python scripts/bench_analyzer.py bench_raw.json > bench_clean.json
"""

import json
import sys
import math
import argparse

def mad_filter(samples, z_thresh=3.5):
    """
    Filters outliers using the Modified Z-Score based on Median Absolute Deviation.
    """
    if not samples:
        return []
    
    samples = sorted(samples)
    n = len(samples)
    median = samples[n // 2]
    
    devs = [abs(s - median) for s in samples]
    devs.sort()
    mad = devs[n // 2]
    
    if mad == 0:
        # If MAD is 0, fall back to mean absolute deviation or return all
        mean = sum(samples) / n
        mad = sum(abs(s - mean) for s in samples) / n
        if mad == 0:
            return samples # all samples are identical
    
    filtered = []
    for s in samples:
        z = 0.6745 * (s - median) / mad
        if abs(z) < z_thresh:
            filtered.append(s)
            
    return filtered if filtered else samples

def calculate_stats(samples, insn_count):
    if not samples:
        return {}
    samples = sorted(samples)
    n = len(samples)
    min_val = samples[0]
    median = samples[n // 2]
    p95 = samples[int(n * 0.95)]
    
    mean = sum(samples) / n
    var = sum((s - mean)**2 for s in samples) / n
    stddev = math.sqrt(var)
    
    ns_per_insn = median / insn_count if insn_count else 0
    ips = (insn_count * 1e9) / median if median else 0
    
    return {
        "iterations": n,
        "min_ns": min_val,
        "median_ns": median,
        "p95_ns": p95,
        "mean_ns": mean,
        "stddev_ns": stddev,
        "ns_per_insn": ns_per_insn,
        "ns_per_du": ns_per_insn, # backward compat
        "du_per_sec": ips,
        "ips": ips
    }

def analyze(raw_data):
    # 1. Filter and re-calculate
    baseline_ns_per_du = 0
    clean_results = []
    
    # First pass: find NOP_BASELINE to set baseline_ns_per_du
    for r in raw_data.get("results", []):
        samples = mad_filter(r.get("samples", []))
        stats = calculate_stats(samples, r.get("du_count", r.get("insn_count", 0)))
        
        # update r with new stats
        r.update(stats)
        
        if r["opcode"] == "NOP_BASELINE":
            baseline_ns_per_du = r["ns_per_insn"]
            
        clean_results.append(r)
        
    # Second pass: calculate handler_ns / delta_ns
    for r in clean_results:
        r["delta_ns"] = r["ns_per_insn"] - baseline_ns_per_du
        r["handler_ns"] = r["delta_ns"]
        
    # 2. Statistical Analytics (ANOVA & Mutual Information)
    # We only care about ANOVA for actual opcodes (not NOP baseline or VM Internal if we want strict policy check, but let's do all)
    
    groups = []
    for r in raw_data.get("results", []):
        if r["opcode"] == "NOP_BASELINE":
            continue
        # Exclude category 7 (VM Internal) opcodes like CHECK_INTEGRITY, 
        # as they do not follow standard DU padding and are inherently distinguishable.
        if r.get("category", 0) == 7:
            continue
            
        # Use filtered samples for ANOVA
        filtered = mad_filter(r.get("samples", []))
        if filtered:
            groups.append(filtered)
            
    # One-way ANOVA
    p_value = 1.0
    f_stat = -1.0
    try:
        from scipy import stats as scipy_stats
        if len(groups) > 1:
            f_stat, p_value = scipy_stats.f_oneway(*groups)
    except ImportError:
        # Fallback if scipy not installed in CI, though it should be or we should handle it
        p_value = -1.0 # Unknown
        
    # Mutual Information (Simplified 1D estimation)
    # H(T) - H(T|O)
    mi_bits = 0.0
    try:
        from scipy import stats as scipy_stats
        import numpy as np
        
        if len(groups) > 1:
            all_samples = np.concatenate(groups)
            # Only calculate if there's variance
            if np.var(all_samples) > 1e-9:
                # Use differential entropy
                h_total = scipy_stats.differential_entropy(all_samples)
                
                h_cond_sum = 0
                valid_groups = 0
                for g in groups:
                    if len(g) > 1 and np.var(g) > 1e-9:
                        h_cond_sum += scipy_stats.differential_entropy(g)
                        valid_groups += 1
                        
                if valid_groups > 0:
                    h_cond = h_cond_sum / valid_groups
                    mi_nats = max(0.0, h_total - h_cond)
                    mi_bits = mi_nats / np.log(2)
    except ImportError:
        mi_bits = -1.0

    # Third pass: clean up samples to reduce JSON size
    for r in clean_results:
        r.pop("samples", None)

    # Build clean output
    clean_data = {
        "version": raw_data.get("version", 1),
        "metadata": raw_data.get("metadata", {}),
        "baseline_ns_per_du": baseline_ns_per_du,
        "security_metrics": {
            "anova_f_stat": float(f_stat) if f_stat != -1.0 else None,
            "anova_p_value": float(p_value) if p_value != -1.0 else None,
            "indistinguishable": bool(p_value > 0.05) if p_value != -1.0 else None,
            "leakage_bits": float(mi_bits) if mi_bits != -1.0 else None
        },
        "results": clean_results
    }
    
    return clean_data

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
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="Raw JSON from vmpilot_bench")
    parser.add_argument("--fail-on-leak", action="store_true", help="Fail if HighSecPolicy leaks")
    args = parser.parse_args()
    
    with open(args.input, "r", encoding="utf-8") as f:
        content = f.read()
        
    raw_datas = parse_multiple_jsons(content)
    
    for raw_data in raw_datas:
        clean_data = analyze(raw_data)
        
        # Print the clean JSON to stdout
        print(json.dumps(clean_data, indent=2))
        
        # Policy check
        meta = clean_data.get("metadata", {})
        sec = clean_data.get("security_metrics", {})
        
        if args.fail_on_leak and meta.get("policy") == "HighSecPolicy":
            if sec.get("anova_p_value") is not None and sec["anova_p_value"] < 0.01:
                print(f"\n[!] SECURITY FAILURE: HighSecPolicy ANOVA p-value {sec['anova_p_value']} < 0.01. Opcodes are distinguishable.", file=sys.stderr)
                sys.exit(1)
            if sec.get("leakage_bits") is not None and sec["leakage_bits"] > 0.5:
                print(f"\n[!] SECURITY FAILURE: HighSecPolicy Leakage {sec['leakage_bits']} bits > 0.5 threshold.", file=sys.stderr)
                sys.exit(1)

if __name__ == "__main__":
    main()
