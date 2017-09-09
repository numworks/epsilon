#include "executor_controller.h"

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

mp_obj_t execute_from_str(const char *str) {
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0/*MP_QSTR_*/, str, strlen(str), false);
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, MP_EMIT_OPT_NONE, false);
    mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::A6);
    mp_call_function_0(module_fun);
    mp_hal_set_interrupt_char(-1); // disable interrupt
    nlr_pop();
    return 0;
  } else {
    // uncaught exception
    return (mp_obj_t)nlr.ret_val;
  }
}

ExecutorController::ContentView::ContentView(Program * program) :
  View(),
  m_program(program),
  m_printLocation(KDPointZero)
{
}

void ExecutorController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  assert(ctx == KDIonContext::sharedContext());
  clearScreen(ctx);

  enableCatch();

  // Reinitialize the print location
  m_printLocation = KDPointZero;
  runPython();

  disableCatch();
}

void ExecutorController::ContentView::print(const char * str) const {
  KDContext * ctx = KDIonContext::sharedContext();
  m_printLocation = ctx->drawString(str, m_printLocation);
  if (bounds().height() < m_printLocation.y()) {
    clearScreen(ctx);
    m_printLocation = KDPoint(m_printLocation.x(), 0);
  }
}

void ExecutorController::ContentView::runPython() const {
  // Initialized stack limit
  mp_stack_set_limit(40000);

  mp_port_init_stack_top();

  char * pythonHeap = (char *)malloc(16384);

  gc_init(pythonHeap, pythonHeap + 16384);

  // Initialize interpreter
  mp_init();

  if (execute_from_str(m_program->readOnlyContent())) {
    mp_hal_stdout_tx_strn_cooked("Error", 0);
  }

  free(pythonHeap);
}

void ExecutorController::ContentView::clearScreen(KDContext * ctx) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

ExecutorController::ExecutorController(Program * program) :
  ViewController(nullptr),
  m_view(program)
{
}

View * ExecutorController::view() {
  return &m_view;
}

bool ExecutorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

}
