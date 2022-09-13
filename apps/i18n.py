# -*- coding: utf-8 -*-

# This script gathers all .i18n files and aggregates them as a pair of .h/.cpp
# file.
# In practice, it enforces a NFKD normalization. Because Epsilon does not
# properly draw upper case letters with accents, we remove them here.
# If compression is activated, texts are grouped by languages, and compressed
# with LZ4 algorithm to save memory. When another language is selected, it is
# uncompressed into a huge buffer, where the messages texts are retrieved.
# It works with Python 3 only

import argparse
import csv
import io
import re
import sys
import unicodedata
import lz4.frame


parser = argparse.ArgumentParser(description="Process some i18n files.")
parser.add_argument('--header', help='the .h file to generate')
parser.add_argument('--implementation', help='the .cpp file to generate')
parser.add_argument('--locales', nargs='+', help='locale to actually generate')
parser.add_argument('--countries', nargs='+', help='countries to actually generate')
parser.add_argument('--codepoints', help='the code_points.h file')
parser.add_argument('--countrypreferences', help='the country_preferences.csv file')
parser.add_argument('--languagepreferences', help='the language_preferences.csv file')
parser.add_argument('--files', nargs='+', help='an i18n file')
parser.add_argument('--compress', action='store_true', help='if the texts should be compressed')\

args = parser.parse_args()

message_length_limit_for_type = {
    "toolbox" : 35, # Toolbox maximal available space with small font
    "default" : 45  # Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small)
}

# When building with less locales, redundant messages are not checked for
default_number_of_locales = 7

def has_glyph(glyph):
    return glyph in codepoints

def source_definition(i18n_string):
    s = unicodedata.normalize("NFKD", i18n_string)
    result = u""
    if not(args.compress):
        result += u"\""
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
        if args.compress and s[i] == '\\' and i+1 < length:
            # When compressing, we need to use combined chars so that the
            # correct chars will be uncompressed.
            combinedChar = ''
            if s[i+1] == '\\':
                # Combine "\\" into '\'
                combinedChar = '\\'
            elif s[i+1] == 'n':
                # Combine "\n" into '\n'
                combinedChar = '\n'
            elif s[i+1] == '"':
                # Combine "\"" into '"'
                combinedChar = '"'
            elif (i+3 < length) and s[i+1] == 'x' and s[i+2] == '1' and s[i+3] == '1':
                # Combine "\x11" into '\x11'
                combinedChar = '\x11'
                i+=2
            else :
                sys.stderr.write(" \\" + str(s[i+1]) + " is not handled with compression. Exiting !\n")
                sys.exit(-1)
            if combinedChar != '':
                result = result + combinedChar
                i+=2
                continue
        if copyCodePoint:
            # Remove the uppercase characters with combining chars
            checkForCombining = s[i].isupper()
            result = result + s[i]
        if not has_glyph(s[i]):
            sys.stderr.write(s[i] + " (" + str(hex(ord(s[i]))) + ") is not a character present in " + args.codepoints + " . Exiting !\n")
            sys.exit(-1)
        i = i+1
    if not(args.compress):
        result += u"\""
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

def type_from_filename(filename):
    return re.match(r".*\/([a-z_]+)\.[a-z]+\.i18n", filename).group(1)

def message_exceeds_length_limit(definition, type):
    if not(type in message_length_limit_for_type):
        type = "default"
    length_limit = message_length_limit_for_type[type]
    # Handle multi-line messages
    if args.compress:
        iterator = re.split(r"\n", definition.decode('utf-8'))
    else:
        iterator = re.split(r"\\n", definition.decode('utf-8')[1:-1])
    for definition_line in iterator:
        # Ignore combining characters
        if (len([c for c in definition_line if not unicodedata.combining(c)]) > length_limit):
            return True
    return False

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
        type = type_from_filename(path)
        if locale not in data:
            data[locale] = {}
        with io.open(path, "r", encoding='utf-8') as file:
            for line in file:
                if is_commented_line(line):
                    continue
                name,definition = split_line(line)
                if name in data[locale]:
                    sys.stderr.write("Error: Redefinition of message \"" + name + "\" in locale " + locale + "\n")
                    sys.exit(-1)
                if locale == "universal":
                    if name in messages:
                        sys.stderr.write("Error: Redefinition of message \"" + name + "\" as universal\n")
                        sys.exit(-1)
                    universal_messages.add(name)
                else:
                    if name in universal_messages:
                        sys.stderr.write("Error: Redefinition of universal message \"" + name + "\" in locale " + locale + "\n")
                        sys.exit(-1)
                    messages.add(name)
                if message_exceeds_length_limit(definition, type) and name != "TermsOfUseLink":
                    sys.stderr.write("Error: Message exceeds length limits for " + type + " : " + definition.decode('utf-8') + " (" + name + ")\n")
                    sys.exit(-1)
                data[locale][name] = definition
    if (len(args.locales) >= default_number_of_locales):
        check_redundancy(messages, data, args.locales)
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

