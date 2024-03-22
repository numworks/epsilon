import argparse
import helper
import args_types

parser = argparse.ArgumentParser(
    description="This script takes a screenshot for each event of a scenario and creates a gif."
)
parser.add_argument(
    "executable",
    metavar="EXE",
    type=args_types.existing_file,
    help="epsilon executable",
)
parser.add_argument(
    "state_file",
    metavar="STATE_FILE",
    type=args_types.existing_state_file,
    help="state file (with extension .nws)",
)
parser.add_argument(
    "-o", "--output-folder", default="screenshots_each_step", help="output folder"
)
parser.add_argument(
    "--skip-idle",
    action="store_true",
    help="skip screenshots associated with idle events",
)


def main():
    args = parser.parse_args()
    helper.generate_all_screenshots_and_create_gif(
        args.state_file, args.executable, args.output_folder, args.skip_idle, False
    )


if __name__ == "__main__":
    main()
