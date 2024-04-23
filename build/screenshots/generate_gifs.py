import os, shutil, glob, argparse

from helpers.args_types import *
from helpers.miscellaneous import *
from helpers.screenshot_helper import *

parser = argparse.ArgumentParser(
    description="This script generates gifs for all scenarios in the screenshots test folder."
)
parser.add_argument(
    "executable",
    metavar="EXE",
    type=existing_file,
    help="epsilon executable to test",
)
parser.add_argument(
    "-f",
    "--filter",
    default="",
    help="run only scenarios with a name starting with filter argument",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Create output folder
    output_folder = "generate_gifs_output"
    clean_or_create_folder(output_folder)

    regex = dataset() + "/" + args.filter + "*"
    print("Generating gifs of all scenarios matching " + regex)
    print("------------------------------")

    count = 0
    for scenario_folder in sorted(glob.glob(regex)):
        if not os.path.isdir(scenario_folder):
            continue

        scenario_name = os.path.split(scenario_folder)[-1]
        state_file = get_file_with_extension(scenario_folder, ".nws")
        if state_file == "":
            continue

        # Create gif
        create_gif_from_state_file(
            state_file, args.executable, output_folder, scenario_name
        )

        count = count + 1
        print("------------------------------")

    print(count, "gifs taken")


if __name__ == "__main__":
    main()
