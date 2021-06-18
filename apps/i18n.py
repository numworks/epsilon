# -*- coding: utf-8 -*-

# This script gathers all .i18n files and aggregates them as a pair of .h/.cpp
# file.
# In practice, it enforces a NFKD normalization. Because Epsilon does not
# properly draw upper case letters with accents, we remove them here.
# It works with Python 2 and Python 3

import argparse
import csv
import io
import re
import sys
import unicodedata


parser = argparse.ArgumentParser(description="Process some i18n files.")
parser.add_argument('--header', help='the .h file to generate')
parser.add_argument('--implementation', help='the .cpp file to generate')
parser.add_argument('--locales', nargs='+', help='locale to actually generate')
parser.add_argument('--countries', nargs='+', help='countries to actually generate')
parser.add_argument('--codepoints', help='the code_points.h file')
parser.add_argument('--countrypreferences', help='the country_preferences.csv file')
parser.add_argument('--languagepreferences', help='the language_preferences.csv file')
parser.add_argument('--files', nargs='+', help='an i18n file')
parser.add_argument('--generateISO6391locales', type=int, nargs='+', help='whether to generate the ISO6391 codes for the languages (for instance "en" for english)')

args = parser.parse_args()

def generate_ISO6391():
    return args.generateISO6391locales[0] == 1

def has_glyph(glyph):
    return glyph in codepoints

def source_definition(i18n_string):
    s = unicodedata.normalize("NFKD", i18n_string)
    result = u"\""
    i = 0
    length = len(s)
    checkForCombining = False
    while i < length:
        copyCodePoint = True
        if checkForCombining:
            # We remove combining code points, which are between 0x300 and 0x36F
            # (for the non-extended set)
            copyCodePoint = (ord(s[i]) < 0x300) or (ord(s[i]) > 0x36F)
            checkForCombining = False
        if copyCodePoint:
            # Remove the uppercase characters with combining chars
            checkForCombining = s[i].isupper()
            result = result + s[i]
        if not has_glyph(s[i]):
            sys.stderr.write(s[i] + " (" + str(hex(ord(s[i]))) + ") is not a character present in " + args.codepoints + " . Exiting !\n")
            sys.exit(-1)
        i = i+1
    result = result + u"\""
    return result.encode("utf-8")

def is_commented_line(line):
    match_comment = re.match(r"^#(.*)$", line)
    return match_comment

def split_line(line):
    match = re.match(r"^(\w+)\s*=\s*\"(.*)\"$", line)
    if not match:
        sys.stderr.write("Error: Invalid line \"" + line + "\"\n")
        sys.exit(-1)
    return (match.group(1), source_definition(match.group(2)))

def locale_from_filename(filename):
    return re.match(r".*\.([a-z]+)\.i18n", filename).group(1)

def check_redundancy(messages, data, locales):
    redundant_names = set()
    for name in messages:
        redundancy = True
        for i in range(1, len(locales)):
            redundancy = redundancy and data[locales[i]][name] == data[locales[i-1]][name]
        if redundancy:
            redundant_names.add(name)
    if (len(redundant_names) > 0):
        sys.stderr.write("Some localized messages are redundant and can be made universal :\n\t" + "\n\t".join(sorted(redundant_names)) + "\n")
        sys.exit(-1)

def parse_files(files):
    data = {}
    messages = set()
    universal_messages = set()
    for path in files:
        locale = locale_from_filename(path)
        if locale not in data:
            data[locale] = {}
        with io.open(path, "r", encoding='utf-8') as file:
            for line in file:
                if is_commented_line(line):
                    continue
                name,definition = split_line(line)
                if locale == "universal":
                    if name in messages:
                        sys.stderr.write("Error: Redefinition of message \"" + name + "\" as universal\n")
                        sys.exit(-1)
                    if name in universal_messages:
                        sys.stderr.write("Error: Redefinition of universal message \"" + name + "\"\n")
                        sys.exit(-1)
                    universal_messages.add(name)
                else:
                    messages.add(name)
                data[locale][name] = definition
    #check_redundancy(messages, data, args.locales) # FIXME
    return {"messages": sorted(messages), "universal_messages": sorted(universal_messages), "data": data}

def parse_codepoints(file):
    codepoints = []
    with io.open(file, "r", encoding='utf-8') as file:
        IsCodePoint = False
        for line in file:
            if "};" in line:
                IsCodePoint = False
            if IsCodePoint:
                start = line.find('0x')
                stop = line.find(',')
                if not (start == -1 or stop == -1):
                    hexstring = line[start:stop]
                    value = int(hexstring, 16)
                    char = chr(value)
                    codepoints.append(char)
            if "CodePoints[]" in line:
                IsCodePoint = True
    return codepoints

codepoints = parse_codepoints(args.codepoints)

def parse_csv_with_header(file):
    res = []
    with io.open(file, 'r', encoding='utf-8') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=',')
        for row in csvreader:
            res.append(row)
    return (res[0], res[1:])

def parse_country_preferences(file):
    countryPreferences = {}
    header, records = parse_csv_with_header(file)
    for record in records:
        countryPreferences[record[0]] = [header[i] + "::" + record[i] for i in range(1, len(record))]
    return countryPreferences

countryPreferences = parse_country_preferences(args.countrypreferences)

def parse_language_preferences(file):
    languagePreferences = {}
    header, records = parse_csv_with_header(file)
    for record in records:
        languagePreferences[record[0]] = (header[1], record[1])
    return languagePreferences

languagePreferences = parse_language_preferences(args.languagepreferences)

def print_block_from_list(target, header, data, beautify=lambda arg: arg, prefix="  ", footer="};\n\n"):
    target.write(header)
    for i in range(len(data)):
        target.write(prefix + beautify(data[i]) + ",\n")
    target.write(footer)

