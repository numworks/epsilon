#!/usr/bin/env python3

from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from elftools.elf.constants import SH_FLAGS
import argparse
import os
import re
import urllib.parse

EXCLUDED_APPS = {'Escher', 'HardwareTest', 'Home', 'OnBoarding', 'USB'}

# binary_size.py \
#        --sections @Flash .text .rodata @RAM .bss .data
#        @Base foo_a.elf foo_b.elf @Head bar_a.elf bar_b.elf

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

# Convert a flat list '@foo', 'bar', 'baz', '@faa', 'boo' into a nested list
# [['foo', ['bar', 'baz']], ['faa', ['boo']]]. We use it to pass dict-like data
# on the command line.
def named_groups(tagged_list):
  groups = []
  current_group_name = None
  current_group = []
  for item in tagged_list:
    if item.startswith('@'):
      if current_group:
        groups.append([current_group_name, current_group])
      current_group_name = item[1:]
      current_group = []
      continue
    current_group.append(item)
  groups.append([current_group_name, current_group])
  return groups

def section_size(file, section_name):
  elffile = ELFFile(open(file, 'rb'))
  for section in elffile.iter_sections():
    if section.name == section_name:
      return section.data_size
  return None

def app_sizes(file):
  sizes = {}
  elffile = ELFFile(open(file, 'rb'))
  dwarf = elffile.get_dwarf_info()
  for cu in dwarf.iter_CUs():
    if not cu.get_top_DIE().get_full_path().endswith("/app.cpp"):
      continue
    for die in cu.iter_DIEs():
      if die.tag == 'DW_TAG_class_type' and die.get_full_path() == 'App':
        if "DW_AT_byte_size" in die.attributes:
          namespace = die.get_parent().get_full_path()
          if namespace in EXCLUDED_APPS: continue
          app_size = die.attributes["DW_AT_byte_size"].value
          for child in die.iter_children():
            if child.tag == 'DW_TAG_class_type' and child.get_full_path() == 'Snapshot':
              snapshot_size = child.attributes["DW_AT_byte_size"].value
              sizes[namespace] = (app_size, snapshot_size)
              break
  return sizes

def format_html_apps_table(sizes):
  output = ""
  output += '<table>'
  apps = next(iter(sizes.values()))
  output += '<tr><th/>'
  for tag in sizes.keys():
    output += f'<th align="left">{tag}</th>'
  add_delta = len(sizes) == 2
  if add_delta:
    output += '<th align="left">Delta</th>'
    before, after = sizes
  output += '</tr>'
  for type_id, type_name in enumerate(('App','Snapshot')):
    output += f'<tr><th align="left">{type_name}</th></tr>'
    for app in apps:
      output += '<tr>'
      output += f'<td>{app}</td>'
      for size in sizes.values():
        output += f'<td align="right">{size[app][type_id]:_}</td>'
      if add_delta:
        delta = sizes[after][app][type_id] - sizes[before][app][type_id]
        if delta:
          output += f'<td align="right">{delta:+_}</td>'
      output += '</tr>'

  output += '</table>'
  return output

def format_html_section_table(grouped_sections, grouped_files, file_section_sizes, show_file_detail):
  output = ""
  output += '<table>'

  show_section_groups = (len(grouped_sections) > 1)
  show_file_groups = (len(grouped_files) > 1)
  show_file_detail = not show_file_detail
  show_group_size_diff = True
  show_section_size_diff = False

  if show_section_groups:
    # Header title row (e.g. "Flash", "RAM")
    output += '<tr>'
    output += f'<td rowspan="2" colspan="{show_section_groups + show_file_detail}"></td>'
    for title,sections in grouped_sections:
      output += f'<th colspan="{len(sections)}" align="left">{title}</th>'
    output += '</tr>'

  # Header sections row (e.g. ".text", ".rodata")
  output += '<tr>'
  if not show_section_groups:
    if show_file_groups or show_file_detail:
      output += f'<td colspan="{show_file_groups + show_file_detail}"></td>'
  for title,sections in grouped_sections:
    for section in sections:
      output += f'<td>{section}</td>'
  output += '</tr>'

  reference_sizes = None
  for group_index,group in enumerate(grouped_files):
    title, files = group
    if show_file_detail:
      for i,file in enumerate(files):
        output += '<tr>'
        current_sizes = []
        if i == 0 and show_file_groups:
          output += f'<td rowspan="{len(files)}">{title}</td>'
        if show_file_detail:
          output += f'<td>{os.path.basename(file)}</td>'
        for _,sections in grouped_sections:
          for section in sections:
            size = file_section_sizes[file][section]
            output += f'<td align="right">{format_bytes(size)}</td>'
            current_sizes.append(size)
        output += '</tr>'
    else:
      output += '<tr>'
      current_sizes = []
      if show_file_groups:
        output += f'<td>{title}</td>'
      for _,sections in grouped_sections:
        for section in sections:
          section_size = 0
          for file in files:
            section_size += file_section_sizes[file][section]
          current_sizes.append(section_size)
          output += f'<td align="right">{format_bytes(section_size)}</td>'
      output += '</tr>'
      if group_index == 0:
        reference_sizes = current_sizes
      if show_section_size_diff and group_index > 0:
        output += '<tr>'
        output += f'<td colspan="{show_file_groups+show_file_detail}"></td>'
        for index,size in enumerate(current_sizes):
          output += f'<td align="right">{format_bytes(size-reference_sizes[index], force_sign=True)}</td>'
        output += '</tr>'
      if show_group_size_diff and group_index > 0:
        output += '<tr>'
        output += f'<td colspan="{show_file_groups+show_file_detail}"></td>'
        section_index_start = 0
        for _,sections in grouped_sections:
          total_reference_size = 0
          total_size = 0
          for i in range(len(sections)):
            total_reference_size += reference_sizes[section_index_start+i]
            total_size += current_sizes[section_index_start+i]
          section_index_start += len(sections)
          output += f'<th colspan="{len(sections)}" align="left">{format_bytes(total_size-total_reference_size, force_sign=True)}</th>'
        output += '</tr>'

  output += '</table>'
  return output

# Argument parsing

parser = argparse.ArgumentParser(description='Compute binary size metrics')
parser.add_argument('files', type=str, nargs='+', help='an ELF file')
parser.add_argument('--sections', type=str, nargs='+', help='Section (prefix) to list')
parser.add_argument('--summarize', action=argparse.BooleanOptionalAction, help='Show a summarized version')
parser.add_argument('--app-sizes', action=argparse.BooleanOptionalAction, help='Show app sizes')

args = parser.parse_args()

grouped_files = named_groups(args.files)

if args.app_sizes:
  sizes = {}
  for tag,files in grouped_files:
    assert len(files) == 1 and "userland" in files[0]
    sizes[tag] = app_sizes(files[0])
  print(format_html_apps_table(sizes))
else:
  grouped_sections = named_groups(args.sections)
  # For each file, compute the size of each section
  file_section_sizes = {}
  for _,files in grouped_files:
    for file in files:
      file_section_sizes.setdefault(file, {})
      for _,sections in grouped_sections:
        for section in sections:
          file_section_sizes[file][section] = section_size(file, section)
  print(format_html_section_table(grouped_sections, grouped_files, file_section_sizes, args.summarize))
