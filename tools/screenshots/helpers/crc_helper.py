from subprocess import Popen, DEVNULL, PIPE

from helpers.print_format import *


def compute_crc32_process(state_file, executable):
    return Popen(
        "./"
        + executable
        + " --headless --load-state-file "
        + state_file
        + " --limit-stack-usage --compute-hash --hide-events",
        shell=True,
        stdout=PIPE,
        stderr=DEVNULL,
    )


def find_crc32_in_log(stdout):
    output = stdout.read().decode()
    if len(output.splitlines()) != 1 or "CRC32 of all screenshots: " not in output:
        print(
            bold("Error:"),
            "couldn't find crc32 in log. The scenario may have failed an assert or raised a segfault.",
        )
        return ""
    return output.split()[-1]


def compute_crc32(state_file, executable):
    print("Computing crc32 of", state_file)
    p = compute_crc32_process(state_file, executable)
    return find_crc32_in_log(p.stdout)


def store_crc32(crc32, crc_file):
    with open(crc_file, "w") as f:
        f.write(crc32)


def compute_and_store_crc32(state_file, executable, crc_file):
    crc32 = compute_crc32(state_file, executable)
    store_crc32(crc32, crc_file)
