#!/usr/bin/env python3
"""Remove all unused translations to save flash space."""
# Builtins imports
import os
import re
import argparse


# Initialize the parser
parser = argparse.ArgumentParser(
    description="Remove unused translations from the i18n file to save flash \
        space.", epilog="You can use this script without arguments"
)
# Add arguments to the parser
parser.add_argument("--base-path", default=".",
                    help="The base path of the files")
parser.add_argument("--no-recursive", default=False, action="store_true",
                    help="Disable finding the files recursively")
parser.add_argument("--dry-run", default=False, action="store_true",
                    help="Disable saving the i18n files")
# Parse command line arguments
args = parser.parse_args()

# Initialize the settings
BASE_PATH = args.base_path
FIND_RECURSIVE = not args.no_recursive
DRYRUN = args.dry_run
# Don't add / at the end of the path
IGNORE_PATHS = [
    "./output", "./.git", "./.mypy_cache",
    "./ion/src/simulator/external/sdl",
]
IGNORE_PATHS_CONTENTS = [
    "__pycache__", ".png", ".esc", ".ttf", ".ico", ".jpg", ".jar", ".icn",
    ".bnr", ".i18n", ".cache"
]

# If the key contain something that's in this list, keep it to prevent code breaking
KEEP_KEY_IF_CONTAIN = ["Language", "Country"]


def get_all_files_in_directory(directory: str)\
        -> list[str]:
    """Get all files in the given directory recursively.

    Args:
        directory (str): The directory

    Returns:
        List[str]: The list of files in the directory

    """
    # Initialize the list of files
    files = []
    # Iterate over all files in the directory
    for i in os.listdir(directory):
        # Add the current directory to the file name
        i = f'{directory}/{i}'
        # If the file is a directory
        if FIND_RECURSIVE and os.path.isdir(i):
            # Iter the directory and add the files to the list
            files.extend(get_all_files_in_directory(i))
        # Else the file is a file
        else:
            # Add the file to the list
            files.append(i)
    # Return the list of files
    return files


def get_files(directory: str) -> list[str]:
    """Get all files in the directory recursively with ignoring list.

    Args:
        directory (str): The start directory

    Returns:
        List[str]: The list of files
    """
    # Initialize the list of files
    files = []
    # Iterate over the directory
    for filename in os.listdir(directory):
        # Add the full path to the directory/file
        filename = f'{directory}/{filename}'
        # Exclude path in IGNORE_PATHS
        if filename in IGNORE_PATHS:
            continue
        # Exclude paths in IGNORE_PATHS_CONTENTS
        if any(extension in filename for extension in IGNORE_PATHS_CONTENTS):
            continue
        # If it is a directory, find recursively the files into it
        if os.path.isdir(filename):
            files.extend(get_files(filename))
        else:
            files.append(filename)
    # Return the file list
    return files


def get_keys_in_i18n_file(filename: str) -> list[list[str]]:
    """Return a list of keys in the file.

    Args:
        filename (str): The name of the file to read

    Returns:
        List[list[str]]: The keys in the file

    """
    # Initialize the list of keys in the file
    keys: list[list[str]] = []
    # Open the file read only
    with open(filename, 'r', encoding='utf-8') as file_read:
        # Read the content of the file line by line
        for line in file_read.readlines():
            # Ignore commented lines
            if re.match(r"^#(.*)$", line):
                continue
            # Ignore lines without =
            if '=' not in line:
                continue
            # Get the key by spliting the line by =
            key = line.split("=")[0]
            # Remove spaces from the start of the key
            while key[0] == " ":
                key = key[1:]
            # Remove spaces from the end of the key
            while key[-1] == " ":
                key = key[:-1]
            # Get generic filename into a list separated by dots
            generic_filename_list = filename.split(".")[:-2]
            # Get if the locale is universal
            locale = filename.split(".")[-2]
            # Get the filename as string with a trailing dot at the end
            generic_filename = "".join(f'{i}.' for i in generic_filename_list)
            # Remove trailing dot from the end of the generic filename
            generic_filename = generic_filename[:-1]
            # Add the key and the generic filename to the list of keys
            keys.append([key, generic_filename, locale])
    return keys


def list_keys_in_i18n_file_list(i18n_files: list[str]) -> list[list[str]]:
    """List all keys in the i18n files.

    Args:
        i18n_files (list[str]): I18n files list

    Returns:
        List[list[str]]: The dictionnary of keys in the i18n files by
                              locale.

    """
    # Initialize the list of keys in the i18n files
    keys_dict: list[list[str]] = []
    # Iterate on the file list
    for actual_file in i18n_files:
        # Get the keys in the file and add them to the list
        keys_dict.extend(get_keys_in_i18n_file(actual_file))
    # Return the dictionary of keys in the i18n files sorted by locale
    return keys_dict


