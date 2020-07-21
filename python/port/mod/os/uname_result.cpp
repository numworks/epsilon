extern "C" {
#include <py/smallint.h>
#include <py/objint.h>
#include <py/objstr.h>
#include <py/objtype.h>
#include <py/runtime.h>
#include <py/stream.h>
#include <py/builtin.h>
#include <py/obj.h>
#include <py/misc.h>
#include "uname_result.h"
}

STATIC void uname_result_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
STATIC mp_obj_t uname_result_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC void uname_result_attr(mp_obj_t self_in, qstr attribute, mp_obj_t *destination);


