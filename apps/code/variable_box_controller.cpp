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
#include <algorithm>

extern "C" {
#include "py/lexer.h"
#include "py/nlr.h"
#include "py/objmodule.h"
}

namespace Code {

// Got these in python/py/src/compile.cpp compiled file
constexpr static uint PN_file_input_2 = 1;
constexpr static uint PN_funcdef = 3;
constexpr static uint PN_expr_stmt = 5;
constexpr static uint PN_import_name = 14; // import math // import math as m // import math, cmath // import math as m, cmath as cm
constexpr static uint PN_import_from = 15; // from math import * // from math import sin // from math import sin as stew // from math import sin, cos // from math import sin as stew, cos as cabbage // from a.b import *
constexpr static uint PN_import_as_name = 99; // sin as stew
constexpr static uint PN_import_as_names = 102; // ... import sin as stew, cos as cabbage
constexpr static uint PN_dotted_name = 104;
/* These are not used for now but might be relevant at some point?
constexpr static uint PN_import_stmt = 92;
constexpr static uint PN_import_from_2 = 93;
constexpr static uint PN_import_from_2b = 94; // "from .foo import"
constexpr static uint PN_import_from_3 = 95;
constexpr static uint PN_import_as_names_paren = 96;
*/

VariableBoxController::VariableBoxController(ScriptStore * scriptStore) :
  AlternateEmptyNestedMenuController(I18n::Message::FunctionsAndVariables),
  m_scriptStore(scriptStore),
  m_currentScriptNodesCount(0),
  m_builtinNodesCount(0),
  m_importedNodesCount(0)
{
  for (int i = 0; i < k_scriptOriginsCount; i++) {
    m_subtitleCells[i].setBackgroundColor(Palette::WallScreen);
    m_subtitleCells[i].setTextColor(Palette::SecondaryText);
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
  displayEmptyControllerIfNeeded();
}

KDCoordinate VariableBoxController::rowHeight(int j) {
  NodeOrigin cellOrigin = NodeOrigin::CurrentScript;
  int cumulatedOriginsCount = 0;
  int cellType = typeAndOriginAtLocation(j, &cellOrigin, &cumulatedOriginsCount);
  if (cellType == k_itemCellType) {
    if (scriptNodeAtIndex(j - (m_displaySubtitles ? cumulatedOriginsCount : 0))->description() != nullptr) {
      // If there is a node description, the cell is bigger
      return ScriptNodeCell::k_complexItemHeight;
    }
    return ScriptNodeCell::k_simpleItemHeight;
  }
  assert(m_displaySubtitles);
  assert(cellType == k_subtitleCellType);
  return k_subtitleRowHeight;
}

int VariableBoxController::numberOfRows() const {
  int result = 0;
  NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
  for (NodeOrigin origin : origins) {
    int nodeCount = nodesCountForOrigin(origin);
    if (nodeCount > 0) {
      result += nodeCount + (m_displaySubtitles ? 1 : 0);
    }
  }
  return result;
}

HighlightCell * VariableBoxController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_itemCellType) {
    return m_itemCells + index;
  }
  assert(m_displaySubtitles);
  assert(type == k_subtitleCellType);
  return m_subtitleCells + index;
}

int VariableBoxController::reusableCellCount(int type) {
  if (type == k_subtitleCellType) {
    assert(m_displaySubtitles);
    return k_scriptOriginsCount;
  }
  assert(type == k_itemCellType);
  return k_maxNumberOfDisplayedItems;
}

void VariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index < numberOfRows());
  NodeOrigin cellOrigin = NodeOrigin::CurrentScript;
  int cumulatedOriginsCount = 0;
  int cellType = typeAndOriginAtLocation(index, &cellOrigin, &cumulatedOriginsCount);
  if (cellType == k_itemCellType) {
    static_cast<ScriptNodeCell *>(cell)->setScriptNode(scriptNodeAtIndex(index - (m_displaySubtitles ? cumulatedOriginsCount : 0)));
    return;
  }
  assert(m_displaySubtitles);
  assert(cellType == k_subtitleCellType);
  I18n::Message subtitleMessages[k_scriptOriginsCount] = {
    I18n::Message::ScriptInProgress,
    I18n::Message::BuiltinsAndKeywords,
    I18n::Message::ImportedModulesAndScripts
  };
  static_cast<MessageTableCell<> *>(cell)->setMessage(subtitleMessages[(int)cellOrigin]);
}

