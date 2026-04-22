#!/usr/bin/env python3
"""Lint makefiles: no trailing whitespace; non-recipe continuations use spaces."""

import sys
from pathlib import Path


def process_file(path: Path, fix: bool) -> list[str]:
    msgs = []
    new_lines = []
    prev_continued = False
    chain_is_recipe = False
    for i, line in enumerate(path.read_text().splitlines(), 1):
        fixed = line

        if fixed != fixed.rstrip():
            msgs.append(f"{path}:{i}: trailing whitespace")
            if fix:
                fixed = fixed.rstrip()

        if prev_continued and not chain_is_recipe:
            indent_end = len(fixed) - len(fixed.lstrip(" \t"))
            indent = fixed[:indent_end]
            if "\t" in indent:
                msgs.append(f"{path}:{i}: tab in non-recipe continuation; use spaces")
                if fix:
                    fixed = indent.replace("\t", "  ") + fixed[indent_end:]

        if not prev_continued:
            chain_is_recipe = fixed.startswith("\t")
        prev_continued = fixed.rstrip().endswith("\\")
        new_lines.append(fixed)

    if fix and msgs:
        path.write_text("\n".join(new_lines) + "\n")

    return msgs


def main():
    args = sys.argv[1:]
    check = "--check" in args
    fix = not check
    args = [a for a in args if a != "--check"]

    files = [Path(a) for a in args]

    msgs = [m for f in files for m in process_file(f, fix)]
    for m in msgs:
        print(m)
    sys.exit(1 if msgs else 0)


if __name__ == "__main__":
    main()
