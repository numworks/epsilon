# themes_manager.py
# Authors:
#   - Maxime "M4x1m3" FRIESS
#   - Quentin Guidee

import sys
import argparse
import os
import json
import shutil
import subprocess


def check_for_git():
    try:
        output = subprocess.check_output(["git", "--version"])
        return True
    except FileNotFoundError:
        print("ERROR: Git not found! Can't download theme from the internet.", file=sys.stderr)
        return False


def check_git_remote(remote_url):
    try:
        output = subprocess.check_output(["git", "ls-remote", remote_url], env=dict(os.environ, GIT_ASKPASS="/bin/true"), stderr=subprocess.PIPE)
        return True
    except subprocess.CalledProcessError:
        print("ERROR: Invalid remote " + remote_url + "!", file=os.stderr)
        return False


def git_pull(folder):
    try:
        print("Updating " + folder + "...", file=sys.stderr)
        output = subprocess.check_output(["git", "-C", folder, "pull"], env=dict(os.environ, GIT_ASKPASS="/bin/true"), stderr=subprocess.PIPE)
        return True
    except subprocess.CalledProcessError:
        return False


def git_clone(url, folder):
    try:
        print("Cloning " + url + " into " + folder + "...", file=sys.stderr)
        output = subprocess.check_output(["git", "clone", url, folder, "--recurse-submodules"], env=dict(os.environ, GIT_ASKPASS="/bin/true"), stderr=subprocess.PIPE)
        return True
    except subprocess.CalledProcessError:
        return False


def get_icons_list():
    """
    Load icon list from file
    """
    with open(os.path.dirname(os.path.realpath(__file__)) + os.path.sep + "icons.json", "r") as json_file:
        data = json.load(json_file)

    return data


def get_data(theme, path):
    """
    Load theme from file
    """
    try:
        with open(path + os.path.sep + theme + ".json", "r") as json_file:
            data = json.load(json_file)
    except FileNotFoundError:
        print("ERROR: Theme " + theme + " doesn't exist!", file=sys.stderr)
        sys.exit(3)

    return data


def write_palette_h(data, file_p):
    """
    Write the header to file_p
    """
    file_p.write("#ifndef ESCHER_PALETTE_H\n")
    file_p.write("#define ESCHER_PALETTE_H\n\n")
    file_p.write("#include <kandinsky/color.h>\n")
    file_p.write("#include <stddef.h>\n\n")
    file_p.write("class Palette {\n")
    file_p.write("public:\n")

    for key in data["colors"].keys():
        if type(data["colors"][key]) is str:
            file_p.write("  constexpr static KDColor " + key + " = KDColor::RGB24(0x" + data["colors"][key] + ");\n")
        else:
            for sub_key in data["colors"][key].keys():
                file_p.write("  constexpr static KDColor " + key + sub_key + " = KDColor::RGB24(0x" + data["colors"][key][sub_key] + ");\n")

    # Default values - Sometimes never used
    file_p.write("  constexpr static KDColor YellowDark = KDColor::RGB24(0xffb734);\n")
    file_p.write("  constexpr static KDColor YellowLight = KDColor::RGB24(0xffcc7b);\n")
    file_p.write("  constexpr static KDColor PurpleBright = KDColor::RGB24(0x656975);\n")
    file_p.write("  constexpr static KDColor PurpleDark = KDColor::RGB24(0x414147);\n")
    file_p.write("  constexpr static KDColor GrayWhite = KDColor::RGB24(0xf5f5f5);\n")
    file_p.write("  constexpr static KDColor GrayBright = KDColor::RGB24(0xececec);\n")
    file_p.write("  constexpr static KDColor GrayMiddle = KDColor::RGB24(0xd9d9d9);\n")
    file_p.write("  constexpr static KDColor GrayDark = KDColor::RGB24(0xa7a7a7);\n")
    file_p.write("  constexpr static KDColor GrayVeryDark = KDColor::RGB24(0x8c8c8c);\n")
    file_p.write("  constexpr static KDColor Select = KDColor::RGB24(0xd4d7e0);\n")
    file_p.write("  constexpr static KDColor SelectDark = KDColor::RGB24(0xb0b8d8);\n")
    file_p.write("  constexpr static KDColor WallScreen = KDColor::RGB24(0xf7f9fa);\n")
    file_p.write("  constexpr static KDColor WallScreenDark = KDColor::RGB24(0xe0e6ed);\n")
    file_p.write("  constexpr static KDColor SubTab = KDColor::RGB24(0xb8bbc5);\n")
    file_p.write("  constexpr static KDColor LowBattery = KDColor::RGB24(0xf30211);\n")
    file_p.write("  constexpr static KDColor Red = KDColor::RGB24(0xff000c);\n")
    file_p.write("  constexpr static KDColor RedLight = KDColor::RGB24(0xfe6363);\n")
    file_p.write("  constexpr static KDColor Magenta = KDColor::RGB24(0xff0588);\n")
    file_p.write("  constexpr static KDColor Turquoise = KDColor::RGB24(0x60c1ec);\n")
    file_p.write("  constexpr static KDColor Pink = KDColor::RGB24(0xffabb6);\n")
    file_p.write("  constexpr static KDColor Blue = KDColor::RGB24(0x5075f2);\n")
    file_p.write("  constexpr static KDColor BlueLight = KDColor::RGB24(0x718fee);\n")
    file_p.write("  constexpr static KDColor Orange = KDColor::RGB24(0xfe871f);\n")
    file_p.write("  constexpr static KDColor Green = KDColor::RGB24(0x50c102);\n")
    file_p.write("  constexpr static KDColor GreenLight = KDColor::RGB24(0x52db8f);\n")
    file_p.write("  constexpr static KDColor Brown = KDColor::RGB24(0x8d7350);\n")
    file_p.write("  constexpr static KDColor Purple = KDColor::RGB24(0x6e2d79);\n")
    file_p.write("  constexpr static KDColor BlueishGrey = KDColor::RGB24(0x919ea4);\n")
    file_p.write("  constexpr static KDColor Cyan = KDColorBlue;\n")
    # End

    file_p.write("  constexpr static KDColor DataColor[] = {Red, Blue, Green, YellowDark, Magenta, Turquoise, Pink, Orange};\n")
    file_p.write("  constexpr static KDColor DataColorLight[] = {RedLight, BlueLight, GreenLight, YellowLight};\n")

    file_p.write("  constexpr static KDColor AtomColor[] = {\n")
    file_p.write("    AtomUnknown, AtomAlkaliMetal, AtomAlkaliEarthMetal, AtomLanthanide, AtomActinide, AtomTransitionMetal,\n")
    file_p.write("    AtomPostTransitionMetal, AtomMetalloid, AtomHalogen, AtomReactiveNonmetal, AtomNobleGas\n")
    file_p.write("  };\n\n")

    file_p.write("  constexpr static size_t numberOfDataColors() { return sizeof(DataColor)/sizeof(KDColor); }\n")
    file_p.write("  constexpr static size_t numberOfLightDataColors() { return sizeof(DataColorLight)/sizeof(KDColor); }\n")
    file_p.write("  static KDColor nextDataColor(int * colorIndex);\n")
    file_p.write("};\n\n")

    file_p.write("#endif\n")


