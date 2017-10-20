#include "script_store.h"
#include "string.h"
#include <stddef.h>

namespace Code {

constexpr char ScriptStore::k_defaultScriptName[];

ScriptStore::ScriptStore() :
  m_numberOfScripts(0),
  m_lastEditedStringPosition(0)
{
  for (int i = 0; i<k_historySize; i ++) {
    m_history[i] = FreeSpaceMarker;
  }
}

Script ScriptStore::editableScript(int i) {
  assert(i >= 0 && i < numberOfScripts());
  cleanAndMoveFreeSpaceAfterScriptContent(i);
  int beginningOfScriptContent = indexOfScriptContent(i);

  // Compute the size of the script, including the free space of m_history
  int sizeOfEditableScript = 0;
  for (int j=beginningOfScriptContent; j<k_historySize-1; j++) {
    if (m_history[j] == FreeSpaceMarker && m_history[j+1] != FreeSpaceMarker) {
      break;
    }
    sizeOfEditableScript++;
  }
  return Script(&m_history[beginningOfScriptContent], sizeOfEditableScript);
}

Script ScriptStore::script(int i) {
  assert(i >= 0 && i < numberOfScripts());
  cleanFreeSpace();
  int beginningOfScriptContent = indexOfScriptContent(i);
  return Script(&m_history[beginningOfScriptContent], strlen(&m_history[beginningOfScriptContent]) + 1);
}

Script ScriptStore::script(const char * name) {
  cleanFreeSpace();
  for (int i=0; i<numberOfScripts(); i++) {
    const char * nameScripti = nameOfScript(i);
    if (strcmp(nameScripti, name) == 0) {
      return script(i);
    }
  }
  return Script();
}

char * ScriptStore::nameOfScript(int i) {
  cleanFreeSpace();
  return &m_history[indexOfScriptName(i)];
}

char * ScriptStore::editableNameOfScript(int i) {
  assert (i >= 0 && i < numberOfScripts());
  cleanAndMoveFreeSpaceAfterScriptName(i);
  return &m_history[indexOfScriptName(i)];
}

int ScriptStore::sizeOfEditableNameOfScript(int i) {
  // Compute the size of the name of the script, including the free space of m_history
  int sizeOfEditableNameScript = 0;
  for (int j=indexOfScriptName(i); j<k_historySize-1; j++) {
    if (m_history[j] == FreeSpaceMarker && m_history[j+1] != FreeSpaceMarker) {
      break;
    }
    sizeOfEditableNameScript++;
  }
  return sizeOfEditableNameScript;
}

int ScriptStore::numberOfScripts() const {
  return m_numberOfScripts;
}

bool ScriptStore::addNewScript() {
  int sizeFreeSpace = sizeOfFreeSpace();
  if (sizeFreeSpace < 4) {
    return false;
  }
  cleanAndMoveFreeSpaceAfterScriptContent(numberOfScripts()-1);
  int beginningNewScript = indexOfFirstFreeSpaceMarker();
  m_history[beginningNewScript] = AutoImportationMarker;
  if (copyName(beginningNewScript + 1)) {
    if (copyEmptyScriptOnFreeSpace()) {
      m_numberOfScripts++;
      m_lastEditedStringPosition = indexOfScriptContent(numberOfScripts()-1);
      return true;
    }
  }
  for (int i = beginningNewScript; i<k_historySize; i++) {
    m_history[i] = FreeSpaceMarker;
  }
  return false;
}

bool ScriptStore::addFactorialScript() {
  int sizeFreeSpace = sizeOfFreeSpace();
  if (sizeFreeSpace < 4) {
    return false;
  }
  cleanAndMoveFreeSpaceAfterScriptContent(numberOfScripts()-1);
  int beginningNewScript = indexOfFirstFreeSpaceMarker();
  m_history[beginningNewScript] = AutoImportationMarker;
  if (copyName(beginningNewScript + 1, "factorial.py")) {
    if (copyFactorialScriptOnFreeSpace()) {
      m_numberOfScripts++;
      m_lastEditedStringPosition = indexOfScriptContent(numberOfScripts()-1);
      return true;
    }
  }
  for (int i = beginningNewScript; i<k_historySize; i++) {
    m_history[i] = FreeSpaceMarker;
  }
  return false;
}

bool ScriptStore::addMandelbrotScript() {
  int sizeFreeSpace = sizeOfFreeSpace();
  if (sizeFreeSpace < 5) {
    return false;
  }
  cleanAndMoveFreeSpaceAfterScriptContent(numberOfScripts()-1);
  int beginningNewScript = indexOfFirstFreeSpaceMarker();
  m_history[beginningNewScript] = AutoImportationMarker;
  if (copyName(beginningNewScript + 1, "mandelbrot.py")) {
    if (copyMandelbrotScriptOnFreeSpace()) {
      m_numberOfScripts++;
      m_lastEditedStringPosition = indexOfScriptContent(numberOfScripts()-1);
      return true;
    }
  }
  for (int i = beginningNewScript; i<k_historySize; i++) {
    m_history[i] = FreeSpaceMarker;
  }
  return false;
}

bool ScriptStore::renameScript(int i, const char * newName) {
  assert (i >= 0 && i < numberOfScripts());
  cleanAndMoveFreeSpaceAfterScriptName(i);
  if (strlen(newName) <= sizeOfEditableNameOfScript(i)) {
    copyName(indexOfScriptName(i), newName);
    return true;
  }
  return false;
}

void ScriptStore::deleteScript(int i) {
  assert (i >= 0 && i < numberOfScripts());
  cleanAndMoveFreeSpaceAfterScriptContent(i);
  int indexOfCharToDelete = indexOfScript(i);
  while (m_history[indexOfCharToDelete] != FreeSpaceMarker && indexOfCharToDelete < k_historySize) {
    m_history[indexOfCharToDelete] = FreeSpaceMarker;
    indexOfCharToDelete++;
  }
  m_numberOfScripts--;
}

void ScriptStore::deleteAll() {
  for (int i = 0; i<k_historySize; i++){
    m_history[i] = FreeSpaceMarker;
  }
  m_numberOfScripts = 0;
}

const char * ScriptStore::contentOfScript(const char * name) {
  int filenameIndex = indexBeginningFilename(name);
  const char * filename = &name[filenameIndex];
  return script(filename).readOnlyContent();
}

bool ScriptStore::copyName(int position, const char * name) {
  if (name) {
    int len = strlen(name);
    if (len > sizeOfFreeSpace() - 1) { // We keep at keast one free char.
      return false;
    }
    memcpy(&m_history[position], name, len+1);
    return true;
  } else {
    int len = strlen(k_defaultScriptName);
    if (len > sizeOfFreeSpace() - 1) { // We keep at keast one free char.
      return false;
    }
    memcpy(&m_history[position], k_defaultScriptName, len+1);
    return true;

  }
}

int ScriptStore::indexOfScript(int i) const {
  assert (i >= 0 && i < numberOfScripts());
  int currentScriptNumber = 0;
  int beginningOfScript = 0;
  while (m_history[beginningOfScript] == FreeSpaceMarker && beginningOfScript < k_historySize) {
    beginningOfScript++;
  }
  if (i == 0) {
    return beginningOfScript;
  }
  bool goingThroughName = true;
  for (int j=beginningOfScript; j<k_historySize; j++) {
    if (m_history[j] == 0) {
      if (goingThroughName) {
        goingThroughName = false;
      } else {
        goingThroughName = true;
        currentScriptNumber++;
        if (currentScriptNumber == i) {
          j++;
          while (m_history[j] == FreeSpaceMarker && j < beginningOfScript) {
            j++;
          }
          return j;
        }
      }
    }
  }
  assert(false);
  return 0;
}

int ScriptStore::indexOfScriptName(int i) const {
  assert (i >= 0 && i < numberOfScripts());
  return indexOfScript(i)+1;
}


int ScriptStore::indexOfScriptContent(int i) const {
  assert (i >= 0 && i < numberOfScripts());
  int indexOfScriptContent = indexOfScriptName(i);
  while (m_history[indexOfScriptContent] != 0 && indexOfScriptContent<k_historySize) {
    indexOfScriptContent++;
  }
  indexOfScriptContent++;
  return indexOfScriptContent;
}

int ScriptStore::lastIndexOfScript(int i) const {
  assert (i >= 0 && i < numberOfScripts());
  int indexOfPrgm = indexOfScriptContent(i);
  int lastIndexOfScript = indexOfPrgm + strlen(&m_history[indexOfPrgm]);
  return lastIndexOfScript;
}

int ScriptStore::indexOfFirstFreeSpaceMarker() const {
  for (int i=0; i<k_historySize; i++) {
    if (m_history[i] == FreeSpaceMarker) {
      return i;
    }
  }
  assert(false);
  return 0;
}

int ScriptStore::sizeOfFreeSpace() const {
  int sizeOfFreeSpace = 0;
  for (int i=0; i<k_historySize; i++) {
    if (m_history[i] == FreeSpaceMarker) {
      sizeOfFreeSpace++;
    } else {
      if (sizeOfFreeSpace > 0) {
        return sizeOfFreeSpace;
      }
    }
  }
  return sizeOfFreeSpace;
}

void ScriptStore::cleanFreeSpace() {
  if (m_history[m_lastEditedStringPosition] == FreeSpaceMarker
      || m_history[m_lastEditedStringPosition] ==  AutoImportationMarker
      || m_history[m_lastEditedStringPosition] == NoAutoImportationMarker)
  {
    return;
  }
  int indexOfCharToChangeIntoFreeSpaceMarker = m_lastEditedStringPosition
    + strlen (&m_history[m_lastEditedStringPosition]) + 1;
  while (m_history[indexOfCharToChangeIntoFreeSpaceMarker] != FreeSpaceMarker
      && indexOfCharToChangeIntoFreeSpaceMarker<k_historySize)
  {
    m_history[indexOfCharToChangeIntoFreeSpaceMarker] = FreeSpaceMarker;
    indexOfCharToChangeIntoFreeSpaceMarker ++;
  }
}

void ScriptStore::moveFreeSpaceAfterScriptContent(int i) {
  assert (i >= 0 && i < numberOfScripts());
  int indexOfFreeSpace = indexOfFirstFreeSpaceMarker();
  int newFreeSpacePosition = lastIndexOfScript(i) + 1;
  if (indexOfFreeSpace < newFreeSpacePosition) {
    newFreeSpacePosition -= sizeOfFreeSpace();
  }
  moveFreeSpaceAtPosition(newFreeSpacePosition);
}

void ScriptStore::moveFreeSpaceAfterScriptName(int i) {
  assert (i >= 0 && i < numberOfScripts());
  int newFreeSpacePosition = indexOfScriptName(i);
  while (m_history[newFreeSpacePosition] != 0 && newFreeSpacePosition < k_historySize) {
    newFreeSpacePosition++;
  }
  newFreeSpacePosition++;
  moveFreeSpaceAtPosition(newFreeSpacePosition);
}

void ScriptStore::moveFreeSpaceAtPosition(int i) {
  assert (i >= 0 && i < k_historySize);
  int indexOfFreeSpace = indexOfFirstFreeSpaceMarker();
  if (indexOfFreeSpace != i){

    // First, move the chars that would be overriden by the free space.
    int freeSpaceSize = sizeOfFreeSpace();
    int len, src, dst;
    // The indexes depend on the relative positions of the free space and the
    // new destination.
    if (indexOfFreeSpace > i) {
      len = indexOfFreeSpace - i;
      src = i;
      dst = i + freeSpaceSize;
    } else {
      src = indexOfFreeSpace + freeSpaceSize;
      len = i + freeSpaceSize - src;
      dst = indexOfFreeSpace;
    }
    memmove(&m_history[dst], &m_history[src], len);

    // Then move the free space.
    for (int j = i ; j<i+freeSpaceSize; j++) {
      m_history[j] = FreeSpaceMarker;
    }
  }

  m_lastEditedStringPosition = i-1;
  while (m_lastEditedStringPosition > 0
      && m_history[m_lastEditedStringPosition-1] != 0
      && m_history[m_lastEditedStringPosition-1] != AutoImportationMarker
      && m_history[m_lastEditedStringPosition-1] != NoAutoImportationMarker)
  {
    m_lastEditedStringPosition--;
  }
}

void ScriptStore::cleanAndMoveFreeSpaceAfterScriptContent(int i) {
  if (i >= 0 && i<numberOfScripts()){
    cleanFreeSpace();
    moveFreeSpaceAfterScriptContent(i);
  }
}
void ScriptStore::cleanAndMoveFreeSpaceAfterScriptName(int i) {
  if (i >= 0 && i<numberOfScripts()){
    cleanFreeSpace();
    moveFreeSpaceAfterScriptName(i);
  }
}

bool ScriptStore::copyMandelbrotScriptOnFreeSpace() {
  const char script[] = R"(# This script draws a Mandelbrot fractal set
# N_iteration: degree of precision
import kandinsky
N_iteration = 10
for x in range(320):
  for y in range(240):
# Compute the mandelbrot sequence for the point c = (c_r, c_i) with start value z = (z_r, z_i)
    z_r = 0
    z_i = 0
# Rescale to fit the drawing screen 320x222
    c_r = 2.7*x/319-2.1
    c_i = -1.87*y/221+0.935
    i = 0
    while (i < N_iteration) and ((z_r * z_r) + (z_i * z_i) < 4):
      i = i + 1
      stock = z_r
      z_r = z_r * z_r - z_i * z_i + c_r
      z_i = 2 * stock * z_i + c_i
# Choose the color of the dot from the Mandelbrot sequence
    rgb = int(255*i/N_iteration)
    col = kandinsky.color(int(rgb),int(rgb*0.75),int(rgb*0.25))
# Draw a pixel colored in 'col' at position (x,y)
    kandinsky.set_pixel(x,y,col))";

