#!/bin/bash
# Extract screenshot for each event in the state file

if [ $# -lt 2 ]; then
  echo "Not enough parameters"
  echo "take_all_screenshots.sh path/to/Epsilon path/to/state_file [output_folder]"
  exit 1
fi

epsilon=$1
state_file=$2
output_folder=${3:-screenshots}

mkdir -p $output_folder

HEADER_SIZE=15

original_size=$(stat -f %z ${state_file})
size=$original_size

while [ $size -gt $HEADER_SIZE ]
do
  event=$(xxd -p -l1 -s $(( size - 1)) ${state_file})
  output=${output_folder}/img-`printf %03d $(( size - HEADER_SIZE ))`-${event}.png
  echo $output
  ./${epsilon} --headless --load-state-file <(head -c${size} ${state_file}) --take-screenshot ${output} > /dev/null
  size=$(( size - 1 ))
done

echo ${output_folder}/bug.gif
convert -loop 0 ${output_folder}/img*.png ${output_folder}/bug.gif

echo "All done, screenshots taken in ${output_folder}/"