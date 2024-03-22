import argparse
import helper

parser = argparse.ArgumentParser(
    description="This script creates a gif from a folder of screenshots"
)
parser.add_argument(
    "-o", "--output-folder", default="screenshots_each_step", help="output folder"
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
    helper.create_gif(
        helper.list_images_in_folder(helper.folder_images(args.output_folder)),
        args.output_folder,
        delay=args.delay,
        end_delay=args.end_delay,
    )


if __name__ == "__main__":
    main()
