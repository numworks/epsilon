#include "editor_controller.h"

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


EditorController::EditorController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(this, m_buffer, 256)
{
  const char * program =

  "import kandinsky\n"
"red = kandinsky.color(255,0,0)\n"
"blue = kandinsky.color(0,0,255)\n"
"for i in range(100):\n"
"  for j in range(100):\n"
"    if ((i+j)%2 == 0):\n"
"      kandinsky.set_pixel(i, j, red)\n"
"    else:\n"
"      kandinsky.set_pixel(i, j, blue)\n";

   memcpy(m_buffer, program, 1024);
}

void EditorController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_view);
}

View * EditorController::view() {
  return &m_view;
}

void EditorController::runPython() {
  KDContext * c = KDIonContext::sharedContext();
  c->setOrigin(KDPoint(0,0));
  c->setClippingRect(KDRect(0, 0, 300, 200));

    // Initialized stack limit
    mp_stack_set_limit(40000);

    char * pythonHeap = (char *)malloc(16384);

    gc_init(pythonHeap, pythonHeap + 16384);

    // Initialize interpreter
    mp_init();

    if (execute_from_str(m_buffer)) {
        mp_hal_stdout_tx_strn_cooked("Error", 0);
    }

    free(pythonHeap);
}

bool EditorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    runPython();
    return true;
  }
  return false;
}

}
