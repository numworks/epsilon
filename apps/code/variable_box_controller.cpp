#include "variable_box_controller.h"
#include "script.h"
#include "app.h"
#include "../shared/toolbox_helpers.h"
#include <apps/i18n.h>
#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/palette.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>

extern "C" {
#include "py/lexer.h"
#include "py/nlr.h"
}

namespace Code {

static inline int minInt(int x, int y) { return x < y ? x : y; }
  // Got these in python/py/src/compile.cpp compiled file
constexpr static uint PN_import_name = 14; // import math // import math as m // import math, cmath // import math as m, cmath as cm
constexpr static uint PN_import_from = 15; // from math import * // from math import sin // from math import sin as stew // from math import sin, cos // from math import sin as stew, cos as cabbage
constexpr static uint PN_import_stmt = 92; // ?
constexpr static uint PN_import_from_2 = 93; // ?
constexpr static uint PN_import_from_2b = 94; // "from .foo import"
constexpr static uint PN_import_from_3 = 95; // ?
constexpr static uint PN_import_as_names_paren = 96; // ?
constexpr static uint PN_import_as_name = 99; //  sin as stew
constexpr static uint PN_import_as_names = 102; // ... import sin as stew, cos as cabbage

//TODO LEA use this
static bool shouldAddObject(const char * name, int maxLength) {
  if (strlen(name)+1 > (size_t)maxLength) {
    return false;
  }
  assert(name != nullptr);
  if (UTF8Helper::CodePointIs(name, '_')) {
    return false;
  }
  return true;
}

VariableBoxController::VariableBoxController(ScriptStore * scriptStore) :
  NestedMenuController(nullptr, I18n::Message::FunctionsAndVariables),
  m_scriptStore(scriptStore),
  m_currentScriptNodesCount(0),
  m_builtinNodesCount(0),
  m_importedNodesCount(0)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setScriptStore(scriptStore);
  }
}

bool VariableBoxController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    return true;
  }
  return NestedMenuController::handleEvent(event);
}

void VariableBoxController::didEnterResponderChain(Responder * previousFirstResponder) {
  /* This Code::VariableBoxController should always be called from an
   * environment where Python has already been inited. This way, we do not
   * deinit Python when leaving the VariableBoxController, so we do not lose the
   * environment that was loaded when entering the VariableBoxController. */
  assert(App::app()->pythonIsInited());
}

int VariableBoxController::numberOfRows() const {
  assert(m_currentScriptNodesCount <= k_maxScriptNodesCount);
  assert(m_importedNodesCount <= k_maxScriptNodesCount);
  return m_currentScriptNodesCount + m_builtinNodesCount + m_importedNodesCount;
}

void VariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index < numberOfRows());
  ScriptNodeCell * myCell = static_cast<ScriptNodeCell *>(cell);
  myCell->setScriptNode(scriptNodeAtIndex(index));
}

/*TODO LEA very dirty
 * This is done to get the lexer position during lexing. As the _mp_reader_mem_t
 * struct is private and declared in python/src/py/reader.c, we copy-paste it
 * here to be able to use it. */
typedef struct _mp_reader_mem_t {
    size_t free_len; // if >0 mem is freed on close by: m_free(beg, free_len)
    const byte *beg;
    const byte *cur;
    const byte *end;
} mp_reader_mem_t;
/* TODO end*/


void VariableBoxController::addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength) {

  uint structKind = (uint) MP_PARSE_NODE_STRUCT_KIND(parseNode);
  bool structKindIsImportWithoutFrom = structKind == PN_import_name;

  if (!structKindIsImportWithoutFrom
      && structKind != PN_import_from
      && structKind != PN_import_as_names
      && structKind != PN_import_as_name)
  {
    return;
  }

  bool loadModuleContent = structKindIsImportWithoutFrom;

  // TODO from ScriptStore
  size_t childNodesCount = MP_PARSE_NODE_STRUCT_NUM_NODES(parseNode);
  for (int i = 0; i < childNodesCount; i++) {
    mp_parse_node_t child = parseNode->nodes[i];
    if (MP_PARSE_NODE_IS_LEAF(child) && MP_PARSE_NODE_LEAF_KIND(child) == MP_PARSE_NODE_ID) {
      uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(child);
      const char * id = qstr_str(arg);
      if ((strncmp(textToAutocomplete, id, textToAutocompleteLength) == 0)
          && (*(id + textToAutocompleteLength) != 0))
      {
        // TODO LEA check if not already present
        addNode(ScriptNode::Type::Variable, NodeOrigin::Importation, id, -1, 1/*TODO LEA*/);
      }
    } else if (MP_PARSE_NODE_IS_STRUCT(child)) {
      addNodesFromImportMaybe((mp_parse_node_struct_t *)child, textToAutocomplete, textToAutocompleteLength);
    } else if (MP_PARSE_NODE_IS_TOKEN(child) && MP_PARSE_NODE_IS_TOKEN_KIND(child, MP_TOKEN_OP_STAR)) {
      /* Parsing something like from math import *
       * -> Load all the module content */
      loadModuleContent = true;
    }
  }

  if (loadModuleContent) {
    assert(childNodesCount > 0);
    mp_parse_node_t moduleName = parseNode->nodes[0];
    assert(MP_PARSE_NODE_IS_LEAF(moduleName) && MP_PARSE_NODE_LEAF_KIND(moduleName) == MP_PARSE_NODE_ID);
    

  }
}

