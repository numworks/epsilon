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

namespace Code {

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

void VariableBoxController::loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete) {
  //TODO LEA Prune these + add modules
  //TODO LEA load buitins only once
  int index = 0;
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_abs), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_all), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_and), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_any), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_as), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_ascii), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_assert), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bin), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bool), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_break), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_breakpoint), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bytearray), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_bytes), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_callable), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_chr), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_class), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_classmethod), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_compile), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_complex), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_continue), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_def), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_del), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_delattr), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_dict), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_dir), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_divmod), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_elif), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_else), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_enumerate), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_eval), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_except), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_exec), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_False), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_filter), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_finally), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_float), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_for), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_format), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_from), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_frozenset), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_getattr), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_global), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_globals), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_hasattr), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_hash), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_help), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_hex), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_id), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_if), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_import), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_in), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_input), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_int), 0);
 // m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_is), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_isinstance), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_issubclass), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_iter), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_lambda), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_len), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_list), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_locals), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_map), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_max), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_memoryview), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_min), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_next), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_None), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_nonlocal), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_not), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_object), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_oct), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_open), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_or), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_ord), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_pass), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_pow), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_print), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_property), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_raise), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_range), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_repr), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_return), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_reversed), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_round), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_set), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_setattr), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_slice), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_sorted), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_staticmethod), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_str), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_sum), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_super), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_True), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_try), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_tuple), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_type), 0);
  //m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_vars), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_while), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_with), 0);
  //m_builtinNodes[index++] = ScriptNode::VariableNode(qstr_str(MP_QSTR_yield), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR_zip), 0);
  m_builtinNodes[index++] = ScriptNode::FunctionNode(qstr_str(MP_QSTR___import__), 0); //TODO LEA alphabetical order?
  assert(index == k_builtinNodesCount);

#if 0
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

const char * VariableBoxController::autocompletionForText(int scriptIndex, const char * text) {
  // TODO LEA Accelerate
  loadFunctionsAndVariables(scriptIndex, text);
  const char * endOfText = UTF8Helper::EndOfWord(text);
  const int textLength = endOfText - text;
  assert(textLength >= 1);
  for (int i = 0; i < numberOfRows(); i++) {
    const char * currentName = scriptNodeAtIndex(i)->name();
    if (strncmp(text, currentName, textLength) == 0 && *(currentName + textLength)
        != 0) {
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
  insertTextInCaller(selectedScriptNode->name());
  if (selectedScriptNode->type() == ScriptNode::Type::Function) {
    insertTextInCaller(ScriptNodeCell::k_parenthesesWithEmpty);
  }
  Container::activeApp()->dismissModalViewController();
  return true;
}

void VariableBoxController::insertTextInCaller(const char * text) {
  int commandBufferMaxSize = strlen(text)+1;
  char commandBuffer[k_maxScriptObjectNameSize];
  assert(commandBufferMaxSize <= k_maxScriptObjectNameSize);
  Shared::ToolboxHelpers::TextToInsertForCommandText(text, commandBuffer, commandBufferMaxSize, true);
  sender()->handleEventWithText(commandBuffer);
}

// TODO LEA remane method (index)
void VariableBoxController::addFunctionAtIndex(const char * functionName, int scriptIndex) {
  // TODO LEA check if current script or imported
  if (m_currentScriptNodesCount < k_maxScriptNodesCount) {
    m_currentScriptNodes[m_currentScriptNodesCount] = ScriptNode::FunctionNode(functionName, scriptIndex);
    m_currentScriptNodesCount++;
  }
}

// TODO LEA remane method (index)
// + factorize with addFunctionAtIndex?
void VariableBoxController::addVariableAtIndex(const char * variableName, int scriptIndex) {
  // TODO LEA check if current script or imported
  if (m_currentScriptNodesCount < k_maxScriptNodesCount) {
    m_currentScriptNodes[m_currentScriptNodesCount] = ScriptNode::VariableNode(variableName, scriptIndex);
    m_currentScriptNodesCount++;
  }
}

ScriptNode * VariableBoxController::scriptNodeAtIndex(int index) {
  assert(index >= 0 && index < numberOfRows());
  assert(m_currentScriptNodesCount <= k_maxScriptNodesCount);
  assert(m_importedNodesCount <= k_maxScriptNodesCount);
  ScriptNode * node = nullptr;
  if (index < m_currentScriptNodesCount) {
    node = m_currentScriptNodes + index;
  } else if (index < m_currentScriptNodesCount + k_builtinNodesCount) {
    node = m_builtinNodes + (index - m_currentScriptNodesCount);
  } else {
    node = m_importedNodes + (index - m_currentScriptNodesCount - k_builtinNodesCount);
  }
  return node;
}

}
