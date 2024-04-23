import sys, os, shutil
from subprocess import Popen, DEVNULL, PIPE
from pathlib import Path

from helpers.print_format import *
from helpers.gif_helper import *
from helpers.miscellaneous import *


def dataset():
    return "tests/screenshots_dataset"


def folder(scenario_name):
    return os.path.join(dataset(), scenario_name)


def folder_images(folder):
    return os.path.join(folder, "images")


def generate_all_screenshots(state_file, executable, folder, exit_if_error=True):
    print("Generating all screenshots of", state_file)
    clean_or_create_folder(folder)
    p = Popen(
        "./"
        + executable
        + " --headless --load-state-file "
        + state_file
        + " --take-all-screenshots "
        + folder,
        shell=True,
        stdout=DEVNULL,
        stderr=PIPE,
    )
    print_error(p.stderr, exit_if_error)
    list_images = list_images_in_folder(folder)
    if len(list_images) == 0:
        print("Error: no screenshots taken")
        if exit_if_error:
            sys.exit(1)
    print("All done, screenshots taken in", folder)
    return list_images


def generate_all_screenshots_and_create_gif(
    state_file, executable, folder, exit_if_error=True, gif_name="scenario"
):
    clean_or_create_folder(folder)
    list_images = generate_all_screenshots(
        state_file, executable, folder_images(folder), exit_if_error
    )
    create_gif_from_images(list_images, folder, gif_name)
    return list_images


def create_gif_from_state_file(state_file, executable, folder, gif_name="scenario"):
    temp_folder = os.path.join(folder, "temp")
    generate_all_screenshots_and_create_gif(
        state_file, executable, temp_folder, True, gif_name
    )
    # Move gif out of temp folder
    gif = gif_name + ".gif"
    shutil.move(
        os.path.join(temp_folder, gif),
        os.path.join(folder, gif),
    )
    # Delete temp folder
    shutil.rmtree(temp_folder)
