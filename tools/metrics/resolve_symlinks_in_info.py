# Resolve symbolink links in a .info coverage profile

# This is needed because sources paths contained in info files are using the
# project symlinks ("epsilon/ion" for instance), the diff file is using the
# real paths ("shared/ion") and genhtml does not resolve the links.

from pathlib import Path
from sys import argv

if len(argv) != 2:
    print("Usage: resolve_symlinks_in_info.py base.info")
    exit(0)

filename = argv[1]

content = Path(filename).read_text().split("\n")
content_resolved = []

for line in content:
    # Sources paths are in SF fields
    if line.startswith("SF:"):
        line = "SF:" + str(Path(line[3:]).resolve())
    content_resolved.append(line)

Path(filename).write_text("\n".join(content_resolved))
