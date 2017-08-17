#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"

mp_uint_t gc_helper_get_regs_and_sp(mp_uint_t *regs);

void gc_collect(void) {
#if 0
    gc_collect_start();

    // get the registers and the sp
    mp_uint_t regs[10];
    mp_uint_t sp = gc_helper_get_regs_and_sp(regs);



    /* Next we want to iterate through the whole stack
     * Without threads, the stmhal version just refers to the top of the stack as _ram_end, because that's where the
     * stack actually starts.
     * This is going to need some help from Ion.
     */


    // trace the stack, including the registers (since they live on the stack in this function)
    #if MICROPY_PY_THREAD
    gc_collect_root((void**)sp, ((uint32_t)MP_STATE_THREAD(stack_top) - sp) / sizeof(uint32_t));
    #else
    gc_collect_root((void**)sp, ((uint32_t)&_ram_end - sp) / sizeof(uint32_t));
    #endif

    // trace root pointers from any threads
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif

    // end the GC
    gc_collect_end();

  // Let's just do nothing for now
#else
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    // FIXME
    //gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    //gc_dump_info();
#endif
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1);
}
