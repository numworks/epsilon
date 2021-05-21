#!/bin/bash

set -e

# === Functions definition ===

function print_help() {
  echo -e "Usage: compare [--debug] [MAKEFLAGS=...] <folder_with_scenarii> <source_1> <source_2>"
  echo -e "\nCompare two sources of screenshots on a sequence of scenari (state files)"
  echo -e "A source can either be:"
  echo -e " - a folder, containing png images of the same name as the state files"
  echo -e " - an Epsilon executable"
  echo -e " - a git ref (i.e. a commit hash, a branch, HEAD...)"
  echo -e "Outputs a report of which screenshot mismatched, and stores the corresponding images"
  echo -e "\nExample:"
  echo -e "\t$ compare Epsilon_master Epsilon_new"
  echo -e "\t$ compare folder_with_images/ Epsilon_new"
  echo -e "\t$ compare MAKEFLAGS=\"-j4 PLATFORM=simulator DEBUG=1\" folder_with_images/ HEAD"
}


debug=0
if [[ $1 == "--debug" ]]
then
  debug=1
  shift
fi
function log() {
  if [[ ${debug} == "1" ]]; then
    echo -e DEBUG: "$@"
  fi
}

function error() {
  echo -e "$@" 1>&2
}

function stem() {
  filename=$(basename "$1")
  echo "${filename%.*}"
}

count=0

# compare <png1> <png2> <png_output>
function compare() {
  log "compare $1 $2 $3"
  res=$(magick compare -metric mae "$1" "$2" "$3" 2>&1 || return 0)
  if [[ ${res} == "0 (0)" ]]
  then
    rm "$3"
    echo -e "\033[1m${state_file}\t \033[32mOK\033[0m"
  else
    echo -e "\033[1m${state_file}\t\033[0m \033[31m${res}\033[0m (diff saved as $3)"
    count=$((count+1))
  fi
}

# imgs_for_executable <executable> <png_output>
function img_for_executable() {
  log "img_for_exe $1 $2"
  args="--headless --load-state-file ${state_file} --take-screenshot $2"
  cmd="./$1 ${args}"
  log "${cmd}"
  eval "$cmd"
}

# create_git_executable <ref> <arg_number>
function create_git_executable() {
  # TODO catch if --take-screenshot not supported
  echo "Creating executable from ref $1"
  git checkout $1 > /dev/null
  output_exe="${output_folder}/Epsilon_$1"
  log "make ${MAKEFLAGS}"
  make ${MAKEFLAGS}

  # TODO guess real path
  cp output/debug/simulator/macos/epsilon.app/Contents/MacOS/Epsilon "${output_exe}"
  eval exe$2=${output_exe}
  echo "Executable stored at ${output_exe}"
}

MAKEFLAGS=

function parse_makeflags() {
  if [[ $1 == "MAKEFLAGS="* ]]
  then
    MAKEFLAGS="${1#MAKEFLAGS=}"
    echo true
  else
    MAKEFLAGS="PLATFORM=simulator DEBUG=1"
  fi
  log MAKEFLAGS=${MAKEFLAGS}
}

arg1_mode=
arg2_mode=
arg1=
arg2=
exe1=
exe2=

# parse_arg <arg> <arg_number>
function parse_arg() {
  log parse_arg $1 $2
  eval arg$2="$1"
  if [[ -d "$1" ]]
  then
    # directory -> images must be stored inside
    eval arg$2_mode="d"
  elif [[ -x "$1" ]]
  then
    # executable -> must be espilon
    eval arg$2_mode="e"
    eval exe$2=$1
  elif git show-ref "$1" > /dev/null
  then
    # git ref
    eval arg$2_mode="g"
    create_git_executable "$1" $2
  else
    echo "Error: argument not recognised $1"
    exit 1
  fi
}

# create_img <arg_number> <output>
function create_img() {
  log "create_img $1 $2"
  arg=arg$1
  arg_mode=arg${1}_mode
  case "${!arg_mode}" in
    "d")
      img="${!arg}/$(stem ${state_file}).png"
      echo img="${img}"
      cp "${img}" "$2"
      ;;
    "e" | "g")
      exe=exe$1
      echo executable at ${!exe}
      img_for_executable "${!exe}" "$2"
      ;;
  esac
}


function print_report() {
  echo "=============================="
  if [[ "$count" -gt 0 ]]
  then
    echo "${count} failed"
  else
    echo "All good!"
  fi
}


# === Main ===


if [[ $# -lt 3 ]]; then
  error "Error: not enough arguments"
  print_help
  exit 1
fi

log "START"

if parse_makeflags "$1"
then
  shift
fi

scenarii_folder="$1"
if [[ $( find "${scenarii_folder}" -type f -name "*.nws" -depth 1 | wc -w ) -eq 0 ]]
then
  error "No state file found in ${scenarii_folder}"
  exit 3
fi

output_folder="compare_output_$(date +%d-%m-%Y_%Hh%M)"
mkdir -p ${output_folder}

parse_arg "$2" 1
parse_arg "$3" 2

log args ${arg1_mode} ${arg1} ${arg2_mode} ${arg2}

for state_file in "${scenarii_folder}"/*.nws
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
  compare "${out_file1}" "${out_file2}" "${out_diff}"

done

print_report
