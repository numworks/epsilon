#include "console_controller.h"
#include "app.h"
#include "script.h"
#include "variable_box_controller.h"
#include <apps/i18n.h>
#include <assert.h>
#include <escher/metric.h>
#include "../apps_container.h"

extern "C" {
#include <stdlib.h>
}

namespace Code {

static const char * sStandardPromptText = ">>> ";

ConsoleController::ConsoleController(Responder * parentResponder, App * pythonDelegate, ScriptStore * scriptStore
#if EPSILON_GETOPT
      , bool lockOnConsole
#endif
    ) :
  ViewController(parentResponder),
  SelectableTableViewDataSource(),
  TextFieldDelegate(),
  MicroPython::ExecutionEnvironment(),
  m_pythonDelegate(pythonDelegate),
  m_rowHeight(k_font->glyphSize().height()),
  m_importScriptsWhenViewAppears(false),
  m_selectableTableView(this, this, this, this),
  m_editCell(this, pythonDelegate, this),
  m_scriptStore(scriptStore),
  m_sandboxController(this),
  m_inputRunLoopActive(false)
#if EPSILON_GETOPT
      , m_locked(lockOnConsole)
#endif
{
  m_selectableTableView.setMargins(0, Metric::CommonRightMargin, 0, Metric::TitleBarExternHorizontalMargin);
  m_selectableTableView.setBackgroundColor(KDColorWhite);
  m_editCell.setPrompt(sStandardPromptText);
  for (int i = 0; i < k_numberOfLineCells; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
  }
}

bool ConsoleController::loadPythonEnvironment() {
  if (m_pythonDelegate->isPythonUser(this)) {
    return true;
  }
  emptyOutputAccumulationBuffer();
  m_pythonDelegate->initPythonWithUser(this);
  MicroPython::registerScriptProvider(m_scriptStore);
  m_importScriptsWhenViewAppears = m_autoImportScripts;
  /* We load functions and variables names in the variable box before running
   * any other python code to avoid failling to load functions and variables
   * due to memory exhaustion. */
  static_cast<App *>(app())->variableBoxController()->loadFunctionsAndVariables();
  return true;
}

void ConsoleController::unloadPythonEnvironment() {
  if (!m_pythonDelegate->isPythonUser(nullptr)) {
    m_consoleStore.startNewSession();
    m_pythonDelegate->deinitPython();
  }
}

void ConsoleController::autoImport() {
  for (int i = 0; i < m_scriptStore->numberOfScripts(); i++) {
    autoImportScript(m_scriptStore->scriptAtIndex(i));
  }
}

void ConsoleController::runAndPrintForCommand(const char * command) {
  m_consoleStore.pushCommand(command, strlen(command));
  assert(m_outputAccumulationBuffer[0] == '\0');

  runCode(command);
  flushOutputAccumulationBufferToStore();
  m_consoleStore.deleteLastLineIfEmpty();
}

void ConsoleController::terminateInputLoop() {
  assert(m_inputRunLoopActive);
  m_inputRunLoopActive = false;
  interrupt();
}

const char * ConsoleController::inputText(const char * prompt) {
  AppsContainer * a = (AppsContainer *)(app()->container());
  m_inputRunLoopActive = true;

  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
  m_editCell.setPrompt(prompt);
  m_editCell.setText("");

  a->redrawWindow();
  a->runWhile([](void * a){
      ConsoleController * c = static_cast<ConsoleController *>(a);
      return c->inputRunLoopActive();
  }, this);

  flushOutputAccumulationBufferToStore();
  m_consoleStore.deleteLastLineIfEmpty();
  m_editCell.setPrompt(sStandardPromptText);

  return m_editCell.text();
}

void ConsoleController::viewWillAppear() {
  loadPythonEnvironment();
  m_sandboxIsDisplayed = false;
  if (m_importScriptsWhenViewAppears) {
    m_importScriptsWhenViewAppears = false;
    autoImport();
  }
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
  m_editCell.setEditing(true);
  m_editCell.setText("");
}

void ConsoleController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_editCell);
}

