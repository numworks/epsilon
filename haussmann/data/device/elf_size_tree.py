import json
import subprocess
import sys
import argparse
from rich.tree import Tree
from rich import print


class Node:
    def __init__(self, name):
        self.name = name
        self.size = 0
        self.children = {}

    def add_symbol(self, namespaces, symbol_name, size):
        if namespaces:
            ns = namespaces[0]
            if ns not in self.children:
                self.children[ns] = Node(ns)
            child = self.children[ns]
            child.add_symbol(namespaces[1:], symbol_name, size)
            self.size += size
        else:
            if symbol_name not in self.children:
                self.children[symbol_name] = Node(symbol_name)
            self.children[symbol_name].size += size
            self.size += size

    def to_dict(self):
        return {
            "name": self.name,
            "size": self.size,
            "children": [
                child.to_dict()
                for child in sorted(
                    self.children.values(), key=lambda c: c.size, reverse=True
                )
            ],
        }


def find_at_depth_zero(s, start, substring):
    """
    Find the first occurrence of `substring` at depth zero in `s` starting from `start`.
    Depth zero means no nested parentheses or brackets.
    """
    depth = 0
    i = start

    while i < len(s):
        if depth == 0 and s.startswith(substring, i):
            return i

        if s[i] == '(' or s[i] == '{' or s[i] == '[' or s[i] == '<':
            depth += 1
        elif s[i] == ')' or s[i] == '}' or s[i] == ']' or s[i] == '>':
            depth -= 1

        i += 1

    return None


def strip_return_type(symbol_str):
    """
    Strip return type from C++ symbols.

    e.g.:
    "A::B::C<D::E, F>::G H::I::J<K::L, M>::(anonymous namespace)::N()"
      -> "H::I::J<K::L, M>::(anonymous namespace)::N()"
    """
    # Find the last space at depth zero that is followed by parentheses.
    symbol_start = 0
    i = find_at_depth_zero(symbol_str, 0, " ")
    while i is not None:
        if find_at_depth_zero(symbol_str, i + 1, "(") is None:
            break
        symbol_start = i + 1
        i = find_at_depth_zero(symbol_str, symbol_start, " ")

    return symbol_str[symbol_start:]


def split_namespace_and_symbol(symbol_str):
    """
    "A::B::C<D::E, F>::(anonymous namespace)::G()"
      -> (["A", "B", "C<D::E, F>", "(anonymous namespace)"], "G()")
    """
    namespaces = []
    symbol_start = 0
    i = find_at_depth_zero(symbol_str, 0, "::")
    while i is not None:
        namespaces.append(symbol_str[symbol_start:i])
        symbol_start = i + 2  # skip the "::"
        i = find_at_depth_zero(symbol_str, symbol_start, "::")
    return (namespaces, symbol_str[symbol_start:])


allowed_types = {"T", "t", "R", "r"}  # Only .text and .rodata


def parse_nm_line(line):
    """
    Parse a line from `arm-none-eabi-nm` output.
    Returns (symbol, size) or None if invalid.
    Expects lines like:
    "00000000 00000000 T A::B::C<D::E, F>::(anonymous namespace)::G()"
    """
    parts = line.strip().split(None, 3)
    if len(parts) != 4:
        return None
    _, size_hex, sym_type, symbol = parts

    if sym_type not in allowed_types:
        return None

    try:
        size = int(size_hex, 16)
    except ValueError:
        return None
    if size <= 0:
        return None

    # Normalize compiler-generated symbols
    if symbol.startswith("vtable for "):
        symbol = symbol.replace("vtable for ", "") + "::vtable"
    elif symbol.startswith("typeinfo for "):
        symbol = symbol.replace("typeinfo for ", "") + "::typeinfo"
    elif symbol.startswith("typeinfo name for "):
        symbol = symbol.replace("typeinfo name for ", "") + "::typeinfo_name"
    else:
        symbol = strip_return_type(symbol)

    return symbol, size


def build_rich_tree(node, tree):
    for child in sorted(node.children.values(), key=lambda c: c.size, reverse=True):
        label = f"{child.name} ({child.size})"
        branch = tree.add(label)
        build_rich_tree(child, branch)


def main():
    parser = argparse.ArgumentParser(
        description="Analyze ELF symbol sizes by namespace."
    )
    parser.add_argument("elf_file", help="Path to ELF binary")
    parser.add_argument("--json", action="store_true", help="Print JSON output")
    parser.add_argument(
        "--rich", action="store_true", help="Print rich terminal tree (default)"
    )
    args = parser.parse_args()

    if not (args.json or args.rich):
        args.rich = True  # Default to rich if no option given

    root = Node("<global>")

    try:
        proc = subprocess.run(
            ["arm-none-eabi-nm", "-SCn", args.elf_file],
            capture_output=True,
            text=True,
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print(f"[red]Error running nm: {e}[/red]", file=sys.stderr)
        sys.exit(1)

    for line in proc.stdout.splitlines():
        parsed = parse_nm_line(line)
        if not parsed:
            continue
        symbol, size = parsed
        namespaces, symbol_name = split_namespace_and_symbol(symbol)
        root.add_symbol(namespaces, symbol_name, size)

    if args.json:
        print(json.dumps(root.to_dict(), indent=2))
    else:
        rich_tree = Tree(f"{root.name} ({root.size})")
        build_rich_tree(root, rich_tree)
        print(rich_tree)


if __name__ == "__main__":
    main()
