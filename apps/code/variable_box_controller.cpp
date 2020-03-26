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

void VariableBoxController::loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength) {
  //TODO LEA could be great to use strings defined in STATIC const char *const tok_kw[] from python/lexer.c
  //TODO LEA Prune these (check all are usable in our Python, but just comment those which aren't -> there might become usable later)
  //TODO LEA add modules

  // Reset the node counts
  m_currentScriptNodesCount = 0;
  m_builtinNodesCount = 0;
  m_importedNodesCount = 0;

  // Add buitin nodes
  static const struct { const char * name; NodeType type; } builtinNames[] = {
    {qstr_str(MP_QSTR_abs), NodeType::Function},
    {qstr_str(MP_QSTR_all), NodeType::Function},
    {"and", NodeType::Variable},
    {qstr_str(MP_QSTR_any), NodeType::Function},
    {"as", NodeType::Variable},
    //{qstr_str(MP_QSTR_ascii), NodeType::Function},
    {"assert", NodeType::Variable},
    {qstr_str(MP_QSTR_bin), NodeType::Function},
    {qstr_str(MP_QSTR_bool), NodeType::Function},
    {"break", NodeType::Variable},
    //{qstr_str(MP_QSTR_breakpoint), NodeType::Function},
    {qstr_str(MP_QSTR_bytearray), NodeType::Function},
    {qstr_str(MP_QSTR_bytes), NodeType::Function},
    {qstr_str(MP_QSTR_callable), NodeType::Function},
    {qstr_str(MP_QSTR_chr), NodeType::Function},
    {"class", NodeType::Variable},
    {qstr_str(MP_QSTR_classmethod), NodeType::Function},
    //{qstr_str(MP_QSTR_compile), NodeType::Function},
    {qstr_str(MP_QSTR_complex), NodeType::Function},
    {"continue", NodeType::Variable},
    {"def", NodeType::Variable},
    {"__del__", NodeType::Variable},
    //{qstr_str(MP_QSTR_delattr), NodeType::Function},
    {qstr_str(MP_QSTR_dict), NodeType::Function},
    {qstr_str(MP_QSTR_dir), NodeType::Function},
    {qstr_str(MP_QSTR_divmod), NodeType::Function},
    {"elif", NodeType::Variable},
    {"else", NodeType::Variable},
    {qstr_str(MP_QSTR_enumerate), NodeType::Function},
    {qstr_str(MP_QSTR_eval), NodeType::Function},
    {"except", NodeType::Variable},
    {qstr_str(MP_QSTR_exec), NodeType::Function},
    {"False", NodeType::Variable},
    {qstr_str(MP_QSTR_filter), NodeType::Function},
    {"finally", NodeType::Variable},
    {qstr_str(MP_QSTR_float), NodeType::Function},
    {"for", NodeType::Variable},
    {qstr_str(MP_QSTR_format), NodeType::Function},
    {"from", NodeType::Variable},
    {qstr_str(MP_QSTR_frozenset), NodeType::Function},
    {qstr_str(MP_QSTR_getattr), NodeType::Function},
    {"global", NodeType::Variable},
    {qstr_str(MP_QSTR_globals), NodeType::Function},
    {qstr_str(MP_QSTR_hasattr), NodeType::Function},
    {qstr_str(MP_QSTR_hash), NodeType::Function},
    //{qstr_str(MP_QSTR_help), NodeType::Function},
    {qstr_str(MP_QSTR_hex), NodeType::Function},
    {qstr_str(MP_QSTR_id), NodeType::Function},
    {"if", NodeType::Variable},
    {"import", NodeType::Variable},
    {"in", NodeType::Variable},
    {qstr_str(MP_QSTR_input), NodeType::Function},
    {qstr_str(MP_QSTR_int), NodeType::Function},
    {"is", NodeType::Variable},
    {qstr_str(MP_QSTR_isinstance), NodeType::Function},
    {qstr_str(MP_QSTR_issubclass), NodeType::Function},
    {qstr_str(MP_QSTR_iter), NodeType::Function},
    {"lambda", NodeType::Variable},
    {qstr_str(MP_QSTR_len), NodeType::Function},
    {qstr_str(MP_QSTR_list), NodeType::Function},
    {qstr_str(MP_QSTR_locals), NodeType::Function},
    {qstr_str(MP_QSTR_map), NodeType::Function},
    {qstr_str(MP_QSTR_max), NodeType::Function},
    //{qstr_str(MP_QSTR_memoryview), NodeType::Function},
    {qstr_str(MP_QSTR_min), NodeType::Function},
    {qstr_str(MP_QSTR_next), NodeType::Function},
    {"None", NodeType::Variable},
    {"nonlocal", NodeType::Variable},
    {"not", NodeType::Variable},
    {qstr_str(MP_QSTR_object), NodeType::Function},
    {qstr_str(MP_QSTR_oct), NodeType::Function},
    {qstr_str(MP_QSTR_open), NodeType::Function},
    {"or", NodeType::Variable},
    {qstr_str(MP_QSTR_ord), NodeType::Function},
    {"pass", NodeType::Variable},
    {qstr_str(MP_QSTR_pow), NodeType::Function},
    {qstr_str(MP_QSTR_print), NodeType::Function},
    //{qstr_str(MP_QSTR_property), NodeType::Function},
    {"raise", NodeType::Variable},
    {qstr_str(MP_QSTR_range), NodeType::Function},
    {qstr_str(MP_QSTR_repr), NodeType::Function},
    {"return", NodeType::Variable},
    {qstr_str(MP_QSTR_reversed), NodeType::Function},
    {qstr_str(MP_QSTR_round), NodeType::Function},
    {qstr_str(MP_QSTR_set), NodeType::Function},
    {qstr_str(MP_QSTR_setattr), NodeType::Function},
    {qstr_str(MP_QSTR_slice), NodeType::Function},
    {qstr_str(MP_QSTR_sorted), NodeType::Function},
    {qstr_str(MP_QSTR_staticmethod), NodeType::Function},
    {qstr_str(MP_QSTR_str), NodeType::Function},
    {qstr_str(MP_QSTR_sum), NodeType::Function},
    {qstr_str(MP_QSTR_super), NodeType::Function},
    {"True", NodeType::Variable},
    {"try", NodeType::Variable},
    {qstr_str(MP_QSTR_tuple), NodeType::Function},
    {qstr_str(MP_QSTR_type), NodeType::Function},
    //{qstr_str(MP_QSTR_vars), NodeType::Function},
    {"while", NodeType::Variable},
    {"with", NodeType::Variable},
    {"yield", NodeType::Variable},
    {qstr_str(MP_QSTR_zip), NodeType::Function},
    {qstr_str(MP_QSTR___import__), NodeType::Function} //TODO LEA alphabetical order?
  };
  assert(sizeof(builtinNames) / sizeof(builtinNames[0]) == k_totalBuiltinNodesCount);
  for (int i = 0; i < k_totalBuiltinNodesCount; i++) {
    // TODO remove those two constructors
    ScriptNode node = builtinNames[i].type == NodeType::Function ? ScriptNode::FunctionNode( builtinNames[i].name, -1, 0) : ScriptNode::VariableNode(builtinNames[i].name, -1, 0);
    int startsWith = textToAutocomplete == nullptr ? 0 : NodeNameStartsWith(&node, textToAutocomplete, textToAutocompleteLength);
    if (startsWith == 0) {
      m_builtinNodes[m_builtinNodesCount++] = node;
    } else if (startsWith > 0) {
      break;
    }
  }

  // Load the imported variables and functions
  //TODO LEA

#if 1
  if (scriptIndex < 0) {
    //TODO LEA
    return;
  }
  const char * script = m_scriptStore->scriptAtIndex(scriptIndex).scriptContent();

  /* To find variable and funtion names: we lex the script and keep all
   * MP_TOKEN_NAME that are not already in the builtins or imported scripts. */

  m_currentScriptNodesCount = 0;
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {

    // 1) Lex the script
    _mp_lexer_t *lex = mp_lexer_new_from_str_len(0, script, strlen(script), false);
    bool defToken = false;

    // This is a trick to get the token position in the text.
    const char * tokenInText = (const char *)(((_mp_reader_mem_t*)(lex->reader.data))->cur);

    while (lex->tok_kind != MP_TOKEN_END) {
      if (lex->tok_kind == MP_TOKEN_NAME) {
        /* 2) Detect MP_TOKEN_NAME that are not already in the variable box.
         * Keep track of DEF tokens to differentiate between variables and
         * functions. */
        const char * name = lex->vstr.buf;
        int nameLength = lex->vstr.len;
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
          tokenInText -= lex->chr1 == -1 ? (lex->chr2 == -1 ? 0 : 1): 2;
          if (strncmp(tokenInText, name, nameLength) != 0) {
            tokenInText--;
          }
          if (strncmp(tokenInText, name, nameLength) != 0) {
            tokenInText--;
          }
          assert(strncmp(tokenInText, name, nameLength) == 0);
          addNode(defToken ? NodeType::Function : NodeType::Variable, NodeOrigin::CurrentScript, tokenInText, nameLength, scriptIndex);
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
  // TODO LEA Accelerate
  for (int i = 0; i < numberOfRows(); i++) {
    ScriptNode * node = scriptNodeAtIndex(i);
    const char * currentName = node->name();
    int currentNameLength = node->nameLength();
    if ((currentNameLength < 0 || currentNameLength != textLength) && strncmp(text, currentName, textLength) == 0) {
      *textToInsertLength = (currentNameLength < 0 ? strlen(currentName) : currentNameLength) - textLength;
      return currentName + textLength;
    }
  }
  return nullptr;
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
  insertTextInCaller(selectedScriptNode->name(), selectedScriptNode->nameLength());
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

void VariableBoxController::addNode(NodeType type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex) {
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
  nodes[insertionIndex] = type == NodeType::Variable ?
    ScriptNode::VariableNode(name, nameLength, scriptIndex) :
    ScriptNode::FunctionNode(name, nameLength, scriptIndex);
  // Increase the node count
  *currentNodeCount = *currentNodeCount + 1;
}

}