void VariableBoxController::loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength) {
  //TODO LEA could be great to use strings defined in STATIC const char *const tok_kw[] from python/lexer.c
  //TODO LEA Prune these (check all are usable in our Python, but just comment those which aren't -> there might become usable later)

  // Reset the node counts
  m_currentScriptNodesCount = 0;
  m_builtinNodesCount = 0;
  m_importedNodesCount = 0;

  if (textToAutocomplete != nullptr && textToAutocompleteLength < 0) {
    textToAutocompleteLength = strlen(textToAutocomplete);
  }
  m_shortenResultBytesCount = textToAutocomplete == nullptr ? 0 : textToAutocompleteLength;

  // Add buitin nodes
  static const struct { const char * name; ScriptNode::Type type; } builtinNames[] = {
    {qstr_str(MP_QSTR_abs), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_all), ScriptNode::Type::Function},
    {"and", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_any), ScriptNode::Type::Function},
    {"as", ScriptNode::Type::Variable},
    //{qstr_str(MP_QSTR_ascii), ScriptNode::Type::Function},
    {"assert", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_bin), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_bool), ScriptNode::Type::Function},
    {"break", ScriptNode::Type::Variable},
    //{qstr_str(MP_QSTR_breakpoint), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_bytearray), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_bytes), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_callable), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_chr), ScriptNode::Type::Function},
    {"class", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_classmethod), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_cmath), ScriptNode::Type::Variable},
    //{qstr_str(MP_QSTR_compile), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_complex), ScriptNode::Type::Function},
    {"continue", ScriptNode::Type::Variable},
    {"def", ScriptNode::Type::Variable},
    {"__del__", ScriptNode::Type::Variable},
    //{qstr_str(MP_QSTR_delattr), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_dict), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_dir), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_divmod), ScriptNode::Type::Function},
    {"elif", ScriptNode::Type::Variable},
    {"else", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_enumerate), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_eval), ScriptNode::Type::Function},
    {"except", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_exec), ScriptNode::Type::Function},
    {"False", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_filter), ScriptNode::Type::Function},
    {"finally", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_float), ScriptNode::Type::Function},
    {"for", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_format), ScriptNode::Type::Function},
    {"from", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_frozenset), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_getattr), ScriptNode::Type::Function},
    {"global", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_globals), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_hasattr), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_hash), ScriptNode::Type::Function},
    //{qstr_str(MP_QSTR_help), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_hex), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_id), ScriptNode::Type::Function},
    {"if", ScriptNode::Type::Variable},
    {"import", ScriptNode::Type::Variable},
    {"in", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_input), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_int), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_ion), ScriptNode::Type::Variable},
    {"is", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_isinstance), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_issubclass), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_iter), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_kandinsky), ScriptNode::Type::Variable},
    {"lambda", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_len), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_list), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_locals), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_map), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_math), ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_max), ScriptNode::Type::Function},
    //{qstr_str(MP_QSTR_memoryview), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_min), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_next), ScriptNode::Type::Function},
    {"None", ScriptNode::Type::Variable},
    {"nonlocal", ScriptNode::Type::Variable},
    {"not", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_object), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_oct), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_open), ScriptNode::Type::Function},
    {"or", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_ord), ScriptNode::Type::Function},
    {"pass", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_pow), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_print), ScriptNode::Type::Function},
    //{qstr_str(MP_QSTR_property), ScriptNode::Type::Function},
    {"raise", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_random), ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_range), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_repr), ScriptNode::Type::Function},
    {"return", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_reversed), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_round), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_set), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_setattr), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_slice), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_sorted), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_staticmethod), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_str), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_sum), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_super), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_time), ScriptNode::Type::Variable},
    {"True", ScriptNode::Type::Variable},
    {"try", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_tuple), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_turtle), ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_type), ScriptNode::Type::Function},
    //{qstr_str(MP_QSTR_vars), ScriptNode::Type::Function},
    {"while", ScriptNode::Type::Variable},
    {"with", ScriptNode::Type::Variable},
    {"yield", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_zip), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR___import__), ScriptNode::Type::Function} //TODO LEA alphabetical order?
  };
  assert(sizeof(builtinNames) / sizeof(builtinNames[0]) == k_totalBuiltinNodesCount);
  for (int i = 0; i < k_totalBuiltinNodesCount; i++) {
    ScriptNode node = ScriptNode(builtinNames[i].type, builtinNames[i].name, -1, 0);
    int startsWith = textToAutocomplete == nullptr ? 0 : NodeNameStartsWith(&node, textToAutocomplete, textToAutocompleteLength);
    if (startsWith == 0) {
      m_builtinNodes[m_builtinNodesCount++] = node;
    } else if (startsWith > 0) {
      break;
    }
  }

  // Load the imported variables and functions
