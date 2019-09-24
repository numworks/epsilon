#!/usr/bin/env python
import subprocess

NUMBER_OF_FUZZERS=8

def afl_command(name):
  return ["afl-fuzz", "-t", "10000", "-i", "scenari", "-o", "afl_out", "-M", "epsilon-fuzz-" + name, "./epsilon.bin"]

def run_afl(commands, name):
  # Launch the fuzzer
  subprocess.run(["tmux"] + commands + [" ".join(afl_command(name))])
  # Re-tile the window (so more fuzzers can be added)
  subprocess.run(["tmux", "select-layout", "tiled"])

# Clean up the "afl_out" folder
subprocess.run(["rm", "-rf", "afl_out"])

# Launch fuzzers
run_afl(["new", "-d"], "master")
for i in range(NUMBER_OF_FUZZERS-1):
  run_afl(["split-window"], "slave-"+str(i))
