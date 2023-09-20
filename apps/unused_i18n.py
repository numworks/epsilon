import os

def discardMessagesInFile(fileName, messages):
  f = open(fileName, 'r')
  file = f.read()
  currentNMessages = len(messages)
  j = 0
  while j < currentNMessages:
    if "Message::" + messages[j] in file:
      # Message foud in file
      messages.pop(j)
      currentNMessages -= 1
    else:
      j += 1
  f.close()

def discardMessagesInDir(dirName, messages):
  # Exploring a directory
  direc = os.listdir(dirName)
  for fname in direc:
    fullFileName = dirName + os.sep + fname
    if os.path.isfile(fullFileName) and (fname.endswith(".h") or fname.endswith(".cpp")) :
      discardMessagesInFile(fullFileName, messages)
    elif os.path.isdir(fullFileName) :
      discardMessagesInDir(fullFileName, messages)

def checkUnusedMessages(i18n_header, i18n_implementation):
  with open(i18n_header) as f:
    messages = f.read().splitlines()
    nLines = len(messages)
    start = -1
    end = -1
    for i in range(nLines):
      if messages[i].startswith("enum class Message"):
        start = i + 1
      if start >= 0 and end < 0 and messages[i].startswith("};"):
        end = i
    messages = messages[start + 1:end] # Skip DEFAULT
    nLines = len(messages)
    for k in range(nLines):
      messages[k] = messages[k].strip()
      messages[k] = messages[k].strip(',')

    # Remove messages that are used in i18n.h and i18n.cpp enums and methods
    # (namely CountryXX, LanguageXX and LocalizedMessageMarker)
    discardMessagesInFile(i18n_header, messages)
    if (len(i18n_implementation) != 0):
      discardMessagesInFile(i18n_implementation, messages)

    discardMessagesInDir("apps", messages)
    discardMessagesInDir("escher", messages)
    if (len(messages) > 0):
      print(" >>> WARNING: Unused i18n messages :" + str(messages))
