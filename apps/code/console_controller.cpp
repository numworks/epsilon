#include "console_controller.h"
#include <apps/i18n.h>
#include "app.h"

extern "C" {
#include <stdlib.h>
#include "port.h"
#include "py/mphal.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
}

namespace Code {

/* mp_hal_stdout_tx_strn_cooked symbol required by micropython at printing
 * needs to access information about where to print. This 'context' is provided
 * by the global sCurrentConsoleStore that points to the console store. */

static ConsoleStore * sCurrentConsoleStore = nullptr;

extern "C"
void mp_hal_stdout_tx_strn_cooked(const char * str, size_t len) {
  assert(sCurrentConsoleStore != nullptr);
  sCurrentConsoleStore->pushResult(str, len);
}

mp_obj_t execute_from_str(const char *str) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, MP_EMIT_OPT_NONE, true);
    mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::A6);
    mp_call_function_0(module_fun);
    mp_hal_set_interrupt_char(-1); // Disable interrupt
    nlr_pop();
    return 0;
  } else {
    // Uncaught exception
    return (mp_obj_t) nlr.ret_val;
  }
}

ConsoleController::ConsoleController(Responder * parentResponder) :
  ViewController(parentResponder),
  TextFieldDelegate(),
  m_rowHeight(KDText::charSize(k_fontSize).height()),
  m_tableView(this, this, 0, 0),
  m_editCell(this, this)
{
  assert(sCurrentConsoleStore == nullptr);
  sCurrentConsoleStore = &m_consoleStore;
  initPython();
}

ConsoleController::~ConsoleController() {
  stopPython();
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
  executePython(text);
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

void ConsoleController::initPython() {
  mp_stack_set_limit(40000);
  mp_port_init_stack_top();

  m_pythonHeap = (char *)malloc(16384);
  gc_init(m_pythonHeap, m_pythonHeap + 16384);

  mp_init();
}

void ConsoleController::executePython(const char * str) {
  if (execute_from_str(str)) {
    mp_hal_stdout_tx_strn_cooked(I18n::translate(I18n::Message::ConsoleError), 5);
  }
  m_consoleStore.deleteLastLineIfEmpty();
}

void ConsoleController::stopPython() {
  free(m_pythonHeap);
  sCurrentConsoleStore = nullptr;
}

}
