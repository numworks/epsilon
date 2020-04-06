#include "script_store.h"
#include "string.h"
#include <stddef.h>

extern "C" {
#include "py/lexer.h"
#include "py/nlr.h"
}

namespace Code {

constexpr char ScriptStore::k_scriptExtension[];


bool ScriptStore::ScriptNameIsFree(const char * baseName) {
  return Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, k_scriptExtension).isNull();
}

ScriptStore::ScriptStore()
{
  addScriptFromTemplate(ScriptTemplate::Squares());
  addScriptFromTemplate(ScriptTemplate::Parabola());
  addScriptFromTemplate(ScriptTemplate::Mandelbrot());
  addScriptFromTemplate(ScriptTemplate::Polynomial());
}

void ScriptStore::deleteAllScripts() {
  for (int i = numberOfScripts() - 1; i >= 0; i--) {
    scriptAtIndex(i).destroy();
  }
}

bool ScriptStore::isFull() {
  return Ion::Storage::sharedStorage()->availableSize() < k_fullFreeSpaceSizeLimit;
}

void ScriptStore::scanScriptsForFunctionsAndVariables(void * context, ScanCallback storeFunction, ScanCallback storeVariable) {
  for (int scriptIndex = 0; scriptIndex < numberOfScripts(); scriptIndex++) {
    
    //Don't scan not loaded script
    if (!scriptAtIndex(scriptIndex).importationStatus()){
      continue;
    }

    // Handle lexer or parser errors with nlr.
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
      const char * scriptContent = scriptAtIndex(scriptIndex).scriptContent();
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
  return script.scriptContent();
}

Script::ErrorStatus ScriptStore::addScriptFromTemplate(const ScriptTemplate * scriptTemplate) {
  size_t valueSize = strlen(scriptTemplate->content())+1+1;// scriptcontent size + 1 char for the importation status
  assert(Script::nameCompliant(scriptTemplate->name()));
  Script::ErrorStatus err = Ion::Storage::sharedStorage()->createRecordWithFullName(scriptTemplate->name(), scriptTemplate->value(), valueSize);
  assert(err != Script::ErrorStatus::NonCompliantName);
  return err;
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
