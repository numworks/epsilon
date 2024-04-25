import os, shutil, argparse
from subprocess import Popen, DEVNULL

from helpers.args_types import *
from helpers.miscellaneous import *
from helpers.screenshot_helper import *
from helpers.nws_helper import *

parser = argparse.ArgumentParser(
    description="This script enables editing a state file. You can give an executable to follow the changes."
)
parser.add_argument(
    "state_file",
    metavar="STATE_FILE",
    type=existing_state_file,
    help="state file (with extension .nws)",
)
parser.add_argument(
    "-e",
    "--executable",
    type=existing_file,
    help="epsilon executable used to compare state file before the changes and after the changes",
)
parser.add_argument(
    "-c",
    "--copy",
    action="store_true",
    default=0,
    help="Use this flag if you want to create a copy instead of editing you original nws",
)


EDITOR = os.environ.get("EDITOR", "vim")


def main():
    # Parse args
    args = parser.parse_args()
    original_nws = args.state_file

    # Create output folder and temp files
    output_folder = "edit_nws_output"
    clean_or_create_folder(output_folder)
    temp_txt = os.path.join(output_folder, "temp.txt")
    edited_nws = os.path.join(output_folder, "edited.nws")

    # Convert the original nws into txt
    convert_nws_to_txt(original_nws, temp_txt)

    # Open vim to edit the txt
    p = Popen(EDITOR + " " + temp_txt, shell=True, stderr=DEVNULL)
    p.wait()

    # Convert the txt into nws
    convert_txt_to_nws(temp_txt, edited_nws)

    # Generate gifs before and after the changes
    if args.executable is not None:
        print("Creating gifs before and after the changes")
        create_gif_from_state_file(
            original_nws, args.executable, output_folder, "before"
        )
        create_gif_from_state_file(edited_nws, args.executable, output_folder, "after")

    print("==============================")

    # Clean folder
    copy_text = ""
    os.remove(temp_txt)
    if args.copy:
        copy_text = "The copy of "
    else:
        # Replace original nws with copy
        shutil.copy(edited_nws, original_nws)

        if args.executable is None:
            # Folder is empty
            shutil.rmtree(output_folder)

    print(copy_text + original_nws + " was successfully edited !")


if __name__ == "__main__":
    main()
