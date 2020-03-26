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

VariableBoxController::VariableBoxController(ScriptStore * scriptStore) :
  NestedMenuController(nullptr, I18n::Message::FunctionsAndVariables),
  m_scriptStore(scriptStore),
  m_currentScriptNodesCount(0),
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

int VariableBoxController::numberOfRows() const {
  assert(m_currentScriptNodesCount <= k_maxScriptNodesCount);
  assert(m_importedNodesCount <= k_maxScriptNodesCount);
  return m_currentScriptNodesCount + k_builtinNodesCount + m_importedNodesCount;
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

// TODO LEA could be great to use strings defined in STATIC const char *const tok_kw[]
// from python/lexer.c
void VariableBoxController::loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete) {
  //TODO LEA Prune these + add modules
  //TODO LEA Load according to textToAutocomplete
  int index = 0;
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_abs), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_all), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_and), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_any), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_as), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_ascii), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_assert), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bin), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bool), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_break), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_breakpoint), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bytearray), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bytes), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_callable), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_chr), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_class), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_classmethod), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_compile), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_complex), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_continue), -1, 0);
  m_builtinNodes[index++] = ScriptNode::VariableNode("def", -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR___del__), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_delattr), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_dict), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_dir), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_divmod), -1, 0);
  m_builtinNodes[index++] = ScriptNode::VariableNode("elif", -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_else), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_enumerate), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_eval), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_except), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_exec), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_False), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_filter), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_finally), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_float), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_for), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_format), -1, 0);
  m_builtinNodes[index++] = ScriptNode::VariableNode("from", -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_frozenset), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_getattr), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_global), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_globals), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_hasattr), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_hash), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_help), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_hex), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_id), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_if), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_import), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_in), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_input), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_int), -1, 0);
 // m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_is), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_isinstance), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_issubclass), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_iter), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_lambda), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_len), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_list), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_locals), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_map), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_max), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_memoryview), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_min), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_next), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_None), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_nonlocal), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_not), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_object), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_oct), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_open), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_or), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_ord), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_pass), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_pow), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_print), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_property), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_raise), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_range), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_repr), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_return), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_reversed), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_round), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_set), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_setattr), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_slice), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_sorted), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_staticmethod), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_str), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_sum), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_super), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_True), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_try), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_tuple), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_type), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_vars), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_while), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_with), -1, 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_yield), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_zip), -1, 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR___import__), -1, 0); //TODO LEA alphabetical order?
  assert(index == k_builtinNodesCount);


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

#if 1
  m_currentScriptNodesCount = 0;
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {

    // 1) Lex the script
    _mp_lexer_t *lex = mp_lexer_new_from_str_len(0, script, strlen(script), false);
    bool defToken = false;

    // This is a trick to get the token position in the text.
    const char * tokenInText = (const char *)(((_mp_reader_mem_t*)(lex->reader.data))->cur);

    while (lex->tok_kind != MP_TOKEN_END) {
      /* 2) Detect MP_TOKEN_NAME that are not already in the variable box. Keep
       * track of DEF  tokens to differentiate between variables and functions. */
      if (lex->tok_kind == MP_TOKEN_NAME) {
        const char * name = lex->vstr.buf;
        int nameLength = lex->vstr.len;
        // Check if this token is already in the var box
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
          tokenInText -= 2;
          addNode(defToken ? NodeType::Function : NodeType::Variable, NodeOrigin::CurrentScript, tokenInText, nameLength, scriptIndex);
        }
      }
      defToken = lex->tok_kind == MP_TOKEN_KW_DEF;

      /* This is a trick to get the token position in the text. The -1 and -2
       * were found from stepping in the code and trying. */
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
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    // 1) Lex the script
    _mp_lexer_t *lex = mp_lexer_new_from_str_len(0, script0, strlen(script0), false);
    while (lex->tok_kind != MP_TOKEN_END) {
      // 2) Detect patterns on a line per line basis
      while (lex->tok_kind == MP_TOKEN_INDENT || lex->tok_kind == MP_TOKEN_DEDENT) {
        mp_lexer_to_next(lex);
      }
      if (lex->tok_kind == MP_TOKEN_NAME) {
        // Look for variable definition "name ="
        const char * name = lex->vstr.buf;
        size_t len = lex->vstr.len;
        mp_lexer_to_next(lex);
        if (lex->tok_kind == MP_TOKEN_DEL_EQUAL) {
          addVariableAtIndex(name, len, 0);
        }
      } else if (lex->tok_kind != MP_TOKEN_KW_DEF) {
        // Look for function definition "def name1(name2, name3 = ...):"
        mp_lexer_to_next(lex);
        if (lex->tok_kind == MP_TOKEN_NAME) {
          const char * name = lex->vstr.buf;
          size_t len = lex->vstr.len;
          addFunctionAtIndex(name, len, 0);
          mp_lexer_to_next(lex);
          if (lex->tok_kind == MP_TOKEN_DEL_PAREN_OPEN) {

          }
    MP_TOKEN_DEL_PAREN_CLOSE,
        }


      }

      // Forward until the end of the line
      while (lex->tok_kind != MP_TOKEN_END
          && lex->tok_kind != MP_TOKEN_DEL_SEMICOLON
          && lex->tok_kind != MP_TOKEN_NEWLINE)
      {
        mp_lexer_to_next(lex);
      }
      if (lex->tok_kind != MP_TOKEN_END) {
        mp_lexer_to_next(lex);
      }
    }
    mp_lexer_free(lex);
    nlr_pop();
  }
#endif
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
  // TODO LEA Accelerate
  loadFunctionsAndVariables(scriptIndex, text);
  const char * endOfText = UTF8Helper::EndOfWord(text);
  const int textLength = endOfText - text;
  assert(textLength >= 1);
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

HighlightCell * VariableBoxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
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

int VariableBoxController::MaxNodesCountForOrigin(NodeOrigin origin) {
  assert(origin == NodeOrigin::CurrentScript || origin == NodeOrigin::Importation);
  return k_maxScriptNodesCount;
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
    return k_builtinNodesCount;
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

int VariableBoxController::NodeNameCompare(ScriptNode * node, const char * name, int nameLength) {
  const char * nodeName = node->name();
  const int nodeNameLength = node->nameLength();
  const int comparisonLength = minInt(nameLength, nodeNameLength);
  int result = strncmp(nodeName, name, comparisonLength);
  if (result != 0) {
    return result;
  }
  if (nodeNameLength == nameLength) {
    return 0;
  }
  return comparisonLength == nodeNameLength ? -1 : 1;
}

}
