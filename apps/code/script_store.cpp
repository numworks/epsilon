#include "script_store.h"
#include "string.h"
#include <stddef.h>

extern "C" {
#include "py/lexer.h"
#include "py/nlr.h"
}

namespace Code {

constexpr char ScriptStore::k_scriptExtension[];
constexpr char ScriptStore::k_defaultScriptName[];

ScriptStore::ScriptStore() :
  m_accordion(m_scriptData, k_scriptDataSize)
{
  addScriptFromTemplate(ScriptTemplate::Factorial());
  addScriptFromTemplate(ScriptTemplate::Mandelbrot());
  addScriptFromTemplate(ScriptTemplate::Polynomial());
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
  return (numberOfScripts() >= k_maxNumberOfScripts || m_accordion.freeSpaceSize() < k_fullFreeSpaceSizeLimit);
}

void ScriptStore::scanScriptsForFunctionsAndVariables(void * context, ScanCallback storeFunction, ScanCallback storeVariable) {
  for (int scriptIndex = 0; scriptIndex < numberOfScripts(); scriptIndex++) {
    // Handle lexer or parser errors with nlr.
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
      const char * scriptContent = scriptAtIndex(scriptIndex).content();
      if (scriptContent == nullptr) {
        continue;
      }
      mp_lexer_t *lex = mp_lexer_new_from_str_len(0, scriptContent, strlen(scriptContent), false);
      mp_parse_tree_t parseTree = mp_parse(lex, MP_PARSE_FILE_INPUT);
      mp_parse_node_t pn = parseTree.root;

      if (!MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_tree_clear(&parseTree);
        nlr_pop();
        continue;
      }

      mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;

      // The script is only a single function definition.
      if (((uint)(MP_PARSE_NODE_STRUCT_KIND(pns))) == k_functionDefinitionParseNodeStructKind) {
        const char * id = structID(pns);
        if (id == nullptr) {
          continue;
        }
        storeFunction(context, id, scriptIndex);
        mp_parse_tree_clear(&parseTree);
        nlr_pop();
        continue;
      }

      // The script is only a single global variable definition.
      if (((uint)(MP_PARSE_NODE_STRUCT_KIND(pns))) == k_expressionStatementParseNodeStructKind) {
        const char * id = structID(pns);
        if (id == nullptr) {
          continue;
        }
        storeVariable(context, id, scriptIndex);
        mp_parse_tree_clear(&parseTree);
        nlr_pop();
        continue;
      }

      if (((uint)(MP_PARSE_NODE_STRUCT_KIND(pns))) != k_fileInput2ParseNodeStructKind) {
        // The script node is not of type "file_input_2", thus it will not have main
        // structures of the wanted type.
        mp_parse_tree_clear(&parseTree);
        nlr_pop();
        continue;
      }

      // Count the number of structs in child nodes.

      size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
      for (size_t i = 0; i < n; i++) {
        mp_parse_node_t child = pns->nodes[i];
        if (MP_PARSE_NODE_IS_STRUCT(child)) {
          mp_parse_node_struct_t *child_pns = (mp_parse_node_struct_t*)(child);
          if (((uint)(MP_PARSE_NODE_STRUCT_KIND(child_pns))) == k_functionDefinitionParseNodeStructKind) {
            const char * id = structID(child_pns);
            if (id == nullptr) {
              continue;
            }
            storeFunction(context, id, scriptIndex);
          } else if (((uint)(MP_PARSE_NODE_STRUCT_KIND(child_pns))) == k_expressionStatementParseNodeStructKind) {
            const char * id = structID(child_pns);
            if (id == nullptr) {
              continue;
            }
            storeVariable(context, id, scriptIndex);
          }
        }
      }

      mp_parse_tree_clear(&parseTree);
      nlr_pop();
    }
  }
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

const char * ScriptStore::structID(mp_parse_node_struct_t *structNode) {
  // Find the id child node, which stores the struct's name
  size_t childNodesCount = MP_PARSE_NODE_STRUCT_NUM_NODES(structNode);
  if (childNodesCount < 1) {
    return nullptr;
  }
  mp_parse_node_t child = structNode->nodes[0];
  if (MP_PARSE_NODE_IS_LEAF(child)
      && MP_PARSE_NODE_LEAF_KIND(child) == MP_PARSE_NODE_ID)
  {
    uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(child);
    return qstr_str(arg);
  }
  return nullptr;
}

}
