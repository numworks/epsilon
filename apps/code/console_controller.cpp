#include "console_controller.h"
#include "app.h"
#include "script.h"
#include "variable_box_controller.h"
#include <apps/i18n.h>
#include <algorithm>
#include <assert.h>
#include <ion/storage.h>
#include <escher/metric.h>
#include <apps/global_preferences.h>
#include <apps/apps_container.h>
#include <python/port/helpers.h>

extern "C" {
#include <stdlib.h>
}

using namespace Escher;

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
  m_importScriptsWhenViewAppears(false),
  m_selectableTableView(this, this, this, this),
  m_editCell(this, this, this),
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
  if (!m_pythonDelegate->isPythonUser(this)) {
    m_scriptStore->clearConsoleFetchInformation();
    emptyOutputAccumulationBuffer();
    m_pythonDelegate->initPythonWithUser(this);
    MicroPython::registerScriptProvider(m_scriptStore);
    m_importScriptsWhenViewAppears = m_autoImportScripts;
  }
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
  const char * storedCommand = m_consoleStore.pushCommand(command);
  assert(m_outputAccumulationBuffer[0] == '\0');

  // Draw the console before running the code
  m_editCell.setText("");
  m_editCell.setPrompt("");
  refreshPrintOutput();

  runCode(storedCommand);

  m_editCell.setPrompt(sStandardPromptText);
  m_editCell.setEditing(true);

  flushOutputAccumulationBufferToStore();
  m_consoleStore.deleteLastLineIfEmpty();
}

void ConsoleController::terminateInputLoop() {
  assert(m_inputRunLoopActive);
  m_inputRunLoopActive = false;
  interrupt();
}

const char * ConsoleController::inputText(const char * prompt) {
  AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
  m_inputRunLoopActive = true;

  // Hide the sandbox if it is displayed
  hideAnyDisplayedViewController();

  const char * promptText = prompt;
  char * s = const_cast<char *>(prompt);

  if (promptText != nullptr) {
    /* Set the prompt text. If the prompt text has a '\n', put the prompt text in
     * the history until the last '\n', and put the remaining prompt text in the
     * edit cell's prompt. */
    char * lastCarriageReturn = nullptr;
    while (*s != 0) {
      if (*s == '\n') {
        lastCarriageReturn = s;
      }
      s++;
    }
    if (lastCarriageReturn != nullptr) {
      printText(prompt, lastCarriageReturn-prompt+1);
      promptText = lastCarriageReturn+1;
    }
  }

  const char * previousPrompt = m_editCell.promptText();
  m_editCell.setPrompt(promptText);

  /* The user will input some text that is stored in the edit cell. When the
   * input is finished, we want to clear that cell and return the input text.
   * We choose to shift the input in the edit cell and put a null char in first
   * position, so that the cell seems cleared but we can still use it to store
   * the input.
   * To do so, we need to reduce the cell buffer size by one, so that the input
   * can be shifted afterwards, even if it has maxSize.
   *
   * Illustration of a input sequence:
   * | | | | | | | | |  <- the edit cell buffer
   * |0| | | | | | |X|  <- clear and reduce the size
   * |a|0| | | | | |X|  <- user input
   * |a|b|0| | | | |X|  <- user input
   * |a|b|c|0| | | |X|  <- user input
   * |a|b|c|d|0| | |X|  <- last user input
   * | |a|b|c|d|0| | |  <- increase the buffer size and shift the user input by one
   * |0|a|b|c|d|0| | |  <- put a zero in first position: the edit cell seems empty
   */

   m_editCell.clearAndReduceSize();

  // Reload the history
  reloadData(true);
  appsContainer->redrawWindow();

  // Launch a new input loop
  appsContainer->runWhile([](void * a){
      ConsoleController * c = static_cast<ConsoleController *>(a);
      return c->inputRunLoopActive();
  }, this);

  // Print the prompt and the input text
  if (promptText != nullptr) {
    printText(promptText, s - promptText);
  }
  const char * text = m_editCell.text();
  size_t textSize = strlen(text);
  printText(text, textSize);
  flushOutputAccumulationBufferToStore();

  // Clear the edit cell and return the input
  text = m_editCell.shiftCurrentTextAndClear();
  m_editCell.setPrompt(previousPrompt);
  refreshPrintOutput();

  return text;
}

void ConsoleController::viewWillAppear() {
  ViewController::viewWillAppear();
  loadPythonEnvironment();
  if (m_importScriptsWhenViewAppears) {
    m_importScriptsWhenViewAppears = false;
    autoImport();
  }

  reloadData(true);
}

