import argparse

from helpers.args_types import *
from helpers.nws_helper import *


parser = argparse.ArgumentParser(description="This script converts a .txt into a .nws")
parser.add_argument(
    "state_file",
    metavar="TXT_STATE_FILE",
    type=existing_file,
    help="textual state file (with extension .txt)",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Convert the nws into txt
    convert_txt_to_nws(args.state_file)


if __name__ == "__main__":
    main()
