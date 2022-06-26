#include "modmatplotlib.h"

extern const mp_obj_module_t modpyplot_module;

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modmatplotlib___init___obj, modmatplotlib___init__);

// Define the module table as non-const, because MicroPython needs to be able to modify it.
STATIC mp_rom_map_elem_t modmatplotlib_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_matplotlib) },
  { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&modmatplotlib___init___obj) },
  { MP_ROM_QSTR(MP_QSTR_pyplot), MP_ROM_PTR(&modpyplot_module) }
};

// Define the module object, not as a constant, because MicroPython needs to be able to dynamically add attributes to it.
mp_obj_dict_t modmatplotlib_module_globals = { \
    .base = {&mp_type_dict}, \
    .map = { \
        .all_keys_are_qstrs = 1, \
        .is_fixed = 0, \
        .is_ordered = 1, \
        .used = MP_ARRAY_SIZE(modmatplotlib_module_globals_table), \
        .alloc = MP_ARRAY_SIZE(modmatplotlib_module_globals_table), \
        .table = (mp_map_elem_t *)(mp_rom_map_elem_t *)modmatplotlib_module_globals_table, \
    }, \
};


const mp_obj_module_t modmatplotlib_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modmatplotlib_module_globals,
};
