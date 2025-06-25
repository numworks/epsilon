import sys, os, shutil, argparse

from helpers.args_types import *
from helpers.crc_helper import *
from helpers.screenshot_helper import *


parser = argparse.ArgumentParser(
    description="This script adds a scenario to the test screenshots dataset. It takes a state file, computes its crc32 and place them in a subfolder of the screenshots dataset."
)
parser.add_argument(
    "executable",
    metavar="EXE",
    type=existing_file,
    help="epsilon executable",
)
parser.add_argument(
    "state_file",
    metavar="STATE_FILE",
    type=existing_state_file,
    help="state file (with extension .nws)",
)
parser.add_argument(
    "-n",
    "--name",
    help="name of the subfolder (if no name is given, we take the name of the state file)",
)


def main():
    # Parse args
    args = parser.parse_args()
    name = args.name
    if name is None:
        # If no name was given, we take the name of the state file
        name = os.path.splitext(args.state_file)[0]

    # Create new folder
    new_dir = folder(name)
    if os.path.exists(new_dir):
        print("Error:", new_dir, "already exists. Choose another name.")
        sys.exit(1)
    print("Creating folder", new_dir)
    os.mkdir(new_dir)

    # Add the state file
    new_state_file_path = os.path.join(new_dir, "scenario.nws")
    shutil.copy(args.state_file, new_state_file_path)

    # Compute the corresponding crc32
    crc_file = os.path.join(new_dir, "crc32.txt")
    compute_and_store_crc32(new_state_file_path, args.executable, crc_file)


if __name__ == "__main__":
    main()