bool ConsoleController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (m_consoleStore.numberOfLines() > 0 && m_selectableTableView.selectedRow() < m_consoleStore.numberOfLines()) {
      const char * text = m_consoleStore.lineAtIndex(m_selectableTableView.selectedRow()).text();
      m_editCell.setEditing(true);
      m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
      app()->setFirstResponder(&m_editCell);
      return m_editCell.insertText(text);
    }
  } else if (event == Ion::Events::Clear) {
    m_selectableTableView.deselectTable();
    m_consoleStore.clear();
    m_selectableTableView.reloadData();
    m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
    return true;
  } else if (event == Ion::Events::Backspace) {
    int selectedRow = m_selectableTableView.selectedRow();
    assert(selectedRow >= 0 && selectedRow < m_consoleStore.numberOfLines());
    m_selectableTableView.deselectTable();
    int firstDeletedLineIndex = m_consoleStore.deleteCommandAndResultsAtIndex(selectedRow);
    m_selectableTableView.reloadData();
    m_selectableTableView.selectCellAtLocation(0, firstDeletedLineIndex);
    return true;
  }
#if EPSILON_GETOPT
  if (m_locked && (event == Ion::Events::Home || event == Ion::Events::Back)) {
    if (m_inputRunLoopActive) {
      terminateInputLoop();
    }
    return true;
  }
#endif
  return false;
}

int ConsoleController::numberOfRows() {
  return m_consoleStore.numberOfLines()+1;
}

KDCoordinate ConsoleController::rowHeight(int j) {
  return m_rowHeight;
}

KDCoordinate ConsoleController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int ConsoleController::indexFromCumulatedHeight(KDCoordinate offsetY ){
  return offsetY/rowHeight(0);
}

HighlightCell * ConsoleController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == LineCellType) {
    assert(index < k_numberOfLineCells);
    return m_cells+index;
  } else {
    assert(type == EditCellType);
    assert(index == 0);
    return &m_editCell;
  }
}

int ConsoleController::reusableCellCount(int type) {
  if (type == LineCellType) {
    return k_numberOfLineCells;
  } else {
    return 1;
  }
}

int ConsoleController::typeAtLocation(int i, int j) {
  assert(i == 0);
  assert(j >= 0);
  if (j < m_consoleStore.numberOfLines()) {
    return LineCellType;
  } else {
    assert(j == m_consoleStore.numberOfLines());
    return EditCellType;
  }
}

void ConsoleController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  assert(i == 0);
  if (j < m_consoleStore.numberOfLines()) {
    static_cast<ConsoleLineCell *>(cell)->setLine(m_consoleStore.lineAtIndex(j));
  }
}

void ConsoleController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (t->selectedRow() == m_consoleStore.numberOfLines()) {
    m_editCell.setEditing(true);
    return;
  }
  if (t->selectedRow()>-1) {
    if (previousSelectedCellY > -1 && previousSelectedCellY < m_consoleStore.numberOfLines()) {
      // Reset the scroll of the previous cell
      ConsoleLineCell * previousCell = (ConsoleLineCell *)(t->cellAtLocation(previousSelectedCellX, previousSelectedCellY));
      if (previousCell) {
        previousCell->reloadCell();
      }
    }
    ConsoleLineCell * selectedCell = (ConsoleLineCell *)(t->selectedCell());
    selectedCell->reloadCell();
  }
}

bool ConsoleController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  assert(textField->isEditing());
  return (textField->draftTextLength() > 0
      && (event == Ion::Events::OK || event == Ion::Events::EXE));
}

bool ConsoleController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Up && m_inputRunLoopActive) {
    m_inputRunLoopActive = false;
    /* We need to return true here because we want to actually exit from the
     * input run loop, which requires ending a dispatchEvent cycle. */
    return true;
  }
  if (event == Ion::Events::Up) {
    if (m_consoleStore.numberOfLines() > 0 && m_selectableTableView.selectedRow() == m_consoleStore.numberOfLines()) {
      m_editCell.setEditing(false);
      m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines()-1);
      return true;
    }
  }
  return static_cast<App *>(textField->app())->textInputDidReceiveEvent(textField, event);
}

bool ConsoleController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (m_inputRunLoopActive) {
    m_inputRunLoopActive = false;
    return false;
  }
  runAndPrintForCommand(text);
  if (m_sandboxIsDisplayed) {
    return true;
  }
  m_selectableTableView.reloadData();
  m_editCell.setEditing(true);
  textField->setText("");
  m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
  return true;
}

