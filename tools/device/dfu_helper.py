# Common DFU file generation utilities

import struct
import zlib


def generate_dfu_file(targets, usb_vid_pid, dfu_file, altsetting_offset):
    """Generate a DFU file from a list of targets.

    Args:
        targets: List of target lists. Each target is a list of dicts with 'address' and 'data' keys.
        usb_vid_pid: String in format "VID:PID" (e.g., "0x0483:0xDF11")
        dfu_file: Path to output DFU file
        altsetting_offset: Offset for alternate setting numbering
    """
    data = b""
    for t, target in enumerate(targets):
        target_data = b""
        alt_setting = altsetting_offset
        for image in target:
            # TODO: this should be somewhere in epsilon
            if (image["address"] >= 0x20000000 and image["address"] < 0x20040000) or (
                image["address"] >= 0x24000000 and image["address"] < 0x24040000
            ):
                # sRAM alt setting is one greater than flash alt setting
                alt_setting = altsetting_offset + 1
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