def print_block_from_list(target, header, data, beautify=lambda arg: arg, prefix="  ", footer="};\n\n", elementPerLine=1):
    target.write(header)
    lineIndex = 0
    for i in range(len(data)):
        if (i%elementPerLine == 0):
            target.write(prefix)
        target.write(beautify(data[i]) + ",")
        if ((i+1)%elementPerLine == 0):
            target.write("\n")
        else:
            target.write(" ")
    target.write(footer)

def print_header(data, path, locales, countries):
    f = open(path, "w")
    f.write("#ifndef APPS_I18N_H\n")
    f.write("#define APPS_I18N_H\n\n")
    f.write("// This file is auto-generated by i18n.py\n\n")
    f.write("#include <escher/i18n.h>\n")
    f.write("#include <apps/country_preferences.h>\n\n")
    f.write("namespace I18n {\n\n")
    f.write("constexpr static int NumberOfLanguages = %d;\n\n" % len(locales))
    f.write("constexpr static int NumberOfCountries = %d;\n\n" % len(countries))

    # Messages enumeration
    print_block_from_list(f,
            "enum class Message : uint16_t {\n",
            ["Default = 0"] + data["universal_messages"],
            footer="\n")

    if not(args.compress):
        f.write("  LocalizedMessageMarker,\n")

    print_block_from_list(f,
            "\n",
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

    # Language ISO639-1 codes
    f.write("constexpr const char * LanguageISO6391Codes[NumberOfLanguages] = {\n");
    for locale in locales:
        f.write("  \"" + locale + "\",\n")
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
    code = """
const char * translate(Message m) {
  assert(m != Message::LocalizedMessageMarker);
  int localizedMessageOffset = (int)Message::LocalizedMessageMarker+1;
  if ((int)m < localizedMessageOffset) {
    assert(universalMessages[(int)m] != nullptr);
    return universalMessages[(int)m];
  }
  int languageIndex = (int)GlobalPreferences::sharedGlobalPreferences()->language();
  int messageIndex = (int)m - localizedMessageOffset;
  assert((messageIndex*NumberOfLanguages+languageIndex)*sizeof(char *) < sizeof(messages));
  return messages[messageIndex][languageIndex];
}

}
"""
    f.write(code)
    f.close()

def print_compressed_implementation(data, path, locales):
    f = open(path, "w")
    f.write("#include \"i18n.h\"\n")
    f.write("#include <apps/global_preferences.h>\n")
    f.write("#include <assert.h>\n\n")
    f.write("namespace I18n {\n\n")

    # Messages count
    f.write("constexpr static size_t universalMessagesCount = " + str(len(data["universal_messages"]) + 1) + ";\n")
    f.write("constexpr static size_t localizedMessagesCount = " + str(len(data["messages"])) + ";\n")
    f.write("constexpr static size_t totalMessagesCount = universalMessagesCount + localizedMessagesCount;\n\n")

    # Concatenate texts by locale (+ universal), keep track of message positions
    messagePosition = [0]
    concatenatedData={"universal" : b''}
    for locale in locales:
        if not locale in data["data"]:
            sys.stderr.write("Error: Undefined locale \"" + locale + "\"\n")
            sys.exit(-1)
        concatenatedData[locale] = b''

    # Add universalDefault message
    messagePosition.append(1 + messagePosition[-1])
    concatenatedData["universal"] += b'\x00'

    # Concatenate the universal messages
    for message in data["universal_messages"]:
        messageSize = len(data["data"]["universal"][message])+1
        messagePosition.append(messageSize + messagePosition[-1])
        concatenatedData["universal"] += data["data"]["universal"][message] + b'\x00'

    # Concatenate the localized messages
    for message in data["messages"]:
        maxMessageSize = 0
        # Compute max message size
        for locale in locales:
            if not message in data["data"][locale]:
                sys.stderr.write("Error: Undefined key \"" + message + "\" for locale \"" + locale + "\"\n")
                sys.exit(-1)
            messageSize = len(data["data"][locale][message])+1
            maxMessageSize = max(maxMessageSize, messageSize)
            concatenatedData[locale] += data["data"][locale][message] + b'\x00'
        # Fill remaining space with empty chars. This allow having the same
        # concatenatedData size for all locales
        for locale in locales:
            messageSize = len(data["data"][locale][message])+1
            concatenatedData[locale] += b'\x00' * (maxMessageSize - messageSize)
        messagePosition.append(maxMessageSize + messagePosition[-1])
    f.write("\n")

    # Remove last message position, which is total uncompressed size
    TotalUncompressedSize = messagePosition.pop()

    # Message position within the uncompressed text buffer
    print_block_from_list(f,
        "constexpr static const size_t messagePosition[totalMessagesCount] = {\n",
        messagePosition, lambda arg: str(arg), prefix="  ", elementPerLine=254)

    # Uncompressed sizes data
    localizedUncompressedSize = 0
    for locale in locales:
        uncompressedSize = len(concatenatedData[locale])
        # assert all language take the same space uncompressed
        if (localizedUncompressedSize != 0 and uncompressedSize != localizedUncompressedSize):
            sys.stderr.write("Error: Language uncompressed size differs : \"" + locale + "\"\n")
            sys.exit(-1)
        localizedUncompressedSize = uncompressedSize
    universalUncompressedSize = len(concatenatedData["universal"])
    if (universalUncompressedSize + localizedUncompressedSize != TotalUncompressedSize):
        sys.stderr.write("Error: Total uncompressed size differs : " + str(universalUncompressedSize + localizedUncompressedSize) + " instead of " + str(TotalUncompressedSize) + "\n")
        sys.exit(-1)
    f.write("constexpr static size_t universalUncompressedSize = " + str(universalUncompressedSize) + ";\n")
    f.write("constexpr static size_t localizedUncompressedSize = " + str(localizedUncompressedSize) + ";\n")
    f.write("// Big buffer where current language messages are stored.\n")
    f.write("static char uncompressedText[universalUncompressedSize + localizedUncompressedSize] = \"\";\n")
    f.write("\n")

    # Compressed data
    for locale in ["universal"] + locales:
        # Compress concatenatedData
        compressedData = lz4.frame.compress(concatenatedData[locale], compression_level=12)
        # Trim unused headers and footers
        compressedData = compressedData[4+15:-4]
        # Compressed data size
        f.write("constexpr static size_t " + locale + "CompressedSize = " + str(len(compressedData)) + ";\n")
        # Compressed bytes
        print_block_from_list(f,
            "const uint8_t " + locale + "Compressed[" + locale + "CompressedSize] = {\n",
            compressedData, lambda arg: "0x{:02x}".format(arg), prefix="  ", elementPerLine=254)

    # Arrange locale specific compressed data information in an array
    print_block_from_list(f,
        "constexpr static const size_t localizedCompressedSizes[] = {\n",
        locales, lambda arg: arg + "CompressedSize", prefix="  ")

    print_block_from_list(f,
        "constexpr static const uint8_t * localizedCompressed[] = {\n",
        locales, lambda arg: arg + "Compressed", prefix="  ")

    # Write the translate method
    code = """
void unpackUniversal() {
  Ion::decompress(universalCompressed, (uint8_t *)uncompressedText, universalCompressedSize, universalUncompressedSize);
}

void unpackLocalized(int selectedLanguage) {
  assert(selectedLanguage >= 0 && selectedLanguage < NumberOfLanguages);
  Ion::decompress(localizedCompressed[selectedLanguage], (uint8_t *)(uncompressedText + universalUncompressedSize), localizedCompressedSizes[selectedLanguage], localizedUncompressedSize);
}

// Last loaded language
static int currentLanguage = -1;

const char * translate(Message m) {
  if (currentLanguage == -1) {
    // Universal messages must be loaded once
    unpackUniversal();
  }
  int selectedLanguage = (int)GlobalPreferences::sharedGlobalPreferences()->language();
  if (selectedLanguage != currentLanguage) {
    // Load and update selected language
    unpackLocalized(selectedLanguage);
    currentLanguage = selectedLanguage;
  }
  size_t messageIndex = (size_t)m;
  assert(messagePosition[messageIndex] < sizeof(uncompressedText));
  return uncompressedText + messagePosition[messageIndex];
}

}
"""
    f.write(code)
    f.close()

data = parse_files(args.files)
if args.header:
    print_header(data, args.header, args.locales, args.countries)
if args.implementation:
    if args.compress:
        print_compressed_implementation(data, args.implementation, args.locales)
    else:
        print_implementation(data, args.implementation, args.locales)
