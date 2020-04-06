#include "variable_box_controller.h"
#include "python_toolbox.h"
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
constexpr static uint PN_file_input_2 = 1;
constexpr static uint PN_funcdef = 3;
constexpr static uint PN_expr_stmt = 5;
constexpr static uint PN_import_name = 14; // import math // import math as m // import math, cmath // import math as m, cmath as cm
constexpr static uint PN_import_from = 15; // from math import * // from math import sin // from math import sin as stew // from math import sin, cos // from math import sin as stew, cos as cabbage // from a.b import *
constexpr static uint PN_import_stmt = 92; // ?
constexpr static uint PN_import_from_2 = 93; // ?
constexpr static uint PN_import_from_2b = 94; // "from .foo import"
constexpr static uint PN_import_from_3 = 95; // ?
constexpr static uint PN_import_as_names_paren = 96; // ?
constexpr static uint PN_import_as_name = 99; //  sin as stew
constexpr static uint PN_import_as_names = 102; // ... import sin as stew, cos as cabbage
constexpr static uint PN_dotted_name = 104;

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
  for (int i = 0; i < k_scriptOriginsCount; i++) {
    m_subtitleCells[i].setBackgroundColor(Palette::WallScreen);
    m_subtitleCells[i].setTextColor(Palette::BlueishGrey);
  }
}

bool VariableBoxController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    return true;
  }
  return NestedMenuController::handleEvent(event);
}

void VariableBoxController::didEnterResponderChain(Responder * previousFirstResponder) {
  /* Code::VariableBoxController should always be called from an environment
   * where Python has already been inited. This way, we do not deinit Python
   * when leaving the VariableBoxController, so we do not lose the environment
   * that was loaded when entering the VariableBoxController. */
  assert(App::app()->pythonIsInited());
}

KDCoordinate VariableBoxController::rowHeight(int j) {
  int cellType = typeAndOriginAtLocation(j);
  if (cellType == k_itemCellType) {
    return k_simpleItemRowHeight; // TODO LEA
  }
  assert(cellType == k_subtitleCellType);
  return k_subtitleRowHeight;
}

int VariableBoxController::numberOfRows() const {
  int result = 0;
  NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
  for (NodeOrigin origin : origins) {
    int nodeCount = nodesCountForOrigin(origin);
    if (nodeCount > 0) {
      result += nodeCount + 1; // 1 for the subtitle cell
    }
  }
  return result;
}

HighlightCell * VariableBoxController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_itemCellType) {
    return m_itemCells + index;
  }
  assert(type == k_subtitleCellType);
  return m_subtitleCells + index;
}

int VariableBoxController::reusableCellCount(int type) {
  if (type == k_subtitleCellType) {
    return k_scriptOriginsCount;
  }
  assert(type == k_itemCellType);
  return k_maxNumberOfDisplayedRows;
}

void VariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index < numberOfRows());
  NodeOrigin cellOrigin = NodeOrigin::CurrentScript;
  int cumulatedOriginsCount = 0;
  int cellType = typeAndOriginAtLocation(index, &cellOrigin, &cumulatedOriginsCount);
  if (cellType == k_itemCellType) {
    static_cast<ScriptNodeCell *>(cell)->setScriptNode(scriptNodeAtIndex(index - cumulatedOriginsCount)); // Remove the number of subtitle cells from the index
    return;
  }
  assert(cellType == k_subtitleCellType);
  I18n::Message subtitleMessages[k_scriptOriginsCount] = {
    I18n::Message::CurrentScript,
    I18n::Message::BuiltinFunctionsAndKeyWords,
    I18n::Message::ImportedModulesAndScripts
  };
  static_cast<MessageTableCell *>(cell)->setMessage(subtitleMessages[(int)cellOrigin]);
}

int VariableBoxController::typeAtLocation(int i, int j) {
  assert(i == 0);
  return typeAndOriginAtLocation(j);
}

