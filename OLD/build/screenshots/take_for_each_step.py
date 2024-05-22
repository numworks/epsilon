import argparse

from helpers.args_types import *
from helpers.screenshot_helper import *


parser = argparse.ArgumentParser(
    description="This script takes a screenshot for each event of a scenario and creates a gif."
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
    "-o", "--output_folder", default="screenshots_each_step", help="output folder"
)


def main():
    args = parser.parse_args()
    generate_all_screenshots_and_create_gif(
        args.state_file, args.executable, args.output_folder, False
    )


if __name__ == "__main__":
    main()
