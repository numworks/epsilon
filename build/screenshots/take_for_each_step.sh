#!/bin/bash
# Extract screenshot for each event in the state file

if [ $# -lt 2 ]; then
  echo "Not enough parameters"
  echo "take_for_each_step.sh path/to/Epsilon path/to/state_file [output_folder]"
  exit 1
fi

epsilon=$1
state_file=$2
output_folder=${3:-screenshots}

mkdir -p $output_folder
rm ${output_folder}/img*.png

./${epsilon} --headless --load-state-file ${state_file} --take-all-screenshots ${output_folder} > /dev/null

echo ${output_folder}/bug.gif
convert -loop 0 ${output_folder}/img*.png ${output_folder}/bug.gif

echo "All done, screenshots taken in ${output_folder}/"