def print_header(data, path, locales, countries):
    f = open(path, "w")
    f.write("#ifndef APPS_I18N_H\n")
    f.write("#define APPS_I18N_H\n\n")
    f.write("// This file is auto-generated by i18n.py\n\n")
    f.write("#include <escher.h>\n")
    f.write("#include <apps/country_preferences.h>\n\n")
    f.write("namespace I18n {\n\n")
    f.write("constexpr static int NumberOfLanguages = %d;\n\n" % len(locales))
    f.write("constexpr static int NumberOfCountries = %d;\n\n" % len(countries))

    # Messages enumeration
    print_block_from_list(f,
            "enum class Message : uint16_t {\n  Default = 0,\n",
            data["universal_messages"],
            footer="\n")
    print_block_from_list(f,
            "  LocalizedMessageMarker,\n\n",
            data["messages"])

    # Languages enumeration
    print_block_from_list(f,
            "enum class Language : uint8_t {\n",
            locales,
            lambda arg: arg.upper())

    # Language names
    print_block_from_list(f,
            "constexpr const Message LanguageNames[NumberOfLanguages] = {\n",
            locales,
            lambda arg: arg.upper(),
            "  Message::Language")

    if generate_ISO6391():
        print_block_from_list(f,
                "constexpr const Message LanguageISO6391Names[NumberOfLanguages] = {\n",
                locales,
                lambda arg: arg.upper(),
                "  Message::LanguageISO6391")

    # Countries enumeration
    print_block_from_list(f,
            "enum class Country : uint8_t {\n",
            countries,
            lambda arg: arg.upper())
    defaultCountry = countries[-1]

    # Country names
    print_block_from_list(f,
            "constexpr const Message CountryNames[NumberOfCountries] = {\n",
            countries,
            lambda arg: arg.upper(),
            "  Message::Country")

    # Language preferences
    f.write("constexpr static Country DefaultCountryForLanguage[NumberOfLanguages] = {\n")
    for language in locales:
        key = language if (language in languagePreferences) else '??'
        header, country = languagePreferences[key]
        line = "  " + header + "::" + (country if country in countries else defaultCountry)
        f.write(line + ",\n")
    f.write("};\n\n")

    # Country preferences
    f.write("constexpr static CountryPreferences CountryPreferencesArray[] = {\n")
    for country in countries:
        key = country if (country in countryPreferences) else defaultCountry
        line = "  CountryPreferences("
        for param in countryPreferences[key]:
            line += param + ", "
        f.write(line[:-2] + "),\n")
    f.write("};\n\n")

    f.write("}\n\n")
    f.write("#endif\n")
    f.close()

def print_implementation(data, path, locales):
    f = open(path, "w")
    f.write("#include \"i18n.h\"\n")
    f.write("#include <apps/global_preferences.h>\n")
    f.write("#include <assert.h>\n\n")
    f.write("namespace I18n {\n\n")


    # Write the default message
    f.write("constexpr static char universalDefault[] = {0};\n")

    # Write the universal messages
    for message in data["universal_messages"]:
        f.write("constexpr static char universal" + message + "[] = ")
        f = open(path, "ab") # Re-open the file as binary to output raw UTF-8 bytes
        f.write(data["data"]["universal"][message])
        f = open(path, "a") # Re-open the file as text
        f.write(";\n")
    f.write("\n")
    print_block_from_list(f,
            "constexpr static const char * universalMessages[%d] = {\n  universalDefault,\n" % (len(data["universal_messages"])+1),
            data["universal_messages"],
            prefix="  universal")

    # Write the localized messages
    for message in data["messages"]:
        for locale in locales:
            if not locale in data["data"]:
                sys.stderr.write("Error: Undefined locale \"" + locale + "\"\n")
                sys.exit(-1)
            if not message in data["data"][locale]:
                sys.stderr.write("Error: Undefined key \"" + message + "\" for locale \"" + locale + "\"\n")
                sys.exit(-1)
            f.write("constexpr static char " + locale + message + "[] = ")
            f = open(path, "ab") # Re-open the file as binary to output raw UTF-8 bytes
            f.write(data["data"][locale][message])
            f = open(path, "a") # Re-open the file as text
            f.write(";\n")
    f.write("\n")
    f.write("constexpr static const char * messages[%d][%d] = {\n" % (len(data["messages"]), len(locales)))
    for message in data["messages"]:
        f.write("  {")
        for locale in locales:
            f.write(locale + message + ", ")
        f.write("},\n")
    f.write("};\n\n")


    # Write the translate method
    f.write("const char * translate(Message m) {\n")
    f.write("  assert(m != Message::LocalizedMessageMarker);\n")
    f.write("  int localizedMessageOffset = (int)Message::LocalizedMessageMarker+1;\n")
    f.write("  if ((int)m < localizedMessageOffset) {\n")
    f.write("    assert(universalMessages[(int)m] != nullptr);\n")
    f.write("    return universalMessages[(int)m];\n")
    f.write("  }\n")
    f.write("  int languageIndex = (int)GlobalPreferences::sharedGlobalPreferences()->language();\n")
    f.write("  int messageIndex = (int)m - localizedMessageOffset;\n")
    f.write("  assert((messageIndex*NumberOfLanguages+languageIndex)*sizeof(char *) < sizeof(messages));\n")
    f.write("  return messages[messageIndex][languageIndex];\n")
    f.write("}\n\n")
    f.write("}\n")
    f.close()

data = parse_files(args.files)
if args.header:
    print_header(data, args.header, args.locales, args.countries)
if args.implementation:
    print_implementation(data, args.implementation, args.locales)
