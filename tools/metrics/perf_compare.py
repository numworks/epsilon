#!/usr/bin/env python3

"""Compare per-test instruction counts from two epsilon_test --perf-counter logs.

Each log is the stdout of `epsilon_test --perf-counter`, i.e. alternating
lines:

    <test_name>
    INSTRUCTIONS: <count>
    ...

Tests whose head count differs from base by more than the threshold (default
5%) are reported. Exit status is 0 (no significant change), 1 (regressions
or improvements above threshold).
"""

import argparse
import re
import sys


def parse_log(path):
    counts = {}
    pending_name = None
    with open(path) as f:
        for raw in f:
            line = raw.rstrip()
            m = re.match(r"^INSTRUCTIONS: (\d+)$", line)
            if m:
                if pending_name is not None:
                    counts[pending_name] = int(m.group(1))
                    pending_name = None
                continue
            if (
                line
                and not line.startswith("TEST:")
                and not line.startswith("ALL ")
                and not line.startswith("DURATION:")
            ):
                pending_name = line
            else:
                pending_name = None
    return counts


def format_count(n):
    return f"{n:,}".replace(",", " ")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("base", help="base log file")
    parser.add_argument("head", help="head log file")
    parser.add_argument(
        "--threshold",
        type=float,
        default=5.0,
        help="percentage threshold for reporting (default 5)",
    )
    args = parser.parse_args()

    base = parse_log(args.base)
    head = parse_log(args.head)

    common = sorted(set(base) & set(head))

    changed = []
    for name in common:
        b, h = base[name], head[name]
        if b == 0:
            continue
        delta = (h - b) / b * 100.0
        if abs(delta) >= args.threshold:
            changed.append((name, b, h, delta))

    if not changed:
        print(f"No test moved by more than {args.threshold:.1f}%.")
        return 0

    changed.sort(key=lambda r: -abs(r[3]))
    print(f"Tests with instruction count change >= {args.threshold:.1f}%:")
    print()
    print("| Test | BASE | HEAD | Delta |")
    print("| --- | --- | --- | --- |")
    for name, b, h, d in changed:
        sign = "+" if d > 0 else ""
        print(f"| {name} | {format_count(b)} | {format_count(h)} | {sign}{d:.2f}% |")
    print()
    return 1


if __name__ == "__main__":
    sys.exit(main())