void VariableBoxController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection || !m_displaySubtitles) {
    return;
  }
  // Make sure subtitle cells cannot be selected
  const int currentSelectedRow = selectedRow();
  if (currentSelectedRow >= 0 && typeAtLocation(0, currentSelectedRow) == k_subtitleCellType) {
    if (currentSelectedRow == 0) {
      // We scroll to the first cell, otherwise it will never appear again
      t->scrollToCell(0, 0);
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
  assert(scriptIndex >= 0);

  // Reset the node counts
  empty();

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
  Script script = m_scriptStore->scriptAtIndex(scriptIndex);
  assert(!script.isNull());

  /* Handle the fetchedForVariableBox status: we will import the current script
   * variables in loadCurrentVariablesInScript, so we do not want to import
   * those variables before, if any imported script also imported the current
   * script. */
  assert(!script.fetchedForVariableBox());
  script.setFetchedForVariableBox(true);

  // Load the imported and current variables
  const char * scriptContent = script.content();
  assert(scriptContent != nullptr);
  loadImportedVariablesInScript(scriptContent, textToAutocomplete, textToAutocompleteLength);
  loadCurrentVariablesInScript(scriptContent, textToAutocomplete, textToAutocompleteLength);
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

void VariableBoxController::loadVariablesImportedFromScripts() {
  empty();
  const int scriptsCount = m_scriptStore->numberOfScripts();
  for (int i = 0; i < scriptsCount; i++) {
    Script script = m_scriptStore->scriptAtIndex(i);
    if (script.fetchedFromConsole()) {
      loadGlobalAndImportedVariablesInScriptAsImported(script, nullptr, -1, false);
    }
  }
}

void VariableBoxController::empty() {
  m_builtinNodesCount = 0;
  m_currentScriptNodesCount = 0;
  m_importedNodesCount = 0;
  m_shortenResultCharCount = 0;
  m_scriptStore->clearVariableBoxFetchInformation();
}

void VariableBoxController::insertAutocompletionResultAtIndex(int index) {
  ScriptNode * selectedScriptNode = scriptNodeAtIndex(index);

  /* We need to check now if we need to add parentheses: insertTextInCaller
   * calls handleEventWithText, which will reload the autocompletion for the
   * added text, which will probably change the script nodes and
   * selectedScriptNode will become invalid. */
  const bool shouldAddParentheses = selectedScriptNode->type() == ScriptNode::Type::WithParentheses;
  insertTextInCaller(selectedScriptNode->name() + m_shortenResultCharCount, selectedScriptNode->nameLength() - m_shortenResultCharCount);
  // WARNING: selectedScriptNode is now invalid

  if (shouldAddParentheses) {
    insertTextInCaller(ScriptNodeCell::k_parenthesesWithEmpty);
  }
}

// PRIVATE METHODS

int VariableBoxController::NodeNameCompare(ScriptNode * node, const char * name, int nameLength, bool * strictlyStartsWith) {
  assert(strictlyStartsWith == nullptr || *strictlyStartsWith == false);
  assert(nameLength > 0);
  const char * nodeName = node->name();
  const int nodeNameLength = node->nameLength() < 0 ? strlen(nodeName) : node->nameLength();
  const int comparisonLength = std::min(nameLength, nodeNameLength);
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
  return nodeNameLengthStartsWithName ? *(nodeName + nameLength)  : - *(name + nodeNameLength) ;
}

int VariableBoxController::nodesCountForOrigin(NodeOrigin origin) const {
  if (origin == NodeOrigin::Builtins) {
    return static_cast<int>(m_builtinNodesCount);
  }
  return static_cast<int>(*(const_cast<VariableBoxController *>(this)->nodesCountPointerForOrigin(origin)));
}

size_t * VariableBoxController::nodesCountPointerForOrigin(NodeOrigin origin) {
  switch(origin) {
    case NodeOrigin::CurrentScript:
      return &m_currentScriptNodesCount;
    case NodeOrigin::Builtins:
      return &m_builtinNodesCount;
    default:
      assert(origin == NodeOrigin::Importation);
      return &m_importedNodesCount;
  }
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
      if (m_displaySubtitles && i == cellIndex) {
        result = k_subtitleCellType;
      } else {
        cellIndex += nodeCount + (m_displaySubtitles ? 1 : 0);
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
        assert(result != k_subtitleCellType || m_displaySubtitles);
        return result;
      }
    }
  }
  assert(false);
  return k_itemCellType;

}

