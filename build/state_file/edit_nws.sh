#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Not enough parameters."
  echo "Syntax: 'edit_nws.sh NWS_PATH' [NWS2TXT_PATH]"
  exit 1
fi

NWS_FILE=$1
TMP_FILE=nws_edit_$(date +%d.%m.%y-%H.%M.%S).txt

python3 build/state_file/nws2txt.py $NWS_FILE > $TMP_FILE &&
${EDITOR:-vim} $TMP_FILE &&
python3 build/state_file/nws2txt.py $TMP_FILE > $NWS_FILE &&
rm $TMP_FILE &&
echo "$1 was successfully edited"
