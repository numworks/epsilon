#include "menu_controller.h"
#include "../i18n.h"
#include "../apps_container.h"
#include <assert.h>
#include <escher/metric.h>
#include <ion/events.h>

namespace Code {

MenuController::MenuController(Responder * parentResponder, ScriptStore * scriptStore, ButtonRowController * footer
#if EPSILON_GETOPT
      , bool lockOnConsole
#endif
    ) :
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
  }, this), KDText::FontSize::Large),
  m_selectableTableView(this, this, 0, 1, 0, 0, 0, 0, this, this, false),
  m_consoleController(parentResponder, m_scriptStore
#if EPSILON_GETOPT
      , lockOnConsole
#endif
      ),
  m_scriptParameterController(nullptr, I18n::Message::ScriptOptions, this),
  m_editorController(this),
  m_reloadConsoleWhenBecomingFirstResponder(false),
  m_shouldDisplayAddScriptRow(true)
{
  for (int i = 0; i < k_maxNumberOfDisplayableScriptCells; i++) {
    m_scriptCells[i].setParentResponder(&m_selectableTableView);
    m_scriptCells[i].editableTextCell()->textField()->setDelegate(this);
    m_scriptCells[i].editableTextCell()->textField()->setDraftTextBuffer(m_draftTextBuffer);
    m_scriptCells[i].editableTextCell()->textField()->setAlignment(0.0f, 0.5f);
    m_scriptCells[i].editableTextCell()->setMargins(0, 0, 0, Metric::HistoryHorizontalMargin);
  }
}

StackViewController * MenuController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

void MenuController::willExitResponderChain(Responder * nextFirstResponder) {
  int selectedRow = m_selectableTableView.selectedRow();
  int selectedColumn = m_selectableTableView.selectedColumn();
  if (selectedRow >= 0 && selectedRow < m_scriptStore->numberOfScripts() && selectedColumn == 0) {
    TextField * tf = static_cast<EvenOddEditableTextCell *>(m_selectableTableView.selectedCell())->editableTextCell()->textField();
    if (tf->isEditing()) {
      tf->setEditing(false);
      textFieldDidAbortEditing(tf, tf->text());
    }
  }
}

void MenuController::didBecomeFirstResponder() {
  if (m_reloadConsoleWhenBecomingFirstResponder) {
    reloadConsole();
  }
  if (footer()->selectedButton() == 0) {
    assert(m_selectableTableView.selectedRow() < 0);
    app()->setFirstResponder(&m_consoleButton);
    return;
  }
  if (m_selectableTableView.selectedRow() < 0) {
    m_selectableTableView.selectCellAtLocation(0,0);
  }
  assert(m_selectableTableView.selectedRow() < m_scriptStore->numberOfScripts() + 1);
  app()->setFirstResponder(&m_selectableTableView);
#if EPSILON_GETOPT
  if (consoleController()->locked() && consoleController()->loadPythonEnvironment()) {
    stackViewController()->push(consoleController());
    return;
  }
#endif
}

void MenuController::viewWillAppear() {
  updateAddScriptRowDisplay();
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    footer()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Up) {
    if (footer()->selectedButton() == 0) {
      footer()->setSelectedButton(-1);
      m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
      app()->setFirstResponder(&m_selectableTableView);
      return true;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int selectedRow = m_selectableTableView.selectedRow();
    int selectedColumn = m_selectableTableView.selectedColumn();
    if (selectedRow >= 0 && selectedRow < m_scriptStore->numberOfScripts()) {
      if (selectedColumn == 1) {
        configureScript();
        return true;
      }
      assert(selectedColumn == 0);
      editScriptAtIndex(selectedRow);
      return true;
    } else if (m_shouldDisplayAddScriptRow
        && selectedColumn == 0
        && selectedRow == m_scriptStore->numberOfScripts())
    {
      addScript();
      return true;
    }
  }
  return false;
}