void ConsoleController::didBecomeFirstResponder() {
  if (!isDisplayingViewController()) {
    Container::activeApp()->setFirstResponder(&m_editCell);
  } else {
    /* A view controller might be displayed: for example, when pushing the
     * console on the stack controller, we auto-import scripts during the
     * 'viewWillAppear' and then we set the console as first responder. The
     * sandbox or the matplotlib controller might have been pushed in the
     * auto-import. */
    Container::activeApp()->setFirstResponder(stackViewController()->topViewController());
  }
}

bool ConsoleController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (m_consoleStore.numberOfLines() > 0 && m_selectableTableView.selectedRow() < m_consoleStore.numberOfLines()) {
      const char * text = m_consoleStore.lineAtIndex(m_selectableTableView.selectedRow()).text();
      m_editCell.setEditing(true);
      m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
      Container::activeApp()->setFirstResponder(&m_editCell);
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
  if (m_locked && (event == Ion::Events::USBEnumeration || event == Ion::Events::Home || event == Ion::Events::Back)) {
    if (m_inputRunLoopActive) {
      terminateInputLoop();
    }
    return true;
  }
#endif
  return false;
}

int ConsoleController::numberOfRows() const {
  return m_consoleStore.numberOfLines()+1;
}

KDCoordinate ConsoleController::rowHeight(int j) {
  return GlobalPreferences::sharedGlobalPreferences()->font()->glyphSize().height();
}

KDCoordinate ConsoleController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int ConsoleController::indexFromCumulatedHeight(KDCoordinate offsetY ){
  return offsetY/rowHeight(0);
}

HighlightCell * ConsoleController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_lineCellType) {
    assert(index < k_numberOfLineCells);
    return m_cells+index;
  } else {
    assert(type == k_editCellType);
    assert(index == 0);
    return &m_editCell;
  }
}

int ConsoleController::reusableCellCount(int type) {
  if (type == k_lineCellType) {
    return k_numberOfLineCells;
  } else {
    return 1;
  }
}

int ConsoleController::typeAtIndex(int index) {
  assert(index >= 0);
  if (index < m_consoleStore.numberOfLines()) {
    return k_lineCellType;
  } else {
    assert(index == m_consoleStore.numberOfLines());
    return k_editCellType;
  }
}

void ConsoleController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  assert(i == 0);
  if (j < m_consoleStore.numberOfLines()) {
    static_cast<ConsoleLineCell *>(cell)->setLine(m_consoleStore.lineAtIndex(j));
  }
}

void ConsoleController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
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
    if (selectedCell) {
      selectedCell->reloadCell();
    }
  }
}

bool ConsoleController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  assert(textField->isEditing());
  return (textField->draftTextLength() > 0
      && (event == Ion::Events::OK || event == Ion::Events::EXE));
}

bool ConsoleController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (m_inputRunLoopActive
      && (event == Ion::Events::Up
        || event == Ion::Events::OK
        || event == Ion::Events::EXE))
  {
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
  return App::app()->textInputDidReceiveEvent(textField, event);
}

bool ConsoleController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (m_inputRunLoopActive) {
    m_inputRunLoopActive = false;
    return false;
  }
  telemetryReportEvent("Console", text);
  runAndPrintForCommand(text);
  if (!isDisplayingViewController()) {
    reloadData(true);
  }
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

VariableBoxController * ConsoleController::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  VariableBoxController * varBox = App::app()->variableBoxController();
  // Subtitle display status must be set before as it alter loaded node order
  varBox->setDisplaySubtitles(false);
  varBox->loadVariablesImportedFromScripts();
  varBox->setTitle(I18n::Message::FunctionsAndVariables);
  return varBox;
}

void ConsoleController::resetSandbox() {
  if (stackViewController()->topViewController() != sandbox()) {
    return;
  }
  m_sandboxController.reset();
}

void ConsoleController::displayViewController(ViewController * controller) {
  if (stackViewController()->topViewController() == controller) {
    return;
  }
  hideAnyDisplayedViewController();
  stackViewController()->push(controller);
}

void ConsoleController::hideAnyDisplayedViewController() {
  if (!isDisplayingViewController()) {
    return;
  }
  stackViewController()->pop();
}

bool ConsoleController::isDisplayingViewController() {
  /* The StackViewController model state is the best way to know wether the
   * console is displaying a View Controller (Sandbox or Matplotlib). Indeed,
   * keeping a boolean or a pointer raises the issue of when updating it - when
   * 'viewWillAppear' or when 'didEnterResponderChain' - in both cases, the
   * state would be wrong at some point... */
  return stackViewController()->depth() > 2;
}

void ConsoleController::refreshPrintOutput() {
  if (!isDisplayingViewController()) {
    reloadData(false);
    AppsContainer::sharedAppsContainer()->redrawWindow();
  }
}

