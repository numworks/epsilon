import os, shutil, argparse

from helpers.args_types import *
from helpers.print_format import *
from helpers.gif_helper import *
from helpers.miscellaneous import *
from helpers.screenshot_helper import *

parser = argparse.ArgumentParser(
    description="This script takes a screenshot for each event and compares them to a reference."
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
    "reference_images",
    metavar="FOLDER",
    type=existing_directory,
    help="folder containing reference images for each step",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Ouput folder
    output_folder = "output_compare_each_step"
    clean_or_create_folder(output_folder)

    # Reference images
    print_underlined("Reference images")
    list_reference_images = list_images_in_folder(args.reference_images)
    reference_output_folder = os.path.join(output_folder, "reference")
    shutil.copytree(
        args.reference_images, os.path.join(reference_output_folder, "images")
    )
    create_gif_from_images(list_reference_images, reference_output_folder)

    # Computed images
    print_underlined("New images")
    list_computed_images = generate_all_screenshots_and_create_gif(
        args.state_file, args.executable, os.path.join(output_folder, "computed")
    )

    # Crop images: we need to have same size images
    # Images in debug display the event at the bottom. Not in release.
    # Computed images were taken in debug mode, but we don't know for reference images.
    _, ref_height = image_size(list_reference_images[0])
    if ref_height == 240:
        crop_images(list_computed_images)
    else:
        assert ref_height == 270

    # Compare
    print_underlined("Diff images")
    create_diff_gif(list_reference_images, list_computed_images, output_folder)


if __name__ == "__main__":
    main()