void MenuController::renameSelectedScript() {
  assert(m_selectableTableView.selectedRow() >= 0);
  assert(m_selectableTableView.selectedRow() < m_scriptStore->numberOfScripts());
  static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::AlphaLock);
  m_selectableTableView.selectCellAtLocation(0, (m_selectableTableView.selectedRow()));
  EvenOddEditableTextCell * myCell = static_cast<EvenOddEditableTextCell *>(m_selectableTableView.selectedCell());
  app()->setFirstResponder(myCell);
  myCell->setHighlighted(false);
  const char * previousText = myCell->editableTextCell()->textField()->text();
  myCell->editableTextCell()->textField()->setEditing(true);
  myCell->editableTextCell()->textField()->setText(previousText);
  myCell->editableTextCell()->textField()->setCursorLocation(strlen(previousText) - strlen(ScriptStore::k_scriptExtension));
}

void MenuController::deleteScript(Script script) {
  assert(!script.isNull());
  script.destroy();
  updateAddScriptRowDisplay();
}

void MenuController::reloadConsole() {
  m_consoleController.unloadPythonEnvironment();
  m_reloadConsoleWhenBecomingFirstResponder = false;
}

void MenuController::loadPythonIfNeeded() {
  m_consoleController.loadPythonEnvironment(false);
}

void MenuController::openConsoleWithScript(Script script) {
  reloadConsole();
  if (m_consoleController.loadPythonEnvironment(false)) {
    stackViewController()->push(&m_consoleController);
    m_consoleController.autoImportScript(script, true);
  }
  m_reloadConsoleWhenBecomingFirstResponder = true;
}

void MenuController::scriptContentEditionDidFinish(){
  reloadConsole();
}

int MenuController::numberOfRows() {
  return m_scriptStore->numberOfScripts() + m_shouldDisplayAddScriptRow;
}

void MenuController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (i == 0 && j < m_scriptStore->numberOfScripts()) {
    willDisplayScriptTitleCellForIndex(cell, j);
  }
  static_cast<EvenOddCell *>(cell)->setEven(j%2 == 0);
  cell->setHighlighted(i == selectedColumn() && j == selectedRow());
}

KDCoordinate MenuController::columnWidth(int i) {
  switch (i) {
    case 0:
      return m_selectableTableView.bounds().width()-k_parametersColumnWidth;
    case 1:
      return k_parametersColumnWidth;
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate MenuController::cumulatedWidthFromIndex(int i) {
  switch (i) {
    case 0:
      return 0;
    case 1:
      return m_selectableTableView.bounds().width()-k_parametersColumnWidth;
    case 2:
      return m_selectableTableView.bounds().width();
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate MenuController::cumulatedHeightFromIndex(int j) {
  return Metric::StoreRowHeight * j;
}

int MenuController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= m_selectableTableView.bounds().width()-k_parametersColumnWidth) {
    return 0;
  }
  if (offsetX <= m_selectableTableView.bounds().width()) {
    return 1;
  }
  else {
    return 2;
  }
  assert(false);
  return 0;
}

int MenuController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (Metric::StoreRowHeight == 0) {
    return 0;
  }
  return (offsetY - 1) / Metric::StoreRowHeight;
}

HighlightCell * MenuController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == ScriptCellType) {
    assert(index >=0 && index < k_maxNumberOfDisplayableScriptCells);
    return &m_scriptCells[index];
  }
  if (type == ScriptParameterCellType) {
    assert(index >=0 && index < k_maxNumberOfDisplayableScriptCells);
    return &m_scriptParameterCells[index];
  }
  if (type == AddScriptCellType) {
    assert(index == 0);
    return &m_addNewScriptCell;
  }
  if(type == EmptyCellType) {
    return &m_emptyCell;
  }
  assert(false);
  return nullptr;
}

int MenuController::reusableCellCount(int type) {
  if (type == AddScriptCellType) {
    return 1;
  }
  if (type == ScriptCellType || type == ScriptParameterCellType) {
    return k_maxNumberOfDisplayableScriptCells;
  }
  if (type == EmptyCellType) {
    return 1;
  }
  assert(false);
  return 0;
}

int MenuController::typeAtLocation(int i, int j) {
  assert(i >= 0 && i < numberOfColumns());
  assert(j >= 0 && j < numberOfRows());
  if (i == 0) {
    if (j == numberOfRows()-1 && m_shouldDisplayAddScriptRow) {
      return AddScriptCellType;
    }
    return ScriptCellType;
  }
  assert(i == 1);
  if (j == numberOfRows()-1 && m_shouldDisplayAddScriptRow) {
    return EmptyCellType;
  }
  return ScriptParameterCellType;
}

