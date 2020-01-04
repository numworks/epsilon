#!/usr/bin/env python

import re
import subprocess
import sys
import matplotlib.pyplot as plt
import random
from matplotlib.ticker import FormatStrFormatter

def filter_set(data, pred):
  result = {}
  for k,v in data.items():
    if (pred(v)):
      result[k] = v
  return result

def pred_ram(symbol):
  return (symbol[0] >= 0x20000000) and (symbol[0] <= 0x20040000)

def pred_size(symbol):
  return (symbol[1] >= 64)

def load_symbols(filename):
  nm_output = subprocess.check_output([
    "arm-none-eabi-nm",
    "--print-size",
    filename
  ]).decode('utf-8').splitlines()
  nm_symbol_regex = re.compile("^([0-9A-Fa-f]+) ([0-9A-Fa-f]+) (.) (.+)$")
  nm_sizeless_regex = re.compile("^([0-9a-z]+) (.) (.+)$")
  symbol_results = [ re.match(nm_symbol_regex, line).groups() for line in nm_output if re.match(nm_symbol_regex, line) ]
  sizeless_results = [ re.match(nm_sizeless_regex, line).groups() for line in nm_output if re.match(nm_sizeless_regex, line) ]
  results = {}
  for result in symbol_results:
    results[result[3]] = ((int(result[0],16),int(result[1],16),result[2],result[3]))
  for result in sizeless_results:
    results[result[2]] = ((int(result[0],16),0,result[1],result[2]))
  # Fixup stack and heap
  for i in (("_stack_start", "_stack_end", "_stack"), ("_heap_start", "_heap_end", "_heap")):
    if i[0] in results and i[1] in results:
      start = results[i[0]]
      end = results[i[1]]
      results[i[2]] = (min(start[0], end[0]), abs(end[0]-start[0]), start[2], i[2])
      del results[i[0]]
      del results[i[1]]
  return results

def demangle_symbols(symbols):
  symbol_names = []
  for name in symbols.keys():
    symbol_names.append(name)
  symbols_encoded = "\n".join(symbol_names).encode('utf-8')
  demangled_output = subprocess.check_output(["c++filt"], input=symbols_encoded).decode('utf-8').splitlines()
  demangled_symbols = {}
  cpt=0
  for symbol in symbols.values():
    demangled_symbols[demangled_output[cpt]] = (symbol[0], symbol[1], symbol[2], demangled_output[cpt])
    cpt += 1
  return demangled_symbols

def format_kb(i):
  return ("%.3f KiB" % (i/1024))

def plot_symbols(symbols, range_start, range_end):
  fig,ax = plt.subplots()
  cpt = 0
  for symbol in symbols.values():
    symbol_name = symbol[3].lstrip("_")
    symbol_color=(random.uniform(0,1),random.uniform(0,1),random.uniform(0,1))
    ax.broken_barh([(symbol[0], symbol[1])], (0, 1), color=symbol_color, label=symbol_name + " - " + format_kb(symbol[1]))
    cpt += 1
  ax.set_yticks([])
  ax.set_xticks(list(range(range_start, range_end+1, int((range_end-range_start)/16))))
  xlabels = map(lambda t: '0x%08X' % int(t), ax.get_xticks())
  ax.set_xticklabels(xlabels);
  ax.legend()
  fig.set_size_inches(20, 2)
  return fig

data=load_symbols(sys.argv[1])
data=demangle_symbols(data)
data = filter_set(data, pred_ram)
data = filter_set(data, pred_size)

fig=plot_symbols(data,0x20000000,0x20040000)

fig.show()
input()

