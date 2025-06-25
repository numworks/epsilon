import sys, os, shutil
from pathlib import Path

from helpers.print_format import *


def clean_or_create_folder(folder):
    if os.path.exists(folder):
        print("Warning:", folder, "already exists. Its content will be erased.")
        shutil.rmtree(folder)
    os.mkdir(folder)


def get_file_with_extension(folder, file_extension):
    found_file = ""
    for file in sorted(os.listdir(folder)):
        if os.path.splitext(file)[1] == file_extension:
            if found_file != "":
                print(bold("Error:"), "too many", file_extension, "in", folder)
                return ""
            found_file = os.path.join(folder, file)
    if found_file == "":
        print(bold("Error:"), "no", file_extension, "in", folder)
    return found_file


def list_images_in_folder(folder):
    return [image.as_posix() for image in sorted(Path(folder).glob("*.png"))]


def print_error(stderr, exit=True):
    err = stderr.read().decode()
    if err:
        if err[-1] == "\n":
            err = err[:-1]
        print(err)
        if exit:
            sys.exit(1)