  int len = strlen(script);
  if (len + 1 > sizeOfFreeSpace() - 1) { // We keep at keast one free char.
    return false;
  }
  memcpy(&m_history[indexOfFirstFreeSpaceMarker()], script, len+1);
  return true;
}

bool ScriptStore::copyFactorialScriptOnFreeSpace() {
  const char script[] = R"(def factorial(n):
  if n == 0:
    return 1
  else:
    return n * factorial(n-1)
)";

  int len = strlen(script);
  if (len + 1 > sizeOfFreeSpace() - 1) { // We keep at keast one free char.
    return false;
  }
  memcpy(&m_history[indexOfFirstFreeSpaceMarker()], script, len+1);
  return true;
}

bool ScriptStore::copyEmptyScriptOnFreeSpace() {
  const char script[] = "\0";

  int len = 1;
  if (len > sizeOfFreeSpace() - 1) { // We keep at keast one free char.
    return false;
  }
  memcpy(&m_history[indexOfFirstFreeSpaceMarker()], script, len);
  return true;
}

int ScriptStore::indexBeginningFilename(const char * path) {
  int beginningFilename = strlen(path)-1;
  while (beginningFilename > 0 && path[beginningFilename - 1] != '\\') {
    beginningFilename--;
  }
  return beginningFilename;
}

}