bool VariableBoxController::selectLeaf(int rowIndex, bool quitToolbox) {
  assert(rowIndex >= 0 && rowIndex < numberOfRows());
  m_selectableTableView.deselectTable();

  int cumulatedOriginsCount = 0;
  int cellType = typeAndOriginAtLocation(rowIndex, nullptr, &cumulatedOriginsCount);
  assert(cellType == k_itemCellType);
  (void)cellType; // Silence warnings

  insertAutocompletionResultAtIndex(rowIndex - (m_displaySubtitles ? cumulatedOriginsCount : 0));

  Container::activeApp()->dismissModalViewController();
  return true;
}

void VariableBoxController::insertTextInCaller(const char * text, int textLength) {
  int textLen = textLength < 0 ? strlen(text) : textLength;
  constexpr int k_maxScriptObjectNameSize = 100; // Ad hoc value
  int commandBufferMaxSize = std::min(k_maxScriptObjectNameSize, textLen + 1);
  char commandBuffer[k_maxScriptObjectNameSize];
  Shared::ToolboxHelpers::TextToInsertForCommandText(text, textLen, commandBuffer, commandBufferMaxSize, true);
  sender()->handleEventWithText(commandBuffer);
}

void VariableBoxController::loadBuiltinNodes(const char * textToAutocomplete, int textToAutocompleteLength) {
  //TODO Could be great to use strings defined in STATIC const char *const tok_kw[] in python/lexer.c
  /* The commented values do not work with our current MicroPython but might
   * work later, which is why we keep them. */
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
  for (int i = 0; i < k_totalBuiltinNodesCount; i++) {
    if (addNodeIfMatches(textToAutocomplete, textToAutocompleteLength, builtinNames[i].type, NodeOrigin::Builtins, builtinNames[i].name)) {
      /* We can leverage on the fact that builtin nodes are stored in
       * alphabetical order. */
      return;
    }
  }
}

/* WARNING: This is very dirty.
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
   * line basis until parsing fails, while detecting import structures. */
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    const char * parseStart = scriptContent;
    // Skip new lines at the beginning of the script
    while (*parseStart == '\n' && *parseStart != 0) {
      parseStart++;
    }
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
  /* To find variable and function names: we lex the script and keep all
   * MP_TOKEN_NAME that complete the text to autocomplete and are not already in
   * the builtins or imported scripts. */

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {

    // Lex the script
    _mp_lexer_t *lex = mp_lexer_new_from_str_len(0, scriptContent, strlen(scriptContent), false);

    // Keep track of DEF tokens to differentiate between variables and functions
    bool defToken = false;
    const char * beginningLine = scriptContent;
    size_t beginningLineIndex = 0;

    while (lex->tok_kind != MP_TOKEN_END) {
      // Keep only MP_TOKEN_NAME tokens
      if (lex->tok_kind == MP_TOKEN_NAME) {

        int nameLength = lex->vstr.len;

        /* If the token autocompletes the text and it is not already in the
         * variable box, add it. */
        const NodeOrigin origin = NodeOrigin::CurrentScript;

        // Find the token position in the text
        size_t line = lex->tok_line - 1; // tok_line starts at 1, not 0
        if (beginningLineIndex < line) {
          while (beginningLineIndex < line) {
            beginningLine = UTF8Helper::CodePointSearch(beginningLine, '\n') + 1;
            beginningLineIndex++;
            assert(*beginningLine != 0); // We should not get to the end of the text
          }
        }
        assert(beginningLineIndex == line);
        const char * tokenInText = beginningLine + lex->tok_column - 1; // tok_column starts at 1, not 0
        assert(strncmp(tokenInText, lex->vstr.buf, nameLength) == 0);

        ScriptNode::Type nodeType = (defToken || *(tokenInText + nameLength) == '(')? ScriptNode::Type::WithParentheses : ScriptNode::Type::WithoutParentheses;
        if (addNodeIfMatches(textToAutocomplete, textToAutocompleteLength, nodeType, origin, tokenInText, nameLength)) {
          break;
        }
      }

      defToken = lex->tok_kind == MP_TOKEN_KW_DEF;
      mp_lexer_to_next(lex);
    }

    mp_lexer_free(lex);
    nlr_pop();
  }
}

