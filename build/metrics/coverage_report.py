# ### Coverage report

# #### Coverage rate for commit bace878

# |               	        |   **Lines**   	        | **Functions** 	|
# |---------------	|:-------------:	|:-------------:	|
# | Coverage rate 	|     65.6%     	        |     73.2%     	        |
# | Hits / Total  	        | 55059 / 83952 	| 12592 / 17209 	|

# #### Differences with base branch (bace878)

# |               	                              |   **Lines**   	        | **Functions** 	|
# |---------------	                      |:-------------:	|:-------------:	|
# | ⚠️ Uncovered New Code 	      |     10      	        |     3       	        |
# | ⚠️ Lost Baseline Coverage        |     311               |     86       	        |
# | ✅ Gained New Coverage          |     6      	        |     2       	        |
# | ✅ Gained Baseline Coverage   |     4      	        |     2       	        |

# Download full report: https://github.com/numworks/epsilon-internal/actions/runs/10490330078/artifacts/1837391704

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

@dataclass
class Diff:
  UNC: int = 0
  LBC: int = 0
  GNC: int = 0
  GBC: int = 0

@dataclass
class FullDiff:
  lines: Diff
  functions: Diff

def format_summary(summary: FullSummary):
  return f"""|    |   **Lines**                                   | **Functions**                                         |
|---------------  |:-------------:                                |:-------------:                                        |
| Coverage rate   |          {summary.lines.percentage} %         |            {summary.functions.percentage} %           |
| Hits / Total    | {summary.lines.hits} / {summary.lines.total}  | {summary.functions.hits} / {summary.functions.total}  |"""


def format_diff(differential: FullDiff):
  return "diff"

def parse_summary(genhtml_txt: str):
  with open(genhtml_txt, "r") as f:
    lines = f.readlines()
  for line in lines:
    if line.lstrip().startswith("lines"):
      result = parse.search(": {}% ({} of {} lines)", line)
      line_summary = Summary(result[0], result[1], result[2])
    if line.lstrip().startswith("functions"):
      result = parse.search(": {}% ({} of {} functions)", line)
      function_summary = Summary(result[0], result[1], result[2])
  return FullSummary(line_summary, function_summary)

def parse_diff(genhtml_txt):
  return FullDiff(Diff(UNC=3, GNC=10), Diff(GBC=2, LBC=24))


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="Parse genhtml output")
  parser.add_argument("genhtml_txt", type=str, help="file containing the text output of the genhtml command (starting from 'Overall coverage rate')")
  parser.add_argument("--summary", help="parse the coverage summary", action="store_true")
  parser.add_argument("--diff", help="parse the coverage differential data", action="store_true")
  args = parser.parse_args()

  if args.summary:
    print(format_summary(parse_summary(args.genhtml_txt)))

  if args.diff:
    print(format_diff(parse_diff(args.genhtml_txt)))
