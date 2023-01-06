#!/usr/bin/env python3
# This scripts will follow a clang (or clang++) invocation with a clang-tidy one
from os import environ
import subprocess
from sys import argv

# Check the command line
if argv[-4] != '-c':
  print("Error: cxxlint cannot parse the provided compiler command-line")
  exit(-1)

def run(*args, **kwargs):
  if environ.get("V"):
    print(*args)
  p = subprocess.run(*args, **kwargs)
  if p.returncode:
    print("Error running", args)
    exit(p.returncode)

# Run the compiler
run(argv[1:])

# Don't run the linter if warnings are suppressed.
# This is typically done on external files.
if "-w" in argv:
  exit(0)

# Prepare the linter
tidy = [
  environ["CLANG_TIDY"],
  "-quiet",
  "-load=output/host/tidy.lib",
  "--config-file=build/tidy/config.yml",
  argv[-3],
  "--"
]
tidy.extend(argv[2:-4])

# Run the linter
# Note: clang-tidy insists on displaying things on stderr even if the "-quiet"
# flag is used.
run(tidy, stderr=subprocess.DEVNULL)
