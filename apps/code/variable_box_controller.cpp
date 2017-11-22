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
  m_currentDepth(0),
  m_firstSelectedRow(0),
  m_previousSelectedRow(0),
  m_scriptFunctionsCount(-1),
  m_scriptVariablesCount(-1),
  m_menuController(menuController),
  m_scriptStore(scriptStore),
  m_selectableTableView(this, this, 0, 1, 0, 0, 0, 0, this, nullptr, false)
{
  for (int i = 0; i < ScriptStore::k_maxNumberOfScripts; i++) {
    m_functionDefinitionsCount[i] = 0;
    m_globalVariablesCount[i] = 0;
  }
  for (int i = 0; i < k_functionsAndVarsNamePointersArrayLength; i++) {
    m_functionNamesPointers[i] = nullptr;
    m_globalVariablesNamesPointers[i] = nullptr;
  }
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

void VariableBoxController::ContentViewController::resetDepth() {
  m_currentDepth = 0;
}

void VariableBoxController::ContentViewController::setFunctionsCountInScriptAtIndex(int functionsCount, int scriptIndex) {
  assert(functionsCount >= 0);
  assert(scriptIndex >= 0 && scriptIndex < ScriptStore::k_maxNumberOfScripts);
  m_functionDefinitionsCount[scriptIndex] = functionsCount;
}

void VariableBoxController::ContentViewController::setFunctionNameAtIndex(const char * functionName, int functionIndex) {
  assert(functionIndex >= 0 && functionIndex < k_functionsAndVarsNamePointersArrayLength);
  m_functionNamesPointers[functionIndex] = functionName;
}

void VariableBoxController::ContentViewController::setGlobalVariablesCountInScriptAtIndex(int globalVariablesCount, int scriptIndex) {
  assert(globalVariablesCount >= 0);
  assert(scriptIndex >= 0 && scriptIndex < ScriptStore::k_maxNumberOfScripts);
  m_globalVariablesCount[scriptIndex] = globalVariablesCount;
}

void VariableBoxController::ContentViewController::setGlobalVariableNameAtIndex(const char * globalVariableName, int globalVariableIndex) {
  assert(globalVariableIndex >= 0 && globalVariableIndex < k_functionsAndVarsNamePointersArrayLength);
  m_globalVariablesNamesPointers[globalVariableIndex] = globalVariableName;
}

const char * VariableBoxController::ContentViewController::title() {
  return  I18n::translate(I18n::Message::FunctionsAndVariables);
}

void VariableBoxController::ContentViewController::viewWillAppear() {
  m_menuController->loadPythonIfNeeded();
  for (int i = 0; i < ScriptStore::k_maxNumberOfScripts; i++) {
    m_functionDefinitionsCount[i] = 0;
    m_globalVariablesCount[i] = 0;
  }
  m_scriptStore->scanScriptsForFunctionsAndVariables(
      this,
      [](void * context, int scriptIndex, int functionsCount) {
      VariableBoxController::ContentViewController * cvc = static_cast<VariableBoxController::ContentViewController *>(context);
      cvc->setFunctionsCountInScriptAtIndex(functionsCount, scriptIndex);
      },
      [](void * context, int functionIndex, const char * functionName) {
      VariableBoxController::ContentViewController * cvc = static_cast<VariableBoxController::ContentViewController *>(context);
      cvc->setFunctionNameAtIndex(functionName, functionIndex);
      },
      [](void * context, int scriptIndex, int globalVariablesCount) {
      VariableBoxController::ContentViewController * cvc = static_cast<VariableBoxController::ContentViewController *>(context);
      cvc->setGlobalVariablesCountInScriptAtIndex(globalVariablesCount, scriptIndex);
      },
      [](void * context, int globalVariableIndex, const char * globalVariableName) {
      VariableBoxController::ContentViewController * cvc = static_cast<VariableBoxController::ContentViewController *>(context);
      cvc->setGlobalVariableNameAtIndex(globalVariableName, globalVariableIndex);
      });

  m_scriptFunctionsCount = 0;
  m_scriptVariablesCount = 0;
  for (int i = 0; i < ScriptStore::k_maxNumberOfScripts; i++) {
    m_scriptFunctionsCount+= m_functionDefinitionsCount[i];
    m_scriptVariablesCount+= m_globalVariablesCount[i];
  }
}

void VariableBoxController::ContentViewController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

void VariableBoxController::ContentViewController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.scrollToCell(0,0);
  selectCellAtLocation(0, m_firstSelectedRow);
  app()->setFirstResponder(&m_selectableTableView);
}

