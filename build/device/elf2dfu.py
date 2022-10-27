#coding=utf-8
# Translate ELF file into DfuSe file

import sys
import struct
import zlib # for CRC-32 calculation
import subprocess
import re
import argparse
import os

# arm-none-eabi-objdump -h -w file.elf
# arm-none-eabi-objcopy -O binary -j .data file.elf file.bin

# We can sort sections on their address prefix or on their name suffix
def loadable_sections(elf_file, address_prefix = "", name_suffix = ""):
  objdump_section_headers_pattern = re.compile("^\s+\d+\s+(\.[\w\.]*"+name_suffix+")\s+([0-9a-f]+)\s+([0-9a-f]+)\s+("+address_prefix+"[0-9a-f]+)\s+([0-9a-f]+).*LOAD", flags=re.MULTILINE)
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
    alt_setting = 0
    for image in target:
      if (image['address'] >= 0x20000000 and image['address'] < 0x20040000):
        # sRAM corresponds to the alternate setting 1
        alt_setting = 1
      # Pad the image to 8 bytes, this seems to be needed
      pad = (8 - len(image['data']) % 8 ) % 8
      image['data'] = image['data'] + bytes(bytearray(8)[0:pad])
      target_data += struct.pack('<2I', image['address'], len(image['data'])) + image['data']
    target_data = struct.pack('<6sBI255s2I', b'Target', alt_setting, 1, b'ST...', len(target_data), len(target)) + target_data
    data += target_data
  data = struct.pack('<5sBIB', b'DfuSe', 1, len(data) + 11, len(targets)) + data
  v, d = map(lambda x: int(x,0) & 0xFFFF, usb_vid_pid.split(':'))
  data += struct.pack('<4H3sB', 0, d, v, 0x011a, b'UFD', 16)
  crc = (0xFFFFFFFF & -zlib.crc32(data) - 1)
  data += struct.pack('<I', crc)
  open(dfu_file, 'wb+').write(data)

def bin_file_for_path(elf_file, name_extension = ""):
    return "firmware." + os.path.splitext(os.path.basename(elf_file))[0] + "_" + name_extension + ".bin"

def print_sections(sections):
    for s in sections:
        print("\t\t%s-%s: %s, %s" % (hex(s['lma']), hex(s['lma'] + s['size'] - 1), s['name'], "{:,} bytes".format(s['size'])))

def print_block(block, address, dataSize):
  print("Block name: "+ block['name'])
  print("\tSections:")
  print_sections(block['sections'])
  print("\tData size: " + str(dataSize) + " (" + str(hex(dataSize)) + ")")
  print("\tAddress: " + str(hex(address)))
  print("\n")

def elf2target_single_block(elf_file, verbose):
  single_block = {'sections': loadable_sections(elf_file)}
  # Early escape for empty blocks
  if not single_block['sections']:
      sys.stderr.write("Error: the elf file " + elf_file + "has no loadable section\n")
      sys.exit(-1)
  subprocess.call(["arm-none-eabi-objcopy", "-O", "binary", elf_file, bin_file_for_path(elf_file)])
  address = min([section['lma'] for section in single_block['sections']])
  # We turn ITCM flash addresses to equivalent AXIM flash addresses because
  # ITCM address cannot be written and are physically equivalent to AXIM flash
  # addresses.
  if (address >= 0x00200000 and address < 0x00210000):
    address = address - 0x00200000 + 0x08000000

  data = open(bin_file_for_path(elf_file), "rb").read()
  target = {'address': address, 'data': data}
  if verbose:
    print_block(single_block, address, len(data))
  subprocess.call(["rm", bin_file_for_path(elf_file)])
  return target

def standard_elf2dfu(elf_files, usb_vid_pid, dfu_file, verbose):
  targets = []
  for elf_file in elf_files:
    targets.append(elf2target_single_block(elf_file, verbose))
  generate_dfu_file([targets], usb_vid_pid, dfu_file)

def customized_elf2dfu(elf_files, usb_vid_pid, dfu_file, verbose):
  bootloader_elf_file = [f for f in elf_files if "bootloader" in f]
  kernel_elf_files = [f for f in elf_files if "kernel" in f]
  userland_elf_files = [f for f in elf_files if "userland" in f]
  if len(bootloader_elf_file) > 1:
    sys.stderr.write("Extra bootloader elf file")
    sys.exit(-1)
  if len(kernel_elf_files) != 2:
    sys.stderr.write("Missing or extra kernel elf file")
    sys.exit(-1)
  if len(userland_elf_files) != 2:
    sys.stderr.write("Missing or extra userland elf file")
    sys.exit(-1)

  targets = []

  # Bootloader
  if len(bootloader_elf_file) > 0:
    targets.append(elf2target_single_block(bootloader_elf_file[0], verbose))

  for i in [0,1]:
    kernel_elf_file = kernel_elf_files[i]
    userland_elf_file = userland_elf_files[i]
    persisting_bytes_block = {'sections': loadable_sections(kernel_elf_file, "", "persisting_bytes_buffer")}
    kernel_block = {'sections': [s for s in loadable_sections(kernel_elf_file) if s not in persisting_bytes_block['sections']]}
    userland_block = {'sections': [s for s in loadable_sections(userland_elf_file)]}
    blocks = {'persisting_bytes': persisting_bytes_block, 'kernel': kernel_block, 'userland': userland_block}
    block_names = blocks.keys();
    for name in block_names:
      block = blocks[name]
      # Error if one block is empty
      if not block['sections']:
        sys.stderr.write("Error: the block " + block['name'] + " has no loadable section\n")
        sys.exit(-1)
      # Fill the address field of each block
      elf_file = userland_elf_file if name == "userland" else kernel_elf_file
      blocks[name]['bin_file'] = bin_file_for_path(elf_file, name)
      subprocess.call(["arm-none-eabi-objcopy", "-O", "binary"]+[item for sublist in [["-j", s['name']] for s in block['sections']] for item in sublist]+[elf_file, blocks[name]['bin_file']])
      blocks[name]['address'] = min([s['lma'] for s in block['sections']])

    for name in block_names:
      block = blocks[name]
      data = open(blocks[name]['bin_file'], "rb").read()
      targets.append({'address': block['address'], 'data': data})
      if verbose:
        print_block(block, block['address'], len(data),)

    for name in block_names:
      block = blocks[name]
      subprocess.call(["rm", blocks[name]['bin_file']])

  generate_dfu_file([targets], usb_vid_pid, dfu_file)


parser = argparse.ArgumentParser(description="Convert an ELF file to DfuSe.")
parser.add_argument('-i', metavar='ELF_FILE', help='Input ELF file', nargs='+')
parser.add_argument('-o', metavar='DFU_FILE', help='Output DfuSe file')
parser.add_argument('--custom', action='store_true', help='Combine bootloader, kernel and userland elf files into one special signed dfu file')
parser.add_argument('-v', '--verbose', action="store_true", help='Show verbose output')

args = parser.parse_args()
if args.custom:
  customized_elf2dfu(args.i, "0x0483:0xa291", args.o, args.verbose)
else:
  standard_elf2dfu(args.i, "0x0483:0xa291", args.o, args.verbose)
