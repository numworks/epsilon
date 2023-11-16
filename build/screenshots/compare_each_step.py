import os, shutil, argparse
import helper
import args_types
from PIL import Image
from print_format import print_underlined

parser = argparse.ArgumentParser(
    description="This script takes a screenshot for each event and compares them to a reference."
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
    "reference_images",
    metavar="FOLDER",
    type=args_types.existing_directory,
    help="folder containing reference images for each step",
)


def main():
    # Parse args
    args = parser.parse_args()

    # Ouput folder
    output_folder = "output_compare_each_step"
    helper.clean_or_create_folder(output_folder)

    # Reference images
    print_underlined("Reference images")
    list_reference_images = helper.list_images_in_folder(args.reference_images)
    reference_output_folder = os.path.join(output_folder, "reference")
    shutil.copytree(
        args.reference_images, os.path.join(reference_output_folder, "images")
    )
    helper.create_gif(list_reference_images, reference_output_folder)

    # Computed images
    print_underlined("New images")
    list_computed_images = helper.generate_all_screenshots_and_create_gif(
        args.state_file, args.executable, os.path.join(output_folder, "computed")
    )

    # Crop images: we need to have same size images
    # Images in debug display the event at the bottom. Not in realease.
    # Computed images were taken in debug mode, but we don't know for reference images.
    _, ref_height = Image.open(list_reference_images[0]).size
    if ref_height == 240:
        for image_path in list_computed_images:
            im = Image.open(image_path)
            width, height = im.size
            im_croppped = im.crop((0, 0, width, 240))
            im_croppped.save(image_path)
    else:
        assert ref_height == 270

    # Compare
    print_underlined("Diff images")
    helper.create_diff_gif(list_reference_images, list_computed_images, output_folder)


if __name__ == "__main__":
    main()