def handle_git(args):
    output_folder = os.path.basename(args.repo)
    output_folder = output_folder[:-4] if output_folder.endswith(".git") else output_folder  # Remove .git extension if present.

    output_path = os.path.dirname(os.path.realpath(__file__)) + os.path.sep + "themes" + os.path.sep + "git" + os.path.sep + output_folder

    if (not args.icon):  # Assume .h is called before icons, avoiding lot of pull for nothing.
        if (os.path.exists(output_path)):  # If directory exists, try to pull
            if (not git_pull(output_path)):  # If can't pull, delete and re-clone.
                shutil.rmtree(output_path)
                git_clone(args.repo, output_path)
        else:  # If directory doesn't exist, clone.
            git_clone(args.repo, output_path)

    handle_theme(args, output_path)


def handle_theme(args, path):
    data = get_data(args.theme, path)

    if (args.icon):
        # Get the icon in the icon theme folder
        icons = get_icons_list()

        icon_path = path + os.path.sep + data["icons"] + os.path.sep + icons[args.output.replace(args.build_dir, "")]

        # Check if the file exists
        if os.path.isfile(icon_path):
            # If yes, copy from theme
            shutil.copyfile(icon_path, args.output)
        else:
            # If no, copy from src
            print(" (!!)   Icon " + icons[args.output.replace(args.build_dir, "")] + " not found in icon theme " + data["icons"] + ". Using default!")
            shutil.copyfile(args.output.replace(args.build_dir, ""), args.output)
    else:
        if (args.stdout):
            write_palette_h(data, sys.stdout)
        else:
            with open(args.output, "w") as palette_file:
                write_palette_h(data, palette_file)


def main(args):
    if (args.list):
        print(" ==== Local themes ====")
        for file_info in os.listdir(os.path.dirname(os.path.realpath(__file__)) + os.path.sep + "themes" + os.path.sep + "local"):
            if (file_info.endswith(".json")):
                filename = os.path.splitext(file_info)[0]
                print(filename)
        sys.exit(0)
    else:
        if (args.theme == None or args.repo == None):
            print("Please specify repo and theme or use --list!", file=sys.stderr)
            sys.exit(2)

    if args.repo == "local":
        handle_theme(args, os.path.dirname(os.path.realpath(__file__)) + os.path.sep + "themes" + os.path.sep + "local")
    else:
        if check_for_git():
            if (check_git_remote(args.repo)):
                handle_git(args)
            else:
                sys.exit(5)
        else:
            sys.exit(6)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process the themes.")
    parser.add_argument("repo", nargs="?", help="git remote from wtich to get the themes from. Set to \"local\" for included themes")
    parser.add_argument("theme", nargs="?", help="the name of the theme")
    parser.add_argument("output", nargs="?", help="path to the output header file")
    parser.add_argument("build_dir", nargs="?", help="path to the output folder")
    parser.add_argument("-l", "--list", help="list locals themes", action="store_true")
    parser.add_argument("-i", "--icon", help="outputs an icon instead of a header", action="store_true")
    parser.add_argument("--stdout", help="print palette.h to stdout", action="store_true")

    args = parser.parse_args()
    main(args)