void VariableBoxController::loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength) {
  // Reset the node counts
  m_currentScriptNodesCount = 0;
  m_builtinNodesCount = 0;
  m_importedNodesCount = 0;

  if (textToAutocomplete != nullptr && textToAutocompleteLength < 0) {
    textToAutocompleteLength = strlen(textToAutocomplete);
  }
  /* If we are autocompleting a text, we want the returned text to not include
   * the beginning that is equal to the text to autocomplete.
   * For instance, if we are displaying the variable box with the text "for" to
   * autocomplete, when the user selects "forward", we want to insert the text
   * "ward" only.
   * While loading the functions and variables, we thus set
   * m_shortenResultCharCount, the number of chars to cut from the text
   * returned. */
  m_shortenResultCharCount = textToAutocomplete == nullptr ? 0 : textToAutocompleteLength;

  // Always load the builtin functions and variables
  loadBuiltinNodes(textToAutocomplete, textToAutocompleteLength);

  if (scriptIndex < 0) {
    //TODO LEA load imported in console
  } else {
    Script script = m_scriptStore->scriptAtIndex(scriptIndex);
    assert(!script.isNull());
    const char * scriptContent = script.scriptContent();
    assert(scriptContent != nullptr);
    loadImportedVariablesInScript(scriptContent, textToAutocomplete, textToAutocompleteLength);
    loadCurrentVariablesInScript(scriptContent, textToAutocomplete, textToAutocompleteLength);
  }
}

const char * VariableBoxController::autocompletionForText(int scriptIndex, const char * textToAutocomplete, int * textToInsertLength) {
  /* The text to autocomplete will most probably not be null-terminated. We thus
   * say that the end of the text to autocomplete is the end if the current word,
   * determined by the next space char or the next null char.*/
  const char * endOfText = UTF8Helper::EndOfWord(textToAutocomplete);
  const int textLength = endOfText - textToAutocomplete;
  assert(textLength >= 1);

  // First load variables and functions that complete the textToAutocomplete
  loadFunctionsAndVariables(scriptIndex, textToAutocomplete, textLength);
  if (numberOfRows() == 0) {
    return nullptr;
  }

  // Return the first node
  ScriptNode * node = scriptNodeAtIndex(0);
  const char * currentName = node->name();
  int currentNameLength = node->nameLength();
  if (currentNameLength < 0) {
    currentNameLength = strlen(currentName);
  }
  // Assert the text we return does indeed autocomplete the text to autocomplete
  assert(currentNameLength != textLength && strncmp(textToAutocomplete, currentName, textLength) == 0);
  // Return the text without the beginning that matches the text to autocomplete
  *textToInsertLength = currentNameLength - textLength;
  return currentName + textLength;
}

// PRIVATE METHODS

int VariableBoxController::NodeNameCompare(ScriptNode * node, const char * name, int nameLength, bool * strictlyStartsWith) {
  // TODO LEA compare until parenthesis
  assert(strictlyStartsWith == nullptr || *strictlyStartsWith == false);
  assert(nameLength > 0);
  const char * nodeName = node->name();
  const int nodeNameLength = node->nameLength() < 0 ? strlen(nodeName) : node->nameLength();
  const int comparisonLength = minInt(nameLength, nodeNameLength);
  int result = strncmp(nodeName, name, comparisonLength);
  if (result != 0) {
    return result;
  }
  if (nodeNameLength == nameLength) {
    return 0;
  }
  bool nodeNameLengthStartsWithName = nodeNameLength > nameLength;
  if (strictlyStartsWith != nullptr && nodeNameLengthStartsWithName) {
    *strictlyStartsWith = true;
  }
  return nodeNameLengthStartsWithName ? 1 : -1;
}

int VariableBoxController::NodeNameStartsWith(ScriptNode * node, const char * name, int nameLength) {
  bool strictlyStartsWith = false;
  int result = NodeNameCompare(node, name, nameLength, &strictlyStartsWith);
  if (strictlyStartsWith) {
    return 0;
  }
  return result <= 0 ? -1 : 1;
}

int VariableBoxController::nodesCountForOrigin(NodeOrigin origin) const {
  if (origin == NodeOrigin::Builtins) {
    return m_builtinNodesCount;
  }
  return *(const_cast<VariableBoxController *>(this)->nodesCountPointerForOrigin(origin));
}

