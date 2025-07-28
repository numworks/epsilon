import argparse
import pathlib

from helpers.args_types import *


parser = argparse.ArgumentParser(
    description="This script deletes successions of duplicated images inside folder/images, leaving just one per event entailing a visual change."
)

parser.add_argument(
    "folder",
    metavar="FOLDER",
    type=existing_directory,
    help="folder containing a subfolder images",
)


def main():
    args = parser.parse_args()
    folder = pathlib.Path(args.folder) / "images"
    # Compare png contents and remove successive duplicates
    previousImage = b""
    for png in sorted(folder.glob("img-*.png")):
        image = png.read_bytes()
        if image == previousImage:
            png.unlink()
        else:
            previousImage = image


if __name__ == "__main__":
    main()
