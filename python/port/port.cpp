#include "port.h"

#include <ion.h>
#include <math.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* py/parsenum.h is a C header which uses C keyword restrict.
 * It does not exist in C++ so we define it here in order to be able to include
 * py/parsenum.h header. */
#ifdef __cplusplus
#define restrict  // disable
#endif

extern "C" {
#include "mod/matplotlib/pyplot/modpyplot.h"
#include "mod/turtle/modturtle.h"
#include "mphalport.h"
#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/parsenum.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
}

#include <escher/palette.h>

static MicroPython::ScriptProvider *sScriptProvider = nullptr;
static MicroPython::ExecutionEnvironment *sCurrentExecutionEnvironment =
    nullptr;

MicroPython::ExecutionEnvironment::~ExecutionEnvironment() {
  sCurrentExecutionEnvironment = nullptr;
}

MicroPython::ExecutionEnvironment *
MicroPython::ExecutionEnvironment::currentExecutionEnvironment() {
  return sCurrentExecutionEnvironment;
}

bool MicroPython::ExecutionEnvironment::runCode(const char *str) {
  assert(sCurrentExecutionEnvironment == nullptr);
  sCurrentExecutionEnvironment = this;

  /* Set the user interruption now, as it is needed for the normal execution and
   * for the exception handling (because of print). */
  mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::Back);

  bool runSucceeded = true;
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
    /* The input type is "single input" because the Python console is supposed
     * to be fed lines and not files. */
    // TODO: add a parameter when other input types (file, eval) are required
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, true);
    mp_call_function_0(module_fun);
    nlr_pop();
  } else {  // Uncaught exception
    runSucceeded = false;
    HandleException(&nlr);
  }

  // Disable the user interruption
  mp_hal_set_interrupt_char(-1);

  assert(sCurrentExecutionEnvironment == this);
  sCurrentExecutionEnvironment = nullptr;
  return runSucceeded;
}

void MicroPython::ExecutionEnvironment::HandleExceptionSilently() {
  // Flush the store if an error is encountered to avoid being stuck with a full
  // memory
  modpyplot_flush_used_heap();
  // TODO: do the same for other modules?
}

void MicroPython::ExecutionEnvironment::HandleException(
    nlr_buf_t *nlr_buf, MicroPython::ExecutionEnvironment *env) {
  /* We need a static execution environment to print. If it's not already set,
   * the caller should give it as a parameter. */
  if (env != nullptr) {
    assert(sCurrentExecutionEnvironment == nullptr);
    sCurrentExecutionEnvironment = env;
  }
  assert(sCurrentExecutionEnvironment != nullptr);

  /* If the exception happens while displaying the view controller, we need to
   * hide it to avoid redrawing uncomplete objects. */
  sCurrentExecutionEnvironment->hideAnyDisplayedViewController();

  /* mp_obj_print_exception is supposed to handle error printing. However,
   * because we want to print custom information, we copied and modified the
   * content of mp_obj_print_exception instead of calling it. */
  if (mp_obj_is_exception_instance((mp_obj_t)(nlr_buf->ret_val))) {
    size_t n, *values;
    mp_obj_exception_get_traceback((mp_obj_t)(nlr_buf->ret_val), &n, &values);
    if (n > 0) {
      assert(n % 3 == 0);
      for (int i = n - 3; i >= 0; i -= 3) {
        if (values[i] != 0 || i == 0) {
          if (values[i] == 0) {
            mp_printf(&mp_plat_print, "  Last command\n");
          } else {
#if MICROPY_ENABLE_SOURCE_LINE
            mp_printf(&mp_plat_print, "  File \"%q\", line %d", values[i],
                      (int)values[i + 1]);
#else
            mp_printf(&mp_plat_print, "  File \"%q\"", values[i]);
#endif
            // the block name can be NULL if it's unknown
            qstr block = values[i + 2];
            if (block == MP_QSTRnull) {
              mp_print_str(&mp_plat_print, "\n");
            } else {
              mp_printf(&mp_plat_print, ", in %q\n", block);
            }
          }
        }
      }
    }
  }
  mp_obj_print_helper(&mp_plat_print, (mp_obj_t)(nlr_buf->ret_val), PRINT_EXC);
  mp_print_str(&mp_plat_print, "\n");
  /* End of mp_obj_print_exception. */

  HandleExceptionSilently();

  // Clean the static ExecutionEnvironment
  if (env != nullptr) {
    sCurrentExecutionEnvironment = nullptr;
  }
}

