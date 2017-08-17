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

/* mp_hal_stdout_tx_strn_cooked symbol required by micropython at printing
 * needs to access information about where to print (depending on the strings
 * printed before). This 'context' is provided by the global sCurrentView that
 * points to the content view only within the drawRect method (as runPython is
 * called within drawRect). */
static const ExecutorController::ContentView * sCurrentView = nullptr;

extern "C"
void mp_hal_stdout_tx_strn_cooked(const char * str, size_t len) {
  assert(sCurrentView != nullptr);
  sCurrentView->print(str);
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

  assert(sCurrentView == nullptr);
  sCurrentView = this;

  // Reinitialize the print location
  m_printLocation = KDPointZero;
  runPython();

  sCurrentView = nullptr;
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