bool VariableBoxController::ContentViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_currentDepth == 0) {
    m_firstSelectedRow = 0;
    app()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Left && m_currentDepth == 0) {
    return true;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Left) {
    assert(m_currentDepth == 1);
    m_firstSelectedRow = m_previousSelectedRow;
    m_selectableTableView.deselectTable();
    m_currentDepth = 0;
    app()->setFirstResponder(this);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && m_currentDepth == 0)) {
    if (m_currentDepth == 0) {
      // Select a submenu from the Root Menu
      m_previousSelectedRow = selectedRow();
      m_firstSelectedRow = 0;
      m_selectableTableView.deselectTable();
      m_currentDepth = 1;
      app()->setFirstResponder(this);
      return true;
    }
    // Select a leaf in a submenu
    m_firstSelectedRow = 0;
    DoubleBufferTextCell * selectedTextCell = static_cast<DoubleBufferTextCell *>(m_selectableTableView.selectedCell());
    insertTextInCaller(selectedTextCell->firstText());
    m_selectableTableView.deselectTable();
    m_currentDepth = 0;
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

int VariableBoxController::ContentViewController::numberOfRows() {
  if (m_currentDepth == 0) {
    return k_numberOfMenuRows;
  }
  assert(m_currentDepth == 1);
  if (m_previousSelectedRow == 0) {
    return m_scriptFunctionsCount;
  }
  assert(m_previousSelectedRow == 1);
  return m_scriptVariablesCount;
}

HighlightCell * VariableBoxController::ContentViewController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == k_leafType) {
    assert(index < k_maxNumberOfDisplayedRows);
    return &m_leafCells[index];
  }
  assert(type == k_nodeType);
  assert(index < k_numberOfMenuRows);
  return &m_nodeCells[index];
}

int VariableBoxController::ContentViewController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == k_leafType) {
    return k_maxNumberOfDisplayedRows;
  }
  assert (type == k_nodeType);
  return k_numberOfMenuRows;
}

void VariableBoxController::ContentViewController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_currentDepth == 0) {
    MessageTableCellWithChevron * myCell = static_cast<MessageTableCellWithChevron *>(cell);
    I18n::Message nodeMessages[2] = {I18n::Message::Functions, I18n::Message::Variables};
    myCell->setMessage(nodeMessages[index]);
    return;
  }
  assert(m_currentDepth == 1);
  DoubleBufferTextCell * myCell = static_cast<DoubleBufferTextCell *>(cell);
  if (m_previousSelectedRow == 0) {
    myCell->setFirstText(m_functionNamesPointers[index]);
    int scriptIndex = -1;
    int functionsCount = 0;
    while (functionsCount < index + 1 && scriptIndex < ScriptStore::k_maxNumberOfScripts) {
      scriptIndex++;
      functionsCount+= m_functionDefinitionsCount[scriptIndex];
    }
    myCell->setSecondText(m_scriptStore->scriptAtIndex(scriptIndex).name());
  } else {
    myCell->setFirstText(m_globalVariablesNamesPointers[index]);
    int scriptIndex = -1;
    int globalVariablesCount = 0;
    while (globalVariablesCount < index + 1 && scriptIndex < ScriptStore::k_maxNumberOfScripts) {
      scriptIndex++;
      globalVariablesCount+= m_globalVariablesCount[scriptIndex];
    }
    myCell->setSecondText(m_scriptStore->scriptAtIndex(scriptIndex).name());
  }
}

KDCoordinate VariableBoxController::ContentViewController::rowHeight(int index) {
  return k_rowHeight;
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
  if (m_currentDepth == 0) {
    return k_nodeType;
  }
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
  m_contentViewController.resetDepth();
  m_contentViewController.reloadData();
}

void VariableBoxController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
}

}