void VariableBoxController::loadGlobalAndImportedVariablesInScriptAsImported(Script script, const char * textToAutocomplete, int textToAutocompleteLength, bool importFromModules) {
  if (script.fetchedForVariableBox()) {
    // We already fetched these script variables
    return;
  }
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    const char * scriptName = script.fullName();
    const char * scriptContent = script.content();
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, scriptContent, strlen(scriptContent), false);
    mp_parse_tree_t parseTree = mp_parse(lex, MP_PARSE_FILE_INPUT);
    mp_parse_node_t pn = parseTree.root;

    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
      mp_parse_node_struct_t * pns = (mp_parse_node_struct_t *)pn;
      uint structKind = (uint)MP_PARSE_NODE_STRUCT_KIND(pns);
      if (structKind == PN_funcdef || structKind == PN_expr_stmt) {
        // The script is only a single function or variable definition
        addImportStructFromScript(pns, structKind, scriptName, textToAutocomplete, textToAutocompleteLength);
      } else if (addNodesFromImportMaybe(pns, textToAutocomplete, textToAutocompleteLength, importFromModules)) {
        // The script is is only an import, handled in addNodesFromImportMaybe
      } else if (structKind == PN_file_input_2) {
        /* At this point, if the script node is not of type "file_input_2", it
         * will not have main structures of the wanted type.
         * We look for structures at first level (not inside nested scopes) that
         * are either function definitions, variables statements or imports. */
        size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (size_t i = 0; i < n; i++) {
          mp_parse_node_t child = pns->nodes[i];
          if (MP_PARSE_NODE_IS_STRUCT(child)) {
            mp_parse_node_struct_t *child_pns = (mp_parse_node_struct_t*)(child);
            structKind = (uint)MP_PARSE_NODE_STRUCT_KIND(child_pns);
            if (structKind == PN_funcdef || structKind == PN_expr_stmt) {
              if (addImportStructFromScript(child_pns, structKind, scriptName, textToAutocomplete, textToAutocompleteLength)) {
                break;
              }
            } else {
              addNodesFromImportMaybe(child_pns, textToAutocomplete, textToAutocompleteLength, importFromModules);
            }
          }
        }
      }
    }
    mp_parse_tree_clear(&parseTree);
    nlr_pop();
  }
  // Mark that we already fetched these script variables
  script.setFetchedForVariableBox(true);
}

