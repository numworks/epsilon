#include "port.h"

#include <ion/keyboard.h>

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>

extern "C" {
#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "mphalport.h"
#include "mod/turtle/modturtle.h"
}

static MicroPython::ScriptProvider * sScriptProvider = nullptr;
static MicroPython::ExecutionEnvironment * sCurrentExecutionEnvironment = nullptr;

MicroPython::ExecutionEnvironment * MicroPython::ExecutionEnvironment::currentExecutionEnvironment() {
  return sCurrentExecutionEnvironment;
}

void MicroPython::ExecutionEnvironment::runCode(const char * str) {
  assert(sCurrentExecutionEnvironment == nullptr);
  sCurrentExecutionEnvironment = this;

  /* Set the user interruption now, as it is needed for the normal execution and
   * for the exception handling (because of print). */
  mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::Back);

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
    /* The input type is "single input" because the Python console is supposed
     * to be fed lines and not files. */
    // TODO: add a parameter when other input types (file, eval) are required
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, MP_EMIT_OPT_NONE, true);
    mp_call_function_0(module_fun);
    nlr_pop();
  } else { // Uncaught exception
    /* mp_obj_print_exception is supposed to handle error printing. However,
     * because we want to print custom information, we copied and modified the
     * content of mp_obj_print_exception instead of calling it. */
    if (mp_obj_is_exception_instance((mp_obj_t)nlr.ret_val)) {
        size_t n, *values;
        mp_obj_exception_get_traceback((mp_obj_t)nlr.ret_val, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);
            for (int i = n - 3; i >= 0; i -= 3) {
              if (values[i] != 0 || i == 0) {
                if (values[i] == 0) {
                  mp_printf(&mp_plat_print, "  Last command\n");
                } else {
#if MICROPY_ENABLE_SOURCE_LINE
                  mp_printf(&mp_plat_print, "  File \"%q\", line %d", values[i], (int)values[i + 1]);
#else
                  mp_printf(&mp_plat_print, "  File \"%q\"", values[i]);
#endif
                  // the block name can be NULL if it's unknown
                  qstr block = values[i + 2];
                  if (block == MP_QSTR_NULL) {
                    mp_print_str(&mp_plat_print, "\n");
                  } else {
                    mp_printf(&mp_plat_print, ", in %q\n", block);
                  }
                }
              }
            }
        }
    }
    mp_obj_print_helper(&mp_plat_print, (mp_obj_t)nlr.ret_val, PRINT_EXC);
    mp_print_str(&mp_plat_print, "\n");
    /* End of mp_obj_print_exception. */
  }

  // Disable the user interruption
  mp_hal_set_interrupt_char(-1);

  assert(sCurrentExecutionEnvironment == this);
  sCurrentExecutionEnvironment = nullptr;
}

void MicroPython::ExecutionEnvironment::interrupt() {
  mp_keyboard_interrupt();
}

void MicroPython::ExecutionEnvironment::setSandboxIsDisplayed(bool display) {
  if (m_sandboxIsDisplayed && !display) {
    modturtle_view_did_disappear();
  }
  m_sandboxIsDisplayed = display;
}

extern "C" {
  extern const void * _stack_start;
  extern const void * _stack_end;
}

void MicroPython::init(void * heapStart, void * heapEnd) {
#if __EMSCRIPTEN__
  static mp_obj_t pystack[1024];
  mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
#endif

  volatile int stackTop;
  void * stackTopAddress = (void *)(&stackTop);
  /* We delimit the stack part that will be used by Python. The stackTop is the
   * address of the first object that can be allocated on Python stack. This
   * boundaries are used:
   * - by gc_collect to determine where to collect roots of the objects that
   *   must be kept on the heap
   * - to check if the maximal recursion depth has been reached. */
#if MP_PORT_USE_STACK_SYMBOLS
  mp_stack_set_top(stackTopAddress);
  size_t stackLimitInBytes = (char *)stackTopAddress - (char *)&_stack_end;
  mp_stack_set_limit(stackLimitInBytes);
#else
  mp_stack_set_top(stackTopAddress);
  /* The stack limit is set to roughly mimic the maximal recursion depth of the
   * device - and actually to be slightly less to be sure not to beat the device
   * performance.  */
  mp_stack_set_limit(29152);
#endif
  gc_init(heapStart, heapEnd);
  mp_init();
}

