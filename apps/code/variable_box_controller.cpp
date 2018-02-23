#include "variable_box_controller.h"
#include "script.h"
#include "../shared/toolbox_helpers.h"
#include <apps/i18n.h>
#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/palette.h>
#include <string.h>

namespace Code {

/* ContentViewController */

VariableBoxController::ContentViewController::ContentViewController(Responder * parentResponder, MenuController * menuController, ScriptStore * scriptStore) :
  ViewController(parentResponder),
  m_scriptNodesCount(0),
  m_menuController(menuController),
  m_scriptStore(scriptStore),
  m_selectableTableView(this, this, 0, 1, 0, 0, 0, 0, this, nullptr, false)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setScriptStore(scriptStore);
  }
}

void VariableBoxController::ContentViewController::setTextInputCaller(TextInput * textInput) {
  m_textInputCaller = textInput;
}

void VariableBoxController::ContentViewController::reloadData() {
  m_selectableTableView.reloadData();
}

void VariableBoxController::ContentViewController::addFunctionAtIndex(const char * functionName, int scriptIndex) {
  m_scriptNodes[m_scriptNodesCount] = ScriptNode::FunctionNode(functionName, scriptIndex);
  m_scriptNodesCount++;
}

void VariableBoxController::ContentViewController::addVariableAtIndex(const char * variableName, int scriptIndex) {
  m_scriptNodes[m_scriptNodesCount] = ScriptNode::VariableNode(variableName, scriptIndex);
  m_scriptNodesCount++;
}

const char * VariableBoxController::ContentViewController::title() {
  return  I18n::translate(I18n::Message::FunctionsAndVariables);
}

void VariableBoxController::ContentViewController::viewWillAppear() {
  m_menuController->loadPythonIfNeeded();
  m_scriptNodesCount = 0;
  m_scriptStore->scanScriptsForFunctionsAndVariables(
    this,
    [](void * context, const char * functionName, int scriptIndex) {
      VariableBoxController::ContentViewController * cvc = static_cast<VariableBoxController::ContentViewController *>(context);
      cvc->addFunctionAtIndex(functionName, scriptIndex);},
    [](void * context, const char * variableName, int scriptIndex) {
      VariableBoxController::ContentViewController * cvc = static_cast<VariableBoxController::ContentViewController *>(context);
      cvc->addVariableAtIndex(variableName, scriptIndex);});
}

void VariableBoxController::ContentViewController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

void VariableBoxController::ContentViewController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.scrollToCell(0,0);
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool VariableBoxController::ContentViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    app()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Left) {
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (m_selectableTableView.selectedRow() < 0 || m_selectableTableView.selectedRow() >= m_scriptNodesCount) {
      return false;
    }
    ScriptNode selectedScriptNode = m_scriptNodes[m_selectableTableView.selectedRow()];
    insertTextInCaller(selectedScriptNode.name());
    if (selectedScriptNode.type() == ScriptNode::Type::Function) {
      insertTextInCaller(ScriptNodeCell::k_parentheses);
    }
    m_selectableTableView.deselectTable();
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

int VariableBoxController::ContentViewController::numberOfRows() {
  return m_scriptNodesCount < k_maxScriptNodesCount ? m_scriptNodesCount : k_maxScriptNodesCount;
}

HighlightCell * VariableBoxController::ContentViewController::reusableCell(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

int VariableBoxController::ContentViewController::reusableCellCount() {
  return k_maxNumberOfDisplayedRows;
}

void VariableBoxController::ContentViewController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ScriptNodeCell * myCell = static_cast<ScriptNodeCell *>(cell);
  myCell->setScriptNode(&m_scriptNodes[index]);
}

void VariableBoxController::ContentViewController::insertTextInCaller(const char * text) {
  char commandBuffer[strlen(text)+1];
  Shared::ToolboxHelpers::TextToInsertForCommandText(text, commandBuffer);
  m_textInputCaller->handleEventWithText(commandBuffer);
}

VariableBoxController::VariableBoxController(MenuController * menuController, ScriptStore * scriptStore) :
  StackViewController(nullptr, &m_contentViewController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_contentViewController(this, menuController, scriptStore)
{
}

void VariableBoxController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_contentViewController);
}

void VariableBoxController::setTextInputCaller(TextInput * textInput) {
  m_contentViewController.setTextInputCaller(textInput);
}

void VariableBoxController::viewWillAppear() {
  StackViewController::viewWillAppear();
  m_contentViewController.reloadData();
}

void VariableBoxController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
}

}