bool ConsoleController::textFieldDidAbortEditing(TextField * textField) {
  if (m_inputRunLoopActive) {
    m_inputRunLoopActive = false;
  } else {
#if EPSILON_GETOPT
    /* In order to lock the console controller, we disable poping controllers
     * below the console controller included. The stack should only hold:
     * - the menu controller
     * - the console controller
     * The depth of the stack controller must always be above or equal to 2. */
    if (!m_locked || stackViewController()->depth() > 2) {
#endif
      stackViewController()->pop();
#if EPSILON_GETOPT
    } else {
      textField->setEditing(true);
    }
#endif
  }
  return true;
}

void ConsoleController::displaySandbox() {
  if (m_sandboxIsDisplayed) {
    return;
  }
  m_sandboxIsDisplayed = true;
  stackViewController()->push(&m_sandboxController);
}

/* printText is called by the Python machine.
 * The text argument is not always null-terminated. */
void ConsoleController::printText(const char * text, size_t length) {
  size_t textCutIndex = firstNewLineCharIndex(text, length);
  // If there is no new line in text, just append it to the output accumulation
  // buffer.
  if (textCutIndex >= length) {
    appendTextToOutputAccumulationBuffer(text, length);
    return;
  }
  // If there is a new line in the middle of the text, we have to store at least
  // two new console lines in the console store.
  if (textCutIndex < length - 1) {
    printText(text, textCutIndex + 1);
    printText(&text[textCutIndex+1], length - (textCutIndex + 1));
    return;
  }
  // If there is a new line at the end of the text, we have to store the line in
  // the console store.
  if (textCutIndex == length - 1) {
    appendTextToOutputAccumulationBuffer(text, length-1);
    flushOutputAccumulationBufferToStore();
  }
}

void ConsoleController::autoImportScript(Script script, bool force) {
  if (script.importationStatus() || force) {
    // Step 1 - Create the command "from scriptName import *".

    assert(strlen(k_importCommand1) + strlen(script.fullName()) - strlen(ScriptStore::k_scriptExtension) - 1 + strlen(k_importCommand2) + 1 <= k_maxImportCommandSize);
    char command[k_maxImportCommandSize];

    // Copy "from "
    size_t currentChar = strlcpy(command, k_importCommand1, k_maxImportCommandSize);
    const char * scriptName = script.fullName();

    /* Copy the script name without the extension ".py". The '.' is overwritten
     * by the null terminating char. */
    int copySizeWithNullTerminatingZero = min(k_maxImportCommandSize - currentChar, strlen(scriptName) - strlen(ScriptStore::k_scriptExtension));
    strlcpy(command+currentChar, scriptName, copySizeWithNullTerminatingZero);
    currentChar += copySizeWithNullTerminatingZero-1;

    // Copy " import *"
    strlcpy(command+currentChar, k_importCommand2, k_maxImportCommandSize - currentChar);

    // Step 2 - Run the command
    runAndPrintForCommand(command);
  }
  if (force) {
    m_selectableTableView.reloadData();
    m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
    m_editCell.setEditing(true);
    m_editCell.setText("");
  }
}

void ConsoleController::flushOutputAccumulationBufferToStore() {
  m_consoleStore.pushResult(m_outputAccumulationBuffer, strlen(m_outputAccumulationBuffer));
  emptyOutputAccumulationBuffer();
}

void ConsoleController::appendTextToOutputAccumulationBuffer(const char * text, size_t length) {
  int endOfAccumulatedText = strlen(m_outputAccumulationBuffer);
  int spaceLeft = k_outputAccumulationBufferSize - endOfAccumulatedText;
  if (spaceLeft > (int)length) {
    memcpy(&m_outputAccumulationBuffer[endOfAccumulatedText], text, length);
    return;
  }
  memcpy(&m_outputAccumulationBuffer[endOfAccumulatedText], text, spaceLeft-1);
  flushOutputAccumulationBufferToStore();
  appendTextToOutputAccumulationBuffer(&text[spaceLeft-1], length - (spaceLeft - 1));
}

void ConsoleController::emptyOutputAccumulationBuffer() {
  for (int i = 0; i < k_outputAccumulationBufferSize; i++) {
    m_outputAccumulationBuffer[i] = 0;
  }
}

size_t ConsoleController::firstNewLineCharIndex(const char * text, size_t length) {
  size_t index = 0;
  while (index < length) {
    if (text[index] == '\n') {
      return index;
    }
    index++;
  }
  return index;
}

StackViewController * ConsoleController::stackViewController() {
 return static_cast<StackViewController *>(parentResponder());
}

}
