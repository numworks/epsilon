import argparse
import pathlib

parser = argparse.ArgumentParser(
    description="This script deletes successions of duplicated images in the folder, leaving just one per event entailing a visual change."
)
parser.add_argument(
    "-o", "--output-folder", default="screenshots_each_step", help="output folder"
)


def main():
    args = parser.parse_args()
    folder = pathlib.Path(args.output_folder) / "images"
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