int * VariableBoxController::nodesCountPointerForOrigin(NodeOrigin origin) {
  if (origin == NodeOrigin::CurrentScript) {
    return &m_currentScriptNodesCount;
  }
  assert(origin == NodeOrigin::Importation);
  return &m_importedNodesCount;
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

  NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
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

int VariableBoxController::typeAndOriginAtLocation(int i, NodeOrigin * resultOrigin, int * cumulatedOriginsCount) const {
  int cellIndex = 0;
  int originsCount = 0;
  NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
  for (NodeOrigin origin : origins) {
    int nodeCount = nodesCountForOrigin(origin);
    if (nodeCount > 0) {
      originsCount++;
      int result = -1;
      if (i == cellIndex) {
        result = k_subtitleCellType;
      } else {
        cellIndex += nodeCount + 1; // 1 for the subtitle cell
        if (i < cellIndex) {
          result = k_itemCellType;
        }
      }
      if (result != -1) {
        if (resultOrigin != nullptr) {
          *resultOrigin = origin;
        }
        if (cumulatedOriginsCount != nullptr) {
          *cumulatedOriginsCount = originsCount;
        }
        return result;
      }
    }
  }
  assert(false);
  return k_itemCellType;

}

bool VariableBoxController::selectLeaf(int rowIndex) {
  assert(rowIndex >= 0 && rowIndex < numberOfRows());
  m_selectableTableView.deselectTable();
  ScriptNode * selectedScriptNode = scriptNodeAtIndex(rowIndex);
  insertTextInCaller(selectedScriptNode->name() + m_shortenResultCharCount, selectedScriptNode->nameLength() - m_shortenResultCharCount);
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

void VariableBoxController::loadBuiltinNodes(const char * textToAutocomplete, int textToAutocompleteLength) {
  //TODO LEA could be great to use strings defined in STATIC const char *const tok_kw[] from python/lexer.c
  //TODO LEA Prune these (check all are usable in our Python, but just comment those which aren't -> there might become usable later)
  //TODO LEA add matplotlib.pyplot
  static const struct { const char * name; ScriptNode::Type type; } builtinNames[] = {
    {"False", ScriptNode::Type::Variable},
    {"None", ScriptNode::Type::Variable},
    {"True", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR___import__), ScriptNode::Type::Function},
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
    //{qstr_str(MP_QSTR_bytearray), ScriptNode::Type::Function},
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
    {"del", ScriptNode::Type::Variable},
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
    {qstr_str(MP_QSTR_filter), ScriptNode::Type::Function},
    {"finally", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_float), ScriptNode::Type::Function},
    {"for", ScriptNode::Type::Variable},
    //{qstr_str(MP_QSTR_format), ScriptNode::Type::Function},
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
    //{qstr_str(MP_QSTR_slice), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_sorted), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_staticmethod), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_str), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_sum), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_super), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_time), ScriptNode::Type::Variable},
    {"try", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_tuple), ScriptNode::Type::Function},
    {qstr_str(MP_QSTR_turtle), ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_type), ScriptNode::Type::Function},
    //{qstr_str(MP_QSTR_vars), ScriptNode::Type::Function},
    {"while", ScriptNode::Type::Variable},
    {"with", ScriptNode::Type::Variable},
    {"yield", ScriptNode::Type::Variable},
    {qstr_str(MP_QSTR_zip), ScriptNode::Type::Function}
  };
  assert(sizeof(builtinNames) / sizeof(builtinNames[0]) == k_totalBuiltinNodesCount);
  for (int i = 0; i < k_totalBuiltinNodesCount; i++) {
    ScriptNode node = ScriptNode(builtinNames[i].type, builtinNames[i].name);
    int startsWith = textToAutocomplete == nullptr ? 0 : NodeNameStartsWith(&node, textToAutocomplete, textToAutocompleteLength);
    if (startsWith == 0) {
      m_builtinNodes[m_builtinNodesCount++] = node;
    } else if (startsWith > 0) {
      break;
    }
  }
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

void VariableBoxController::loadImportedVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength) {
  /* Load the imported variables and functions: lex and the parse on a line per
   * line basis untils parsing fails, while detecting import structures. */
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    const char * parseStart = scriptContent;
    // Skip new lines at the beginning of the script
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
        // End of file
        nlr_pop();
        return;
      }

      parseStart = parseEnd;
      // Skip the following \n
      while (*parseStart == '\n' && *parseStart != 0) {
        parseStart++;
      }
      parseEnd = UTF8Helper::CodePointSearch(parseStart, '\n');
    }
    nlr_pop();
  }
}

