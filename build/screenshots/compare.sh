#!/bin/bash

set -e

source "$(dirname "$0")/helper.sh"

function print_help() {
  echo -e "Usage: compare [--debug] [MAKEFLAGS=...] <folder_with_scenari> <source_1> <source_2>"
  echo -e "\nCompare two sources of screenshots on a sequence of scenari (state files)"
  echo -e "A source can either be:"
  echo -e " - a folder, containing png images of the same name as the state files"
  echo -e " - an Epsilon executable"
  echo -e " - a git ref (i.e. a commit hash, a branch, HEAD...)"
  echo -e "Outputs a report of which screenshot mismatched, and stores the corresponding images"
  echo -e "\nExample:"
  echo -e "\t$ compare scenari/ Epsilon_master Epsilon_new"
  echo -e "\t$ compare scenari/ folder_with_images/ Epsilon_new"
  echo -e "\t$ compare MAKEFLAGS=\"-j4 PLATFORM=simulator DEBUG=1\" scenari/ folder_with_images/ HEAD"
}


debug=0
if [[ $1 == "--debug" ]]
then
  debug=1
  shift
fi

if [[ $# -lt 3 ]]; then
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

# if only 2 args, generate all screenshots
if [[ $# -eq 2 ]]; then
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
fi

# if 3 arguments, compare screenshots
output_folder="compare_output_$(date +%d-%m-%Y_%Hh%M)"
mkdir -p ${output_folder}

parse_arg "$2" 1
parse_arg "$3" 2

log args ${arg1_mode} ${arg1} ${arg2_mode} ${arg2}

echo -e "Comparing screenshots"

for state_file in "${scenari_folder}"/*.nws
do
  filestem=$(stem "${state_file}")
  log state_file: "$filestem"

  out_file1="${output_folder}/${filestem}-1.png"
  out_file2="${output_folder}/${filestem}-2.png"

  # Extract screenshots
  create_img 1 "${out_file1}"
  create_img 2 "${out_file2}"

  # Compare screenshots
  out_diff="${out_file1%-1.png}-diff.png"
  compare_images "${out_file1}" "${out_file2}" "${out_diff}"

done

print_report

# Cleanup
if [[ "$count" == 0 ]] && [[ "$debug" == 0 ]]
then
  rm -r "$output_folder"
fi

exit $count
