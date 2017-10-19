#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

extern "C" {
#include "py/builtin.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/lexer.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mperrno.h"
}

#include <apps/i18n.h>
#include <ion/keyboard.h>
#include "port.h"

static char * python_stack_top = NULL;

static MicroPython::ScriptProvider * sScriptProvider = nullptr;
static MicroPython::ExecutionEnvironment * sCurrentExecutionEnvironment = nullptr;

void MicroPython::ExecutionEnvironment::runCode(const char * str) {
  assert(sCurrentExecutionEnvironment == nullptr);
  sCurrentExecutionEnvironment = this;

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, MP_EMIT_OPT_NONE, true);
    mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::A6);
    mp_call_function_0(module_fun);
    mp_hal_set_interrupt_char(-1); // Disable interrupt
    nlr_pop();
  } else {
    // Uncaught exception
    //return (mp_obj_t) nlr.ret_val;
    printText(I18n::translate(I18n::Message::ConsoleError), 5);
  }

  assert(sCurrentExecutionEnvironment == this);
  sCurrentExecutionEnvironment = nullptr;
}

void MicroPython::init(void * heapStart, void * heapEnd) {
  mp_stack_set_limit(40000);
  char dummy;
  python_stack_top = &dummy;
  gc_init(heapStart, heapEnd);
  mp_init();
}

void MicroPython::deinit(){
  mp_deinit();
}

void MicroPython::registerScriptProvider(ScriptProvider * s) {
  sScriptProvider = s;
}

void gc_collect(void) {
  assert(python_stack_top != NULL);
  gc_collect_start();

  /* get the registers.
   * regs is the also the last object on the stack so the stack is bound by
   * &regs and python_stack_top. */
  jmp_buf regs;
  setjmp(regs);

  void **regs_ptr = (void**)(void*)&regs;
  gc_collect_root(regs_ptr, ((uintptr_t)python_stack_top - (uintptr_t)&regs) / sizeof(uintptr_t));

  gc_collect_end();
}

void nlr_jump_fail(void *val) {
    while (1);
}

mp_lexer_t * mp_lexer_new_from_file(const char * filename) {
  if (sScriptProvider != nullptr) {
    const char * script = sScriptProvider->contentOfScript(filename);
    if (script != nullptr) {
      return mp_lexer_new_from_str_len(qstr_from_str(filename), script, strlen(script), 0 /* size_t free_len*/);
    } else {
      mp_raise_OSError(MP_ENOENT);
    }
  } else {
    mp_raise_OSError(MP_ENOENT);
  }
}

mp_import_stat_t mp_import_stat(const char *path) {
  if (sScriptProvider && sScriptProvider->contentOfScript(path)) {
    return MP_IMPORT_STAT_FILE;
  }
  return MP_IMPORT_STAT_NO_EXIST;
}

void mp_hal_stdout_tx_strn_cooked(const char * str, size_t len) {
  assert(sCurrentExecutionEnvironment != nullptr);
  sCurrentExecutionEnvironment->printText(str, len);
}
