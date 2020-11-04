#!/usr/bin/env python3
import subprocess
import multiprocessing

NUMBER_OF_FUZZERS=multiprocessing.cpu_count()

def afl_command(name):
  master_option = "-M" if name.startswith("master") else "-S"
  return ["afl-fuzz", "-t", "10000", "-i", "scenari", "-o", "afl_out", master_option, "epsilon-fuzz-" + name, "./epsilon.bin"]

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
