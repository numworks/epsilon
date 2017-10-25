#include "menu_controller.h"
#include "../i18n.h"
#include <assert.h>
#include <escher/metric.h>

namespace Code {

MenuController::MenuController(Responder * parentResponder, ScriptStore * scriptStore, ButtonRowController * footer) :
  ViewController(parentResponder),
  ButtonRowDelegate(nullptr, footer),
  m_scriptStore(scriptStore),
  m_addNewScriptCell(I18n::Message::AddScript),
  m_consoleButton(this, I18n::Message::Console, Invocation([](void * context, void * sender) {
    MenuController * menu = (MenuController *)context;
    menu->app()->displayModalViewController(menu->consoleController(), 0.5f, 0.5f);
  }, this)),
  m_selectableTableView(this, this, 0, 1, 0, 0, 0, 0, this, nullptr, false),
  m_consoleController(parentResponder, m_scriptStore),
  m_scriptParameterController(nullptr, I18n::Message::ScriptOptions, m_scriptStore, this)
{
  for (int i = 0; i < k_maxNumberOfDisplayableScriptCells; i++) {
    m_scriptCells[i].setParentResponder(&m_selectableTableView);
    m_scriptCells[i].editableTextCell()->textField()->setDelegate(this);
    m_scriptCells[i].editableTextCell()->textField()->setDraftTextBuffer(m_draftTextBuffer);
    m_scriptCells[i].editableTextCell()->textField()->setAlignment(0.0f, 0.5f);
    m_scriptCells[i].editableTextCell()->setMargins(0, 0, 0, Metric::HistoryHorizontalMargin);
  }
  m_selectableTableView.selectCellAtLocation(0, 0);
}

ConsoleController * MenuController::consoleController() {
  return &m_consoleController;
}

View * MenuController::view() {
  return &m_selectableTableView;
}

void MenuController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    footer()->setSelectedButton(0);
    return true;
  } else if (event == Ion::Events::Up) {
    if (m_selectableTableView.selectedRow() < 0) {
      footer()->setSelectedButton(-1);
      m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
      app()->setFirstResponder(&m_selectableTableView);
      return true;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int selectedRow = m_selectableTableView.selectedRow();
    if (selectedRow >= 0 && selectedRow < m_scriptStore->numberOfScripts()) {
      configureScript();
      return true;
    } else if (selectedRow == m_scriptStore->numberOfScripts()) {
      addScript();
      return true;
    }
  }
  return false;
}

void MenuController::configureScript() {
  m_scriptParameterController.setScript(m_selectableTableView.selectedRow());
  stackViewController()->push(&m_scriptParameterController);
}

void MenuController::addScript() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  m_scriptStore->addNewScript();
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, numberOfRows()-2);
}

void MenuController::renameScriptAtIndex(int i) {
  assert(i>=0 && i<m_scriptStore->numberOfScripts());
  EvenOddEditableTextCell * myCell = static_cast<EvenOddEditableTextCell *>(m_selectableTableView.selectedCell());
  myCell->setHighlighted(false);
  const char * previousText = myCell->editableTextCell()->textField()->text();
  myCell->editableTextCell()->textField()->setEditing(true);
  myCell->editableTextCell()->textField()->setText(previousText);
  app()->setFirstResponder(myCell);
}

void MenuController::deleteScriptAtIndex(int i) {
  m_scriptStore->deleteScriptAtIndex(i);
  m_selectableTableView.reloadData();
}

int MenuController::numberOfRows() {
  return m_scriptStore->numberOfScripts() + 1;
  //TODO do not add the addScript row if there can be no more scripts stored.
};

KDCoordinate MenuController::cellHeight() {
  return k_rowHeight;
}

KDCoordinate MenuController::rowHeight(int j) {
  return cellHeight();
}

KDCoordinate MenuController::cumulatedHeightFromIndex(int j) {
  return cellHeight() * j;
}

int MenuController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = cellHeight();
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

HighlightCell * MenuController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == ScriptCellType) {
    assert(index >=0 && index < k_maxNumberOfDisplayableScriptCells);
    return &m_scriptCells[index];
  } else {
    assert(type == AddScriptCellType && index == 0);
    return &m_addNewScriptCell;
  }
}

int MenuController::reusableCellCount(int type) {
  if (type == AddScriptCellType) {
    return 1;
  }
  assert(type == ScriptCellType);
  return k_maxNumberOfDisplayableScriptCells;
}

int MenuController::typeAtLocation(int i, int j) {
  assert(i==0);
  assert(j>=0 && j<numberOfRows());
  if (j == numberOfRows()-1) {
    return AddScriptCellType;
  } else {
    return ScriptCellType;
  }
}

void MenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index < m_scriptStore->numberOfScripts()) {
    EvenOddEditableTextCell * myCell =  static_cast<EvenOddEditableTextCell *>(cell);
    myCell->editableTextCell()->textField()->setText(m_scriptStore->scriptAtIndex(index).name());
    myCell->setEven(index%2 == 0);
  } else {
    assert(index == m_scriptStore->numberOfScripts());
    Shared::NewFunctionCell * myCell =  static_cast<Shared::NewFunctionCell *>(cell);
    myCell->setEven(index%2 == 0);
  }
}

bool MenuController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE
    || event == Ion::Events::Down || event == Ion::Events::Up;
}

bool MenuController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  return false;
}

bool MenuController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (m_scriptStore->renameScriptAtIndex(m_selectableTableView.selectedRow(), text)) {
    int currentRow = m_selectableTableView.selectedRow();
    if (event == Ion::Events::Down && currentRow < numberOfRows() - 1) {
      m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), currentRow + 1);
    } else if (event == Ion::Events::Up && currentRow > 0) {
      m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), currentRow - 1);
    }
    m_selectableTableView.selectedCell()->setHighlighted(true);
    app()->setFirstResponder(&m_selectableTableView);
    return true;
  } else {
    // TODO: add pop up to explain to the user that the name is too long.
    return false;
  }
}

bool MenuController::textFieldDidAbortEditing(TextField * textField, const char * text) {
  m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  app()->setFirstResponder(&m_selectableTableView);
  return true;
}

Toolbox * MenuController::toolboxForTextField(TextField * textFied) {
  return nullptr;
}

int MenuController::numberOfButtons(ButtonRowController::Position position) const {
  return 1;
}

Button * MenuController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  assert(index == 0);
  return const_cast<Button *>(&m_consoleButton);
}

StackViewController * MenuController::stackViewController() {
 return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

}