bool VariableBoxController::addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength, bool importFromModules) {
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

  /* loadAllSourceContent will be True if the struct imports all the content
   * from a script / module (for instance, "import math"), instead of single
   * items (for instance, "from math import sin"). */
  bool loadAllSourceContent = structKindIsImportWithoutFrom;

  size_t childNodesCount = MP_PARSE_NODE_STRUCT_NUM_NODES(parseNode);
  for (size_t i = 0; i < childNodesCount; i++) {
    mp_parse_node_t child = parseNode->nodes[i];
    if (MP_PARSE_NODE_IS_LEAF(child) && MP_PARSE_NODE_LEAF_KIND(child) == MP_PARSE_NODE_ID) {
      // Parsing something like "import xyz"
      const char * id = qstr_str(MP_PARSE_NODE_LEAF_ARG(child));

      /* xyz might be:
       *  - a module name -> in which case we want no importation source on the
       *    node. The node will not be added if it is already in the builtins.
       *  - a script name -> we want to have xyz.py as the importation source
       *  - a non-existing identifier -> we want no source */
      const char * sourceId = nullptr;
      if (importationSourceIsModule(id)) {
        if (!importFromModules) {
          return true;
        }
      } else {
        /*  If a module and a script have the same name, the micropython
         *  importation algorithm first looks for a module then for a script. We
         *  should thus check that the id is not a module name before retreiving
         *  a script name to put it as source. */
        if (!importationSourceIsScript(id, &sourceId) && !importFromModules) { // Warning : must be done in this order
          /* We call importationSourceIsScript to load the script name in
           * sourceId. We also use it to make sure, if importFromModules is
           * false, that we are not importing variables from something else than
           * scripts. */
          return true;
        }
      }
      if (addNodeIfMatches(textToAutocomplete, textToAutocompleteLength, ScriptNode::Type::WithoutParentheses, NodeOrigin::Importation, id, -1, sourceId)) {
        break;
      }
    } else if (MP_PARSE_NODE_IS_STRUCT(child)) {
      // Parsing something like "from math import sin"
      addNodesFromImportMaybe((mp_parse_node_struct_t *)child, textToAutocomplete, textToAutocompleteLength, importFromModules);
    } else if (MP_PARSE_NODE_IS_TOKEN(child) && MP_PARSE_NODE_IS_TOKEN_KIND(child, MP_TOKEN_OP_STAR)) {
      /* Parsing something like "from math import *"
       * -> Load all the module content */
      loadAllSourceContent = true;
    }
  }

  // Fetch a script / module content if needed
  if (loadAllSourceContent) {
    assert(childNodesCount > 0);
    const char * importationSourceName = importationSourceNameFromNode(parseNode->nodes[0]);
    if (importationSourceName == nullptr) {
      // For instance, the name is a "dotted name" but not matplotlib.pyplot
      return true;
    }
    int numberOfModuleChildren = 0;
    const ToolboxMessageTree * moduleChildren = nullptr;
    if (importationSourceIsModule(importationSourceName, &moduleChildren, &numberOfModuleChildren)) {
      if (!importFromModules) {
        return true;
      }
      if (moduleChildren != nullptr) {
        /* The importation source is a module that we display in the toolbox:
         * get the nodes from the toolbox
         * We skip the 3 first nodes, which are "import ...", "from ... import *"
         * and "....function". */
        constexpr int numberOfNodesToSkip = 3;
        assert(numberOfModuleChildren > numberOfNodesToSkip);
        for (int i = numberOfNodesToSkip; i < numberOfModuleChildren; i++) {
          const char * name = I18n::translate((moduleChildren + i)->label());
          if (addNodeIfMatches(textToAutocomplete, textToAutocompleteLength, ScriptNode::Type::WithoutParentheses, NodeOrigin::Importation, name, -1, importationSourceName, I18n::translate((moduleChildren + i)->text()))) {
            break;
          }
        }
      } else {
        //TODO get module variables that are not in the toolbox
      }
    } else {
      // Try fetching the nodes from a script
      Script importedScript;
      const char * scriptFullName;
      if (importationSourceIsScript(importationSourceName, &scriptFullName, &importedScript)) {
        loadGlobalAndImportedVariablesInScriptAsImported(importedScript, textToAutocomplete, textToAutocompleteLength);
      }
    }
  }
  return true;
}

