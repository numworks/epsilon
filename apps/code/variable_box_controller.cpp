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
  m_scriptNodesCount(0),
  m_scriptStore(scriptStore)
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
  assert(m_scriptNodesCount <= k_maxScriptNodesCount);
  return m_scriptNodesCount;
}

int VariableBoxController::reusableCellCount(int type) {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void VariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index < m_scriptNodesCount);
  assert(m_scriptNodesCount <= k_maxScriptNodesCount);
  ScriptNodeCell * myCell = static_cast<ScriptNodeCell *>(cell);
  myCell->setScriptNode(&m_scriptNodes[index]);
}

int VariableBoxController::typeAtLocation(int i, int j) {
  return 0;
}

void VariableBoxController::loadFunctionsAndVariables() {
  m_scriptNodesCount = 0;
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
}

HighlightCell * VariableBoxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

bool VariableBoxController::selectLeaf(int rowIndex) {
  assert(rowIndex >= 0 && rowIndex < m_scriptNodesCount);
  assert(m_scriptNodesCount <= k_maxScriptNodesCount);
  m_selectableTableView.deselectTable();
  ScriptNode selectedScriptNode = m_scriptNodes[rowIndex];
  insertTextInCaller(selectedScriptNode.name());
  if (selectedScriptNode.type() == ScriptNode::Type::Function) {
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

void VariableBoxController::addFunctionAtIndex(const char * functionName, int scriptIndex) {
  if (m_scriptNodesCount < k_maxScriptNodesCount) {
    m_scriptNodes[m_scriptNodesCount] = ScriptNode::FunctionNode(functionName, scriptIndex);
    m_scriptNodesCount++;
  }
}

void VariableBoxController::addVariableAtIndex(const char * variableName, int scriptIndex) {
  if (m_scriptNodesCount < k_maxScriptNodesCount) {
    m_scriptNodes[m_scriptNodesCount] = ScriptNode::VariableNode(variableName, scriptIndex);
    m_scriptNodesCount++;
  }
}

}