void MicroPython::deinit() {
  mp_deinit();
}

void MicroPython::registerScriptProvider(ScriptProvider * s) {
  sScriptProvider = s;
}

void MicroPython::collectRootsAtAddress(char * address, int byteLength) {
#if __EMSCRIPTEN__
   // All objects are aligned, as asserted.
  assert(((unsigned long)address) % ((unsigned long)sizeof(void *)) == 0);
  assert(byteLength % sizeof(void *) == 0);
  gc_collect_root((void **)address, byteLength / sizeof(void *));
#else
  for (size_t i = 0; i < sizeof(void *); i++) {
    /* Objects on the stack are not necessarily aligned on sizeof(void *),
     * which is also true for pointers refering to the heap. MicroPython
     * gc_collect_root expects a table of void * that will be scanned every
     * sizeof(void *) step. So we have to scan the stack repetitively with a
     * increasing offset to be sure to check every byte for a heap address.
     * If some memory can be reinterpreted as a pointer in the heap, gc_collect_root
     * will prevent the destruction of the pointed heap memory. At worst (if
     * the interpreted pointer was in fact an unaligned object or uninitialized
     * memory), we will just keep extra objects in the heap which is not optimal
     * but does not cause any crash. */
    char * addressWithOffset = address + i;
    // Ensure to round the length to the ceiling
    size_t lengthInAddressSize = (byteLength - i + sizeof(void *) - 1)/sizeof(void *);
    gc_collect_root((void **)addressWithOffset, lengthInAddressSize);
  }
#endif
}

void gc_collect(void) {
  void * python_stack_top = MP_STATE_THREAD(stack_top);
  assert(python_stack_top != NULL);

  gc_collect_start();

  modturtle_gc_collect();

  /* get the registers.
   * regs is the also the last object on the stack so the stack is bound by
   * &regs and python_stack_top. */
  jmp_buf regs;
  /* TODO: we use setjmp to get the registers values to look for python heap
   * root. However, the 'setjmp' does not guarantee that it gets all registers
   * values. We should check our setjmp implementation for the device and
   * ensure that it also works for other platforms. */
  setjmp(regs);

  void **regs_ptr = (void**)&regs;

  /* On the device, the stack is stored in reverse order, but it might not be
   * the case on a computer. We thus have to take the absolute value of the
   * addresses difference. */
  size_t stackLengthInByte;
  void ** scanStart;
  if ((uintptr_t)python_stack_top > (uintptr_t)regs_ptr) {

    /* To compute the stack length:
     *                                  regs
     *                             <----------->
     * STACK <-  ...|  |  |  |  |  |--|--|--|--|  |  |  |  |  |  |
     *                             ^&regs                        ^python_stack_top
     * */

    stackLengthInByte = (uintptr_t)python_stack_top - (uintptr_t)regs_ptr;
    scanStart = regs_ptr;

  } else {

    /* When computing the stack length, take into account regs' size.
     *                                                 regs
     *                                            <----------->
     * STACK ->  |  |  |  |  |  |  |  |  |  |  |  |--|--|--|--|  |  |  |...
     *           ^python_stack_top                ^&regs
     * */

    stackLengthInByte = (uintptr_t)regs_ptr - (uintptr_t)python_stack_top + sizeof(regs);
    scanStart = (void **)python_stack_top;

  }
  /* Memory error detectors might find an error here as they might split regs
   * and stack memory zones. */
  MicroPython::collectRootsAtAddress((char *)scanStart, stackLengthInByte);
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

const char * mp_hal_input(const char * prompt) {
  assert(sCurrentExecutionEnvironment != nullptr);
  return sCurrentExecutionEnvironment->inputText(prompt);
}