void MenuController::willDisplayScriptTitleCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index < m_scriptStore->numberOfScripts());
  EditableTextCell * editableTextCell = static_cast<EvenOddEditableTextCell *>(cell)->editableTextCell();
  editableTextCell->textField()->setText(m_scriptStore->scriptAtIndex(index).name());
}

void MenuController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (selectedRow() == numberOfRows() - 1 && selectedColumn() == 1 && m_shouldDisplayAddScriptRow) {
    t->selectCellAtLocation(0, numberOfRows()-1);
  }
}

bool MenuController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE
    || event == Ion::Events::Down || event == Ion::Events::Up;
}

bool MenuController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Right && textField->isEditing() && textField->cursorLocation() == textField->draftTextLength()) {
    return true;
  }
  if (event == Ion::Events::Clear && textField->isEditing()) {
    textField->setText(ScriptStore::k_scriptExtension);
    textField->setCursorLocation(0);
    return true;
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
  Script::ErrorStatus error = m_scriptStore->scriptAtIndex(m_selectableTableView.selectedRow()).setName(newName);
  if (error == Script::ErrorStatus::None) {
    updateAddScriptRowDisplay();
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
  } else if (error == Script::ErrorStatus::NameTaken) {
    app()->displayWarning(I18n::Message::NameTaken);
  } else if (error == Script::ErrorStatus::NonCompliantName) {
    app()->displayWarning(I18n::Message::NonCompliantName);
  } else {
    assert(error == Script::ErrorStatus::NotEnoughSpaceAvailable);
    app()->displayWarning(I18n::Message::NameTooLong);
  }
  return false;
}

bool MenuController::textFieldDidAbortEditing(TextField * textField, const char * text) {
  if (strlen(text) <= strlen(ScriptStore::k_scriptExtension)) {
    // The previous text was an empty name. Use a numbered default script name.
    char numberedDefaultName[k_defaultScriptNameMaxSize];
    numberedDefaultScriptName(numberedDefaultName);
    Script::ErrorStatus error = m_scriptStore->scriptAtIndex(m_selectableTableView.selectedRow()).setName(numberedDefaultName);
    if (error != Script::ErrorStatus::None) {
      assert(false);
      /* Because we use the numbered default name, the name should not be
       * already taken. Plus, the script could be added only if the storage has
       * enough available space to add a script named 'script99.py' */
    }
    assert(error == Script::ErrorStatus::None);
    updateAddScriptRowDisplay();
  }
  m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  app()->setFirstResponder(&m_selectableTableView);
  static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  return true;
}

bool MenuController::textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) {
  int scriptExtensionLength = strlen(ScriptStore::k_scriptExtension);
  if (textField->isEditing() && textField->cursorLocation() > textField->draftTextLength() - scriptExtensionLength) {
    textField->setCursorLocation(textField->draftTextLength() - scriptExtensionLength);
  }
  return returnValue;
}

void MenuController::addScript() {
  Script::ErrorStatus error = m_scriptStore->addNewScript();
  if (error == Script::ErrorStatus::None) {
    updateAddScriptRowDisplay();
    renameSelectedScript();
    return;
  }
  assert(false); // Adding a new script is called when !m_scriptStore.isFull() which guarantees that the available space in the storage is big enough
}

void MenuController::configureScript() {
  assert(m_selectableTableView.selectedRow() >= 0);
  assert(m_selectableTableView.selectedRow() < m_scriptStore->numberOfScripts());
  m_scriptParameterController.setScript(m_scriptStore->scriptAtIndex(m_selectableTableView.selectedRow()));
  stackViewController()->push(&m_scriptParameterController);
}

void MenuController::editScriptAtIndex(int scriptIndex) {
  assert(scriptIndex >=0 && scriptIndex < m_scriptStore->numberOfScripts());
  Script script = m_scriptStore->scriptAtIndex(scriptIndex);
  m_editorController.setScript(script);
  stackViewController()->push(&m_editorController);
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

void MenuController::updateAddScriptRowDisplay() {
  m_shouldDisplayAddScriptRow = !m_scriptStore->isFull();
  m_selectableTableView.reloadData();
}

}
