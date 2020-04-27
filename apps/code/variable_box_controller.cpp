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
  AlternateEmptyNestedMenuController(I18n::Message::FunctionsAndVariables),
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
  displayEmptyController();
}

KDCoordinate VariableBoxController::rowHeight(int j) {
  NodeOrigin cellOrigin = NodeOrigin::CurrentScript;
  int cumulatedOriginsCount = 0;
  int cellType = typeAndOriginAtLocation(j, &cellOrigin, &cumulatedOriginsCount);
  if (cellType == k_itemCellType) {
    //TODO LEA if cellOrigin == Imported?
    if (scriptNodeAtIndex(j - cumulatedOriginsCount)->description() != nullptr) {
      return ScriptNodeCell::k_complexItemHeight;
    }
    return ScriptNodeCell::k_simpleItemHeight;
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
    I18n::Message::ScriptInProgress,
    I18n::Message::BuiltinsAndKeywords,
    I18n::Message::ImportedModulesAndScripts
  };
  static_cast<MessageTableCell *>(cell)->setMessage(subtitleMessages[(int)cellOrigin]);
}

void VariableBoxController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  const int currentSelectedRow = selectedRow();
  if (currentSelectedRow >= 0 && typeAtLocation(0, currentSelectedRow) == k_subtitleCellType) {
    if (currentSelectedRow == 0) {
      t->selectCellAtLocation(0, 1);
    } else {
      t->selectCellAtLocation(0, selectedRow() + (previousSelectedCellY < currentSelectedRow ? 1 : -1));
    }
  }
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

const char * VariableBoxController::autocompletionForText(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength, int * textToInsertLength, bool * addParentheses) {
  assert(textToAutocompleteLength >= 1);
  assert(addParentheses != nullptr);

  // First load variables and functions that complete the textToAutocomplete
  loadFunctionsAndVariables(scriptIndex, textToAutocomplete, textToAutocompleteLength);
  if (numberOfRows() == 0) {
    return nullptr;
  }

  return autocompletionAlternativeAtIndex(textToAutocompleteLength, textToInsertLength, addParentheses, 0);
}

const char * VariableBoxController::autocompletionAlternativeAtIndex(int textToAutocompleteLength, int * textToInsertLength, bool * addParentheses, int index, int * indexToUpdate) {
  if (numberOfRows() == 0) {
    return nullptr;
  }

  int nodesCount = 0;  // We cannot use numberOfRows as it contains the banners
  NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
  for (NodeOrigin origin : origins) {
    nodesCount += nodesCountForOrigin(origin);
  }
  if (index < 0) {
    assert(index == -1);
    index = nodesCount - 1;
  } else if (index >= nodesCount) {
    assert(index == nodesCount);
    index = 0;
  }

  if (indexToUpdate != nullptr) {
    *indexToUpdate = index;
  }

  ScriptNode * node = scriptNodeAtIndex(index);
  const char * currentName = node->name();
  int currentNameLength = node->nameLength();
  if (currentNameLength < 0) {
    currentNameLength = strlen(currentName);
  }
  *addParentheses = node->type() == ScriptNode::Type::WithParentheses;
  // Return the text without the beginning that matches the text to autocomplete
  *textToInsertLength = currentNameLength - textToAutocompleteLength;
  return currentName + textToAutocompleteLength;
}

