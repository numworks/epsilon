import argparse

from helpers.args_types import *
from helpers.gif_helper import *
from helpers.screenshot_helper import *

parser = argparse.ArgumentParser(
    description="This script creates a gif from a folder of screenshots"
)
parser.add_argument(
    "images_folder",
    metavar="FOLDER",
    type=existing_directory,
    help="folder containing a subfolder images",
)
parser.add_argument(
    "--delay",
    type=int,
    default=350,
    help="delay in ms between images (default 350)",
)
parser.add_argument(
    "--end-delay",
    type=int,
    default=1750,
    help="delay in ms after the last image (default 1750)",
)


def main():
    args = parser.parse_args()
    create_gif_from_images(
        list_images_in_folder(folder_images(args.images_folder)),
        args.images_folder,
        delay=args.delay,
        end_delay=args.end_delay,
    )


if __name__ == "__main__":
    main()
