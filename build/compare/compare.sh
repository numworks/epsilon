#!/bin/bash

if [[ $# -lt 3 ]]; then
  echo "Compare two Epsilon executable on a sequence of scenari (state files)"
  echo "Output a report of all scenari whose screenshot are not exactly the same"
  echo "Usage: compare <epsilon_1> <epsilon_2> <folder_with_scenarii>"
  exit 1
fi

function stem() {
  filename=$(basename "$1")
  echo "${filename%.*}"
}


exe1=$1
exe2=$2
input_folder=$3

output_folder="compare_output_$(date +%d-%m-%Y_%Hh%M)"
mkdir -p ${output_folder}

count=0

for state_file in "${input_folder}"/*.nws
do
  filestem=$(stem "$state_file")
  args="--headless --load-state-file ${state_file} --take-screenshot"
  out_file1="${output_folder}/${filestem}-1.png"
  out_file2="${output_folder}/${filestem}-2.png"

  # Extract screenshots
  cmd1="./${exe1} ${args} ${out_file1}"
  eval "$cmd1"
  cmd2="./${exe2} ${args} ${out_file2}"
  eval "$cmd2"

  # Compare screenshots
  out_diff="${out_file1%-1.png}-diff.png"
  res=$(magick compare -metric mae "${out_file1}" "${out_file2}" "${out_diff}" 2>&1)
  if [[ ${res} == "0 (0)" ]]
  then
    rm ${out_diff}
    echo -e "\033[1m${state_file}\t \033[32mOK\033[0m"
  else
    echo -e "\033[1m${state_file}\t\033[0m \033[31m${res}\033[0m (diff saved as ${out_diff})"
    count=$((count+1))
  fi

done