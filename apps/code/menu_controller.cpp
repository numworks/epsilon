#include "menu_controller.h"
#include "../i18n.h"
#include "../apps_container.h"
#include <assert.h>
#include <escher/metric.h>
#include <ion/events.h>

namespace Code {

MenuController::MenuController(Responder * parentResponder, ScriptStore * scriptStore, ButtonRowController * footer) :
  ViewController(parentResponder),
  ButtonRowDelegate(nullptr, footer),
  m_scriptStore(scriptStore),
  m_addNewScriptCell(I18n::Message::AddScript),
  m_consoleButton(this, I18n::Message::Console, Invocation([](void * context, void * sender) {
    MenuController * menu = (MenuController *)context;
    if (menu->consoleController()->loadPythonEnvironment()) {
      menu->stackViewController()->push(menu->consoleController());
      return;
    }
    //TODO: Pop up warning message: not enough space to load Python
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

void MenuController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() < 0) {
    assert(footer()->selectedButton() == 0);
    app()->setFirstResponder(&m_consoleButton);
    return;
  }
  assert(m_selectableTableView.selectedRow() < numberOfRows());
  app()->setFirstResponder(&m_selectableTableView);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    footer()->setSelectedButton(0);
    return true;
  } else if (event == Ion::Events::Up) {
    if (footer()->selectedButton() == 0) {
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
  setParameteredScript();
  stackViewController()->push(&m_scriptParameterController);
}

void MenuController::setParameteredScript() {
  m_scriptParameterController.setScript(m_selectableTableView.selectedRow());
}

void MenuController::addScript() {
  if (m_scriptStore->addNewScript()) {
    m_selectableTableView.reloadData();
    renameScriptAtIndex(m_scriptStore->numberOfScripts()-1);
    return;
  }
  m_selectableTableView.reloadData();
}

void MenuController::renameScriptAtIndex(int i) {
  assert(i>=0 && i<m_scriptStore->numberOfScripts());
  static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::AlphaLock);
  EvenOddEditableTextCell * myCell = static_cast<EvenOddEditableTextCell *>(m_selectableTableView.selectedCell());
  app()->setFirstResponder(myCell);
  myCell->setHighlighted(false);
  const char * previousText = myCell->editableTextCell()->textField()->text();
  myCell->editableTextCell()->textField()->setEditing(true);
  myCell->editableTextCell()->textField()->setText(previousText);
  myCell->editableTextCell()->textField()->setCursorLocation(strlen(previousText) - strlen(ScriptStore::k_scriptExtension));
  }

void MenuController::deleteScriptAtIndex(int i) {
  m_scriptStore->deleteScriptAtIndex(i);
  m_selectableTableView.reloadData();
}

void MenuController::reloadConsole() {
  m_consoleController.unloadPythonEnvironment();
}

int MenuController::numberOfRows() {
  return m_scriptStore->numberOfScripts() + 1;
  //TODO do not add the addScript row if there can be no more scripts stored.
};

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
    EditableTextCell * editableTextCell = static_cast<EvenOddEditableTextCell *>(cell)->editableTextCell();
    editableTextCell->textField()->setText(m_scriptStore->scriptAtIndex(index).name());
  }
  static_cast<EvenOddCell *>(cell)->setEven(index%2 == 0);
  cell->setHighlighted(index == selectedRow());
}

bool MenuController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE
    || event == Ion::Events::Down || event == Ion::Events::Up;
}

bool MenuController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Right && textField->isEditing()) {
    int scriptExtensionLength = strlen(ScriptStore::k_scriptExtension);
    if (textField->cursorLocation() > textField->textLength() - scriptExtensionLength - 1) {
      return true;
    }
  }
  return false;
}

bool MenuController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  const char * newName;
  char numberedDefaultName[k_defaultScriptNameMaxSize];
  if (strlen(text) <= strlen(ScriptStore::k_scriptExtension)) {
    // The user entered an empty name. Use a numbered default script name.
    numberedDefaultScriptName(numberedDefaultName);
    newName = const_cast<const char *>(numberedDefaultName);
  } else {
    newName = text;
  }
  if (m_scriptStore->renameScriptAtIndex(m_selectableTableView.selectedRow(), newName)) {
    textField->setText(newName);
    int currentRow = m_selectableTableView.selectedRow();
    if (event == Ion::Events::Down && currentRow < numberOfRows() - 1) {
      m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), currentRow + 1);
    } else if (event == Ion::Events::Up && currentRow > 0) {
      m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), currentRow - 1);
    }
    m_selectableTableView.selectedCell()->setHighlighted(true);
    reloadConsole();
    app()->setFirstResponder(&m_selectableTableView);
    static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
    return true;
  }
  // TODO: add pop up to explain to the user that the name is too long.
  return false;
}

bool MenuController::textFieldDidAbortEditing(TextField * textField, const char * text) {
  if (strlen(text) <= strlen(ScriptStore::k_scriptExtension)) {
    // The previous text was an empty name. Use a numbered default script name.
    char numberedDefaultName[k_defaultScriptNameMaxSize];
    numberedDefaultScriptName(numberedDefaultName);
    m_scriptStore->renameScriptAtIndex(m_selectableTableView.selectedRow(), const_cast<const char *>(numberedDefaultName));
    m_selectableTableView.reloadData();
  }
  m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  app()->setFirstResponder(&m_selectableTableView);
  static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  return true;
}

void MenuController::numberedDefaultScriptName(char * buffer) {
  bool foundNewScriptNumber = false;
  int currentScriptNumber = 1;
  char newName[k_defaultScriptNameMaxSize];
  memcpy(newName, ScriptStore::k_defaultScriptName, strlen(ScriptStore::k_defaultScriptName)+1);
  // We will only name scripts from script1.py to script99.py.
  while (!foundNewScriptNumber && currentScriptNumber < 100) {
    // Change the number in the script name.
    intToText(currentScriptNumber, &newName[strlen(ScriptStore::k_defaultScriptName)-strlen(ScriptStore::k_scriptExtension)]);
    memcpy(&newName[strlen(newName)], ScriptStore::k_scriptExtension, strlen(ScriptStore::k_scriptExtension)+1);
    if (m_scriptStore->scriptNamed(const_cast<const char *>(newName)).isNull()) {
      foundNewScriptNumber = true;
    }
    currentScriptNumber++;
  }
  if (foundNewScriptNumber) {
    memcpy(buffer, newName, strlen(newName)+1);
    return;
  }
  memcpy(buffer, ScriptStore::k_defaultScriptName, strlen(ScriptStore::k_defaultScriptName)+1);
}

void MenuController::intToText(int i, char * buffer) {
  // We only support integers from 0 to 99
  // buffer should have the space for three chars.
  assert(i>=0);
  assert(i<100);
  if (i/10 == 0) {
    buffer[0] = i+'0';
    buffer[1] = 0;
    return;
  }
  buffer[0] = i/10+'0';
  buffer[1] = i-10*(i/10)+'0';
  buffer[2] = 0;
}

StackViewController * MenuController::stackViewController() {
 return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

}
