#coding=utf-8
# Translate ELF file into DfuSe file

import sys
import struct
import zlib # for CRC-32 calculation
import subprocess
import re
import argparse

# arm-none-eabi-objdump -h -w file.elf
# arm-none-eabi-objcopy -O binary -j .data file.elf file.bin

def loadable_sections(elf_file, address_prefix = ""):
  objdump_section_headers_pattern = re.compile("^\s+\d+\s+(\.[\w\.]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+("+address_prefix+"[0-9a-f]+)\s+([0-9a-f]+).*LOAD", flags=re.MULTILINE)
  objdump_output = subprocess.check_output(["arm-none-eabi-objdump", "-h", "-w", elf_file]).decode('utf-8')
  sections = []
  for (name, size, vma, lma, offset) in re.findall(objdump_section_headers_pattern, objdump_output):
    int_size = int(size,16)
    if (int_size > 0):
      sections.append({'name': name, 'size': int_size, 'vma': int(vma,16), 'lma': int(lma,16), 'offset':int(offset,16)})
  return sections

def generate_dfu_file(targets, usb_vid_pid, dfu_file):
  data = b''
  for t, target in enumerate(targets):
    target_data = b''
    for image in target:
      # Pad the image to 8 bytes, this seems to be needed
      pad = (8 - len(image['data']) % 8 ) % 8
      image['data'] = image['data'] + bytes(bytearray(8)[0:pad])
      target_data += struct.pack('<2I', image['address'], len(image['data'])) + image['data']
    target_data = struct.pack('<6sBI255s2I', b'Target', 0, 1, b'ST...', len(target_data), len(target)) + target_data
    data += target_data
  data = struct.pack('<5sBIB', b'DfuSe', 1, len(data) + 11, len(targets)) + data
  v, d = map(lambda x: int(x,0) & 0xFFFF, usb_vid_pid.split(':'))
  data += struct.pack('<4H3sB', 0, d, v, 0x011a, b'UFD', 16)
  crc = (0xFFFFFFFF & -zlib.crc32(data) - 1)
  data += struct.pack('<I', crc)
  open(dfu_file, 'wb').write(data)

def bin_file(elf_file, block):
  name_without_extension = ".".join(elf_file.split(".")[:-1])
  return name_without_extension + block['name'] + ".bin"

def print_sections(sections):
  for s in sections:
      print("%s-%s: %s, %s" % (hex(s['lma']), hex(s['lma'] + s['size'] - 1), s['name'], "{:,} bytes".format(s['size'])))

def elf2dfu(elf_file, usb_vid_pid, dfu_file, verbose):
  external_address_prefix = "9"; # External addresses start with 0x9
  # We don't sort sections on their names (.external, .internal) but on their
  # addresses because some sections like dfu_entry_point can either be the
  # internal or the external flash depending on which targets is built (ie
  # flasher executes dfu in place but epsilon executes dfu relocated in RAM)
  external_block = {'name': "external", 'sections': loadable_sections(elf_file, external_address_prefix)}
  internal_block = {'name': "internal", 'sections': [s for s in loadable_sections(elf_file) if s not in external_block['sections']]}
  blocks = [external_block, internal_block]
  blocks = [b for b in blocks if b['sections']]
  if verbose:
    for b in blocks:
     print(b['name'])
     print_sections(b['sections'])
  targets = []
  for b in blocks:
    name = b['name']
    subprocess.call(["arm-none-eabi-objcopy", "-O", "binary"]+[item for sublist in [["-j", s['name']] for s in b['sections']] for item in sublist]+[elf_file, bin_file(elf_file, b)])
    address = min([s['lma'] for s in b['sections']])
    # We turn ITCM flash addresses to equivalent AXIM flash addresses because
    # ITCM address cannot be written and are physically equivalent to AXIM flash
    # addresses.
    if (address >= 0x00200000 and address < 0x00210000):
      address = address - 0x00200000 + 0x08000000
    # TODO: should we pad binary files
    # (We pad the device binary files because there was a bug in an older
    # version (< 1.8.0) of the dfu code, and it did not upload properly a binary
    # of length non-multiple of 32 bits.
    # open(bin_file(elf_file, b), "a").write("\xFF\xFF\xFF\xFF")
    targets.append({'address': address, 'name': name, 'data': open(bin_file(elf_file, b), "rb").read()})
  generate_dfu_file([targets], usb_vid_pid, dfu_file)
  for b in blocks:
    subprocess.call(["rm", bin_file(elf_file, b)])

parser = argparse.ArgumentParser(description="Convert an ELF file to DfuSe.")
parser.add_argument('elf_file', help='Input ELF file')
parser.add_argument('dfu_file', help='Output DfuSe file')
parser.add_argument('-v', '--verbose', action="store_true", help='Show verbose output')
args = parser.parse_args()
elf2dfu(args.elf_file, "0x0483:0xa291", args.dfu_file, args.verbose)