const char * VariableBoxController::importationSourceNameFromNode(mp_parse_node_t & node) {
  if (MP_PARSE_NODE_IS_LEAF(node) && MP_PARSE_NODE_LEAF_KIND(node) == MP_PARSE_NODE_ID) {
    // The importation source is "simple", for instance: from math import *
    return qstr_str(MP_PARSE_NODE_LEAF_ARG(node));
  }
  if (MP_PARSE_NODE_IS_STRUCT(node)) { //TODO replace this with an assert?
    mp_parse_node_struct_t * nodePNS = (mp_parse_node_struct_t *)node;
    uint nodeStructKind = MP_PARSE_NODE_STRUCT_KIND(nodePNS);
    if (nodeStructKind != PN_dotted_name) {
      return nullptr;
    }
    /* The importation source is "complex", for instance:
     * from matplotlib.pyplot import *
     * FIXME The solution would be to build a single qstr for this name,
     * such as in python/src/compile.c, function do_import_name, from line
     * 1117 (found by searching PN_dotted_name).
     * We might do this later, for now the only dotted name we might want to
     * find is matplolib.pyplot, so we do a very specific search. */
    int numberOfSplitNames = MP_PARSE_NODE_STRUCT_NUM_NODES(nodePNS);
    if (numberOfSplitNames != 2) {
      return nullptr;
    }
    const char * nodeSubName = qstr_str(MP_PARSE_NODE_LEAF_ARG(nodePNS->nodes[0]));
    if (strcmp(nodeSubName, qstr_str(MP_QSTR_matplotlib)) == 0) {
      nodeSubName = qstr_str(MP_PARSE_NODE_LEAF_ARG(nodePNS->nodes[1]));
      if (strcmp(nodeSubName, qstr_str(MP_QSTR_pyplot)) == 0) {
        return qstr_str(MP_QSTR_matplotlib_dot_pyplot);
      }
    }
  }
  return nullptr;
}

bool VariableBoxController::importationSourceIsModule(const char * sourceName, const ToolboxMessageTree * * moduleChildren, int * numberOfModuleChildren) {
  const ToolboxMessageTree * children = static_cast<PythonToolbox *>(App::app()->toolboxForInputEventHandler(nullptr))->moduleChildren(sourceName, numberOfModuleChildren);
  if (moduleChildren != nullptr) {
    *moduleChildren = children;
  }
  if (children != nullptr) {
    return true;
  }
  // The sourceName might be a module that is not in the toolbox
  return mp_module_get_loaded_or_builtin(qstr_from_str(sourceName)) != MP_OBJ_NULL;
}

