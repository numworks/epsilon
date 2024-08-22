import argparse
import parse
from dataclasses import dataclass


@dataclass
class Summary:
    percentage: float
    hits: int
    total: int


@dataclass
class FullSummary:
    lines: Summary
    functions: Summary


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


class FullDiff:
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
        self.function_counts = {
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


def format_summary(summary: FullSummary):
    return f"""
  |                     |   **Lines**                                   | **Functions**                                         |
  |---------------------|:---------------------------------------------:|:-----------------------------------------------------:|
  |    Coverage rate    |          {summary.lines.percentage} %         |            {summary.functions.percentage} %           |
  |    Hits / Total     | {summary.lines.hits} / {summary.lines.total}  | {summary.functions.hits} / {summary.functions.total}  |"""


def format_diff(diff: FullDiff):
    output = """
  |                     |   **Lines**                                   | **Functions**                                         |
  |---------------------|:---------------------------------------------:|:-----------------------------------------------------:|"""

    for category, description in categories_to_display.items():
        if diff.line_counts[category] > 0 or diff.function_counts[category] > 0:
            output += f"""
  |     {description}   |        {diff.line_counts[category]}           |             {diff.function_counts[category]}          |"""
    return output


def parse_summary(genhtml_txt: str):
    with open(genhtml_txt, "r") as f:
        lines = f.readlines()
    for line in lines:
        if line.lstrip().startswith("lines"):
            percentage, hits, total = parse.search(": {}% ({} of {} lines)", line)
            line_summary = Summary(percentage, hits, total)
        if line.lstrip().startswith("functions"):
            percentage, hits, total = parse.search(": {}% ({} of {} functions)", line)
            function_summary = Summary(percentage, hits, total)
    return FullSummary(line_summary, function_summary)


def parse_diff(genhtml_txt: str):
    with open(genhtml_txt, "r") as f:
        lines = f.readlines()
        diff = FullDiff()
        editing = None
        for line in lines:
            if line.lstrip().startswith("lines"):
                editing = diff.line_counts
                continue
            if line.lstrip().startswith("functions"):
                editing = diff.function_counts
                continue
            if line.lstrip().startswith("Message"):
                break
            if editing:
                category, count = parse.parse("{}...: {}", line.lstrip())
                if category in categories_to_display:
                    editing[category] = int(count)
    return diff


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Parse genhtml output")
    parser.add_argument(
        "genhtml_txt",
        type=str,
        help="file containing the text output of the genhtml command (starting from 'Overall coverage rate')",
    )
    parser.add_argument(
        "--summary", help="parse the coverage summary", action="store_true"
    )
    parser.add_argument(
        "--diff", help="parse the coverage differential data", action="store_true"
    )
    args = parser.parse_args()

    if args.summary:
        print(format_summary(parse_summary(args.genhtml_txt)))

    if args.diff:
        print(format_diff(parse_diff(args.genhtml_txt)))
