#include "script_store.h"
#include "string.h"
#include <stddef.h>

namespace Code {

constexpr char ScriptStore::k_defaultScriptName[];

ScriptStore::ScriptStore() :
  m_accordion(m_scriptData, k_scriptDataSize)
{
}

const Script ScriptStore::scriptAtIndex(int index, EditableZone zone) {
  assert(index >= 0 && index < numberOfScripts());
  size_t nameBufferSize = 0;
  size_t contentBufferSize = 0;
  int accordionIndex;

  // Move the Free Space at the end of the correct string.
  switch (zone) {
    case EditableZone::None:
      break;
    case EditableZone::Name:
      accordionIndex = accordionIndexOfNameOfScriptAtIndex(index);
      nameBufferSize = m_accordion.sizeOfEditableBufferAtIndex(accordionIndex);
      break;
    case EditableZone::Content:
      accordionIndex = accordionIndexOfContentOfScriptAtIndex(index);
      contentBufferSize = m_accordion.sizeOfEditableBufferAtIndex(accordionIndex);
      break;
  }

  // Compute the positions and lengths of the Script Marker, Name and Content.
  const char * marker = m_accordion.bufferAtIndex(accordionIndexOfMarkersOfScriptAtIndex(index));
  const char * name = m_accordion.bufferAtIndex(accordionIndexOfNameOfScriptAtIndex(index));
  if (nameBufferSize == 0) {
    nameBufferSize = strlen(name);
  }
  const char * content = m_accordion.bufferAtIndex(accordionIndexOfContentOfScriptAtIndex(index));
  if (contentBufferSize == 0) {
    contentBufferSize = strlen(content);
  }
  return Script(marker, name, nameBufferSize, content, contentBufferSize);
}

const Script ScriptStore::scriptNamed(const char * name) {
  for (int i = 0; i < numberOfScripts(); i++) {
    int accordionIndex = accordionIndexOfNameOfScriptAtIndex(i);
    const char * currentScriptName = m_accordion.bufferAtIndex(accordionIndex);
    if (strcmp(currentScriptName, name) == 0) {
      return scriptAtIndex(i);
    }
  }
  return Script();
}

int ScriptStore::numberOfScripts() {
  return (m_accordion.numberOfBuffers())/Script::NumberOfStringsPerScript;
}

bool ScriptStore::addNewScript(DefaultScript defaultScript) {
  const char autoImportationString[2] = {Script::DefaultAutoImportationMarker, 0};
  if (!m_accordion.appendBuffer(autoImportationString)) {
    return false;
  }
  const char * name = nullptr;
  switch (defaultScript) {
    case DefaultScript::Empty:
      name = k_defaultScriptName;
      break;
    case DefaultScript::Mandelbrot:
      name = "mandelbrot.py";
      break;
    case DefaultScript::Factorial:
      name = "factorial.py";
      break;
  }
  if (!m_accordion.appendBuffer(name)) {
    // Delete the Auto Importation Marker
    m_accordion.deleteLastBuffer();
    return false;
  }
  bool didCopy = false;
  switch (defaultScript) {
    case DefaultScript::Empty:
      didCopy = copyEmptyScriptOnFreeSpace();
      break;
    case DefaultScript::Mandelbrot:
      didCopy = copyMandelbrotScriptOnFreeSpace();
      break;
    case DefaultScript::Factorial:
      didCopy = copyFactorialScriptOnFreeSpace();
      break;
  }
  if (didCopy) {
    return true;
  }
  // Delete the Auto Importation Marker and the Name Of the Script
  m_accordion.deleteLastBuffer();
  m_accordion.deleteLastBuffer();
  return false;
}

bool ScriptStore::renameScriptAtIndex(int index, const char * newName) {
  assert (index >= 0 && index < numberOfScripts());
  int accordionIndex = accordionIndexOfNameOfScriptAtIndex(index);
  return m_accordion.replaceBufferAtIndex(accordionIndex, newName);
}

void ScriptStore::switchAutoImportAtIndex(int index) {
  assert(index >= 0 && index < numberOfScripts());
  Script script = scriptAtIndex(index);
  bool autoImportation = script.autoImport();
  int accordionIndex = accordionIndexOfMarkersOfScriptAtIndex(index);
  if (autoImportation) {
    const char autoImportationString[2] = {Script::NoAutoImportationMarker, 0};
    m_accordion.replaceBufferAtIndex(accordionIndex, autoImportationString);
    return;
  }
  const char autoImportationString[2] = {Script::AutoImportationMarker, 0};
  m_accordion.replaceBufferAtIndex(accordionIndex, autoImportationString);
}

void ScriptStore::deleteScriptAtIndex(int index) {
  assert (index >= 0 && index < numberOfScripts());
  int accordionIndex = accordionIndexOfContentOfScriptAtIndex(index);
  // We delete in reverse order because we want the indexes to stay true.
  m_accordion.deleteBufferAtIndex(accordionIndex);
  m_accordion.deleteBufferAtIndex(accordionIndex-1);
  m_accordion.deleteBufferAtIndex(accordionIndex-2);
}

void ScriptStore::deleteAllScripts() {
  m_accordion.deleteAll();
}

const char * ScriptStore::contentOfScript(const char * name) {
  Script script = scriptNamed(name);
  if (script.isNull()) {
    return nullptr;
  }
  return script.content();
}

int ScriptStore::accordionIndexOfMarkersOfScriptAtIndex(int index) const {
  return index * Script::NumberOfStringsPerScript;
}

int ScriptStore::accordionIndexOfNameOfScriptAtIndex(int index) const {
  return index * Script::NumberOfStringsPerScript + 1;
}

int ScriptStore::accordionIndexOfContentOfScriptAtIndex(int index) const {
  return index * Script::NumberOfStringsPerScript + 2;
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

  return m_accordion.appendBuffer(script);
}
bool ScriptStore::copyFactorialScriptOnFreeSpace() {
  const char script[] = R"(def factorial(n):
  if n == 0:
    return 1
  else:
    return n * factorial(n-1))";

  return m_accordion.appendBuffer(script);
}

bool ScriptStore::copyEmptyScriptOnFreeSpace() {
  const char script[] = " ";
  return m_accordion.appendBuffer(script);
}

}