def extract_keys_from_line(line: str) -> list[str]:
    """Extract keys from a line.

    Args:
        line (str): The line to extract keys from

    Returns:
        list[str]: The extracted keys
    """
    # Initialize the list of separator to separate the key from the part
    keys_separator: list[str] = ["}", ":", ";", ",", " ", ")"]
    # Initialize the list of keys
    keys: list[str] = []
    # Split the line by ::
    line_splitted: list[str] = line.split("::")
    # Initialize loop variables
    # The last part (used inside the loop)
    last_part: str = ""
    # Mark the next line contain the key
    save_next_line: bool = False
    # The key part
    key_parts: list[str] = []
    # Iterate over the splitted line
    for part in line_splitted:
        # print(part)
        if save_next_line:
            key_parts.append(part)
            save_next_line = False
        # If the actual part is Message and the last part contain I18n
        # (I18n::Message), the next part will contain the key name
        # TODO: Improve catching
        if part == "Message" and "I18n" in last_part:
            save_next_line = True
        # Save the current part into the last part
        # (loop iteration is finished)
        last_part = part
    # Get the key from the key part
    # TODO: Improve catching
    # Iterate over all the keys in the line
    for actual_key in key_parts:
        # Initialize real key variable
        key_real: str = actual_key
        for separator_to_test in keys_separator:
            key_separated: list[str] = key_real.split(separator_to_test)
            # print(key_real, separator_to_test, key_separated)
            # If the key was splitted, save the separated key
            if len(key_separated) > 1:
                key_real = key_separated[0]
        if key_real:
            keys.append(key_real)
    return keys


def keys_from_file_list(files: list[str]) -> list[str]:
    """Get an array of keys from files.

    Args:
        files (list[str]): The list of files to read

    Returns:
        list[str]: The keys
    """
    # Initialize the list of keys from the files
    keys: list[str] = []
    # Iterate over the file list
    for filename in files:
        # Read the file contents into "content" variable
        with open(filename, 'r', encoding="utf-8") as file_obj:
            # Store the contents of the file
            content = file_obj.read()
        # Iterate over the file contents
        for line in content.split("\n"):
            # Get if the line contains an I18n key
            if "I18n::Message" in line:
                # Extract the keys from the line
                keys.extend(extract_keys_from_line(line))
    return keys


def get_i18n_files(directory: str = '.') -> list[str]:
    """Get the list of i18n files in the given directory.

    Args:
        directory (str, optional): The directory to find the i18n files.
                                   Defaults to '.'.

    Returns:
       list[str]: The list of i18n files in a dictionary of languages.

    """
    # Get all files in the directory recursively
    files = get_all_files_in_directory(directory)
    # Return only i18n files
    return [i for i in files if ".i18n" in i]


def get_unused_keys(file_keys: list[str], i18n_keys: list[list[str]]) -> list[list[str]]:
    """Get unused keys.

    Args:
        file_keys (list[str]): The keys in the source files
        i18n_keys (list[list[str]]): The keys in the i18n files

    Returns:
        list[list[str]]: The unused keys
    """
    # Initialize the list of unused keys
    unused_keys: list[list[str]] = []
    # Iterate over the i18n key definitions
    for key_i18n_actual in i18n_keys:
        # Get if the key is used, and mark it as used if it is in the kepping
        # list
        key_used = next(
            (
                True
                for string_to_test in KEEP_KEY_IF_CONTAIN
                if string_to_test in key_i18n_actual[0]
            ),
            any(key_i18n_actual[0] == file_key for file_key in file_keys),
        )

        # If the key is not used, add it to the list
        if not key_used:
            if key_i18n_actual not in unused_keys:
                print(f"{key_i18n_actual[0]} unused")
            unused_keys.append(key_i18n_actual)
    return unused_keys


def remove_keys_from_i18n_files(unused_keys: list[list[str]]):
    """Remove unused keys from i18n files.

    Args:
        unused_keys (list[list[str]]): The list of keys to remove
    """
    # Initialize the dictionary of files
    # (to prevent intensive writing to disk)
    files_to_write: dict[str, str] = {}
    # Iterate over the keys to remove
    for key in unused_keys:
        key_name_to_remove = key[0]
        filename_generic = key[1]
        locale = key[2]
        # Get the filename of from the generic filename
        filename = f'{filename_generic}.{locale}.i18n'
        # If the file is not in the dictionary, add it
        if filename not in files_to_write:
            # Save the file contents
            with open(filename, 'r', encoding='utf8') as file_read:
                files_to_write[filename] = file_read.read()
        # Split the file by new lines
        file_splitted = files_to_write[filename].split("\n")
        # Iterate over the file contents
        for line, value in enumerate(file_splitted):
            # Ignore lines without =
            if '=' not in value:
                continue
            # Get the key from the line
            key_to_check: str = value.split("=")[0]
            # Remove spaces from the start of the key
            while key_to_check[0] == " ":
                key_to_check = key_to_check[1:]
            # Remove spaces from the end of the key
            while key_to_check[-1] == " ":
                key_to_check = key_to_check[:-1]
            # If the key is the key to remove, remove it
            if key_to_check == key_name_to_remove:
                del file_splitted[line]
                break
        file_str = "".join(line + "\n" for line in file_splitted)
        # Remove double line return
        while file_str[-2:] == '\n\n':
            file_str = file_str[:-1]
        files_to_write[filename] = file_str
    # When the loop is end, write the files
    for actual_file, content_to_write in files_to_write.items():
        with open(actual_file, 'w', encoding='utf-8') as file_to_write:
            file_to_write.write(content_to_write)
        # print(actual_file, content_to_write)


def main():
    """Execute the program."""
    # Get the file list
    file_list = get_files(BASE_PATH)
    # Get the keys in the file list
    files_keys = keys_from_file_list(file_list)
    # Get i18n files list
    i18n_files = get_i18n_files(BASE_PATH)
    # Get keys from i18n files
    i18n_files_keys = list_keys_in_i18n_file_list(i18n_files)
    # Get unused keys
    unused_keys = get_unused_keys(files_keys, i18n_files_keys)
    # If dryrun is disabled, remove the keys definitions from the i18n files
    if not DRYRUN:
        remove_keys_from_i18n_files(unused_keys)


main()
