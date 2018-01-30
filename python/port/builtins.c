#include "py/builtin.h"
#include "py/obj.h"
#include <string.h>
#include "mphalport.h"

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

mp_obj_t mp_builtin_input(size_t n_args, const mp_obj_t *args) {
  // 1 - Retrieve the prompt if any
  const char * prompt = NULL;
  if (n_args == 1) {
    prompt = mp_obj_str_get_str(args[0]);
  }

  // 2 - Perform the HAL input command
  const char * result = mp_hal_input(prompt);

  // 3 - Log the prompt, result and flush a new line
  mp_obj_t resultStr = mp_obj_new_str(result, strlen(result), false);
  if (n_args == 1) {
    mp_obj_print(args[0], PRINT_STR);
  }
  mp_obj_print(resultStr, PRINT_STR);
  mp_print_str(MP_PYTHON_PRINTER, "\n");

  return resultStr;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj, 0, 1, mp_builtin_input);