// TODO TODO TODO LEA

#if 1
  if (scriptIndex < 0) {
    //TODO LEA load imported in console
  } else {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
      const char * parseStart = m_scriptStore->scriptAtIndex(scriptIndex).scriptContent();
      while (*parseStart == '\n' && *parseStart != 0) {
        parseStart++;
      }
      //TODO LEA also look for ";" ? But what if in string?
      const char * parseEnd = UTF8Helper::CodePointSearch(parseStart, '\n');

      while (parseStart != parseEnd) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(0, parseStart, parseEnd - parseStart, 0);
        mp_parse_tree_t parseTree = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
        mp_parse_node_t pn = parseTree.root;

        if (MP_PARSE_NODE_IS_STRUCT(pn)) {
          addNodesFromImportMaybe((mp_parse_node_struct_t *) pn, textToAutocomplete, textToAutocompleteLength);
        }

        mp_parse_tree_clear(&parseTree);

        if (*parseEnd == 0) {
          nlr_pop();
          goto importCurrent;
        }
        parseStart = parseEnd + 1; // Go after the \n
        while (*parseStart == '\n' && *parseStart != 0) {
          parseStart++;
        }
        parseEnd = UTF8Helper::CodePointSearch(parseStart, '\n');
      }
    }
  }
importCurrent:

#endif

