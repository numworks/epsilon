import argparse

from helpers.args_types import *
from helpers.nws_helper import *


parser = argparse.ArgumentParser(description="This script converts a .nws into a .txt")
parser.add_argument(
    "state_file",
    metavar="STATE_FILE",
    type=existing_state_file,
    help="state file (with extension .nws)",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Convert the nws into txt
    convert_nws_to_txt(args.state_file)


if __name__ == "__main__":
    main()
