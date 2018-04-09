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

ConsoleController::ConsoleController(Responder * parentResponder, ScriptStore * scriptStore
#if EPSILON_GETOPT
      , bool lockOnConsole
#endif
    ) :
  ViewController(parentResponder),
  SelectableTableViewDataSource(),
  TextFieldDelegate(),
  MicroPython::ExecutionEnvironment(),
  m_rowHeight(KDText::charSize(k_fontSize).height()),
  m_importScriptsWhenViewAppears(false),
  m_selectableTableView(this, this, 0, 1, 0, Metric::CommonRightMargin, 0, Metric::TitleBarExternHorizontalMargin, this, this, true, true, KDColorWhite),
  m_editCell(this, this),
  m_pythonHeap(nullptr),
  m_scriptStore(scriptStore),
  m_sandboxController(this),
  m_inputRunLoopActive(false)
#if EPSILON_GETOPT
      , m_locked(lockOnConsole)
#endif
{
  m_editCell.setPrompt(sStandardPromptText);
  for (int i = 0; i < k_numberOfLineCells; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
  }
}

ConsoleController::~ConsoleController() {
  unloadPythonEnvironment();
}

bool ConsoleController::loadPythonEnvironment(bool autoImportScripts) {
  if(pythonEnvironmentIsLoaded()) {
    return true;
  }
  emptyOutputAccumulationBuffer();
  m_pythonHeap = (char *)malloc(k_pythonHeapSize);
  if (m_pythonHeap == nullptr) {
    // In DEBUG mode, the assert at the end of malloc would have already failed
    // and the program crashed.
    return false;
  }
  MicroPython::init(m_pythonHeap, m_pythonHeap + k_pythonHeapSize);
  MicroPython::registerScriptProvider(m_scriptStore);
  m_importScriptsWhenViewAppears = autoImportScripts;
  return true;
}

void ConsoleController::unloadPythonEnvironment() {
  if (pythonEnvironmentIsLoaded()) {
    m_consoleStore.startNewSession();
    MicroPython::deinit();
    free(m_pythonHeap);
    m_pythonHeap = nullptr;
  }
}

bool ConsoleController::pythonEnvironmentIsLoaded() {
  return (m_pythonHeap != nullptr);
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
  assert(pythonEnvironmentIsLoaded());
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
  if ((event == Ion::Events::Home || event == Ion::Events::Up) && inputRunLoopActive()) {
    askInputRunLoopTermination();
    // We need to return true here because we want to actually exit from the
    // input run loop, which requires ending a dispatchEvent cycle.
    return true;
  }
  if (event == Ion::Events::Up) {
    if (m_consoleStore.numberOfLines() > 0 && m_selectableTableView.selectedRow() == m_consoleStore.numberOfLines()) {
      m_editCell.setEditing(false);
      m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines()-1);
      return true;
    }
  } else if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (m_consoleStore.numberOfLines() > 0 && m_selectableTableView.selectedRow() < m_consoleStore.numberOfLines()) {
      const char * text = m_consoleStore.lineAtIndex(m_selectableTableView.selectedRow()).text();
      m_editCell.setEditing(true);
      m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
      app()->setFirstResponder(&m_editCell);
      return m_editCell.insertText(text);
    }
  } else if (event == Ion::Events::Copy) {
    return copyCurrentLineToClipboard();
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
      previousCell->reloadCell();
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
  if (event == Ion::Events::Var) {
    if (!textField->isEditing()) {
      textField->setEditing(true);
    }
  }
  return static_cast<App *>(textField->app())->textInputDidReceiveEvent(textField, event);
}

bool ConsoleController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (inputRunLoopActive()) {
    askInputRunLoopTermination();
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

bool ConsoleController::textFieldDidAbortEditing(TextField * textField, const char * text) {
  if (inputRunLoopActive()) {
    askInputRunLoopTermination();
  } else {
#if EPSILON_GETOPT
    if (!m_locked) {
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

Toolbox * ConsoleController::toolboxForTextInput(TextInput * textInput) {
  Code::App * codeApp = static_cast<Code::App *>(app());
  return codeApp->pythonToolbox();
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
    // Create the command "from scriptName import *".
    char command[k_maxImportCommandSize];
    size_t currentChar = strlcpy(command, k_importCommand1, strlen(k_importCommand1)+1);
    const char * scriptName = script.name();
    currentChar += strlcpy(command+currentChar, scriptName, strlen(scriptName)+1);
    // Remove the name extension ".py"
    currentChar -= strlen(ScriptStore::k_scriptExtension);
    currentChar += strlcpy(command+currentChar, k_importCommand2, strlen(k_importCommand2)+1);
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

bool ConsoleController::copyCurrentLineToClipboard() {
  int row = m_selectableTableView.selectedRow();
  if (row < m_consoleStore.numberOfLines()) {
    Clipboard::sharedClipboard()->store(m_consoleStore.lineAtIndex(row).text());
    return true;
  }
  return false;
}

}
