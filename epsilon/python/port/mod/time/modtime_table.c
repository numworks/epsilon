#include "modtime.h"

MP_DEFINE_CONST_FUN_OBJ_1(modtime_sleep_obj, modtime_sleep);
MP_DEFINE_CONST_FUN_OBJ_0(modtime_monotonic_obj, modtime_monotonic);

STATIC const mp_rom_map_elem_t modtime_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },
  { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&modtime_sleep_obj) },
  { MP_ROM_QSTR(MP_QSTR_monotonic), MP_ROM_PTR(&modtime_monotonic_obj) },
};

STATIC MP_DEFINE_CONST_DICT(modtime_module_globals, modtime_module_globals_table);

const mp_obj_module_t modtime_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modtime_module_globals,
};
