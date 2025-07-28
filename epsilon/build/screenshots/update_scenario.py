import sys, os, argparse

from helpers.args_types import *
from helpers.print_format import *
from helpers.crc_helper import *
from helpers.screenshot_helper import *

parser = argparse.ArgumentParser(
    description="This script updates the crc32 of a scenario in the test screenshots dataset."
)
parser.add_argument(
    "executable",
    metavar="EXE",
    type=existing_file,
    help="epsilon executable",
)
parser.add_argument(
    "name",
    metavar="NAME",
    help="name of scenario folder in the test screenshots dataset",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Print warning
    print_bold("Your are updating the crc32 of a scenario.")
    print_bold(
        "Please make sure before launching this script that you checked screenshots AT EACH STEP and with THE SAME executable."
    )
    print_bold(
        "If you have a doubt, run take_for_each_step.py with the same executable."
    )
    print("-------")

    scenario_folder = folder(args.name)
    if not os.path.isdir(scenario_folder):
        print("Error:", args.name, "is not a folder in the test screenshots dataset")
        sys.exit(1)
    state_file = get_file_with_extension(scenario_folder, ".nws")
    crc_file = get_file_with_extension(scenario_folder, ".txt")
    if state_file == "" or crc_file == "":
        print("Error:", scenario_folder, "should contain one .nws and one .txt")
        sys.exit(1)

    # Update crc32
    print("Rewriting", crc_file)
    compute_and_store_crc32(state_file, args.executable, crc_file)


if __name__ == "__main__":
    main()