void VariableBoxController::empty() {
  m_builtinNodesCount = 0;
  m_currentScriptNodesCount = 0;
  m_importedNodesCount = 0;
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

  int cumulatedOriginsCount = 0;
  int cellType = typeAndOriginAtLocation(rowIndex, nullptr, &cumulatedOriginsCount);
  assert(cellType == k_itemCellType);
  (void)cellType; // Silence warnings
  ScriptNode * selectedScriptNode = scriptNodeAtIndex(rowIndex - cumulatedOriginsCount); // Remove the number of subtitle cells from the index

  /* We need to check now if we need to add parentheses: insertTextInCaller
   * calls handleEventWithText, which will reload the autocompletion for the
   * added text, which will probably chande the script nodes and
   * selectedScriptNode will become invalid. */
  const bool shouldAddParentheses = selectedScriptNode->type() == ScriptNode::Type::WithParentheses;
  insertTextInCaller(selectedScriptNode->name() + m_shortenResultCharCount, selectedScriptNode->nameLength() - m_shortenResultCharCount);
  // WARNING: selectedScriptNode is now invalid

  if (shouldAddParentheses) {
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
  static const struct { const char * name; ScriptNode::Type type; } builtinNames[] = {
    {"False", ScriptNode::Type::WithoutParentheses},
    {"None", ScriptNode::Type::WithoutParentheses},
    {"True", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_abs), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_all), ScriptNode::Type::WithParentheses},
    {"and", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_any), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_append), ScriptNode::Type::WithParentheses},
    {"as", ScriptNode::Type::WithoutParentheses},
    //{qstr_str(MP_QSTR_ascii), ScriptNode::Type::WithParentheses},
    {"assert", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_bin), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_bool), ScriptNode::Type::WithParentheses},
    {"break", ScriptNode::Type::WithoutParentheses},
    //{qstr_str(MP_QSTR_breakpoint), ScriptNode::Type::WithParentheses},
    //{qstr_str(MP_QSTR_bytearray), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_bytes), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_callable), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_chr), ScriptNode::Type::WithParentheses},
    {"class", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_classmethod), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_clear), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_cmath), ScriptNode::Type::WithoutParentheses},
    //{qstr_str(MP_QSTR_compile), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_complex), ScriptNode::Type::WithParentheses},
    {"continue", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_count), ScriptNode::Type::WithParentheses},
    {"def", ScriptNode::Type::WithoutParentheses},
    {"del", ScriptNode::Type::WithoutParentheses},
    //{qstr_str(MP_QSTR_delattr), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_dict), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_dir), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_divmod), ScriptNode::Type::WithParentheses},
    {"elif", ScriptNode::Type::WithoutParentheses},
    {"else", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_enumerate), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_eval), ScriptNode::Type::WithParentheses},
    {"except", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_exec), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_filter), ScriptNode::Type::WithParentheses},
    {"finally", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_float), ScriptNode::Type::WithParentheses},
    {"for", ScriptNode::Type::WithoutParentheses},
    //{qstr_str(MP_QSTR_format), ScriptNode::Type::WithParentheses},
    {"from", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_frozenset), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_getattr), ScriptNode::Type::WithParentheses},
    {"global", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_globals), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_hasattr), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_hash), ScriptNode::Type::WithParentheses},
    //{qstr_str(MP_QSTR_help), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_hex), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_id), ScriptNode::Type::WithParentheses},
    {"if", ScriptNode::Type::WithoutParentheses},
    {"import", ScriptNode::Type::WithoutParentheses},
    {"in", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_index), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_input), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_insert), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_int), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_ion), ScriptNode::Type::WithoutParentheses},
    {"is", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_isinstance), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_issubclass), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_iter), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_kandinsky), ScriptNode::Type::WithoutParentheses},
    {"lambda", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_len), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_list), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_locals), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_map), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_math), ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_matplotlib_dot_pyplot), ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_max), ScriptNode::Type::WithParentheses},
    //{qstr_str(MP_QSTR_memoryview), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_min), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_next), ScriptNode::Type::WithParentheses},
    {"nonlocal", ScriptNode::Type::WithoutParentheses},
    {"not", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_object), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_oct), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_open), ScriptNode::Type::WithParentheses},
    {"or", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_ord), ScriptNode::Type::WithParentheses},
    {"pass", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_pow), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_pop), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_print), ScriptNode::Type::WithParentheses},
    //{qstr_str(MP_QSTR_property), ScriptNode::Type::WithParentheses},
    {"raise", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_random), ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_range), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_remove), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_repr), ScriptNode::Type::WithParentheses},
    {"return", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_reverse), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_reversed), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_round), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_set), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_setattr), ScriptNode::Type::WithParentheses},
    //{qstr_str(MP_QSTR_slice), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_sort), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_sorted), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_staticmethod), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_str), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_sum), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_super), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_time), ScriptNode::Type::WithoutParentheses},
    {"try", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_tuple), ScriptNode::Type::WithParentheses},
    {qstr_str(MP_QSTR_turtle), ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_type), ScriptNode::Type::WithParentheses},
    //{qstr_str(MP_QSTR_vars), ScriptNode::Type::WithParentheses},
    {"while", ScriptNode::Type::WithoutParentheses},
    {"with", ScriptNode::Type::WithoutParentheses},
    {"yield", ScriptNode::Type::WithoutParentheses},
    {qstr_str(MP_QSTR_zip), ScriptNode::Type::WithParentheses}
  };
  assert(sizeof(builtinNames) / sizeof(builtinNames[0]) == k_totalBuiltinNodesCount);
  /* We can leverage on the fact that buitin nodes are stored in alphabetical
   * order, so we do not use shouldAddNode. */
  for (int i = 0; i < k_totalBuiltinNodesCount; i++) {
    ScriptNode node = ScriptNode(builtinNames[i].type, builtinNames[i].name);

    int startsWith = 0;
    if (textToAutocomplete != nullptr) {
      bool strictlyStartsWith = false;
      startsWith = NodeNameCompare(&node, textToAutocomplete, textToAutocompleteLength, &strictlyStartsWith);
      if (startsWith == 0) { // The node name and name are equal
        startsWith = builtinNames[i].type == ScriptNode::Type::WithParentheses ? 0 : -1; // We accept the node only if it has parentheses
      } else if (strictlyStartsWith) {
        startsWith = 0;
      } else if (startsWith > 0) {
        return;
      }
    }
    if (startsWith == 0) {
      m_builtinNodes[m_builtinNodesCount++] = node;
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
        ScriptNode::Type nodeType = defToken ? ScriptNode::Type::WithParentheses : ScriptNode::Type::WithoutParentheses;
        if (shouldAddNode(textToAutocomplete, textToAutocompleteLength, name, nameLength, nodeType)) {
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
          addNode(nodeType, NodeOrigin::CurrentScript, tokenInText, nameLength);
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
  for (size_t i = 0; i < childNodesCount; i++) {
    mp_parse_node_t child = parseNode->nodes[i];
    if (MP_PARSE_NODE_IS_LEAF(child) && MP_PARSE_NODE_LEAF_KIND(child) == MP_PARSE_NODE_ID) {
      // Parsing something like "import math"
      const char * id = qstr_str(MP_PARSE_NODE_LEAF_ARG(child));
      checkAndAddNode(textToAutocomplete, textToAutocompleteLength, ScriptNode::Type::WithoutParentheses, NodeOrigin::Importation, id, -1, "math", "desc" /*TODO LEA*/);
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
      for (int i = numberOfNodesToSkip; i < numberOfChildren; i++) {
        const char * name = I18n::translate((moduleChildren + i)->label());
        checkAndAddNode(textToAutocomplete, textToAutocompleteLength, ScriptNode::Type::WithoutParentheses, NodeOrigin::Importation, name, -1, importationSourceName, I18n::translate((moduleChildren + i)->text()) /*TODO LEA text or label?*/);
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
  checkAndAddNode(textToAutocomplete, textToAutocompleteLength, structKind == PN_funcdef ? ScriptNode::Type::WithParentheses : ScriptNode::Type::WithoutParentheses, NodeOrigin::Importation, name, -1, scriptName);
}

void VariableBoxController::checkAndAddNode(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type type, NodeOrigin origin, const char * nodeName, int nodeNameLength, const char * nodeSourceName, const char * description) {
  if (nodeNameLength < 0) {
    nodeNameLength = strlen(nodeName);
  }
  if (shouldAddNode(textToAutocomplete, textToAutocompleteLength, nodeName, nodeNameLength, type)) {
    // Add the node in alphabetical order
    addNode(type, origin, nodeName, nodeNameLength, nodeSourceName, description);
  }
}

bool VariableBoxController::shouldAddNode(const char * textToAutocomplete, int textToAutocompleteLength, const char * nodeName, int nodeNameLength, ScriptNode::Type type) {
  assert(nodeNameLength > 0);
  if (textToAutocomplete != nullptr) {
    /* Check that nodeName autocompletes the text to autocomplete
     *  - The start of nodeName must be equal to the text to autocomplete */
    if (strncmp(textToAutocomplete, nodeName, textToAutocompleteLength) != 0) {
      return false;
    }
    /*  - nodeName should be longer than the text to autocomplete. Beware of the
     *  case where nodeName is textToAutocompleteLength(), where we want to add
     *  the node to autocomplete with parentheses. */
    if (nodeNameLength == textToAutocompleteLength && type != ScriptNode::Type::WithParentheses) {
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
