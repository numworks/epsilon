#include "modmatplotlib.h"

extern const mp_obj_module_t modpyplot_module;

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modmatplotlib___init___obj, modmatplotlib___init__);

STATIC const mp_rom_map_elem_t modmatplotlib_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_matplotlib) },
  { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&modmatplotlib___init___obj) },
  { MP_ROM_QSTR(MP_QSTR_pyplot), MP_ROM_PTR(&modpyplot_module) }
};

STATIC MP_DEFINE_CONST_DICT(modmatplotlib_module_globals, modmatplotlib_module_globals_table);

const mp_obj_module_t modmatplotlib_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modmatplotlib_module_globals,
};
