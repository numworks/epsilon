#include "executor_controller.h"

extern "C" {
#include <stdlib.h>
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
    mp_call_function_0(module_fun);
    nlr_pop();
    return 0;
  } else {
    // uncaught exception
    return (mp_obj_t)nlr.ret_val;
  }
}

extern "C"
void mp_hal_stdout_tx_strn_cooked(const char * str, size_t len) {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->drawString(str, KDPoint(0, 0));
}

ExecutorController::ContentView::ContentView(Program * program) :
  View(),
  m_program(program)
{
}

void ExecutorController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  runPython();
}

void ExecutorController::ContentView::runPython() const {
  // Initialized stack limit
  mp_stack_set_limit(40000);

  char * pythonHeap = (char *)malloc(16384);

  gc_init(pythonHeap, pythonHeap + 16384);

  // Initialize interpreter
  mp_init();

  if (execute_from_str(m_program->readOnlyContent())) {
    mp_hal_stdout_tx_strn_cooked("Error", 0);
  }

  free(pythonHeap);
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
