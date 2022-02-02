#!/usr/bin/env python3
"""Generate missing translations for i18n files."""
# Build-in library imports
import os
import re
import sys
import argparse

# Deep translator import
try:
    from deep_translator import GoogleTranslator
except ModuleNotFoundError:
    print('Module deep_translator not found. Please install it with ',
          '"pip3 install deep_translator"')
    sys.exit(1)


# Initialize the parser
parser = argparse.ArgumentParser(description='Generate missing translations' +
                                 ' for i18n files', epilog='You can execute' +
                                 ' this script without arguments')
# Add arguments to the parser
parser.add_argument("--default-input-language", default="en",
                    help="The language code of the input language")
parser.add_argument("--base-path", default=".",
                    help="The base path of the i18n files")
parser.add_argument("--no-recursive", default=False, action="store_true",
                    help="Disable finding the i18n files recursively")
parser.add_argument("--dry-run", default=False, action="store_true",
                    help="Disable saving the i18n files")
# Parse command line arguments
args = parser.parse_args()

# Save the user language into the global variable
DEFAULT_INPUT_LANGUAGE = args.default_input_language
# Save the base path into the global variable
BASE_PATH = args.base_path
# Save if i18n files can be found recursively
FIND_RECURSIVE = not args.no_recursive
# Save if dryrun is enabled
DRYRUN = args.dry_run


def translate(text: str, output_language: str, input_language: str = 'auto')\
              -> str:
    """Translate the given text.

    Args:
        text (str): The text to translate
        output_language (str): The output language
        input_language (str): The input language

    Returns:
        str: The translated text

    """
    # Ensure of the text exists (not empty)
    if not text:
        return ""
    # Initialize the translator
    translator = GoogleTranslator(source=input_language,
                                  target=output_language)
    # Translate and return the translated text
    return translator.translate(text)


def get_all_files_in_directory(directory: str) -> list[str]:
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
        i = directory + '/' + i
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


def get_i18n_files(directory: str = '.') -> dict[str, list[str]]:
    """Get the list of i18n files in the given directory.

    Args:
        directory (str, optional): The directory to find the i18n files.
                                   Defaults to '.'.

    Returns:
        dict[list[str]]: The list of i18n files in a dictionary of languages.

    """
    # Get all files in the directory recursively
    files = get_all_files_in_directory(directory)
    # Initialize the dictionary
    i18n_files_language: dict[str, list[str]] = {}
    # Iterate over all files in the directory
    for i in files:
        # If the file is an i18n file
        if ".i18n" in i:
            # Get the language of the file
            file_language = i.split('.')[-2]
            # If the dictionary already contains the language
            if file_language in i18n_files_language:
                # Append the file to the dictionary
                i18n_files_language[file_language].append(i)
            # Else add the language to the dictionary
            else:
                # Add the file to the dictionary
                i18n_files_language[file_language] = [i]
    # Return the dictionary
    return i18n_files_language


def need_to_be_translated(keys: dict[str, list[list[str]]])\
                          -> dict[str, list[list[str]]]:
    """Return the key that needs to be translated by locale.

    Args:
        keys (dict[str, list[str]]): The keys of the i18n files

    Returns:
        dict[str, list[str]]: The keys that needs to be translated,
                              sorted by locale

    """
    # Initialize the list of keys
    keys_list: list[list[str]] = []
    keys_to_translate: dict[str, list[list[str]]] = {}
    # Iterate over all locales
    for value_ in keys.values():
        # Iterate on keys of the locale
        for key in value_:
            # Skip if the key is already in the list
            if key[0] in keys_list:
                continue
            # Else add the key to the list
            keys_list.append(key)
    for locale, value in keys.items():
        # Initialize the list of keys in the locale
        keys_in_locale: list[str] = [i[0] for i in value]
        # Get the keys of keys that need to be translated
        keys_to_translate_in_locale: list[list[str]] = [
            key for key in keys_list if key[0] not in keys_in_locale
        ]
        # Remove duplicates from the list
        # Initialize the deduplicated list
        keys_to_translate_in_locale_deduplicated: list[list[str]] = []
        # Iterate over the duplicated list
        for item in keys_to_translate_in_locale:
            # If the key is not in the deduplicated list, add it
            if item not in keys_to_translate_in_locale_deduplicated:
                keys_to_translate_in_locale_deduplicated.append(item)
            # Else, ignore the key, because it is already in the list
        # Save the deduplicated list into the dictionary
        keys_to_translate[locale] = keys_to_translate_in_locale_deduplicated
    return keys_to_translate


