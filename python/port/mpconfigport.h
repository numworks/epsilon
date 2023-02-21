#include <alloca.h>
#include <stdint.h>

#include "helpers.h"

/* MicroPython configuration options
 * We're not listing the default options as defined in mpconfig.h */

#if __EMSCRIPTEN__
// Enable a PyStack where most objects are allocated instead of always using the
// heap
/* This enables to allocate and free memory in a scope (thus, Python can call
 * Python) but also has the collateral effect of removing bugs regarding
 * garbage collection on the web simulator. Indeed, fewer objetcts are
 * allocated on the heap and the garbage collection is less frequently called.
 * We suspect that garbage collection failed in javascript because when
 * collecting roots the transpiled C code is denied access to Javascript
 * variables that can store pointers to the Python heap. The pointed objects
 * are therefore erased prematurely. */
#define MICROPY_ENABLE_PYSTACK (1)
#endif

// Maximum length of a path in the filesystem
#define MICROPY_ALLOC_PATH_MAX (32)

// Whether to include the garbage collector
#define MICROPY_ENABLE_GC (1)

// Whether to check C stack usage
#define MICROPY_STACK_CHECK (1)

// Whether to provide the mp_kbd_exception object, and micropython.kbd_intr
// function
#define MICROPY_KBD_EXCEPTION (1)

// Long int implementation
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)

// Whether to include information in the byte code to determine source
#define MICROPY_ENABLE_SOURCE_LINE (1)

// Exception messages provide full info, e.g. object names
#define MICROPY_ERROR_REPORTING (MICROPY_ERROR_REPORTING_DETAILED)

// Float and complex implementation
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_DOUBLE)

// Enable features which improve CPython compatibility
// but may lead to more code size/memory usage.
// TODO: Originally intended as generic category to not
// add bunch of once-off options. May need refactoring later
#define MICROPY_CPYTHON_COMPAT (0)

// modturtle needs a hook upon init
#define MICROPY_MODULE_BUILTIN_INIT (1)

// Support for async/await/async for/async with
#define MICROPY_PY_ASYNC_AWAIT (0)

// Whether to support bytearray object
#define MICROPY_PY_BUILTINS_BYTEARRAY (0)

// Whether to support frozenset object
#define MICROPY_PY_BUILTINS_FROZENSET (1)

// Whether to support property object
#define MICROPY_PY_BUILTINS_PROPERTY (0)

// Whether to support unicode strings
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)

// Whether to set __file__ for imported modules
#define MICROPY_PY___FILE__ (0)

// Whether to provide "array" module. Note that large chunk of the
// underlying code is shared with "bytearray" builtin type, so to
// get real savings, it should be disabled too.
#define MICROPY_PY_ARRAY (0)

// Whether to support attrtuple type (MicroPython extension)
// It provides space-efficient tuples with attribute access
#define MICROPY_PY_ATTRTUPLE (0)

// Whether to provide "collections" module
#define MICROPY_PY_COLLECTIONS (0)

// Whether to provide special math functions: math.{erf,erfc,gamma,lgamma}
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (1)

// Whether to provide math.factorial function
#define MICROPY_PY_MATH_FACTORIAL (1)

// Whether math.factorial is large, fast and recursive (1) or small and slow
// (0).
#define MICROPY_OPT_MATH_FACTORIAL (1)

// Whether to provide "cmath" module
#define MICROPY_PY_CMATH (1)

// Whether to provide "gc" module
#define MICROPY_PY_GC (0)

// Whether to provide "io" module
#define MICROPY_PY_IO (0)

// Whether to provide "struct" module
#define MICROPY_PY_STRUCT (0)

// Whether to provide "sys" module
#define MICROPY_PY_SYS (0)

// Whether to provide the "urandom" module
#define MICROPY_PY_URANDOM (1)

// Whether to include: randrange, randint, choice, random, uniform
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)

// Whether to support rounding of integers (incl bignum); eg round(123,-1)=120
#define MICROPY_PY_BUILTINS_ROUND_INT (1)

// Function to seed URANDOM with on init
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC micropython_port_random()

// Make a pointer to RAM callable (eg set lower bit for Thumb code)
// (This scheme won't work if we want to mix Thumb and normal ARM code.)
#define MICROPY_MAKE_POINTER_CALLABLE(p) (p)

#define MICROPY_VM_HOOK_LOOP micropython_port_vm_hook_loop();

typedef intptr_t mp_int_t;    // must be pointer size
typedef uintptr_t mp_uint_t;  // must be pointer size

typedef long mp_off_t;

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS                                      \
  {MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj}, \
      {MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj},

#define MP_STATE_PORT MP_STATE_VM

extern const struct _mp_obj_module_t modion_module;
extern const struct _mp_obj_module_t modkandinsky_module;
extern const struct _mp_obj_module_t modmatplotlib_module;
extern const struct _mp_obj_module_t modpyplot_module;
extern const struct _mp_obj_module_t modtime_module;
extern const struct _mp_obj_module_t modturtle_module;

#define MICROPY_PORT_BUILTIN_MODULES                                        \
  {MP_ROM_QSTR(MP_QSTR_ion), MP_ROM_PTR(&modion_module)},                   \
      {MP_ROM_QSTR(MP_QSTR_kandinsky), MP_ROM_PTR(&modkandinsky_module)},   \
      {MP_ROM_QSTR(MP_QSTR_matplotlib), MP_ROM_PTR(&modmatplotlib_module)}, \
      {MP_ROM_QSTR(MP_QSTR_matplotlib_dot_pyplot),                          \
       MP_ROM_PTR(&modpyplot_module)},                                      \
      {MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&modtime_module)},             \
      {MP_ROM_QSTR(MP_QSTR_turtle), MP_ROM_PTR(&modturtle_module)},
