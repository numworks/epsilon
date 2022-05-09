#!/usr/bin/env python

import re
import subprocess
import sys
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter

readelf_line_regex = re.compile("[0-9]+:\s+([0-9a-f]+)\s+([0-9]+)\s+[A-Z]+")
def parse_line(line):
  hex_start, dec_size = re.findall(readelf_line_regex, line)[0]
  return (int(hex_start, 16), int(dec_size))

readelf_output = subprocess.check_output([
  "arm-none-eabi-readelf",
  "-W", # Don't limit line lenght
  "-s", # Sizes
  sys.argv[1]
]).decode('utf-8')

for line in readelf_output.splitlines():
  words = line.split()
  if not words:
    continue
  symbol = words[-1]
  if symbol == "_ZN3Ion7Storage17staticStorageAreaE":
    storage = parse_line(line)
  if symbol == "_ZZN13AppsContainer19sharedAppsContainerEvE20appsContainerStorage":
    container = parse_line(line)
  if symbol == "_stack_start":
    stack_start, _ = parse_line(line)
  if symbol == "_stack_end":
    stack_end, _ = parse_line(line)
  if symbol == "_heap_start":
    heap_start, _ = parse_line(line)
  if symbol == "_heap_end":
    heap_end, _ = parse_line(line)

stack_size = stack_start - stack_end # Stack grows downwards
stack = (stack_end, stack_size)
heap_size = heap_end - heap_start
heap = (heap_start, heap_size)

def format_kb(i):
  return ("%d KB" % (i/1024))

fig,ax = plt.subplots()

def plot(value, name, c):
  ax.broken_barh([value], (0, 1), color=c, label=name + " - " + format_kb(value[1]))

plot(container, "Container", "blue")
plot(storage, "Storage", "red")
plot(heap, "Heap", "pink")
plot(stack, "Stack", "green")

ax.set_yticks([])
ax.set_xticks(list(range(0x20000000,0x20040001,0x10000)))
xlabels = map(lambda t: '0x%08X' % int(t), ax.get_xticks())
ax.set_xticklabels(xlabels);
ax.legend()
fig.set_size_inches(20, 2)
fig.savefig(sys.argv[2])