void MicroPython::ExecutionEnvironment::interrupt() { mp_keyboard_interrupt(); }

extern "C" {
extern const void *_process_stack_end;
}

void MicroPython::init(void *heapStart, void *heapEnd) {
#if __EMSCRIPTEN__
  static mp_obj_t pystack[1024];
  mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
#endif
  /* We delimit the stack part that will be used by Python. The stackTop is the
   * address of the first object that can be allocated on Python stack. This
   * boundaries are used:
   * - by gc_collect to determine where to collect roots of the objects that
   *   must be kept on the heap
   * - to check if the maximal recursion depth has been reached.
   * Current stack pointer could go backward after initialization. A stack start
   * pointer defined in main is therefore used. */
  void *stackTopAddress = Ion::stackStart();

#if MP_PORT_USE_STACK_SYMBOLS
  mp_stack_set_top(stackTopAddress);
  size_t stackLimitInBytes =
      (char *)stackTopAddress - (char *)&_process_stack_end;
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

void MicroPython::deinit() { mp_deinit(); }

void MicroPython::registerScriptProvider(ScriptProvider *s) {
  sScriptProvider = s;
}

void MicroPython::collectRootsAtAddress(char *address, int byteLength) {
  /* The given address is not necessarily aligned on sizeof(void *). However,
   * any pointer stored in the range [address, address + byteLength] will be
   * aligned on sizeof(void *). This is a consequence of the alignment
   * requirements of compilers (Cf http://www.catb.org/esr/structure-packing/).
   * Micropython gc_collect_root scans looking for pointers jumping every
   * sizeof(void *). It has to be provided with a sizeof(uintptr_t)-aligned
   * address. */
  // Compute the aligned address
  // 0b000...00011 with 2 (or 3 for x64 arch) 1s
  uintptr_t bitMaskOnes = sizeof(uintptr_t) - 1;
  // 0b111...11100 with sizeof(uintptr_t)-1 0s
  uintptr_t bitMaskZeros = ~bitMaskOnes;
  uintptr_t alignedAddress =
      reinterpret_cast<uintptr_t>(address) & bitMaskZeros;
  /* Increase the length consequently with the new alignment
   * (We don't need to increase the byteLength to a sizeof(uintptr_t)-aligned
   * lenght because no pointer can be stored on less than sizeof(uintptr_t)
   * bytes.) */
  int alignedByteLength = byteLength;
  alignedByteLength += reinterpret_cast<uintptr_t>(address) & bitMaskOnes;

  assert(alignedAddress % ((uintptr_t)sizeof(uintptr_t)) == 0);
  gc_collect_root((void **)alignedAddress,
                  alignedByteLength / sizeof(uintptr_t));
}

KDColor MicroPython::Color::Parse(mp_obj_t input, Mode mode) {
  constexpr static int maxColorIntensity =
      static_cast<int>(Mode::MaxIntensity255);
  if (mp_obj_is_str(input)) {
    size_t l;
    const char *color = mp_obj_str_get_data(input, &l);
    constexpr NamedColor pairs[] = {
        NamedColor("blue", KDColorBlue),
        NamedColor("b", KDColorBlue),
        NamedColor("red", KDColorRed),
        NamedColor("r", KDColorRed),
        NamedColor("green", Escher::Palette::Green),
        NamedColor("g", Escher::Palette::Green),
        NamedColor("yellow", KDColorYellow),
        NamedColor("y", KDColorYellow),
        NamedColor("brown", Escher::Palette::Brown),
        NamedColor("black", KDColorBlack),
        NamedColor("k", KDColorBlack),
        NamedColor("white", KDColorWhite),
        NamedColor("w", KDColorWhite),
        NamedColor("pink", Escher::Palette::Pink),
        NamedColor("orange", Escher::Palette::Orange),
        NamedColor("purple", Escher::Palette::Purple),
        NamedColor("gray", Escher::Palette::GrayDark),
        NamedColor("grey", Escher::Palette::GrayDark),
        NamedColor("cyan", Escher::Palette::Cyan),
        NamedColor("magenta", Escher::Palette::Magenta)};
    for (NamedColor p : pairs) {
      if (strcmp(p.name(), color) == 0) {
        return p.color();
      }
    }

    if (color[0] == '#') {
      // TODO handle #abc as #aabbcc (see matplotlib spec)
      if (l != 7) {
        mp_raise_ValueError("RGB hex values are 6 bytes long");
      }
      uint32_t colorInt = mp_obj_get_int(
          mp_parse_num_integer(color + 1, strlen(color + 1), 16, NULL));
      return KDColor::RGB24(colorInt);
    }

    mp_float_t grayLevel = mp_obj_float_get(
        mp_parse_num_decimal(color, strlen(color), false, false, NULL));
    if (grayLevel >= 0.0 && grayLevel <= 1.0) {
      uint8_t color = maxColorIntensity * (float)grayLevel;
      return KDColor::RGB888(color, color, color);
    }
    mp_raise_ValueError("Gray levels are between 0.0 and 1.0");
  } else if (mp_obj_is_int(input)) {
    mp_raise_TypeError("Int are not colors");
    // See
    // https://github.com/numworks/epsilon/issues/1533#issuecomment-618443492
  } else {
    size_t len;
    mp_obj_t *elem;

    mp_obj_get_array(input, &len, &elem);

    if (len != 3) {
      mp_raise_TypeError("Color needs 3 components");
    }
    int intensityFactor = maxColorIntensity / static_cast<int>(mode);
    return KDColor::RGB888(intensityFactor * mp_obj_get_float(elem[0]),
                           intensityFactor * mp_obj_get_float(elem[1]),
                           intensityFactor * mp_obj_get_float(elem[2]));
  }
  mp_raise_TypeError("Color couldn't be parsed");
}

void gc_collect_regs_and_stack(void) {
  // get the registers and the sp
  jmp_buf regs;
  uintptr_t sp = Ion::collectRegisters(regs);

  void *python_stack_top = MP_STATE_THREAD(stack_top);
  assert(python_stack_top != NULL);

  /* On the device, the stack is stored in reverse order, but it might not be
   * the case on a computer. We thus have to take the absolute value of the
   * addresses difference. */
  size_t stackLengthInByte;
  void **scanStart;
  if ((uintptr_t)python_stack_top > sp) {
    /* To compute the stack length:
     *                               registers
     *                             <----------->
     * STACK <-  ...|  |  |  |  |  |--|--|--|--|  |  |  |  |  |  |
     *                             ^                             ^
     *                             sp                      python_stack_top
     * */

    stackLengthInByte = (uintptr_t)python_stack_top - sp;
    scanStart = (void **)sp;

  } else {
    /* When computing the stack length, take into account regs' size.
     *                                              registers
     *                                            <----------->
     * STACK ->  |  |  |  |  |  |  |  |  |  |  |  |--|--|--|--|  |  |  |...
     *           ^python_stack_top                ^sp
     * */

    stackLengthInByte = sp - (uintptr_t)python_stack_top + sizeof(regs);
    scanStart = (void **)python_stack_top;
  }
  /* Memory error detectors might find an error here as they might split regs
   * and stack memory zones. */
  MicroPython::collectRootsAtAddress((char *)scanStart, stackLengthInByte);
}

void gc_collect(void) {
  gc_collect_start();
  modturtle_gc_collect();
  modpyplot_gc_collect();
  gc_collect_regs_and_stack();
  gc_collect_end();
}

void nlr_jump_fail(void *val) {
  /* Called by nlr_jump if no nlr buf has been pushed.
   * Bail out with a fatal error. */
  abort();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
  if (sScriptProvider != nullptr) {
    const char *script = sScriptProvider->contentOfScript(filename, true);
    if (script != nullptr) {
      return mp_lexer_new_from_str_len(qstr_from_str(filename), script,
                                       strlen(script), 0 /* size_t free_len*/);
    } else {
      mp_raise_OSError(MP_ENOENT);
    }
  } else {
    mp_raise_OSError(MP_ENOENT);
  }
}

mp_import_stat_t mp_import_stat(const char *path) {
  if (sScriptProvider && sScriptProvider->contentOfScript(path, false)) {
    return MP_IMPORT_STAT_FILE;
  }
  return MP_IMPORT_STAT_NO_EXIST;
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
  assert(sCurrentExecutionEnvironment != nullptr);
  sCurrentExecutionEnvironment->printText(str, len);
}

const char *mp_hal_input(const char *prompt) {
  assert(sCurrentExecutionEnvironment != nullptr);
  return sCurrentExecutionEnvironment->inputText(prompt);
}
