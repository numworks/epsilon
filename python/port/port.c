#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"

typedef jmp_buf regs_t;

STATIC void gc_helper_get_regs(regs_t arr) {
  setjmp(arr);
}

static char * python_stack_top = NULL;

void mp_port_init_stack_top() {
  char dummy;
  python_stack_top = &dummy;
}

void gc_collect(void) {
  assert(python_stack_top != NULL);
  gc_collect_start();

  /* get the registers.
   * regs is the also the last object on the stack so the stack is bound by
   * &regs and python_stack_top. */
  regs_t regs;
  gc_helper_get_regs(regs);

  void **regs_ptr = (void**)(void*)&regs;
  gc_collect_root(regs_ptr, ((uintptr_t)python_stack_top - (uintptr_t)&regs) / sizeof(uintptr_t));

  gc_collect_end();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1);
}
