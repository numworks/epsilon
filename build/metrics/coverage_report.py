import argparse
import parse
from dataclasses import dataclass


@dataclass
class Summary:
    percentage: float
    hits: int
    total: int


# We do not display "UBC" (uncovered base coverage)  and "CBC" (covered base coverage), both of which are always present in
# the differential report, because they are redundant with the coverage summary (hits / total).
# A detailed description of the categories can be found here: https://manpages.debian.org/unstable/lcov/genhtml.1.en.html#
categories_to_display = {
    "UNC": "⚠️ Uncovered New Code",
    "UIC": "⚠️ Uncovered Included Code",
    "LBC": "⚠️ Lost Baseline Coverage",
    "GNC": "✅ Gained New Coverage",
    "GIC": "✅ Gained Included Coverage",
    "GBC": "✅ Gained Baseline Coverage",
    "EUB": "Excluded Uncovered Baseline",
    "ECB": "Excluded Covered Baseline",
    "DUB": "Deleted Uncovered Baseline",
    "DCB": "Deleted Covered Baseline",
}


class Diff:
    def __init__(self):
        self.line_counts = {
            "UNC": 0,
            "UIC": 0,
            "LBC": 0,
            "GNC": 0,
            "GIC": 0,
            "GBC": 0,
            "EUB": 0,
            "ECB": 0,
            "DUB": 0,
            "DCB": 0,
        }


def format_summary(base_summary: Summary, head_summary: Summary):
    return f"""
  |                       |                     **BASE**                      |                     **HEAD**                      |
  |-----------------------|:-------------------------------------------------:|:-------------------------------------------------:|
  |  Line coverage rate   |           {base_summary.percentage} %             |            {head_summary.percentage} %            |
  |     Hits / Total      |     {base_summary.hits} / {base_summary.total}    |    {head_summary.hits} / {head_summary.total}     |"""


def format_diff(diff: Diff):
    output = """
  |                     |   **Lines**                                   |
  |---------------------|:---------------------------------------------:|"""
    for category, description in categories_to_display.items():
        if diff.line_counts[category] > 0:
            output += f"""
  |     {description}   |        {diff.line_counts[category]}           |"""
    return output


def parse_summary(output_txt: str):
    line_summary = None
    with open(output_txt, "r") as f:
        lines = f.readlines()
    for line in lines:
        if line.lstrip().startswith("lines"):
            percentage, hits, total = parse.search(": {}% ({} of {} lines)", line)
            line_summary = Summary(percentage, hits, total)
    return line_summary


def parse_diff(genhtml_txt: str):
    with open(genhtml_txt, "r") as f:
        lines = f.readlines()
        diff = Diff()
        is_editing = False
        for line in lines:
            if line.lstrip().startswith("Message"):
                break
            if is_editing:
                category, count = parse.parse("{}...: {}", line.lstrip())
                if category in categories_to_display:
                    diff.line_counts[category] = int(count)
            if line.lstrip().startswith("lines"):
                is_editing = True
    return diff


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Parse genhtml output")
    parser.add_argument(
        "genhtml_txt",
        type=str,
        help="file containing the text output of the genhtml command (starting from 'Overall coverage rate')",
    )
    parser.add_argument(
        "--base_output",
        help="file containing the text output of the base coverage build (starting from 'Summary coverage rate')",
    )
    parser.add_argument(
        "--summary", help="parse the coverage summary", action="store_true"
    )
    parser.add_argument(
        "--diff", help="parse the coverage differential data", action="store_true"
    )
    args = parser.parse_args()

    if args.summary:
        print(
            format_summary(
                parse_summary(args.base_output), parse_summary(args.genhtml_txt)
            )
        )

    if args.diff:
        print(format_diff(parse_diff(args.genhtml_txt)))
