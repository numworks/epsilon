#include "py/builtin.h"
#include "py/obj.h"
#include <string.h>
#include "mphalport.h"

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

mp_obj_t mp_builtin_input(size_t n_args, const mp_obj_t *args) {
  if (n_args == 1) {
    mp_obj_print(args[0], PRINT_STR);
  }
  /*vstr_t line;
  vstr_init(&line, 16);
  int ret = mp_hal_readline(&line, "");
  if (ret == CHAR_CTRL_C) {
    nlr_raise(mp_obj_new_exception(&mp_type_KeyboardInterrupt));
  }
  if (line.len == 0 && ret == CHAR_CTRL_D) {
    nlr_raise(mp_obj_new_exception(&mp_type_EOFError));
  }
  */
  const char * input = mp_hal_input();
  return mp_obj_new_str(input, strlen(input), false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj, 0, 1, mp_builtin_input);
