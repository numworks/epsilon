ResetAll = "\033[0m"
Bold = "\033[1m"
Underlined = "\033[4m"
Red = "\033[31m"
Green = "\033[32m"


def bold(text):
    return Bold + text + ResetAll


def underlined(text):
    return Underlined + text + ResetAll


def red(text):
    return Red + text + ResetAll


def green(text):
    return Green + text + ResetAll


def print_bold(text):
    print(bold(text))


def print_underlined(text):
    print(underlined(text))
