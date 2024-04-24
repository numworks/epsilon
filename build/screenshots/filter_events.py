import argparse

from helpers.args_types import *
from helpers.nws_helper import *


parser = argparse.ArgumentParser(
    description="This script filters events in a state file."
)
parser.add_argument(
    "state_file",
    metavar="STATE_FILE",
    type=existing_state_file,
    help="state file (with extension .nws)",
)
parser.add_argument(
    "events",
    metavar="EVENTS",
    type=str,
    nargs="+",
    help="list of events to filter",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Create temp file
    temp_txt = os.path.join("temp.txt")

    # Convert the nws into txt and filter events
    convert_nws_to_txt(args.state_file, temp_txt)

    # Convert the txt back into nws
    print(args.events)
    convert_txt_to_nws(temp_txt, args.state_file, args.events)

    # Clean folder
    os.remove(temp_txt)

    print(args.state_file + " was successfully edited !")


if __name__ == "__main__":
    main()
