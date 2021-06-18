#include "modtime.h"

MP_DEFINE_CONST_FUN_OBJ_1(modtime_sleep_obj, modtime_sleep);
MP_DEFINE_CONST_FUN_OBJ_0(modtime_monotonic_obj, modtime_monotonic);

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modtime_localtime_obj, 0, 1, modtime_localtime);
MP_DEFINE_CONST_FUN_OBJ_1(modtime_mktime_obj, modtime_mktime);
MP_DEFINE_CONST_FUN_OBJ_0(modtime_time_obj, modtime_time);

MP_DEFINE_CONST_FUN_OBJ_0(modtime_rtcmode_obj, modtime_rtcmode);
MP_DEFINE_CONST_FUN_OBJ_1(modtime_setrtcmode_obj, modtime_setrtcmode);
MP_DEFINE_CONST_FUN_OBJ_1(modtime_setlocaltime_obj, modtime_setlocaltime);

STATIC const mp_rom_map_elem_t modtime_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },
  { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&modtime_sleep_obj) },
  { MP_ROM_QSTR(MP_QSTR_monotonic), MP_ROM_PTR(&modtime_monotonic_obj) },

  { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&modtime_localtime_obj) },
  { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&modtime_mktime_obj) },
  { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&modtime_time_obj) },

  { MP_ROM_QSTR(MP_QSTR_rtcmode), MP_ROM_PTR(&modtime_rtcmode_obj) },
  { MP_ROM_QSTR(MP_QSTR_setrtcmode), MP_ROM_PTR(&modtime_setrtcmode_obj) },
  { MP_ROM_QSTR(MP_QSTR_setlocaltime), MP_ROM_PTR(&modtime_setlocaltime_obj) },
};

STATIC MP_DEFINE_CONST_DICT(modtime_module_globals, modtime_module_globals_table);

const mp_obj_module_t modtime_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modtime_module_globals,
};
