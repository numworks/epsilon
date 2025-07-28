import argparse
import os


def existing_file(string):
    if not os.path.exists(string):
        raise argparse.ArgumentTypeError(string + " does not exist")
    if not os.path.isfile(string):
        raise argparse.ArgumentTypeError(string + " is not a file")
    return string


def existing_directory(string):
    if not os.path.exists(string):
        raise argparse.ArgumentTypeError(string + " does not exist")
    if not os.path.isdir(string):
        raise argparse.ArgumentTypeError(string + " is not a directory")
    return string


def existing_state_file(string):
    if not os.path.exists(string):
        raise argparse.ArgumentTypeError(string + " does not exist")
    if not os.path.isfile(string) or os.path.splitext(string)[1] != ".nws":
        raise argparse.ArgumentTypeError(string + " is not a state file")
    return string
