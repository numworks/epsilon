#!/bin/bash

set -e

source "$(dirname "$0")/helper.sh"

function print_help() {
  echo -e "\nUsage: take_tests_screenshots [--debug] [MAKEFLAGS=...] <folder_with_scenari> <source>"
  echo -e "Generate a screenshot of the final state of each scenari (useful when creating new scenari)."
  echo -e "A source can either be:"
  echo -e " - an Epsilon executable"
  echo -e " - a git ref (i.e. a commit hash, a branch, HEAD...)"
  echo -e "\nExample:"
  echo -e "\t$ compare scenari/ Epsilon_master"
}

debug=0
if [[ $1 == "--debug" ]]
then
  debug=1
  shift
fi

if [[ $# -lt 2 ]]; then
  error "Error: not enough arguments"
  print_help
  exit 1
fi

log "START"

parse_makeflags "$1"
if [ $hasFlags = 1 ]
then
  shift
fi

scenari_folder="$1"
if ! find "${scenari_folder}" -type f -name '*.nws' > /dev/null
then
  error "No state file found in ${scenari_folder}"
  exit 3
fi

output_folder="generated_screenshots$(date +%d-%m-%Y_%Hh%M)"
mkdir -p ${output_folder}

parse_arg "$2" 1

log args ${arg1_mode} ${arg1}

echo "Generating screenshots"

for state_file in "${scenari_folder}"/*.nws
do
  filestem=$(stem "${state_file}")
  log state_file: "$filestem"

  out_file1="${output_folder}/${filestem}.png"

  # Extract screenshots
  create_img 1 "${out_file1}"
done
print_report
exit