#if 1
  if (scriptIndex < 0) {
    //TODO LEA
    return;
  }
  const char * script = m_scriptStore->scriptAtIndex(scriptIndex).scriptContent();

  /* To find variable and funtion names: we lex the script and keep all
   * MP_TOKEN_NAME that complete the text to autocomplete and are not already in
   * the builtins or imported scripts. */

  m_currentScriptNodesCount = 0;
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {

    // 1) Lex the script
    _mp_lexer_t *lex = mp_lexer_new_from_str_len(0, script, strlen(script), false);

    // This is a trick to get the token position in the text.
    const char * tokenInText = (const char *)(((_mp_reader_mem_t*)(lex->reader.data))->cur);
    // Keep track of DEF tokens to differentiate between variables and functions
    bool defToken = false;

    while (lex->tok_kind != MP_TOKEN_END) {
      // Keep only MP_TOKEN_NAME tokens
      if (lex->tok_kind == MP_TOKEN_NAME) {

        const char * name = lex->vstr.buf;
        int nameLength = lex->vstr.len;

        // Check if the token completes the text to autocomplete
        bool canKeepChecking = textToAutocomplete == nullptr;
        if (!canKeepChecking) {
          if (textToAutocompleteLength < nameLength && strncmp(name, textToAutocomplete, textToAutocompleteLength) == 0) {
            canKeepChecking = true;
          }
        }

        if (canKeepChecking) {
          bool alreadyInVarBox = false;
          // TODO LEA speed this up with dichotomia?
          NodeOrigin origins[] = { NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
          for (NodeOrigin origin : origins) {
            const int nodesCount = nodesCountForOrigin(origin);
            ScriptNode * nodes = nodesForOrigin(origin);
            for (int i = 0; i < nodesCount; i++) {
              ScriptNode * matchingNode = nodes + i;
              int comparisonResult = NodeNameCompare(matchingNode, name, nameLength);
              if (comparisonResult == 0) {
                alreadyInVarBox = true;
                break;
              }
              if (comparisonResult > 0) {
                break;
              }
            }
            if (alreadyInVarBox) {
              break;
            }
          }

          if (!alreadyInVarBox) {
            /* This is a trick to get the token position in the text. The -2 was
             * found from stepping in the code and trying. */
            // TODO LEA FIXME
            for (int i = 0; i < 3; i++) {
              if (strncmp(tokenInText, name, nameLength) != 0) {
                tokenInText--;
              } else {
                break;
              }
            }
            assert(strncmp(tokenInText, name, nameLength) == 0);
            addNode(defToken ? ScriptNode::Type::Function : ScriptNode::Type::Variable, NodeOrigin::CurrentScript, tokenInText, nameLength, scriptIndex);
          }
        }
      }

      defToken = lex->tok_kind == MP_TOKEN_KW_DEF;

      /* This is a trick to get the token position in the text. The -1 was found
       *  from stepping in the code and trying. */
      tokenInText = (const char *)(((_mp_reader_mem_t*)(lex->reader.data))->cur);
      if (lex->tok_kind <= MP_TOKEN_ELLIPSIS || lex->tok_kind >= MP_TOKEN_OP_PLUS) {
        tokenInText--;
      }

      mp_lexer_to_next(lex);
    }

    mp_lexer_free(lex);
    nlr_pop();
  }
#else
  m_scriptNodesCount = 1;
  m_scriptStore->scanScriptsForFunctionsAndVariables(
    this,
    [](void * context, const char * functionName, int scriptIndex) {
      if (!shouldAddObject(functionName, k_maxScriptObjectNameSize)) {
        return;
      }
      VariableBoxController * cvc = static_cast<VariableBoxController *>(context);
      cvc->addFunctionAtIndex(functionName, scriptIndex);},
    [](void * context, const char * variableName, int scriptIndex) {
      if (!shouldAddObject(variableName, k_maxScriptObjectNameSize)) {
        return;
      }
      VariableBoxController * cvc = static_cast<VariableBoxController *>(context);
      cvc->addVariableAtIndex(variableName, scriptIndex);});
#endif
}

const char * VariableBoxController::autocompletionForText(int scriptIndex, const char * text, int * textToInsertLength) {
  const char * endOfText = UTF8Helper::EndOfWord(text);
  const int textLength = endOfText - text;
  assert(textLength >= 1);
  loadFunctionsAndVariables(scriptIndex, text, textLength);
  if (numberOfRows() == 0) {
    return nullptr;
  }
  ScriptNode * node = scriptNodeAtIndex(0);
  const char * currentName = node->name();
  int currentNameLength = node->nameLength();
  if (currentNameLength < 0) {
    currentNameLength = strlen(currentName);
  }
  assert(currentNameLength != textLength && strncmp(text, currentName, textLength) == 0);
  *textToInsertLength = currentNameLength - textLength;
  return currentName + textLength;
}

int VariableBoxController::MaxNodesCountForOrigin(NodeOrigin origin) {
  assert(origin == NodeOrigin::CurrentScript || origin == NodeOrigin::Importation);
  return k_maxScriptNodesCount;
}

int VariableBoxController::NodeNameCompare(ScriptNode * node, const char * name, int nameLengthMaybe, bool * strictlyStartsWith) {
  assert(strictlyStartsWith == nullptr || *strictlyStartsWith == false);
  const char * nodeName = node->name();
  const int nodeNameLength = node->nameLength() < 0 ? strlen(nodeName) : node->nameLength();
  const int nameLength = nameLengthMaybe < 0 ? strlen(name) : nameLengthMaybe;
  const int comparisonLength = minInt(nameLength, nodeNameLength);
  int result = strncmp(nodeName, name, comparisonLength);
  if (result != 0) {
    return result;
  }
  if (nodeNameLength == nameLength) {
    return 0;
  }
  bool nodeNameLengthStartsWithName = comparisonLength == nameLength;
  if (strictlyStartsWith != nullptr && nodeNameLengthStartsWithName) {
    *strictlyStartsWith = true;
  }
  return nodeNameLengthStartsWithName ? -1 : 1;
}

