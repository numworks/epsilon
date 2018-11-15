#include <stdio.h>
#include <string.h>

#include "py/smallint.h"
#include "py/runtime.h"

#include "ion/timing.h"

void delay_ms(mp_uint_t delay) {
  uint32_t start = millis();
  while (millis() - start < delay && !micropython_port_should_interrupt()) {
    msleep(1);
  }
}

STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
  delay_ms((mp_uint_t)(1000 * mp_obj_get_float(seconds_o)));
#else
  delay_ms(1000 * mp_obj_get_int(seconds_o));
#endif
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

STATIC mp_obj_t time_monotonic(void) {
    return mp_obj_new_float(millis() / 1000.0);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_monotonic_obj, time_monotonic);

STATIC const mp_rom_map_elem_t time_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },
  { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&time_sleep_obj) },
  { MP_ROM_QSTR(MP_QSTR_monotonic), MP_ROM_PTR(&time_monotonic_obj) },
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t time_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&time_module_globals,
};
