#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Not enough parameters."
  echo "Syntax: 'edit_nws.sh NWS_PATH [EPSILON_BIN]'"
  exit 1
fi

ORIGINAL_NWS=$1
DATE=$(date +%d.%m.%y-%H.%M.%S)
FOLDER=nws_edit_$DATE
TXT_FILE=$FOLDER/tmp.txt
NWS_FILE=$FOLDER/tmp.nws

EPSILON_BIN=${2:-}

mkdir $FOLDER
python3 build/state_file/nws2txt.py $ORIGINAL_NWS > $TXT_FILE

while true
do
  ${EDITOR:-vim} $TXT_FILE
  python3 build/state_file/nws2txt.py $TXT_FILE > $NWS_FILE
  if [ $# -gt 1 ]; then
    rm $FOLDER/*.png > /dev/null 2>&1
    rm $FOLDER/*.gif > /dev/null 2>&1
    ./build/screenshots/take_for_each_step.sh $EPSILON_BIN $NWS_FILE $FOLDER > /dev/null 2>&1
    rm $FOLDER/*.png > /dev/null 2>&1
    if [ -f "$FOLDER/scenario.gif" ]; then
      echo "Gif of the new state file was saved in $FOLDER"
    else
      echo "Gif of the new state file could not be generated. Your state file could be badly formatted or your epsilon.bin path could be incorrect."
    fi
  fi
  read -s -n 1 -p "Do you want to Continue edition / Save changes and quit / Quit without saving ? [c/S/q]" csq
  echo
  case $csq in
    [Cc]* ) echo "Continue edition";;
    [Qq]* ) echo "$ORIGINAL_NWS was not modified"; break;;
    * ) mv $NWS_FILE $ORIGINAL_NWS && echo "$ORIGINAL_NWS was successfully edited"; break;;
  esac
done
rm -rf $FOLDER