def get_keys_in_file(filename: str) -> list[list[str]]:
    """Return a list of keys in the file.

    Args:
        filename (str): The name of the file to read

    Returns:
        list[str]: The keys in the file

    """
    # Initialize the list of keys in the file
    keys = []
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
            # Get the filename as string with a trailing dot at the end
            generic_filename = "".join(i + "." for i in generic_filename_list)
            # Remove trailing dot from the end of the generic filename
            generic_filename = generic_filename[:-1]
            # Add the key and the generic filename to the list of keys
            keys.append([key, generic_filename])
    return keys


def list_keys(i18n_files: dict[str, list[str]]) -> dict[str, list[list[str]]]:
    """List all keys in the i18n files.

    Args:
        i18n_files (dict[str, list[str]]): I18n files list

    Returns:
        dict[str, list[str]]: The dictionnary of keys in the i18n files by
                              locale.

    """
    # Initialize the list of keys in the i18n files
    keys_dict: dict[str, list[list[str]]] = {}
    # Iterate on the locales
    for locale in i18n_files:
        # Initialize the dictionary for the locale
        keys_dict[locale] = []
        # Iterate on the locale's files
        for actual_file in i18n_files[locale]:
            # Get the keys in the file and add them to the list
            keys_dict[locale].extend(get_keys_in_file(actual_file))
    # Return the dictionary of keys in the i18n files sorted by locale
    return keys_dict


def get_value_from_key(key_to_find: str, filename_generic: str, locale: str)\
                       -> str:
    """Get the value of a key in the i18n files from the given locale.

    Args:
        key (str): The key to fetch the value
        filename_generic (str): The path to the file where the value will be
                                fetched
        locale (str): The locale from the value will be retrieved

    Returns:
        str: The value of the key from the given file

    """
    # Get the filename from the generic filename
    file_path = filename_generic + '.' + locale + '.i18n'
    # Exit if the file does not exist
    if not os.path.exists(file_path):
        return ""
    # Open the file for reading
    with open(file_path, 'r', encoding='utf-8') as file_read:
        # Read the file line by line
        for line in file_read.readlines():
            # Ignore commented lines
            if re.match(r"^#(.*)$", line):
                continue
            # Ignore lines without =
            if '=' not in line:
                continue
            # Get the key and value by spliting the line by =
            key = line.split("=")[0]
            value = "".join(i + " " for i in line.split("=")[1:])
            # print(key, value)
            # Remove spaces from the start of the key
            while key[0] == " ":
                key = key[1:]
            # Remove spaces from the end of the key
            while key[-1] == " ":
                key = key[:-1]
            # Remove spaces from the start of the value
            while value[0] == " ":
                value = value[1:]
            # Remove spaces from the end of the value
            while value[-1] in [" ", "\n"]:
                value = value[:-1]
            # Remove the quotes from the value
            value = value[1:][:-1]
            if key == key_to_find:
                # Add a space to the end of the value if the key is empty,
                # because an empty value is parsed as a error
                if not value:
                    value = " "
                return value
    # Return a empty string if no value were found because we can't use bools
    # because it made that the linters crash
    return ""


