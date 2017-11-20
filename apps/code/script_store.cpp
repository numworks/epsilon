#include "script_store.h"
#include "string.h"
#include <stddef.h>

namespace Code {

constexpr char ScriptStore::k_scriptExtension[];
constexpr char ScriptStore::k_defaultScriptName[];

ScriptStore::ScriptStore() :
  m_accordion(m_scriptData, k_scriptDataSize)
{
  addScriptFromTemplate(ScriptTemplate::Factorial());
  addScriptFromTemplate(ScriptTemplate::Fibonacci());
  addScriptFromTemplate(ScriptTemplate::Mandelbrot());
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

bool ScriptStore::addNewScript() {
  return addScriptFromTemplate(ScriptTemplate::Empty());
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

bool ScriptStore::isFull() {
  return (m_accordion.freeSpaceSize() < k_fullFreeSpaceSizeLimit);
}

const char * ScriptStore::contentOfScript(const char * name) {
  Script script = scriptNamed(name);
  if (script.isNull()) {
    return nullptr;
  }
  return script.content();
}

bool ScriptStore::addScriptFromTemplate(const ScriptTemplate * scriptTemplate) {
  const char autoImportationString[2] = {Script::DefaultAutoImportationMarker, 0};
  if (!m_accordion.appendBuffer(autoImportationString)) {
    return false;
  }

  if (!m_accordion.appendBuffer(scriptTemplate->name())) {
    // Delete the Auto Importation Marker
    m_accordion.deleteLastBuffer();
    return false;
  }

  if (copyStaticScriptOnFreeSpace(scriptTemplate)) {
    return true;
  }
  // Delete the Auto Importation Marker and the Name Of the Script
  m_accordion.deleteLastBuffer();
  m_accordion.deleteLastBuffer();
  return false;
}

bool ScriptStore::copyStaticScriptOnFreeSpace(const ScriptTemplate * scriptTemplate) {
  return m_accordion.appendBuffer(scriptTemplate->content());
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

}
