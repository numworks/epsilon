#include "variable_box_controller.h"
#include "script.h"
#include "../shared/toolbox_helpers.h"
#include <apps/i18n.h>
#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/metric.h>
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
    m_leafCells[i].setFirstTextColor(KDColorBlack);
    m_leafCells[i].setSecondTextColor(Palette::GreyDark);
  }
}

void VariableBoxController::ContentViewController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
  m_textAreaCaller = nullptr;
}

void VariableBoxController::ContentViewController::setTextAreaCaller(TextArea * textArea) {
  m_textAreaCaller = textArea;
  m_textFieldCaller = nullptr;
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
    DoubleBufferTextCell * selectedTextCell = static_cast<DoubleBufferTextCell *>(m_selectableTableView.selectedCell());
    insertTextInCaller(selectedTextCell->firstText());
    m_selectableTableView.deselectTable();
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

int VariableBoxController::ContentViewController::numberOfRows() {
  return m_scriptNodesCount < k_maxScriptNodesCount ? m_scriptNodesCount : k_maxScriptNodesCount;
}

HighlightCell * VariableBoxController::ContentViewController::reusableCell(int index, int type) {
  assert(type == k_leafType);
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

int VariableBoxController::ContentViewController::reusableCellCount(int type) {
  assert(type == k_leafType);
  return k_maxNumberOfDisplayedRows;
}

void VariableBoxController::ContentViewController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  DoubleBufferTextCell * myCell = static_cast<DoubleBufferTextCell *>(cell);
  myCell->setFirstText(m_scriptNodes[index].name());
  myCell->setSecondText(m_scriptStore->scriptAtIndex(m_scriptNodes[index].scriptIndex()).name());
}

KDCoordinate VariableBoxController::ContentViewController::rowHeight(int index) {
  return Metric::ToolboxRowHeight;
}

KDCoordinate VariableBoxController::ContentViewController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int VariableBoxController::ContentViewController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int VariableBoxController::ContentViewController::typeAtLocation(int i, int j) {
  return k_leafType;
}

void VariableBoxController::ContentViewController::insertTextInCaller(const char * text) {
  char commandBuffer[strlen(text)+1];
  Shared::ToolboxHelpers::TextToInsertForCommandText(text, commandBuffer);
  if (m_textFieldCaller != nullptr) {
    if (!m_textFieldCaller->isEditing()) {
      m_textFieldCaller->setEditing(true);
    }
    int previousCursorLocation = m_textFieldCaller->cursorLocation();
    m_textFieldCaller->insertTextAtLocation(commandBuffer, previousCursorLocation);
    m_textFieldCaller->setCursorLocation(previousCursorLocation + Shared::ToolboxHelpers::CursorIndexInCommand(commandBuffer));
    return;
  }
  if (m_textAreaCaller != nullptr) {
    int previousCursorLocation = m_textAreaCaller->cursorLocation();
    m_textAreaCaller->insertText(commandBuffer);
    int deltaCursorLocation = - m_textAreaCaller->cursorLocation() + previousCursorLocation + Shared::ToolboxHelpers::CursorIndexInCommand(commandBuffer);
    m_textAreaCaller->moveCursor(deltaCursorLocation);
  }
}

VariableBoxController::VariableBoxController(MenuController * menuController, ScriptStore * scriptStore) :
  StackViewController(nullptr, &m_contentViewController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_contentViewController(this, menuController, scriptStore)
{
}

void VariableBoxController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_contentViewController);
}

void VariableBoxController::setTextFieldCaller(TextField * textField) {
  m_contentViewController.setTextFieldCaller(textField);
}

void VariableBoxController::setTextAreaCaller(TextArea * textArea) {
  m_contentViewController.setTextAreaCaller(textArea);
}

void VariableBoxController::viewWillAppear() {
  StackViewController::viewWillAppear();
  m_contentViewController.reloadData();
}

void VariableBoxController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
}

}