def translate_missing_keys(keys_to_translate: dict[str, list[list[str]]],
                           input_language: str) -> dict[str, list[list[str]]]:
    """Get a dictionary of file with the keys and translations to add.

    Args:
        keys_to_translate (dict[str, list[list[str]]]): The list of keys to
                                                        translate
        input_language (str): The language to get the text that will be
                              translated

    Returns:
        dict[str, list[str]]: The dictionary of files with translations

    """
    # Initialize the dictionary of translations
    output: dict[str, list[list[str]]] = {}
    # Initialize the variable to store the number of translated keys
    keys_translated: int = 0
    # Iterate over the locales of the dictionary of untranslated keys
    for locale in keys_to_translate:
        # Iterate over the untranslated keys in the locale
        for key in keys_to_translate[locale]:
            # Get the filename from the generic filename
            filename = key[1] + '.' + locale + '.i18n'
            # Get the value of the key
            value = get_value_from_key(key[0], key[1], input_language)
            # If the value was not found in the locale, try other languages
            if not value:
                print(f"Key '{key[0]}' not found in locale {input_language}")
                print("Iterating over all known languages")
                # Initialize the variable to store if a value was found to false
                value_found = False
                # Iterate over all the known languages
                for language in keys_to_translate:
                    # Try to get the value from the language
                    value = get_value_from_key(key[0], key[1], language)
                    # If the value was found in the language, break
                    if value:
                        print(f"Value found in the language '{language}'")
                        # Mark the value as been found
                        value_found = True
                        # Save the language
                        input_language = language
                        break
                    # Else, continue
                # Crash if the value is not available all the known languages
                if not value_found:
                    raise ValueError(f"Key '{key[0]}' not found in all locales")

            # Restore the default value if a space have been
            # inserted
            if value == ' ':
                value = ''
            # Translate the value into the requested language
            value_translated = translate(value, locale, input_language)
            # If the file is already in the dictionary, append the translation
            # to it
            if filename in output:
                output[filename].append([key[0], value_translated])
            # Else create the list for the file in the dictionary
            else:
                output[filename] = [[key[0], value_translated]]
            # Print the key and value
            print(key[0] + ' = "' + value_translated + '"')
            # Increment the number of translated keys
            keys_translated += 1
    # If no keys were translated, print a specific message
    if not keys_translated:
        print('No keys were translated')
    # Else, if only one key was translated, print a specific message
    elif keys_translated == 1:
        print('One key was translated')
    # Else, print a generic message
    else:
        print(f'{keys_translated} keys were translated')
    # Return the dictionary of translations
    return output


def save_translations(missing_keys_translation: dict[str, list[list[str]]]):
    """Save the translations.

    Args:
        missing_keys_translation (dict[str, list[list[str]]]): The dictionary
                                                               of translations

    """
    # Read the dictionary of files
    for filename, keys_for_file in missing_keys_translation.items():
        # Initialize add new line variable to false
        add_new_line = False
        # Open the file for reading only if it exists
        if os.path.exists(filename):
            # Open the file for reading
            with open(filename, 'r', encoding='utf-8') as file_read:
                # Save content of the file into the content variable and
                # ensure sure there is a first character (not a empty file)
                if content := str(file_read.read()):
                    # If the last char isn't a newline
                    if content[-1] != '\n':
                        # Mark that a newline should be added
                        add_new_line = True
        # Else, print of the file will be created
        else:
            print(f"{filename} is not found, creating an empty file")
        # Open the file for writing
        with open(filename, 'a', encoding='utf-8') as file_write:
            # If add new line is true, add a new line at the end of the file
            if add_new_line:
                file_write.write('\n')
            # Iterate on the values to be save in the file
            for value in keys_for_file:
                # Generate the in the style Key = "Value"
                line = value[0] + ' = "' + value[1] + '"'
                # Write the line at the end of the file
                file_write.write(line + '\n')


def main():
    """Run the program."""
    # Get the list of i18n files
    i18n_files = get_i18n_files(BASE_PATH)
    # Get the list of keys in the i18n files
    keys = list_keys(i18n_files)
    # Delete universals keys from the list of keys
    if 'universal' in keys:
        del keys['universal']
    # Get the list of keys to translate in the i18n files
    keys_to_translate = need_to_be_translated(keys)
    # Translate the missings keys
    missing_keys_translation = translate_missing_keys(keys_to_translate,
                                                      DEFAULT_INPUT_LANGUAGE)
    # Save the translations
    if not DRYRUN:
        save_translations(missing_keys_translation)
    else:
        print("Dry run enabled, translations aren't saved")


main()
