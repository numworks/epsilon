import sys, os
from subprocess import Popen, DEVNULL, PIPE

from helpers.miscellaneous import *


def create_gif_from_images(
    list_images, folder, gif_name="scenario", delay=350, end_delay=1750
):
    print("Creating gif")
    gif = os.path.join(folder, gif_name + ".gif")
    p = Popen(
        "magick " + " ".join(list_images)
        # delays are in centiseconds
        + f" -set delay '%[fx:t==(n-1) ? {end_delay / 10} : {delay / 10}]' "
        + " "
        + gif,
        shell=True,
        stdout=DEVNULL,
        stderr=PIPE,
    )
    print_error(p.stderr)
    if not os.path.exists(gif):
        print("Error: couldn't create gif")
        sys.exit(1)
    print("Done, gif created in", folder)


def images_are_identical(screenshot_1, screenshot_2, screenshot_diff):
    p = Popen(
        [
            "magick",
            "compare",
            "-metric",
            "mae",
            screenshot_1,
            screenshot_2,
            screenshot_diff,
        ],
        stdout=DEVNULL,
        stderr=PIPE,
    )
    mae = p.stderr.read().decode().split(" ")[0]
    return mae == "0"


def image_size(image):
    p = Popen(
        ["magick", "identify", "-ping", "-format", "%w %h", image],
        stdout=PIPE,
        stderr=DEVNULL,
    )
    w, h = p.stdout.read().decode().split(" ")
    return int(w), int(h)


def crop_images(list_images):
    Popen(["magick", "mogrify", "-crop", "320x240+0+0", *list_images]).wait()


def concatenate_images(list_images, output):
    Popen(
        ["magick", *list_images, "+append", output],
    ).wait()


def create_diff_gif(list_images_1, list_images_2, gif_destination_folder):
    diff_folder = os.path.join(gif_destination_folder, "diff")
    os.mkdir(diff_folder)
    n = len(list_images_2)
    if len(list_images_1) != n:
        print("Error: lists of images are not the same size. Cannot compare.")
        sys.exit(1)
    _, height_1 = image_size(list_images_1[0])
    _, height_2 = image_size(list_images_2[0])
    if height_1 != height_2:
        crop_images(list_images_1 if height_1 != 240 else list_images_2)
    print("Generating all diff images")
    diff_image = os.path.join(diff_folder, "diff.png")
    images_to_remove = []
    for i in range(n):
        identical = images_are_identical(list_images_1[i], list_images_2[i], diff_image)
        concatenated_image = os.path.join(diff_folder, "img-{:04d}.png".format(i))
        concatenate_images(
            [list_images_1[i], list_images_2[i], diff_image], concatenated_image
        )
        if identical:
            images_to_remove.append(concatenated_image)
    os.remove(diff_image)
    print("All done")
    list_diff_images = list_images_in_folder(diff_folder)
    assert len(list_diff_images) == n
    create_gif_from_images(list_diff_images, gif_destination_folder, "diff")
    for image in images_to_remove:
        os.remove(image)
