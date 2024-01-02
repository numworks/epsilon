# coding=utf-8
# Translate ELF file into DfuSe file

import sys
import struct
import zlib  # for CRC-32 calculation
import subprocess
import re
import argparse
import os

# arm-none-eabi-objdump -h -w file.elf
# arm-none-eabi-objcopy -O binary -j .data file.elf file.bin


# We can sort sections on their address prefix or on their name suffix
def loadable_sections(elf_file, address_prefix="", name_suffix=""):
    objdump_section_headers_pattern = re.compile(
        "^\s+\d+\s+(\.[\w\.]*"
        + name_suffix
        + ")\s+([0-9a-f]+)\s+([0-9a-f]+)\s+("
        + address_prefix
        + "[0-9a-f]+)\s+([0-9a-f]+).*LOAD",
        flags=re.MULTILINE,
    )
    objdump_output = subprocess.check_output(
        ["arm-none-eabi-objdump", "-h", "-w", elf_file]
    ).decode("utf-8")
    sections = []
    for name, size, vma, lma, offset in re.findall(
        objdump_section_headers_pattern, objdump_output
    ):
        int_size = int(size, 16)
        if int_size > 0:
            sections.append(
                {
                    "name": name,
                    "size": int_size,
                    "vma": int(vma, 16),
                    "lma": int(lma, 16),
                    "offset": int(offset, 16),
                }
            )
    return sections


def generate_dfu_file(targets, usb_vid_pid, dfu_file):
    data = b""
    for t, target in enumerate(targets):
        target_data = b""
        alt_setting = 0
        for image in target:
            if image["address"] >= 0x20000000 and image["address"] < 0x20040000:
                # sRAM corresponds to the alternate setting 1
                alt_setting = 1
            # Pad the image to 8 bytes, this seems to be needed
            pad = (8 - len(image["data"]) % 8) % 8
            image["data"] = image["data"] + bytes(bytearray(8)[0:pad])
            target_data += (
                struct.pack("<2I", image["address"], len(image["data"])) + image["data"]
            )
        target_data = (
            struct.pack(
                "<6sBI255s2I",
                b"Target",
                alt_setting,
                1,
                b"ST...",
                len(target_data),
                len(target),
            )
            + target_data
        )
        data += target_data
    data = struct.pack("<5sBIB", b"DfuSe", 1, len(data) + 11, len(targets)) + data
    v, d = map(lambda x: int(x, 0) & 0xFFFF, usb_vid_pid.split(":"))
    data += struct.pack("<4H3sB", 0, d, v, 0x011A, b"UFD", 16)
    crc = 0xFFFFFFFF & -zlib.crc32(data) - 1
    data += struct.pack("<I", crc)
    open(dfu_file, "wb+").write(data)


def bin_file_for_path(elf_file, name_extension=""):
    return (
        "firmware."
        + os.path.splitext(os.path.basename(elf_file))[0]
        + "_"
        + name_extension
        + ".bin"
    )


def print_sections(sections):
    for s in sections:
        print(
            "\t\t%s-%s: %s, %s"
            % (
                hex(s["lma"]),
                hex(s["lma"] + s["size"] - 1),
                s["name"],
                "{:,} bytes".format(s["size"]),
            )
        )


def print_block(sections, address, dataSize):
    print("--- Block ---")
    print("\tSections:")
    print_sections(sections)
    print("\tData size: " + str(dataSize) + " (" + str(hex(dataSize)) + ")")
    print("\tAddress: " + str(hex(address)))
    print("\n")


def add_sections_to_targets(targets, sections, elf_file, verbose):
    # Find the section address
    address = min([s["lma"] for s in sections])
    # We turn ITCM flash addresses to equivalent AXIM flash addresses because
    # ITCM address cannot be written and are physically equivalent to AXIM flash
    # addresses.
    if address >= 0x00200000 and address < 0x00210000:
        address = address - 0x00200000 + 0x08000000

    # Extract the section datas
    command = ["arm-none-eabi-objcopy", "-O", "binary"]
    for s in sections:
        command.append("-j")
        command.append(s["name"])
    bin_file = bin_file_for_path(elf_file)
    command += [elf_file, bin_file]
    subprocess.call(command)
    data = open(bin_file, "rb").read()

    # Append to targets
    targets.append({"address": address, "data": data})
    if verbose:
        print_block(
            sections,
            address,
            len(data),
        )
    subprocess.call(["rm", bin_file])


PERSISTING_BYTES_SECTION_NAME = "persisting_bytes_buffer"


def elf2dfu(elf_files, usb_vid_pid, dfu_file, verbose):
    targets = []
    for elf_file in elf_files:
        sections = loadable_sections(elf_file)
        if len(sections) == 0:
            sys.stderr.write(
                "Error: the elf file " + elf_file + " has no loadable section\n"
            )
            sys.exit(-1)

        # Handle the fact that the userland.elf also contains the persisting_bytes section
        if sum(1 for s in sections if PERSISTING_BYTES_SECTION_NAME in s["name"]) > 0:
            persisting_bytes_sections = loadable_sections(
                elf_file, "", PERSISTING_BYTES_SECTION_NAME
            )
            add_sections_to_targets(
                targets, persisting_bytes_sections, elf_file, verbose
            )
            sections = [s for s in sections if s not in persisting_bytes_sections]
        if len(sections) > 0:
            add_sections_to_targets(targets, sections, elf_file, verbose)

    # Sort targets by increasing address
    sorted_targets = sorted(targets, key=lambda t: t["address"])
    generate_dfu_file([sorted_targets], usb_vid_pid, dfu_file)


parser = argparse.ArgumentParser(description="Convert an ELF file to DfuSe.")
parser.add_argument("-i", metavar="ELF_FILE", help="Input ELF file", nargs="+")
parser.add_argument("-o", metavar="DFU_FILE", help="Output DfuSe file")
parser.add_argument("-v", "--verbose", action="store_true", help="Show verbose output")

args = parser.parse_args()
elf2dfu(args.i, "0x0483:0xa291", args.o, args.verbose)