bool VariableBoxController::importationSourceIsScript(const char * sourceName, const char * * scriptFullName, Script * retrievedScript) {
   // Try fetching the nodes from a script
   Script importedScript = ScriptStore::ScriptBaseNamed(sourceName);
   if (importedScript.isNull()) {
     return false;
   }
   *scriptFullName = importedScript.fullName();
   if (retrievedScript != nullptr) {
      *retrievedScript = importedScript;
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

bool VariableBoxController::addImportStructFromScript(mp_parse_node_struct_t * pns, uint structKind, const char * scriptName, const char * textToAutocomplete, int textToAutocompleteLength) {
  assert(structKind == PN_funcdef || structKind == PN_expr_stmt);
  // Find the id child node, which stores the struct's name
  const char * name = structName(pns);
  if (name == nullptr) {
    return false;
  }
  return addNodeIfMatches(textToAutocomplete, textToAutocompleteLength, structKind == PN_funcdef ? ScriptNode::Type::WithParentheses : ScriptNode::Type::WithoutParentheses, NodeOrigin::Importation, name, -1, scriptName);
}

// The returned boolean means we should escape the process
bool VariableBoxController::addNodeIfMatches(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type nodeType, NodeOrigin nodeOrigin, const char * nodeName, int nodeNameLength, const char * nodeSourceName, const char * nodeDescription) {
  assert(nodeName != nullptr);
  if (nodeNameLength < 0) {
    nodeNameLength = strlen(nodeName);
  }
  // Step 1: Check if the node matches the textToAutocomplete

  // Step 1.1: Few escape cases
  /* If the node will go to imported, do not add it if it starts with an
   * underscore : such identifiers are meant to be private. */
  if (nodeOrigin == NodeOrigin::Importation && UTF8Helper::CodePointIs(nodeName, '_')) {
    return false;
  }
  /* If the node is extracted from the current script, escape the current
   * autocompleted word. */
  if (nodeOrigin == NodeOrigin::CurrentScript && nodeName == textToAutocomplete) {
    return false;
  }
  bool nodeInLexicographicalOrder = nodeOrigin == NodeOrigin::Builtins;

  ScriptNode node(nodeType, nodeName, nodeNameLength, nodeSourceName, nodeDescription);

  // Step 1.2: check if textToAutocomplete matches the node
  if (textToAutocomplete != nullptr) {
    /* Check that nodeName autocompletes the text to autocomplete
     *  - The start of nodeName must be equal to the text to autocomplete */
    bool strictlyStartsWith = false;
    int cmp = NodeNameCompare(&node, textToAutocomplete, textToAutocompleteLength, &strictlyStartsWith);
    if (cmp == 0) {
      // We don't accept the node if it has no parentheses
      if (node.type() != ScriptNode::Type::WithParentheses) {
        return false;
      }
    } else {
      // We don't accept the node if it doesn't start as the textToAutocomplete
      if (!strictlyStartsWith) {
        if (nodeInLexicographicalOrder && cmp > 0) {
          /* Signal to end the nodes scanning because we went past the
           * textToAutocomplete in lexicographical order. */
          return true;
        }
        return false;
      }
    }
  }

  // Step 2: Add Node

  // Step 2.1: don't overflow the node list
  size_t * currentNodeCount = nodesCountPointerForOrigin(nodeOrigin);
  ScriptNode * nodes = nodesForOrigin(nodeOrigin);
  if (*currentNodeCount >= MaxNodesCountForOrigin(nodeOrigin)) {
    // There is no room to add another node
    return true;
  }

  // Step 2.2: find where to add the node (and check that it doesn't exist yet)
  size_t insertionIndex = *currentNodeCount;
  if (nodeOrigin == NodeOrigin::Builtins) {
    /* For builtin nodes, we don't need to check whether the node was already
     * added because they're added first in lexicographical order. Plus, we
     * want to add it at the end of list to respect the lexicographical order. */
    assert(nodeInLexicographicalOrder);
  } else {
    // Look where to add
    bool alreadyInVarBox = false;
    // This could be faster with dichotomia, but there is no speed problem for now
    NodeOrigin origins[] = {NodeOrigin::CurrentScript, NodeOrigin::Builtins, NodeOrigin::Importation};
    for (NodeOrigin origin : origins) {
      const int nodesCount = nodesCountForOrigin(origin);
      ScriptNode * nodes = nodesForOrigin(origin);
      for (int i = 0; i < nodesCount; i++) {
        ScriptNode * matchingNode = nodes + i;
        int comparisonResult = NodeNameCompare(matchingNode, nodeName, nodeNameLength);
        if (comparisonResult == 0 || (comparisonResult == '(' && nodeType == ScriptNode::Type::WithParentheses)) {
          alreadyInVarBox = true;
          break;
        }
        if (comparisonResult > 0) {
          if (nodeOrigin == origin) {
            insertionIndex = i;
          }
          break;
        }
      }
      if (alreadyInVarBox) {
        return false;
      }
    }
  }

  // Step 2.3: Shift all the following nodes
  for (size_t i = *currentNodeCount; i > insertionIndex; i--) {
    nodes[i] = nodes[i - 1];
  }

  // Step 2.4: Check if the node source name fits, if not, do not use it
  if (!ScriptNodeCell::CanDisplayNameAndSource(nodeNameLength, nodeSourceName)) {
    nodeSourceName = nullptr;
  }
  // Step 2.5: Add the node
  nodes[insertionIndex] = ScriptNode(nodeType, nodeName, nodeNameLength, nodeSourceName, nodeDescription);
  // Increase the node count
  *currentNodeCount = *currentNodeCount + 1;
  return false;
}

}
