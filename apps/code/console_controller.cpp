#include "console_controller.h"
#include <apps/i18n.h>
#include "app.h"
#include <assert.h>

extern "C" {
#include <stdlib.h>
}

namespace Code {

ConsoleController::ConsoleController(Responder * parentResponder, ScriptStore * scriptStore) :
  ViewController(parentResponder),
  TextFieldDelegate(),
  m_rowHeight(KDText::charSize(k_fontSize).height()),
  m_tableView(this, this, 0, 0),
  m_editCell(this, this)
{
  m_outputAccumulationBuffer = (char *)malloc(k_outputAccumulationBufferSize);
  emptyOutputAccumulationBuffer();
  m_pythonHeap = (char *)malloc(k_pythonHeapSize);
  MicroPython::init(m_pythonHeap, m_pythonHeap + k_pythonHeapSize);
  MicroPython::registerScriptProvider(scriptStore);
}

ConsoleController::~ConsoleController() {
  MicroPython::deinit();
  free(m_pythonHeap);
  free(m_outputAccumulationBuffer);
}

void ConsoleController::viewWillAppear() {
  m_editCell.setEditing(true);
}

void ConsoleController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_editCell);
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

bool ConsoleController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool ConsoleController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  return false;
}

bool ConsoleController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  m_consoleStore.pushCommand(text, strlen(text));
  assert(m_outputAccumulationBuffer[0] == '\0');
  runCode(text);
  flushOutputAccumulationBufferToStore();
  m_consoleStore.deleteLastLineIfEmpty();
  textField->setText("");
  m_tableView.reloadData();
  m_tableView.scrollToCell(0, m_consoleStore.numberOfLines());
  m_editCell.setEditing(true);
  return true;
}

bool ConsoleController::textFieldDidAbortEditing(TextField * textField, const char * text) {
  app()->dismissModalViewController();
  return true;
}

Toolbox * ConsoleController::toolboxForTextField(TextField * textFied) {
  return nullptr;
}

/* printText is called by the Python machine.
 * The text argument is not always null-terminated. */
void ConsoleController::printText(const char * text, size_t length) {
  int textCutIndex = firstNewLineCharIndex(text, length);
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

void ConsoleController::flushOutputAccumulationBufferToStore() {
  m_consoleStore.pushResult(m_outputAccumulationBuffer, strlen(m_outputAccumulationBuffer));
  emptyOutputAccumulationBuffer();
}

void ConsoleController::appendTextToOutputAccumulationBuffer(const char * text, size_t length) {
  int endOfAccumulatedText = strlen(m_outputAccumulationBuffer);
  int spaceLeft = k_outputAccumulationBufferSize - endOfAccumulatedText;
  if (spaceLeft > length) {
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

int ConsoleController::firstNewLineCharIndex(const char * text, size_t length) {
  int index = 0;
  while (index < length) {
    if (text[index] == '\n') {
      return index;
    }
    index++;
  }
  return index;
}

}