void VariableBoxController::loadCurrentVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength) {
  /* To find variable and funtion names: we lex the script and keep all
   * MP_TOKEN_NAME that complete the text to autocomplete and are not already in
   * the builtins or imported scripts. */

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {

    // 1) Lex the script
    _mp_lexer_t *lex = mp_lexer_new_from_str_len(0, scriptContent, strlen(scriptContent), false);

    // This is a trick to get the token position in the text.
    const char * tokenInText = (const char *)(((_mp_reader_mem_t*)(lex->reader.data))->cur);
    // Keep track of DEF tokens to differentiate between variables and functions
    bool defToken = false;

    while (lex->tok_kind != MP_TOKEN_END) {
      // Keep only MP_TOKEN_NAME tokens
      if (lex->tok_kind == MP_TOKEN_NAME) {

        const char * name = lex->vstr.buf;
        int nameLength = lex->vstr.len;

        /* If the token autocompletes the text and it is not already in the
         * variable box, add it. */
        if (shouldAddNode(textToAutocomplete, textToAutocompleteLength, name, nameLength)) {
          /* This is a trick to get the token position in the text, as name and
           * nameLength are temporary variables that will be overriden when the
           * lexer continues lexing or is destroyed.
           * The -2 was found from stepping in the code and trying. */
          // TODO LEA FIXME
          for (int i = 0; i < 3; i++) {
            if (strncmp(tokenInText, name, nameLength) != 0) {
              tokenInText--;
            } else {
              break;
            }
          }
          assert(strncmp(tokenInText, name, nameLength) == 0);
          addNode(defToken ? ScriptNode::Type::Function : ScriptNode::Type::Variable, NodeOrigin::CurrentScript, tokenInText, nameLength);
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
}

void VariableBoxController::loadGlobalAndImportedVariablesInScriptAsImported(const char * scriptName, const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {

    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, scriptContent, strlen(scriptContent), false);
    mp_parse_tree_t parseTree = mp_parse(lex, MP_PARSE_FILE_INPUT);
    mp_parse_node_t pn = parseTree.root;

    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
      mp_parse_node_struct_t * pns = (mp_parse_node_struct_t *)pn;
      uint structKind = (uint)MP_PARSE_NODE_STRUCT_KIND(pns);
      if (structKind == PN_funcdef || structKind == PN_expr_stmt) {
        // The script is only a single function or variable definition
        addImportStruct(pns, structKind, scriptName, textToAutocomplete, textToAutocompleteLength);
      } else if (addNodesFromImportMaybe(pns, textToAutocomplete, textToAutocompleteLength)) {
        // The script is is only an import, handled in addNodesFromImportMaybe
      } else if (structKind == PN_file_input_2) {
        /* At this point, if the script node is not of type "file_input_2", it
         * will not have main structures of the wanted type.
         * We look for structures at first level (not inside nested scopes) that
         * are either dunction definitions, variables statements or imports. */
        size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (size_t i = 0; i < n; i++) {
          mp_parse_node_t child = pns->nodes[i];
          if (MP_PARSE_NODE_IS_STRUCT(child)) {
            mp_parse_node_struct_t *child_pns = (mp_parse_node_struct_t*)(child);
            structKind = (uint)MP_PARSE_NODE_STRUCT_KIND(child_pns);
            if (structKind == PN_funcdef || structKind == PN_expr_stmt) {
              addImportStruct(child_pns, structKind, scriptName, textToAutocomplete, textToAutocompleteLength);
            } else {
              addNodesFromImportMaybe(child_pns, textToAutocomplete, textToAutocompleteLength);
            }
          }
        }
      }
    }
    mp_parse_tree_clear(&parseTree);
    nlr_pop();
  }
}

bool VariableBoxController::addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength) {
  // Determine if the node is an import structure
  uint structKind = (uint) MP_PARSE_NODE_STRUCT_KIND(parseNode);
  bool structKindIsImportWithoutFrom = structKind == PN_import_name;
  if (!structKindIsImportWithoutFrom
      && structKind != PN_import_from
      && structKind != PN_import_as_names
      && structKind != PN_import_as_name)
  {
    // This was not an import structure
    return false;
  }

  /* loadAllModuleContent will be True if the struct imports all the content
   * from a script / module (for instance, "import math"), instead of single
   * items (for instance, "from math import sin"). */
  bool loadAllContent = structKindIsImportWithoutFrom;

  size_t childNodesCount = MP_PARSE_NODE_STRUCT_NUM_NODES(parseNode);
  for (int i = 0; i < childNodesCount; i++) {
    mp_parse_node_t child = parseNode->nodes[i];
    if (MP_PARSE_NODE_IS_LEAF(child) && MP_PARSE_NODE_LEAF_KIND(child) == MP_PARSE_NODE_ID) {
      // Parsing something like "import math"
      const char * id = qstr_str(MP_PARSE_NODE_LEAF_ARG(child));
      checkAndAddNode(textToAutocomplete, textToAutocompleteLength, ScriptNode::Type::Variable, NodeOrigin::Importation, id, -1, "math", "desc" /*TODO LEA*/);
    } else if (MP_PARSE_NODE_IS_STRUCT(child)) {
      // Parsing something like "from math import sin"
      addNodesFromImportMaybe((mp_parse_node_struct_t *)child, textToAutocomplete, textToAutocompleteLength);
    } else if (MP_PARSE_NODE_IS_TOKEN(child) && MP_PARSE_NODE_IS_TOKEN_KIND(child, MP_TOKEN_OP_STAR)) {
      /* Parsing something like "from math import *"
       * -> Load all the module content */
      loadAllContent = true;
    }
  }

  // Fetch a script / module content if needed
  if (loadAllContent) {
    assert(childNodesCount > 0);
    mp_parse_node_t importationSource = parseNode->nodes[0];
    const char * importationSourceName = nullptr;
    if (MP_PARSE_NODE_IS_LEAF(importationSource)
        && MP_PARSE_NODE_LEAF_KIND(importationSource) == MP_PARSE_NODE_ID)
    {
      // The importation source is "simple", for instance: from math import *
      importationSourceName = qstr_str(MP_PARSE_NODE_LEAF_ARG(importationSource));
    } else if (MP_PARSE_NODE_IS_STRUCT(importationSource)) {
      mp_parse_node_struct_t * importationSourcePNS = (mp_parse_node_struct_t *)importationSource;
      uint importationSourceStructKind = MP_PARSE_NODE_STRUCT_KIND(importationSourcePNS);
      if (importationSourceStructKind == PN_dotted_name) {
        /* The importation source is "complex", for instance:
         * from matplotlib.pyplot import *
         * FIXME The solution would be to build a single qstr for this name,
         * such as in python/src/compile.c, function do_import_name, from line
         * 1117 (found by searching PN_dotted_name).
         * We might do this later, for now the only dotted name we might want to
         * find is matplolib.pyplot, so we do a very specific search. */
        int numberOfSplitNames = MP_PARSE_NODE_STRUCT_NUM_NODES(importationSourcePNS);
        if (numberOfSplitNames != 2) {
          return true;
        }
        const char * importationSourceSubName = qstr_str(MP_PARSE_NODE_LEAF_ARG(importationSourcePNS->nodes[0]));
        if (strcmp(importationSourceSubName, "matplotlib") != 0) { //TODO LEA once rebased

          return true;
        }
        importationSourceSubName = qstr_str(MP_PARSE_NODE_LEAF_ARG(importationSourcePNS->nodes[1]));
        if (strcmp(importationSourceSubName, "pyplot") != 0) { //TODO LEA once rebased

          return true;
        }
        importationSourceName = "matplotlib.pyplot"; //TODO LEA once rebased
      } else {
        assert(false); //TODO LEA can we indeed assert?
      }
    }
    int numberOfChildren = 0;
    const ToolboxMessageTree * moduleChildren = static_cast<PythonToolbox *>(App::app()->toolboxForInputEventHandler(nullptr))->moduleChildren(importationSourceName, &numberOfChildren);
    if (moduleChildren != nullptr) {
      /* If the importation source is a module, get the nodes from the toolbox
       * We skip the 3 first nodes, which are "import ...", "from ... import *"
       * and "....function". */
      constexpr int numberOfNodesToSkip = 3;
      assert(numberOfChildren > numberOfNodesToSkip);
      for (int i = 3; i < numberOfChildren; i++) {
        const char * name = I18n::translate((moduleChildren + i)->label());
        checkAndAddNode(textToAutocomplete, textToAutocompleteLength, ScriptNode::Type::Variable, NodeOrigin::Importation, name, -1, importationSourceName, I18n::translate((moduleChildren + i)->text()) /*TODO LEA text or label?*/);
      }
    } else {
      // Try fetching the nodes from a script
      Script importedScript = ScriptStore::ScriptBaseNamed(importationSourceName);
      if (!importedScript.isNull()) {
        const char * scriptContent = importedScript.scriptContent();
        assert(scriptContent != nullptr);
        loadGlobalAndImportedVariablesInScriptAsImported(importationSourceName, scriptContent, textToAutocomplete, textToAutocompleteLength);
      }
    }
  }
  return true;
}

const char * structName(mp_parse_node_struct_t * structNode) {
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

void VariableBoxController::addImportStruct(mp_parse_node_struct_t * pns, uint structKind, const char * scriptName, const char * textToAutocomplete, int textToAutocompleteLength) {
  assert(structKind == PN_funcdef || structKind == PN_expr_stmt);
  // Find the id child node, which stores the struct's name
  const char * name = structName(pns);
  if (name == nullptr) {
    return;
  }
  checkAndAddNode(textToAutocomplete, textToAutocompleteLength, structKind == PN_funcdef ? ScriptNode::Type::Function : ScriptNode::Type::Variable, NodeOrigin::Importation, name, -1, scriptName);
}

void VariableBoxController::checkAndAddNode(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type type, NodeOrigin origin, const char * nodeName, int nodeNameLength, const char * nodeSourceName, const char * description) {
  if (nodeNameLength < 0) {
    nodeNameLength = strlen(nodeName);
  }
  if (shouldAddNode(textToAutocomplete, textToAutocompleteLength, nodeName, nodeNameLength)) {
    // Add the node in alphabetical order
    addNode(type, origin, nodeName, nodeNameLength, nodeSourceName, description);
  }
}

bool VariableBoxController::shouldAddNode(const char * textToAutocomplete, int textToAutocompleteLength, const char * nodeName, int nodeNameLength) {
  if (textToAutocomplete != nullptr) {
    /* Check that nodeName autocompletes the text to autocomplete
     *  - The start of nodeName must be equal to the text to autocomplete */
    if (strncmp(textToAutocomplete, nodeName, textToAutocompleteLength) != 0) {
      return false;
    }
    /*  - nodeName should be longer than the text to autocomplete. Beware of the
     *  case where nodeName is textToAutocompleteLength(parameters), which we do
     *  not want to accept. */
    char firstCharAfterAutocompletion = *(nodeName + textToAutocompleteLength);
    if (firstCharAfterAutocompletion == 0 || firstCharAfterAutocompletion == '(') {
      return false;
    }
  }

  // Check that node name is not already present in the variable box.
  if (contains(nodeName, nodeNameLength)) {
    return false;
  }

  return true;
}

bool VariableBoxController::contains(const char * name, int nameLength) {
  assert(nameLength > 0);
  bool alreadyInVarBox = false;
  // TODO LEA speed this up with dichotomia?
  NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
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
  return alreadyInVarBox;
}

void VariableBoxController::addNode(ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, const char * nodeSourceName, const char * description) {
  assert(origin == NodeOrigin::CurrentScript || origin == NodeOrigin::Importation);
  int * currentNodeCount = nodesCountPointerForOrigin(origin);
  if (*currentNodeCount >= MaxNodesCountForOrigin(origin)) {
    // There is no room to add another node
    return;
  }
  /* We want to insert the node in alphabetical order, so we look for the
   * insertion index. */
  ScriptNode * nodes = nodesForOrigin(origin);
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
  for (int i = *currentNodeCount - 1; i >= insertionIndex; i--) {
    nodes[i+1] = nodes[i];
  }
  // Add the node
  nodes[insertionIndex] = ScriptNode(type, name, nameLength, nodeSourceName, description);
  // Increase the node count
  *currentNodeCount = *currentNodeCount + 1;
}

}
