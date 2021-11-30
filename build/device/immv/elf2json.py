#!/usr/bin/env python3

from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from elftools.elf.constants import SH_FLAGS
import argparse
import json
import os

def sections(elffile):
  zones = []
  for section in elffile.iter_sections():
    if not section['sh_flags'] & SH_FLAGS.SHF_ALLOC:
      continue
    if section.data_size == 0:
      continue
    name = section.name
    start = section['sh_addr']
    end = start + section.data_size
    zones.append({"name": name, "start": start, "end": end})
  return zones

def symbols(elffile):
  zones = []
  for section in elffile.iter_sections():
    if isinstance(section, SymbolTableSection):
      for symbol in section.iter_symbols():
        name = symbol.name
        start = symbol.entry['st_value']
        end = start + symbol.entry['st_size']
        zones.append({"name": name, "start": start, "end": end})
  return zones

# Argument parsing
parser = argparse.ArgumentParser(description='Parse ELF file and output metrics in JSON')
parser.add_argument('files', type=str, nargs='+', help='an ELF file')
parser.add_argument('-s', '--symbols', action='store_true', help='output each symbol name, start and end')
parser.add_argument('-S', '--sections', action='store_true', help='output each section name, start and end')
args = parser.parse_args()

# Execution
table = []
for file in args.files:
  elffile = ELFFile(open(file, 'rb'))
  zones = []
  if args.sections:
    zones.extend(sections(elffile))
  if args.symbols:
    zones.extend(symbols(elffile))
  zones.sort(key=lambda z:z["start"])

  result = {
    "name": os.path.basename(file),
    "start": min(map(lambda z:z["start"], zones)),
    "end": max(map(lambda z:z["end"], zones)),
    "zones": zones
  }
  print(json.dumps(result))
#and i < len(args.labels):
#    label = args.labels[i]
#  table.append({'label': label, 'values': row_for_elf(filename, args.sections)})
#formatted_table = format_table(table)