int VariableBoxController::NodeNameStartsWith(ScriptNode * node, const char * name, int nameLength) {
  bool strictlyStartsWith = false;
  int result = NodeNameCompare(node, name, nameLength, &strictlyStartsWith);
  if (strictlyStartsWith) {
    return 0;
  }
  return result <= 0 ? -1 : 1;
}

int * VariableBoxController::nodesCountPointerForOrigin(NodeOrigin origin) {
  if (origin == NodeOrigin::CurrentScript) {
    return &m_currentScriptNodesCount;
  }
  assert(origin == NodeOrigin::Importation);
  return &m_importedNodesCount;
}

int VariableBoxController::nodesCountForOrigin(NodeOrigin origin) const {
  if (origin == NodeOrigin::Builtins) {
    return m_builtinNodesCount;
  }
  return *(const_cast<VariableBoxController *>(this)->nodesCountPointerForOrigin(origin));
}

ScriptNode * VariableBoxController::nodesForOrigin(NodeOrigin origin) {
  switch(origin) {
    case NodeOrigin::CurrentScript:
      return m_currentScriptNodes;
    case NodeOrigin::Builtins:
      return m_builtinNodes;
    default:
      assert(origin == NodeOrigin::Importation);
      return m_importedNodes;
  }
}

ScriptNode * VariableBoxController::scriptNodeAtIndex(int index) {
  assert(index >= 0 && index < numberOfRows());
  assert(m_currentScriptNodesCount <= k_maxScriptNodesCount);
  assert(m_builtinNodesCount <= k_totalBuiltinNodesCount);
  assert(m_importedNodesCount <= k_maxScriptNodesCount);
  NodeOrigin origins[] = { NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
  for (NodeOrigin origin : origins) {
    const int nodesCount = nodesCountForOrigin(origin);
    if (index < nodesCount) {
      return nodesForOrigin(origin) + index;
    }
    index -= nodesCount;
  }
  assert(false);
  return nullptr;
}

bool VariableBoxController::selectLeaf(int rowIndex) {
  assert(rowIndex >= 0 && rowIndex < numberOfRows());
  assert(m_currentScriptNodesCount <= k_maxScriptNodesCount);
  assert(m_importedNodesCount <= k_maxScriptNodesCount);
  m_selectableTableView.deselectTable();
  ScriptNode * selectedScriptNode = scriptNodeAtIndex(rowIndex);
  insertTextInCaller(selectedScriptNode->name() + m_shortenResultBytesCount, selectedScriptNode->nameLength() - m_shortenResultBytesCount);
  if (selectedScriptNode->type() == ScriptNode::Type::Function) {
    insertTextInCaller(ScriptNodeCell::k_parenthesesWithEmpty);
  }
  Container::activeApp()->dismissModalViewController();
  return true;
}

void VariableBoxController::insertTextInCaller(const char * text, int textLength) {
  int textLen = textLength < 0 ? strlen(text) : textLength;
  int commandBufferMaxSize = minInt(k_maxScriptObjectNameSize, textLen + 1);
  char commandBuffer[k_maxScriptObjectNameSize];
  Shared::ToolboxHelpers::TextToInsertForCommandText(text, textLen, commandBuffer, commandBufferMaxSize, true);
  sender()->handleEventWithText(commandBuffer);
}

void VariableBoxController::addNode(ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex) {
  assert(origin == NodeOrigin::CurrentScript || origin == NodeOrigin::Importation);
  int * currentNodeCount = nodesCountPointerForOrigin(origin);
  if (*currentNodeCount >= MaxNodesCountForOrigin(origin)) {
    // There is no room to add another node
    return;
  }
  /* We want to insert the node in alphabetical order, so we look for the
   * insertion index. */
  ScriptNode * nodes = origin == NodeOrigin::CurrentScript ? m_currentScriptNodes : m_importedNodes;
  int insertionIndex = 0;
  while (insertionIndex < *currentNodeCount) {
    ScriptNode * node = nodes + insertionIndex;
    int nameComparison = NodeNameCompare(node, name, nameLength);
    assert(nameComparison != 0); // We already checked that the name is not present already
    if (nameComparison > 0) {
      break;
    }
    insertionIndex++;
  }

  // Shift all the following nodes
  for (int i = *currentNodeCount; i >= insertionIndex; i--) {
    nodes[i+1] = nodes[i];
  }
  // Add the node
  nodes[insertionIndex] = ScriptNode(type, name, nameLength, scriptIndex);
  // Increase the node count
  *currentNodeCount = *currentNodeCount + 1;
}

}
