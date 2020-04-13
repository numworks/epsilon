#!/usr/bin/env python3

import argparse
import os
import re
import subprocess
import urllib.parse

# ELF analysis

def loadable_sections(elf_file):
  objdump_section_headers_pattern = re.compile("^\s+\d+\s+(\.[\w\.]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)", flags=re.MULTILINE)
  objdump_output = subprocess.check_output(["arm-none-eabi-objdump", "-h", "-w", elf_file]).decode('utf-8')
  sections = []
  for (name, size, vma, lma, offset) in re.findall(objdump_section_headers_pattern, objdump_output):
    int_size = int(size,16)
    if (int_size > 0):
      sections.append({'name': name, 'size': int_size, 'vma': int(vma,16), 'lma': int(lma,16), 'offset':int(offset,16)})
  return sections


# Data filtering

def row_for_elf(elf, requested_section_prefixes):
  sections = loadable_sections(elf)
  result = {}
  for prefix in requested_section_prefixes:
    for s in sections:
      section_name = s['name']
      if s['name'].startswith(prefix):
        if not prefix in result:
          result[prefix] = 0
        result[prefix] += s['size']
  return result


# String formatting

def iso_separate(string):
  space = ' ' # We may want to use a thin non-breaking space as thousands separator
  return string.replace('_',space).replace('+','+'+space).replace('-','-'+space)

def format_bytes(value, force_sign=False):
  if value is None:
    return ''
  number_format = '{:'
  if force_sign:
    number_format += '+'
  number_format += '_} bytes'
  return iso_separate(number_format.format(value))

def format_percentage(value):
  if value is None:
    return ''
  return iso_separate("{:+.1f} %".format(100*value))


# Markdown

def emphasize(string):
  if string:
    return '_' + string + '_'
  else:
    return ''

def strong(string):
  if string:
    return '**' + string + '**'
  else:
    return ''


# Deltas

def absolute_delta(x,y):
  if x is None or y is None:
    return None
  return x-y

def ratio_delta(x,y):
  if x is None or y is None:
    return None
  return (x-y)/y


# Table formatting

def format_row(row, header=False):
  result = '|'
  if header:
    result += strong(header)
  result += '|'
  for v in row:
    result += v
    result += '|'
  result += '\n'
  return result

def format_table(table):
  base = table[0]['values']
  listed_sections = base.keys()
  result = ''
  result += format_row(listed_sections)
  result += '|-|' + '-:|'*len(listed_sections) + '\n'
  for i,row in enumerate(table):
    v = row['values']
    result += format_row((format_bytes(v.get(s)) for s in listed_sections), header=row['label'])
    if i != 0:
      result += format_row(emphasize(format_bytes(absolute_delta(v.get(s), base.get(s)), force_sign=True)) for s in listed_sections)
      result += format_row(emphasize(format_percentage(ratio_delta(v.get(s), base.get(s)))) for s in listed_sections)
  return result


# Argument parsing

parser = argparse.ArgumentParser(description='Compute binary size metrics')
parser.add_argument('files', type=str, nargs='+', help='an ELF file')
parser.add_argument('--labels', type=str, nargs='+', help='label for ELF file')
parser.add_argument('--sections', type=str, nargs='+', help='Section (prefix) to list')
parser.add_argument('--escape', action='store_true', help='Escape the output')
args = parser.parse_args()


# Execution

table = []
for i,filename in enumerate(args.files):
  label = os.path.basename(filename)
  if args.labels and i < len(args.labels):
    label = args.labels[i]
  table.append({'label': label, 'values': row_for_elf(filename, args.sections)})
formatted_table = format_table(table)

if args.escape:
  print(urllib.parse.quote(formatted_table, safe='| :*+'))
else:
  print(formatted_table)
