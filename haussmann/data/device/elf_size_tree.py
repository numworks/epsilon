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


def strip_return_type(symbol_str):
    """
    Strip return type from C++ symbols, safely handling:
    - Templates: <...>
    - (anonymous namespace)
    - Only removes return type if a space exists outside <> and () before '('

    e.g.:
    "A::B::C<D::E, F>::G A::B::C<D::E, F>::(anonymous namespace)::H()"
      -> "A::B::C<D::E, F>::(anonymous namespace)::H()"
    """
    ANON = "(anonymous namespace)"
    ANON_LEN = len(ANON)

    # Step 1: find the '(' that starts the function signature
    paren_index = -1
    i = 0
    angle_depth = 0
    paren_depth = 0

    while i < len(symbol_str):
        if symbol_str[i : i + ANON_LEN] == ANON:
            i += ANON_LEN
            continue
        if symbol_str[i] == "<":
            angle_depth += 1
        elif symbol_str[i] == ">":
            angle_depth -= 1
        elif symbol_str[i] == "(":
            if angle_depth == 0:
                paren_index = i
                break
            paren_depth += 1
        elif symbol_str[i] == ")":
            paren_depth = max(0, paren_depth - 1)
        i += 1

    if paren_index == -1:
        return symbol_str  # Not a function

    # Step 2: go backward from paren to find last space outside of <>, ()
    i = paren_index - 1
    angle_depth = 0
    paren_depth = 0

    while i >= 0:
        if i - ANON_LEN + 1 >= 0 and symbol_str[i - ANON_LEN + 1 : i + 1] == ANON:
            i -= ANON_LEN
            continue
        c = symbol_str[i]
        if c == ">":
            angle_depth += 1
        elif c == "<":
            angle_depth -= 1
        elif c == ")":
            paren_depth += 1
        elif c == "(":
            paren_depth = max(0, paren_depth - 1)
        elif c == " " and angle_depth == 0 and paren_depth == 0:
            return symbol_str[i + 1 :]
        i -= 1

    return symbol_str  # No return type found


def split_cpp_namespaces(name):
    """
    "A::B::C<D::E, F>::(anonymous namespace)"
      -> ["A", "B", "C<D::E, F>", "(anonymous namespace)"]
    """
    parts = []
    current = []
    depth = 0  # template angle bracket nesting

    i = 0
    while i < len(name):
        c = name[i]
        if c == "<":
            depth += 1
            current.append(c)
            i += 1
        elif c == ">":
            if depth > 0:
                depth -= 1
            current.append(c)
            i += 1
        elif c == ":" and i + 1 < len(name) and name[i + 1] == ":" and depth == 0:
            # hit a :: outside templates — split here
            parts.append("".join(current))
            current = []
            i += 2  # skip both colons
        else:
            current.append(c)
            i += 1

    if current:
        parts.append("".join(current))
    return parts


def split_namespace_and_symbol(symbol_str):
    """
    "A::B::C<D::E, F>::(anonymous namespace)::H()"
      -> (["A", "B", "C<D::E, F>", "(anonymous namespace)"], "H()")
    """
    depth = 0
    last_ns_pos = -1
    i = 0
    while i < len(symbol_str) - 1:
        if symbol_str[i : i + 21] == "(anonymous namespace)":
            i += 21
            continue
        c = symbol_str[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
        elif c == ":" and symbol_str[i + 1] == ":" and depth == 0:
            last_ns_pos = i
            i += 1
        i += 1

    if last_ns_pos == -1:
        return [], symbol_str
    else:
        namespaces_str = symbol_str[:last_ns_pos]
        symbol_name = symbol_str[last_ns_pos + 2 :]
        namespaces = split_cpp_namespaces(namespaces_str) if namespaces_str else []
        return namespaces, symbol_name


allowed_types = {"T", "t", "R", "r"}  # Only .text and .rodata


def parse_nm_line(line):
    """
    Parse a line from `arm-none-eabi-nm` output.
    Returns (symbol, size) or None if invalid.
    Expects lines like:
    "00000000 00000000 T A::B::C<D::E, F>::(anonymous namespace)::H()"
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