void ConsoleController::reloadData(bool isEditing) {
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, m_consoleStore.numberOfLines());
  if (isEditing) {
    m_editCell.setEditing(true);
    m_editCell.setText("");
  } else {
    m_editCell.setEditing(false);
  }
}

/* printText is called by the Python machine.
 * The text argument is not always null-terminated. */
void ConsoleController::printText(const char * text, size_t length) {
  size_t textCutIndex = firstNewLineCharIndex(text, length);
  if (textCutIndex >= length) {
    /* If there is no new line in text, just append it to the output
     * accumulation buffer. */
    appendTextToOutputAccumulationBuffer(text, length);
  } else {
    if (textCutIndex < length - 1) {
      /* If there is a new line in the middle of the text, we have to store at
       * least two new console lines in the console store. */
      printText(text, textCutIndex + 1);
      printText(&text[textCutIndex+1], length - (textCutIndex + 1));
      return;
    }
    /* There is a new line at the end of the text, we have to store the line in
     * the console store. */
    assert(textCutIndex == length - 1);
    appendTextToOutputAccumulationBuffer(text, length-1);
    flushOutputAccumulationBufferToStore();
    micropython_port_vm_hook_refresh_print();
  }
}

void ConsoleController::autoImportScript(Script script, bool force) {
  /* The sandbox might be displayed, for instance if we are auto-importing
   * several scripts that draw at importation. In this case, we want to remove
   * the sandbox. */
  hideAnyDisplayedViewController();

  if (script.autoImportation() || force) {
    // Step 1 - Create the command "from scriptName import *".

    assert(strlen(k_importCommand1) + script.name().baseNameLength + strlen(k_importCommand2) + 1 <= k_maxImportCommandSize);
    char command[k_maxImportCommandSize];

    // Copy "from "
    size_t currentChar = strlcpy(command, k_importCommand1, k_maxImportCommandSize);
    Ion::Storage::Record::Name scriptName = script.name();

    /* Copy the script name without the extension ".py". The '.' is overwritten
     * by the null terminating char. */
    int copySizeWithNullTerminatingZero = std::min(k_maxImportCommandSize - currentChar, scriptName.baseNameLength + 1);
    assert(copySizeWithNullTerminatingZero >= 0);
    assert(copySizeWithNullTerminatingZero <= static_cast<int>(k_maxImportCommandSize - currentChar));
    strlcpy(command+currentChar, scriptName.baseName, copySizeWithNullTerminatingZero);
    currentChar += copySizeWithNullTerminatingZero - 1;

    // Copy " import *"
    assert(k_maxImportCommandSize >= currentChar);
    strlcpy(command+currentChar, k_importCommand2, k_maxImportCommandSize - currentChar);

    // Step 2 - Run the command
    runAndPrintForCommand(command);
  }
  if (!isDisplayingViewController() && force) {
    reloadData(true);
  }
}

void ConsoleController::flushOutputAccumulationBufferToStore() {
  m_consoleStore.pushResult(m_outputAccumulationBuffer);
  emptyOutputAccumulationBuffer();
}

void ConsoleController::appendTextToOutputAccumulationBuffer(const char * text, size_t length) {
  int endOfAccumulatedText = strlen(m_outputAccumulationBuffer);
  int spaceLeft = k_outputAccumulationBufferSize - endOfAccumulatedText;
  if (spaceLeft > (int)length) {
    memcpy(&m_outputAccumulationBuffer[endOfAccumulatedText], text, length);
    return;
  }
  /* The text to append is too long for the buffer. We need to split it in
   * chunks. We take special care not to break in the middle of code points! */
  int maxAppendedTextLength = spaceLeft-1; // we keep the last char to null-terminate the buffer
  int appendedTextLength = 0;
  UTF8Decoder decoder(text);
  while (decoder.stringPosition() - text <= maxAppendedTextLength) {
    appendedTextLength = decoder.stringPosition() - text;
    decoder.nextCodePoint();
  }
  memcpy(&m_outputAccumulationBuffer[endOfAccumulatedText], text, appendedTextLength);
  // The last char of m_outputAccumulationBuffer is kept to 0 to ensure a null-terminated text.
  assert(endOfAccumulatedText+appendedTextLength < k_outputAccumulationBufferSize);
  m_outputAccumulationBuffer[endOfAccumulatedText+appendedTextLength] = 0;
  flushOutputAccumulationBufferToStore();
  appendTextToOutputAccumulationBuffer(&text[appendedTextLength], length - appendedTextLength);
}

// TODO: is it really needed? Maybe discard to optimize?
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
